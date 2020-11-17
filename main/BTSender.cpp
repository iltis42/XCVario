
#include <esp_log.h>
#include "BTSender.h"
#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/semphr.h>
#include <algorithm>
#include <HardwareSerial.h>
#include "RingBufCPP.h"
#include <driver/uart.h>
#include "OpenVario.h"
#include <logdef.h>
#include "Switch.h"
#include "sensor.h"


extern xSemaphoreHandle nvMutex;

const int baud[] = { 0, 4800, 9600, 19200, 38400, 57600, 115200 };

#define QUEUE_SIZE 10
RingBufCPP<SString, QUEUE_SIZE> btbuf;
RingBufCPP<SString, QUEUE_SIZE> ser1txbuf;
RingBufCPP<SString, QUEUE_SIZE> ser2txbuf;

#define RXBUFLEN 256
char rxBuffer[RXBUFLEN];
int BTSender::i=0;

#define RFCOMM_SERVER_CHANNEL 1
#define HEARTBEAT_PERIOD_MS 50


bool      BTSender::bluetooth_up = false;
uint8_t   BTSender::rfcomm_channel_nr = 1;
uint16_t  rfcomm_channel_id = 0;
uint8_t   BTSender::spp_service_buffer[200]; // 100
btstack_timer_source_t BTSender::heartbeat;
btstack_packet_callback_registration_t BTSender::hci_event_callback_registration;
void ( * BTSender::_callback)(char * rx, uint16_t len);
portMUX_TYPE btmux = portMUX_INITIALIZER_UNLOCKED;


int BTSender::queueFull() {
	if( !blue_enable.get() )
		return 1;
	int ret = 0;
	if(btbuf.isFull())
		ret=1;
	return ret;
}

void BTSender::send(char * s){
	ESP_LOGD( FNAME,"XCVario TX %s",s);
	if ( !btbuf.isFull() && blue_enable.get() ) {   // bluetooth enabled (if yes sent XCVario data)
		portENTER_CRITICAL_ISR(&btmux);
		btbuf.add( s );
		portEXIT_CRITICAL_ISR(&btmux);
	}
	if( serial1_tx.get() & 1 ){         //  XCVario data enabled for ttyS1 ?
		portENTER_CRITICAL_ISR(&btmux);
		SString S;
		S.add( s );
		ser1txbuf.add( S );
		portEXIT_CRITICAL_ISR(&btmux);
	}
	if( serial2_tx.get() & 1 ){         //  XCVario data enabled for ttyS2 ?
		portENTER_CRITICAL_ISR(&btmux);
		SString S;
		S.add( s );
		ser2txbuf.add( S );
		portEXIT_CRITICAL_ISR(&btmux);
	}
}

// Bluetooth logic
void BTSender::packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
	bd_addr_t event_addr;
	uint16_t  mtu;
	char *msg = (char *)packet;
	// ESP_LOGD(FNAME,"BTstack packet type %02x size %d", packet_type, size);
	switch (packet_type) {
	case RFCOMM_DATA_PACKET:
		// ESP_LOGD(FNAME,"Received RFCOMM data on channel id %u, size %u %s", channel, size, packet);
		if( size > RXBUFLEN ) {
			ESP_LOGW(FNAME,"Warning, RX data packet truncated len=%d, max=%d", size, RXBUFLEN );
			size = RXBUFLEN;
		}
		ESP_LOGD(FNAME,"BT RFCOMM RX (CH %u), size %u", channel, size );
		ESP_LOG_BUFFER_HEXDUMP(FNAME,msg,size, ESP_LOG_DEBUG);
		if( strncmp( msg, "!g,", 3 )  == 0 ) {
			ESP_LOGD(FNAME,"Matched a Borgelt command");
			OpenVario::parseNMEA( msg );
		}
		if( serial1_tx.get() & 2 ){  // Serial data TX from bluetooth enabled ?
			portENTER_CRITICAL_ISR(&btmux);
			SString s;
			s.addl( msg, size);
			ser1txbuf.add( s );
			portEXIT_CRITICAL_ISR(&btmux);
			ESP_LOGD(FNAME,"BT -> Serial 1 TX %d bytes", size );

		}
		if(serial2_tx.get() & 2){
			portENTER_CRITICAL_ISR(&btmux);
			SString s;
			s.addl( msg, size);
			ser2txbuf.add( s );
			portEXIT_CRITICAL_ISR(&btmux);
		}
		break;

	case HCI_EVENT_PACKET:
		switch (hci_event_packet_get_type(packet)) {

		case BTSTACK_EVENT_STATE:
			if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
				ESP_LOGD(FNAME,"BTstack is up and running");
				bluetooth_up = true;
			}
			break;

		case HCI_EVENT_PIN_CODE_REQUEST:
			// inform about pin code request
			ESP_LOGD(FNAME,"Pin code request - using '0000'\n\r");
			reverse_bd_addr(&packet[2], event_addr);
			hci_send_cmd(&hci_pin_code_request_reply, &event_addr, 4, "0000");
			break;

		case RFCOMM_EVENT_INCOMING_CONNECTION:
			// data: event (8), len(8), address(48), channel (8), rfcomm_cid (16)
			rfcomm_event_incoming_connection_get_bd_addr(packet, event_addr);
			rfcomm_channel_nr = rfcomm_event_incoming_connection_get_server_channel(packet);
			rfcomm_channel_id = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
			ESP_LOGD(FNAME,"RFCOMM channel %u requested for %s", rfcomm_channel_nr, bd_addr_to_str(event_addr));
			rfcomm_accept_connection(rfcomm_channel_id);
			break;

		case RFCOMM_EVENT_CHANNEL_OPENED:
			// data: event(8), len(8), status (8), address (48), server channel(8), rfcomm_cid(16), max frame size(16)
			if (rfcomm_event_channel_opened_get_status(packet)) {
				ESP_LOGE(FNAME,"RFCOMM channel open failed, status %u", rfcomm_event_channel_opened_get_status(packet));
			} else {
				rfcomm_channel_id = rfcomm_event_channel_opened_get_rfcomm_cid(packet);
				mtu = rfcomm_event_channel_opened_get_max_frame_size(packet);
				ESP_LOGD(FNAME,"RFCOMM channel open succeeded. New RFCOMM Channel ID %u, max frame size %u", rfcomm_channel_id, mtu);
			}
			break;

		case RFCOMM_EVENT_CHANNEL_CLOSED:
			rfcomm_channel_id = 0;
			break;


		default:
			// ESP_LOGD(FNAME,"RFCOMM ignored packet event %u size: %d", hci_event_packet_get_type(packet), size );
			break;
		}

	}
};


// #define FLARM_SIM
#ifdef FLARM_SIM
char *flarm[] = {
		"$PFLAA,0,11461,-9272,1436,1,AFFFFF,51,0,257,0.0,0*7A\n",
		"$PFLAA,0,2784,3437,216,1,AFFFFE,141,0,77,0.0,0*56\n",
		"$PFLAA,1,-1375,1113,64,1,AFFFFD,231,0,30,0.0,0*43\n",
		"$PFLAA,0,-3158,3839,1390,1,A858F3,302,0,123,-12.4,0*6B\n",
		"$GPRMC,084854.40,A,44xx.xxx38,N,093xx.xxx15,W,0.0,0.0,300116,,,D*43\n",
		"$PFLAA,0,11621,-9071,1437,1,AFFFFF,52,0,257,0.0,0*7F\n",
		"$PFLAA,0,2724,3485,218,1,AFFFFE,142,0,77,0.0,0*58\n",
		"$PFLAA,1,-1394,1089,65,1,AFFFFD,232,0,30,0.0,0*4C\n",
		"$PFLAA,0,-3158,3839,1384,1,A858F3,302,0,123,-12.4,0*6E\n",
		"$GPRMC,084855.40,A,44xx.xxx38,N,093xx.xxx15,W,0.0,0.0,300116,,,D*42\n",
		"$GPGGA,152011.934,4850.555,N,00839.186,E,1,12,1.0,0.0,M,0.0,M,,*67\n",
		"$GPGSA,A,3,01,02,03,04,05,06,07,08,09,10,11,12,1.0,1.0,1.0*30\n",
		"$GPRMC,152949.571,A,4846.973,N,00843.677,E,,,220620,000.0,W*75\n"

};
#endif


/* Note that the standard NMEA 0183 baud rate is only 4.8 kBaud.
Nevertheless, a lot of NMEA-compatible devices can properly work with
higher transmission speeds, especially at 9.6 and 19.2 kBaud.
As any sentence can consist of 82 characters maximum with 10 bit each (including start and stop bit),
any sentence might take up to 171 ms (at 4.8k Baud), 85 ms (at 9.6 kBaud) or 43 ms (at 19.2 kBaud).
This limits the overall channel capacity to 5 sentences per second (at 4.8k Baud), 11 msg/s (at 9.6 kBaud) or 23 msg/s (at 19.2 kBaud).
If  too  many  sentences  are  produced  with  regard  to  the  available  transmission  speed,
some sentences might be lost or truncated.

 */

int btick = 0;
int numrxp1 = 0;

// Serial Handler
void serialHandler1(void *pvParameters){
	SString serialRx;
#ifdef FLARM_SIM
	int i=0;
#endif
	while(1) {
		// TickType_t xLastWakeTime = xTaskGetTickCount();
#ifdef FLARM_SIM
		portENTER_CRITICAL_ISR(&btmux);
		btbuf.add( flarm[i++] );
		portEXIT_CRITICAL_ISR(&btmux);
		// ESP_LOGD(FNAME,"Serial TX: %s",  flarm[i-1] );
		if(i>12)
			i=0;
#endif
		// Serial Interface tty1 send
		if ( !ser1txbuf.isEmpty() && Serial1.availableForWrite() ){
			ESP_LOGD(FNAME,"Serial Data and avail");
			SString s;
			portENTER_CRITICAL_ISR(&btmux);
			ser1txbuf.pull(&s);
			portEXIT_CRITICAL_ISR(&btmux);
			ESP_LOGD(FNAME,"Serial 2 TX len: %d bytes", s.length() );
			ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_INFO);
			int wr = Serial1.write( s.c_str(), s.length() );
			ESP_LOGD(FNAME,"Serial 2 TX written: %d", wr);
		}
		serialRx.clear();
		int numread=0;
		int num = Serial1.available();
		if( num > 0 )
			ESP_LOGD(FNAME,"Serial 1 RX avail: %d bytes, prev: %d", num, numrxp1 );
		if( num > 0 && ( num == numrxp1 || num > 250 ) )
		{
			// ESP_LOGD(FNAME,"Serial 1 RX avail %d bytes", num );
			numread = Serial1.read( rxBuffer, RXBUFLEN );
			if( numread != num )
				ESP_LOGW(FNAME,"Serial 1 RX read WARNING, avail %d != read %d", num, numread);
			numrxp1 = 0;
			ESP_LOGD(FNAME,"Serial 1 RX read %d", numread );
			// ESP_LOG_BUFFER_HEXDUMP(FNAME,rxBuffer,numread, ESP_LOG_INFO);
			serialRx.addl( rxBuffer, numread );
		}
		else
			numrxp1 = num;
		// ESP_LOGD(FNAME,"serial RX len: %d", serialRx.length() );
		if (serialRx.length() > 0) {
			ESP_LOGD(FNAME,"Serial 1 RX len: %d bytes, Q:%d", serialRx.length(), btbuf.isFull() );
			ESP_LOG_BUFFER_HEXDUMP(FNAME,serialRx.c_str(),serialRx.length(), ESP_LOG_DEBUG);
			// Send to BT device
			if( !btbuf.isFull() &&  (serial1_tx.get() & 2) ) {
				ESP_LOGD(FNAME,"Send to BT device %d bytes", serialRx.length() );
				portENTER_CRITICAL_ISR(&btmux);
				btbuf.add( serialRx );
				portEXIT_CRITICAL_ISR(&btmux);
			}
			else
				ESP_LOGD(FNAME,"serial 1 RX bt buffer overrun");

			if( !ser1txbuf.isFull() && serial1_rxloop.get() & 1 ) {
				ESP_LOGD(FNAME,"Send to ttyS1 TX device RX looped %d bytes", serialRx.length() );
				portENTER_CRITICAL_ISR(&btmux);
				ser1txbuf.add( serialRx );
				portEXIT_CRITICAL_ISR(&btmux);
			}

			if( !ser2txbuf.isFull() && serial1_rxloop.get() & 2 ) {
				ESP_LOGD(FNAME,"Send to ttyS2 TX device RX looped %d bytes", serialRx.length() );
				portENTER_CRITICAL_ISR(&btmux);
				ser2txbuf.add( serialRx );
				portEXIT_CRITICAL_ISR(&btmux);
			}

		}
		if (rfcomm_channel_id ){
			if ( !btbuf.isEmpty() ){
				ESP_LOGD(FNAME,"have data for bluetooth");
				if (rfcomm_can_send_packet_now(rfcomm_channel_id)){
					ESP_LOGD(FNAME,"can send now to bluetooth");
					SString s;
					portENTER_CRITICAL_ISR(&btmux);
					btbuf.pull(&s);
					portEXIT_CRITICAL_ISR(&btmux);
					int err = rfcomm_send(rfcomm_channel_id, (uint8_t*) s.c_str(), s.length() );
					if (err) {
						ESP_LOGE(FNAME,"rfcomm_send -> error %d", err);
					}
					else
					{
						ESP_LOGD(FNAME,"RFCOMM TX, size %u", s.length() );
						ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_DEBUG);
					}
				}
			}

		}
		vTaskDelay( 2/portTICK_PERIOD_MS );
		if( !(btick++ % 8) )
			vTaskDelay( 10/portTICK_PERIOD_MS );
	}
}

int btick2 = 0;
int numrxp2 = 0;

void serialHandler2(void *pvParameters){
	while(1) {
		SString serialRx;
		if ( !ser2txbuf.isEmpty() && Serial2.availableForWrite() ){
			ESP_LOGD(FNAME,"Serial Data and avail");
			SString s;
			portENTER_CRITICAL_ISR(&btmux);
			ser2txbuf.pull(&s);
			portEXIT_CRITICAL_ISR(&btmux);
			ESP_LOGD(FNAME,"Serial 2 TX len: %d bytes", s.length() );
			ESP_LOG_BUFFER_HEXDUMP(FNAME,s.c_str(),s.length(), ESP_LOG_DEBUG);
			int wr = Serial2.write( s.c_str(), s.length() );
			ESP_LOGD(FNAME,"Serial 2 TX written: %d", wr);
		}
		serialRx.clear();
		int numread=0;
		int num = Serial2.available();
		if( num > 0 )
			ESP_LOGD(FNAME,"Serial 2 RX avail: %d bytes, prev: %d", num, numrxp1 );
		if( num > 0 && ( num == numrxp2 || num > 250 ) )
		{
			// ESP_LOGD(FNAME,"Serial 2 RX avail %d bytes", num );
			numread = Serial2.read( rxBuffer, RXBUFLEN );
			if( numread != num )
				ESP_LOGW(FNAME,"Serial 2 RX read WARNING, avail %d != read %d", num, numread);
			numrxp2 = 0;
			ESP_LOGD(FNAME,"Serial 2 RX read %d", numread );
			ESP_LOG_BUFFER_HEXDUMP(FNAME,rxBuffer,numread, ESP_LOG_DEBUG);
			serialRx.addl( rxBuffer, numread );
		}
		else
			numrxp2 = num;

		// ESP_LOGD(FNAME,"serial 2 RX len: %d", serialRx.length() );
		if (serialRx.length() > 0) {
			ESP_LOGD(FNAME,"Serial 2 RX len: %d bytes, Q:%d", serialRx.length(), btbuf.isFull() );
			ESP_LOG_BUFFER_HEXDUMP(FNAME,serialRx.c_str(),serialRx.length(), ESP_LOG_DEBUG);
			// Send to BT device
			if( !btbuf.isFull() &&  (serial2_tx.get() & 2) ) {
				ESP_LOGD(FNAME,"Send to BT device %d bytes", serialRx.length() );
				portENTER_CRITICAL_ISR(&btmux);
				btbuf.add( serialRx );
				portEXIT_CRITICAL_ISR(&btmux);
			}
			else
				ESP_LOGD(FNAME,"serial 2 RX bt buffer overrun");

		}
		vTaskDelay( 3/portTICK_PERIOD_MS );
		if( !(btick2++ % 8) )
			vTaskDelay( 10/portTICK_PERIOD_MS );
	}
}



void BTSender::begin(){
	ESP_LOGI(FNAME,"BTSender::begin()" );

	hci_dump_enable_log_level( ESP_LOG_INFO, 1 );
	hci_dump_enable_log_level( ESP_LOG_ERROR, 1 );
	hci_dump_enable_log_level( ESP_LOG_WARN, 1 );
	hci_dump_enable_log_level( ESP_LOG_DEBUG, 0 );
	if( blue_enable.get() ) {
		l2cap_init();
		le_device_db_init();   // new try 28-09
		rfcomm_init();
		// register for HCI events
		hci_event_callback_registration.callback = &packet_handler;
		hci_add_event_handler(&hci_event_callback_registration);
		rfcomm_register_service(packet_handler, rfcomm_channel_nr, 500); // reserved channel, mtu=100
		memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
		spp_create_sdp_record(spp_service_buffer, 0x10001, RFCOMM_SERVER_CHANNEL, "SPP Counter");
		sdp_register_service(spp_service_buffer);
		ESP_LOGD(FNAME,"SDP service record size: %u", de_get_len(spp_service_buffer));
		gap_discoverable_control(1);
		gap_set_local_name(SetupCommon::getID());
		sdp_init();
		hci_power_control(HCI_POWER_ON);
	}
	if( serial1_speed.get() != 0  || blue_enable.get() ){
		int baudrate = baud[serial1_speed.get()];
		if( baudrate != 0 ) {
			ESP_LOGI(FNAME,"Serial Interface ttyS1 enabled with serial speed: %d baud: %d tx_inv: %d rx_inv: %d",  serial1_speed.get(), baud[serial1_speed.get()], serial1_tx_inverted.get(), serial1_rx_inverted.get() );
			Serial1.begin(baudrate,SERIAL_8N1,16,17, serial1_rx_inverted.get(), serial1_tx_inverted.get());   //  IO16: RXD2,  IO17: TXD2
			Serial1.setRxBufferSize(256);
		}
		// need this for bluetooth
		xTaskCreatePinnedToCore(&serialHandler1, "serialHandler1", 4096, NULL, 25, 0, 0);
	}
	if( serial2_speed.get() != 0  && hardwareRevision >= 3 ){
		ESP_LOGI(FNAME,"Serial Interface ttyS2 enabled with serial speed: %d baud: %d tx_inv: %d rx_inv: %d",  serial2_speed.get(), baud[serial2_speed.get()], serial2_tx_inverted.get(), serial2_rx_inverted.get() );
		if( serial2_pins_twisted.get() )
			Serial2.begin(baud[serial2_speed.get()],SERIAL_8N1,4,18, serial2_rx_inverted.get(), serial2_tx_inverted.get());   //  IO16: RXD2,  IO17: TXD2
		else
			Serial2.begin(baud[serial2_speed.get()],SERIAL_8N1,18,4, serial2_rx_inverted.get(), serial2_tx_inverted.get());   //  IO16: RXD2,  IO17: TXD2

		Serial2.setRxBufferSize(256);
		xTaskCreatePinnedToCore(&serialHandler2, "serialHandler2", 4096, NULL, 23, 0, 0);
	}

};


