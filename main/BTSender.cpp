
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


extern xSemaphoreHandle nvMutex;

const int baud[] = { 0, 4800, 9600, 19200, 38400, 57600, 115200 };


#define QUEUE_SIZE 40
RingBufCPP<SString, QUEUE_SIZE> btbuf;

#define RXBUFLEN 250
char rxBuffer[RXBUFLEN];
int BTSender::i=0;

#define RFCOMM_SERVER_CHANNEL 1
#define HEARTBEAT_PERIOD_MS 50


bool      BTSender::bluetooth_up = false;
uint8_t   BTSender::rfcomm_channel_nr = 1;
uint16_t  BTSender::rfcomm_channel_id = 0;
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
	ESP_LOGD( FNAME,"Vario NMEA %s",s);
	if ( !btbuf.isFull() && blue_enable.get() ) {
		portENTER_CRITICAL_ISR(&btmux);
		btbuf.add( s );
		portEXIT_CRITICAL_ISR(&btmux);
	}
	if( serial2_tx.get() & 1 ) {  // send OpenVario Data on serial interface TX
		ESP_LOGI(FNAME,"Serial TX (CH %u), size %u", rfcomm_channel_id, strlen(s) );
		xSemaphoreTake(nvMutex,portMAX_DELAY );
		Serial2.print(s);
		xSemaphoreGive(nvMutex);
	}
}

void BTSender::heartbeat_handler(struct btstack_timer_source *ts){
	if (rfcomm_channel_id ){
        // ESP_LOGI(FNAME,"HBH..");
		if ( !btbuf.isEmpty() ){
			// ESP_LOGI(FNAME,"HBH..not empty");
			if (rfcomm_can_send_packet_now(rfcomm_channel_id)){
				// ESP_LOGI(FNAME,"HBH..can send now");
				SString s;
				portENTER_CRITICAL_ISR(&btmux);
			    btbuf.pull(&s);
			    portEXIT_CRITICAL_ISR(&btmux);
				int err = rfcomm_send(rfcomm_channel_id, (uint8_t*) s.c_str(), s.length() );
				if (err) {
					ESP_LOGE(FNAME,"rfcomm_send -> error %d", err);
				}
			}
		}

	}
	btstack_run_loop_set_timer(ts, HEARTBEAT_PERIOD_MS);
	btstack_run_loop_add_timer(ts);
}

// Bluetooth logic
void BTSender::packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
	bd_addr_t event_addr;
	uint16_t  mtu;
	int pos;
	// ESP_LOGI(FNAME,"BTstack packet type %02x size %d", packet_type, size);
	switch (packet_type) {
	case RFCOMM_DATA_PACKET:
		// ESP_LOGI(FNAME,"Received RFCOMM data on channel id %u, size %u %s", channel, size, packet);
		if( size > RXBUFLEN ) {
			ESP_LOGW(FNAME,"Warning, RX data packet truncated len=%d, max=%d", size, RXBUFLEN );
			size = RXBUFLEN;
		}
		memcpy( rxBuffer, packet, size );
		rxBuffer[size] = 0;
		ESP_LOGD(FNAME,"RFCOMM RX (CH %u), size %u", channel, size );
		ESP_LOG_BUFFER_HEXDUMP(FNAME,rxBuffer,size, ESP_LOG_INFO);
		// delay(100);
	    if( strncmp( rxBuffer, "!g,", 3 )  == 0 ) {
	    	ESP_LOGI(FNAME,"Matched a Borgelt command");
	    	OpenVario::parseNMEA( rxBuffer );
	    }
	    else {
	    	pos=0;
	    	if( serial2_tx.get() & 2 ){  // Serial TX data from bluetooth
	    		ESP_LOGI(FNAME,"Serial TX len: %d", size);
	    		ESP_LOG_BUFFER_HEXDUMP(FNAME,rxBuffer,size, ESP_LOG_DEBUG);
	    		//while(pos < size) {
	    		rxBuffer[size] = 0;
	    		Serial2.print( rxBuffer );
	    		//	pos++;
	    		//}
	    	}
	    }
		break;

	case HCI_EVENT_PACKET:
		switch (hci_event_packet_get_type(packet)) {

		case BTSTACK_EVENT_STATE:
			if (btstack_event_state_get_state(packet) == HCI_STATE_WORKING){
				ESP_LOGI(FNAME,"BTstack is up and running");
				bluetooth_up = true;
			}
			break;

		case HCI_EVENT_PIN_CODE_REQUEST:
			// inform about pin code request
			ESP_LOGI(FNAME,"Pin code request - using '0000'\n\r");
			reverse_bd_addr(&packet[2], event_addr);
			hci_send_cmd(&hci_pin_code_request_reply, &event_addr, 4, "0000");
			break;

		case RFCOMM_EVENT_INCOMING_CONNECTION:
			// data: event (8), len(8), address(48), channel (8), rfcomm_cid (16)
			rfcomm_event_incoming_connection_get_bd_addr(packet, event_addr);
			rfcomm_channel_nr = rfcomm_event_incoming_connection_get_server_channel(packet);
			rfcomm_channel_id = rfcomm_event_incoming_connection_get_rfcomm_cid(packet);
			ESP_LOGI(FNAME,"RFCOMM channel %u requested for %s", rfcomm_channel_nr, bd_addr_to_str(event_addr));
			rfcomm_accept_connection(rfcomm_channel_id);
			break;

		case RFCOMM_EVENT_CHANNEL_OPENED:
			// data: event(8), len(8), status (8), address (48), server channel(8), rfcomm_cid(16), max frame size(16)
			if (rfcomm_event_channel_opened_get_status(packet)) {
				ESP_LOGE(FNAME,"RFCOMM channel open failed, status %u", rfcomm_event_channel_opened_get_status(packet));
			} else {
				rfcomm_channel_id = rfcomm_event_channel_opened_get_rfcomm_cid(packet);
				mtu = rfcomm_event_channel_opened_get_max_frame_size(packet);
				ESP_LOGI(FNAME,"RFCOMM channel open succeeded. New RFCOMM Channel ID %u, max frame size %u", rfcomm_channel_id, mtu);
			}
			break;

		case RFCOMM_EVENT_CHANNEL_CLOSED:
			rfcomm_channel_id = 0;
			break;


		default:
			// ESP_LOGI(FNAME,"RFCOMM ignored packet event %u size: %d", hci_event_packet_get_type(packet), size );
			break;
		}

	}
};



void BTSender::one_shot_timer_setup(void){
	// set one-shot timer
	heartbeat.process = &heartbeat_handler;
	btstack_run_loop_set_timer(&heartbeat, HEARTBEAT_PERIOD_MS);
	btstack_run_loop_add_timer(&heartbeat);
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

void btBridge(void *pvParameters){
	SString readString;
#ifdef FLARM_SIM
	int i=0;
#endif
	while(1) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
#ifdef FLARM_SIM
		portENTER_CRITICAL_ISR(&btmux);
		btbuf.add( flarm[i++] );
		portEXIT_CRITICAL_ISR(&btmux);
		// ESP_LOGI(FNAME,"Serial TX: %s",  flarm[i-1] );
		if(i>12)
			i=0;
#endif
		int timeout=0;

		while( (readString.length() < SSTRLEN) && (timeout<3) ) {
			xSemaphoreTake(nvMutex,portMAX_DELAY );
			bool avail = Serial2.available();
			xSemaphoreGive(nvMutex);
			if(  avail ) {
				xSemaphoreTake(nvMutex,portMAX_DELAY );
				char c = Serial2.read();
				xSemaphoreGive(nvMutex);
				readString.add(c);
				if( c == '\n' )
					timeout = 100;
			}
			else
			{
				timeout++;
				vTaskDelay( 2/portTICK_PERIOD_MS );
			}
		}

		if (readString.length() > 0) {
			ESP_LOGD(FNAME,"Serial RX len: %d bytes", readString.length() );
			// ESP_LOG_BUFFER_HEXDUMP(FNAME,readString.c_str(),readString.length(), ESP_LOG_INFO);
			if( !btbuf.isFull() ) {
				portENTER_CRITICAL_ISR(&btmux);
				btbuf.add( readString );
				portEXIT_CRITICAL_ISR(&btmux);
			}
			if( serial2_tx.get() & 1 ) {  // resend serial RX (e.g. FLARM) on TX
				ESP_LOGI(FNAME,"Serial TX size %u", readString.length() );
				xSemaphoreTake(nvMutex,portMAX_DELAY );
				Serial2.print(readString.c_str());
				xSemaphoreGive(nvMutex);
			}
			readString.clear();
		}
		if( (btick++ % 10) == 0)
			vTaskDelay( 10/portTICK_PERIOD_MS );
	}
}


void BTSender::begin(){
	ESP_LOGI(FNAME,"BTSender::begin()" );

	hci_dump_enable_log_level( ESP_LOG_INFO, 0 );
	hci_dump_enable_log_level( ESP_LOG_ERROR, 0 );
	hci_dump_enable_log_level( ESP_LOG_WARN, 0 );
	hci_dump_enable_log_level( ESP_LOG_DEBUG, 0 );
	if( blue_enable.get() ) {
		l2cap_init();
		le_device_db_init();   // new try 28-09
		rfcomm_init();
		one_shot_timer_setup();
		// register for HCI events
		hci_event_callback_registration.callback = &packet_handler;
		hci_add_event_handler(&hci_event_callback_registration);
		rfcomm_register_service(packet_handler, rfcomm_channel_nr, 500); // reserved channel, mtu=100
		memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
		spp_create_sdp_record(spp_service_buffer, 0x10001, RFCOMM_SERVER_CHANNEL, "SPP Counter");
		sdp_register_service(spp_service_buffer);
		ESP_LOGI(FNAME,"SDP service record size: %u", de_get_len(spp_service_buffer));
		gap_discoverable_control(1);
		gap_set_local_name(SetupCommon::getID());
		sdp_init();
		hci_power_control(HCI_POWER_ON);
	}
	if( (serial2_speed.get() != 0) && (serial2_rxloop.get() || serial2_tx.get() ) ){
    	ESP_LOGI(FNAME,"Serial TX or Bridge enabled with serial speed: %d baud: %d tx_inv: %d rx_inv: %d",  serial2_speed.get(), baud[serial2_speed.get()], serial2_tx_inverted.get(), serial2_rx_inverted.get() );
    	Serial2.begin(baud[serial2_speed.get()],SERIAL_8N1,16,17);   //  IO16: RXD2,  IO17: TXD2
    	if( serial2_rx_inverted.get() || serial2_tx_inverted.get() ) {
    		uart_signal_inv_t sigrx=UART_SIGNAL_INV_DISABLE;
    		uart_signal_inv_t sigtx=UART_SIGNAL_INV_DISABLE;
    		if( serial2_rx_inverted.get() )
    			sigrx = UART_SIGNAL_RXD_INV;
    		if( serial2_tx_inverted.get() )
    			sigtx = UART_SIGNAL_TXD_INV;
    		ESP_LOGI(FNAME,"Set UART Inversion Mask (4=RX | 32=TX): %d", sigrx | sigtx  );
    		uart_set_line_inverse(2, sigrx | sigtx );
    	}
    }
	if( serial2_rxloop.get() && serial2_speed.get() ) {
		ESP_LOGI(FNAME,"Serial Bluetooth bridge enabled ");
		xTaskCreatePinnedToCore(&btBridge, "btBridge", 4096, NULL, 30, 0, 0);
	}
};


