
#include <esp_log.h>
#include "BTSender.h"
#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include <freertos/semphr.h>
#include <algorithm>
#include "RingBufCPP.h"
#include <driver/uart.h>
#include "OpenVario.h"
#include <logdef.h>
#include "Switch.h"
#include "sensor.h"
#include "Router.h"



bool      BTSender::bluetooth_up = false;
uint8_t   BTSender::rfcomm_channel_nr = 1;
uint16_t  rfcomm_channel_id = 0;
uint8_t   BTSender::spp_service_buffer[SPP_SERVICE_BUFFER_SIZE]; // 500
btstack_packet_callback_registration_t BTSender::hci_event_callback_registration;
void ( * BTSender::_callback)(char * rx, uint16_t len);

int BTSender::queueFull() {
	if( !blue_enable.get() )
		return 1;
	int ret = 0;
	if(bt_tx_q.isFull())
		ret=1;
	return ret;
}


// Formerly there was only BT, tbd: rename to XCVario sender
void BTSender::send(char * s){
	// ESP_LOGI( FNAME,"XCVario message %s",s);
	SString xcv( s );
	ESP_LOGI(FNAME,"Received %d bytes from XCV", xcv.length() );
	routeXCV( xcv );
}

// BTStack Bluetooth Receiver
void BTSender::packet_handler (uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size){
	bd_addr_t event_addr;
	uint16_t  mtu;
	char *msg = (char *)packet;
	// ESP_LOGD(FNAME,"BTstack packet type %02x size %d", packet_type, size);
	switch (packet_type) {

	case RFCOMM_DATA_PACKET:
	{
		// ESP_LOGD(FNAME,"Received RFCOMM data on channel id %u, size %u %s", channel, size, packet);
		if( size >= SSTRLEN ) {
			ESP_LOGW(FNAME,"Warning, RX data packet truncated len=%d, max=%d", size, SSTRLEN );
			size = SSTRLEN;
		}
		ESP_LOGI(FNAME,"BT RFCOMM RX (CH %u), size %u", channel, size );
		SString s;
		pullMsg( bt_rx_q , s );
		s.append( msg, size );
		forwardMsg( s, bt_rx_q );
		ESP_LOGD(FNAME,"BT received %d bytes", size );
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

/* Note that the standard NMEA 0183 baud rate is only 4.8 kBaud.
Nevertheless, a lot of NMEA-compatible devices can properly work with
higher transmission speeds, especially at 9.6 and 19.2 kBaud.
As any sentence can consist of 82 characters maximum with 10 bit each (including start and stop bit),
any sentence might take up to 171 ms (at 4.8k Baud), 85 ms (at 9.6 kBaud) or 43 ms (at 19.2 kBaud).
This limits the overall channel capacity to 5 sentences per second (at 4.8k Baud), 11 msg/s (at 9.6 kBaud) or 23 msg/s (at 19.2 kBaud).
If  too  many  sentences  are  produced  with  regard  to  the  available  transmission  speed,
some sentences might be lost or truncated.

 */

void BTSender::transmit( SString &s ){
	if (rfcomm_channel_id ){
		if ( pullMsg( bt_tx_q, s ) ){
			ESP_LOGD(FNAME,"have data for bluetooth");
			if (rfcomm_can_send_packet_now(rfcomm_channel_id)){
				ESP_LOGD(FNAME,"can send now to bluetooth");
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
}

void BTSender::begin(){
	ESP_LOGI(FNAME,"BTSender::begin()" );
	if( blue_enable.get() ) {
		hci_dump_enable_log_level( ESP_LOG_INFO, 1 );
		hci_dump_enable_log_level( ESP_LOG_ERROR, 1 );
		hci_dump_enable_log_level( ESP_LOG_WARN, 1 );
		hci_dump_enable_log_level( ESP_LOG_DEBUG, 0 );
		l2cap_init();
		le_device_db_init();
		rfcomm_init();
		// register for HCI events
		hci_event_callback_registration.callback = &packet_handler;
		hci_add_event_handler(&hci_event_callback_registration);
		rfcomm_register_service(packet_handler, rfcomm_channel_nr, SPP_SERVICE_BUFFER_SIZE); // reserved channel, mtu=100
		memset(spp_service_buffer, 0, sizeof(spp_service_buffer));
		spp_create_sdp_record(spp_service_buffer, 0x10001, RFCOMM_SERVER_CHANNEL, "SPP Counter");
		sdp_register_service(spp_service_buffer);
		ESP_LOGD(FNAME,"SDP service record size: %u", de_get_len(spp_service_buffer));
		gap_discoverable_control(1);
		gap_set_local_name(SetupCommon::getID());
		sdp_init();
		hci_power_control(HCI_POWER_ON);
	}
	else
		ESP_LOGI(FNAME,"BTSender::begin() Nothing done, BT disabled! " );
};


