#include "Router.h"
#include <esp_log.h>
#include "BTSender.h"
#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include <freertos/semphr.h>
#include "Protocols.h"
#include <logdef.h>
#include "Switch.h"
#include "sensor.h"
#include "Flarm.h"
#include "Serial.h"
#include "UbloxGNSSdecode.h"

RingBufCPP<SString, QUEUE_SIZE> wl_vario_tx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_flarm_tx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_aux_tx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_vario_rx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_flarm_rx_q;
RingBufCPP<SString, QUEUE_SIZE> wl_aux_rx_q;

RingBufCPP<SString, QUEUE_SIZE> bt_tx_q;
RingBufCPP<SString, QUEUE_SIZE> bt_rx_q;

RingBufCPP<SString, QUEUE_SIZE> s1_tx_q;
RingBufCPP<SString, QUEUE_SIZE> s2_tx_q;

RingBufCPP<SString, QUEUE_SIZE> s1_rx_q;
RingBufCPP<SString, QUEUE_SIZE> s2_rx_q;

RingBufCPP<SString, QUEUE_SIZE> xcv_rx_q;

RingBufCPP<SString, QUEUE_SIZE> can_rx_q;
RingBufCPP<SString, QUEUE_SIZE> can_tx_q;

portMUX_TYPE btmux = portMUX_INITIALIZER_UNLOCKED;

// Utility methods to push and pull data into/from queues

// checks if Queue is full, otherwise appends SString
bool Router::forwardMsg( SString &s, RingBufCPP<SString, QUEUE_SIZE>& q, bool nmea ){
	// ESP_LOGI(FNAME,"forwardMsg() len: %d, queueElem: %d queueFull:%d", s.length(), q.numElements(), q.isFull() );
	if( !q.isFull() ) {
		portENTER_CRITICAL_ISR(&btmux);
		q.add( s );
		portEXIT_CRITICAL_ISR(&btmux);
		return true;
	}
	// ESP_LOGW(FNAME,"+++ WARNING +++ dropped msg len: %d, queueElem: %d", s.length(), q.numElements() );
	return false;
}


void Router::clearQueue( RingBufCPP<SString, QUEUE_SIZE>& q ){
	SString dummy;
	while( pullMsg( q, dummy ) ){
		// ESP_LOGI(FNAME,"clear %s", dummy.c_str() );
	}
}

// gets last message from ringbuffer FIFO
bool Router::pullMsg( RingBufCPP<SString, QUEUE_SIZE>& q, SString& s ){
	if( !q.isEmpty() ){
		portENTER_CRITICAL_ISR(&btmux);
		q.pull( s );
		portEXIT_CRITICAL_ISR(&btmux);
		return true;
	}
	return false;
}

int Router::pullMsg( RingBufCPP<SString, QUEUE_SIZE>& q, char *block ){
	int size = 0;
	if( !q.isEmpty() ){
		SString s;
		portENTER_CRITICAL_ISR(&btmux);
		q.pull(  s );
		portEXIT_CRITICAL_ISR(&btmux);
		size = s.length();
		memcpy( block, s.c_str(), size );
	}
	block[size]=0;
	return size;
}

int Router::pullBlock( RingBufCPP<SString, QUEUE_SIZE>& q, char *block, int size ){
	int len = 0;
	while( !q.isEmpty() ){
		SString s;
		portENTER_CRITICAL_ISR(&btmux);
		q.pull(  s );
		portEXIT_CRITICAL_ISR(&btmux);
		memcpy( block+len, s.c_str(), s.length() );
		len += s.length();
		if( (len + SSTRLEN) > size )
			break;
	}
	block[len]=0;
	return len;
}

// XCVario Router
void Router::sendXCV(char * s){
	// ESP_LOGI( FNAME,"XCVario message %s",s);
	if(  !Flarm::bincom  ){
		SString xcv( s );
		if( forwardMsg( xcv, xcv_rx_q ) ){
			// ESP_LOGI(FNAME,"Received %d bytes from XCV", xcv.length() );
		}
	}
}

// XCVario Router
void Router::sendAUX(char * s){
	// ESP_LOGI( FNAME,"AUX message %s",s);
	if(  !Flarm::bincom  ){
		SString xcv( s );
		if( forwardMsg( xcv, wl_aux_tx_q ) ){
			// ESP_LOGI(FNAME,"Sent %d bytes to 8882 port", xcv.length() );
		}
	}
}

// Route XCVario messages
void Router::routeXCV(){
	SString xcv;
	while( pullMsg( xcv_rx_q, xcv ) ){
		if ( strncmp( xcv.c_str(), "!xs", 3 ) != 0 ){  // !xs messages are XCV specific and must not go to BT,WiFi or serial Navi's
			if( wireless == WL_BLUETOOTH ) {
				if( forwardMsg( xcv, bt_tx_q ) ){
					// ESP_LOGI(FNAME,"Send to BT device, XCV received %d bytes", xcv.length() );
				}
			}
			if( wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE  ){
				if( forwardMsg( xcv, wl_vario_tx_q ) ){
					// ESP_LOGI(FNAME,"Send to WLAN port 8880, XCV %d bytes", xcv.length() );
				}
			}
			if( (serial1_tx.get() & RT_XCVARIO) && serial1_speed.get() ){
				if( forwardMsg( xcv, s1_tx_q ) ){
					Serial::setRxTxNotifier( TX1_REQ );
					// ESP_LOGI(FNAME,"X Send to S1 device, XCV %d bytes", xcv.length() );
				}
			}
			if( (serial2_tx.get() & RT_XCVARIO) && serial2_speed.get() ){
				if( forwardMsg( xcv, s2_tx_q ) ){
					Serial::setRxTxNotifier( TX2_REQ );
					// ESP_LOGI(FNAME,"Send to S2 device, XCV %d bytes", xcv.length() );
				}
			}
		}
	}
}

// Route messages from serial interface S1
void Router::routeS1(){
	SString s1;
	while( pullMsg( s1_rx_q, s1) ){
		// ESP_LOGI(FNAME,"routeS1 RX %d bytes, Q:%d  B:%d", s1.length(), s1_rx_q.numElements(), Flarm::bincom );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,s1.c_str(),s1.length(), ESP_LOG_INFO);

		if( wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE ){
			if( forwardMsg( s1, wl_flarm_tx_q )){
				// ESP_LOGI(FNAME,"S1 RX bytes %d forward to wl_flarm_tx_q port 8881", s1.length() );
			}
		}
		if( wireless == WL_BLUETOOTH ){
			if( forwardMsg( s1, bt_tx_q )){
				// ESP_LOGI(FNAME,"S1 RX bytes %d forward to bt_tx_q", s1.length() );
			}
		}
		if( serial1_rxloop.get() ){  // only 0=DISABLE | 1=ENABLE
			if( forwardMsg( s1, s1_tx_q )){
				Serial::setRxTxNotifier( TX1_REQ );
				// ESP_LOGI(FNAME,"S1 RX bytes %d looped to s1_tx_q", s1.length() );
			}
		}
		if( (can_tx.get() & RT_XCVARIO) && can_speed.get() ){
			if( forwardMsg( s1, can_tx_q )){
				// ESP_LOGI(FNAME,"S1 RX bytes %d forward to can_tx_q", s1.length() );
			}
		}
		if( (serial2_tx.get() & RT_S1) && serial2_speed.get() ){
			if( forwardMsg( s1, s2_tx_q )){
				Serial::setRxTxNotifier( TX2_REQ );
				// ESP_LOGI(FNAME,"S1 RX bytes %d looped to s2_tx_q", s1.length() );
			}
		}
		Protocols::parseNMEA( s1.c_str() );
	}
}

// Route messages from serial interface S2
void Router::routeS2(){
	SString s2;
	while( pullMsg( s2_rx_q, s2) ){
		// ESP_LOGI(FNAME,"S2 RX len: %d bytes, Q:%d BC:%d", s2.length(), bt_tx_q.isFull(), Flarm::bincom  );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,s2.c_str(),s2.length(), ESP_LOG_INFO);

		if(wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE )
			if( forwardMsg( s2, wl_aux_tx_q )){
				// ESP_LOGI(FNAME,"S2 RX bytes %d forward to wl_aux_tx_q port 8882", s2.length() );
			}
		if( wireless == WL_BLUETOOTH ){
			if( forwardMsg( s2, bt_tx_q )){
				// ESP_LOGI(FNAME,"S2 RX bytes %d forward to bt_tx_q", s2.length() );
			}
		}
		if( (can_tx.get() & RT_XCVARIO) && can_speed.get() ){
			if( forwardMsg( s2, can_tx_q )){
				// ESP_LOGI(FNAME,"S2 RX bytes %d forward to can_tx_q", s2.length() );
			}
		}
		if( (serial1_tx.get() & RT_S1) && serial1_speed.get() ){ // RT_S1 could be renamed to RT_SERIAL
			if( forwardMsg( s2, s1_tx_q )){  // This might connect XCSoar on S2 with Flarm on S1
				Serial::setRxTxNotifier( TX1_REQ );
				// ESP_LOGI(FNAME,"S2 RX bytes %d forward to s1_tx_q", s2.length() );
			}
		}
		Protocols::parseNMEA( s2.c_str() );
	}
}

// route messages from WLAN
void Router::routeWLAN(){
	SString wlmsg;
	if( wireless != WL_DISABLE  ){
		// Route received data from WLAN ports
		while( pullMsg( wl_vario_rx_q, wlmsg) ){
			// ESP_LOGI(FNAME,"From WLAN port 8880 RX NMEA %s", wlmsg.c_str() );
			if( (serial1_tx.get() & RT_WIRELESS)  && serial1_speed.get() ){
				if( forwardMsg( wlmsg, s1_tx_q ) ){
					Serial::setRxTxNotifier( TX1_REQ );
					// ESP_LOGI(FNAME,"Send to  device, TCP port 8880 received %d bytes", wlmsg.length() );
				}
			}
			if( (serial2_tx.get() & RT_WIRELESS) && serial2_speed.get() ){
				if( forwardMsg( wlmsg, s2_tx_q ) ){
					Serial::setRxTxNotifier( TX2_REQ );
					// ESP_LOGI(FNAME,"Send to S2 device, TCP port 8880 received %d bytes", wlmsg.length() );
				}
			}
			Protocols::parseNMEA( wlmsg.c_str() );
		}
		while( pullMsg( wl_flarm_rx_q, wlmsg ) ){
			if( (serial1_tx.get() & RT_WIRELESS) && serial1_speed.get() ){
				if( forwardMsg( wlmsg, s1_tx_q ) ){
					Serial::setRxTxNotifier( TX1_REQ );
					// ESP_LOGI(FNAME,"Send to  device, TCP port 8881 received %d bytes", wlmsg.length() );
				}
			}
			if( (serial2_tx.get() & RT_WIRELESS) && serial2_speed.get() ){
				if( forwardMsg( wlmsg, s2_tx_q ) ){
					Serial::setRxTxNotifier( TX2_REQ );
					// ESP_LOGI(FNAME,"Send to S2 device, TCP port 8881 received %d bytes", wlmsg.length() );
				}
			}
			Protocols::parseNMEA( wlmsg.c_str() );
		}
		while( pullMsg( wl_aux_rx_q, wlmsg ) ){
			if( (serial1_tx.get() & RT_WIRELESS) && serial1_speed.get() ){
				if( forwardMsg( wlmsg, s1_tx_q ) ){
					Serial::setRxTxNotifier( TX1_REQ );
					// ESP_LOGI(FNAME,"Send to  device, TCP port 8882 received %d bytes", wlmsg.length() );
				}
			}
			if( (serial2_tx.get() & RT_WIRELESS) && serial2_speed.get() ){
				if( forwardMsg( wlmsg, s2_tx_q ) ){
					Serial::setRxTxNotifier( TX2_REQ );
					// ESP_LOGI(FNAME,"Send to S2 device, TCP port 8882 received %d bytes", wlmsg.length() );
				}
			}
			Protocols::parseNMEA( wlmsg.c_str() );
		}
	}
}


// route messages from Bluetooth
void Router::routeBT(){
	if( wireless != WL_BLUETOOTH )
		return;
	SString bt;
	while( pullMsg( bt_rx_q, bt ) ){
		// ESP_LOGI(FNAME,"BT RX %s, len: %d bytes", bt.c_str(), bt.length() );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,bt.c_str(),bt.length(), ESP_LOG_INFO);

		if( (serial1_tx.get() & RT_WIRELESS) && serial1_speed.get() ){  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s1_tx_q ) ){
				Serial::setRxTxNotifier( TX1_REQ );
				// ESP_LOGI(FNAME,"Send to S1 device, BT received %d bytes", bt.length() );
			}
		}
		if( (serial2_tx.get() & RT_WIRELESS) && serial2_speed.get() ){  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s2_tx_q ) ){
				Serial::setRxTxNotifier( TX2_REQ );
				// ESP_LOGI(FNAME,"Send to S2 device, BT received %d bytes", bt.length() );
			}
		}
		// always check if it is a command to ourselves
		if( !strncmp( bt.c_str(), "!g,", 3 ) || !strncmp( bt.c_str(), "$g,", 3 ) ) {
			// ESP_LOGI(FNAME,"BT RX Matched a Borgelt command %s", bt.c_str() );
			Protocols::parseNMEA( bt.c_str() );
		}
		if( SetupCommon::isClient() && (serial2_tx.get() & RT_WIRELESS) ){
			// ESP_LOGI(FNAME,"Send to CAN bus, BT received %d bytes", bt.length() );
			CAN->sendNMEA( bt );
		}
		bt.clear();
	}
}

// route messages from master or can via CAN or Wifi
void Router::routeCAN(){
	SString can;
	while( pullMsg( can_rx_q, can ) ){
		// ESP_LOGI(FNAME,"can received %d bytes %s", can.length(), can.c_str());
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,can.c_str(),can.length(), ESP_LOG_INFO);
		if (strncmp(can.c_str(), "!xs", 3) != 0)
		{
			if( ((serial1_tx.get() & RT_XCVARIO) && serial1_speed.get()) || (SetupCommon::isCanMaster() && serial1_speed.get()) ) {
				if (forwardMsg(can, s1_tx_q)) {
					Serial::setRxTxNotifier( TX1_REQ );
					ESP_LOGI(FNAME, "Send to S1 device, CAN link received %d bytes NMEA", can.length());
				}
			}
			if( (serial2_tx.get() & RT_XCVARIO) && serial2_speed.get()) {
				if (forwardMsg(can, s2_tx_q)) {
					Serial::setRxTxNotifier( TX2_REQ );
					// ESP_LOGI(FNAME, "Send to S2 device, can link received %d bytes NMEA", can.length());
				}
			}
			if( wireless == WL_BLUETOOTH ) {
				if( forwardMsg( can, bt_tx_q )) {
					// ESP_LOGI(FNAME,"Send to BT device, can link received %d bytes NMEA", can.length() );
				}
			}
		}
		Protocols::parseNMEA( can.c_str() );
	}
}
