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

RingBufCPP<SString, QUEUE_SIZE> xcv_tx_q;

RingBufCPP<SString, QUEUE_SIZE> can_rx_q;
RingBufCPP<SString, QUEUE_SIZE> can_tx_q;

portMUX_TYPE btmux = portMUX_INITIALIZER_UNLOCKED;

// Utility methods to push and pull data into/from queues

static xSemaphoreHandle qMutex=NULL;

void Router::begin(){
	qMutex = xSemaphoreCreateMutex();
}

// checks if Queue is full, otherwise appends SString
bool Router::forwardMsg( SString &s, RingBufCPP<SString, QUEUE_SIZE>& q, bool nmea ){
	// ESP_LOGI(FNAME,"forwardMsg() len: %d, queueElem: %d queueFull:%d", s.length(), q.numElements(), q.isFull() );
	if( !q.isFull() ) {
		xSemaphoreTake(qMutex,portMAX_DELAY );
		q.add( s );
		xSemaphoreGive(qMutex);
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
		xSemaphoreTake(qMutex,portMAX_DELAY );
		q.pull( s );
		xSemaphoreGive(qMutex);
		return true;
	}
	return false;
}

int Router::pullBlock( RingBufCPP<SString, QUEUE_SIZE>& q, char *block, int size ){
	xSemaphoreTake(qMutex,portMAX_DELAY );
	int total_len = 0;
	while( !q.isEmpty() ){
		int len = q.pull( block+total_len );
		total_len += len;
		if( (total_len + SSTRLEN) > size )
			break;
	}
	block[total_len]=0;
	xSemaphoreGive(qMutex);
	return total_len;
}

// XCVario Router
void Router::sendXCV(char * s){
	// ESP_LOGI( FNAME,"XCVario message %s",s);
	if(  !Flarm::bincom  ){
		SString xcv( s );
		if( forwardMsg( xcv, xcv_tx_q ) ){
			// ----------------(FNAME,"Received %d bytes from XCV", xcv.length() );
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
	while( pullMsg( xcv_tx_q, xcv ) ){
		// ESP_LOGI(FNAME,"XCV data to be forwarded %d bytes", xcv.length() );
		if ( strncmp( xcv.c_str(), "!xs", 3 ) != 0 ){  // !xs messages are XCV specific and must not go to BT,WiFi or serial Navi's
			if( rt_xcv_wl.get() && ((wireless == WL_BLUETOOTH) || (wireless == WL_BLUETOOTH_LE))  ) {
				if( forwardMsg( xcv, bt_tx_q ) ){
					// ESP_LOGI(FNAME,"XCV data forwarded to BT device, %d bytes", xcv.length() );
				}
			}
			if( rt_xcv_wl.get() && (wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE) ){
				if( forwardMsg( xcv, wl_vario_tx_q ) ){
					// ESP_LOGI(FNAME,"XCV data forwarded to WLAN port 8880, %d bytes", xcv.length() );
				}
			}
			// ESP_LOGI(FNAME,"XCV data for S1 device, %d bytes ena:%d speed:%d", xcv.length(), rt_s1_xcv.get(), serial1_speed.get() );
			if( rt_s1_xcv.get() && serial1_speed.get() ){
				if( forwardMsg( xcv, s1_tx_q ) ){
					Serial::setRxTxNotifier( TX1_REQ );
					// ESP_LOGI(FNAME,"XCV data forwarded to S1 device, %d bytes", xcv.length() );
				}
			}
			// ESP_LOGI(FNAME,"XCV data for S1 device, %d bytes ena:%d speed:%d", xcv.length(), rt_s2_xcv.get(), serial2_speed.get() );
			if( rt_s2_xcv.get() && serial2_speed.get() ){
				if( forwardMsg( xcv, s2_tx_q ) ){
					Serial::setRxTxNotifier( TX2_REQ );
					// ESP_LOGI(FNAME,"XCV data forwarded to S2 device, %d bytes", xcv.length() );
				}
			}
			if( rt_can_xcv.get() && can_speed.get() && (strncmp( xcv.c_str(), "$PXCV", 5 ) != 0) ){ // don't route PXCV to master, has its own
				if( forwardMsg( xcv, can_tx_q ) ){
					// ESP_LOGI(FNAME,"XCV data forwarded to CAN bus, %d bytes", xcv.length() );
				}
			}
		}
	}
}

// Route messages from serial interface S1
void Router::routeS1(){
	SString s1;
	while( pullMsg( s1_rx_q, s1) ){
		// ESP_LOGI(FNAME,"S1 RX %d bytes: %s", s1.length(), s1.c_str() );
		// ESP_LOGI(FNAME,"routeS1 RX %d bytes, Q:%d  B:%d", s1.length(), s1_rx_q.numElements(), Flarm::bincom );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,s1.c_str(),s1.length(), ESP_LOG_INFO);

		if( rt_s1_wl.get() && (wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE) ){
			if( forwardMsg( s1, wl_flarm_tx_q )){
				// ESP_LOGI(FNAME,"S1 RX %d bytes forwarded to WiFi port 8881", s1.length() );
			}
		}
		if( rt_s1_wl.get() && ((wireless == WL_BLUETOOTH) || (wireless == WL_BLUETOOTH_LE)) ){
			if( forwardMsg( s1, bt_tx_q )){
				// ESP_LOGI(FNAME,"S1 RX %d bytes forwarded to BT", s1.length() );
			}
		}
		if( rt_s1_can.get() && can_speed.get() ){
			if( forwardMsg( s1, can_tx_q )){
				// ESP_LOGI(FNAME,"S1 RX %d bytes forwarded to CAN bus", s1.length() );
			}
		}
		if( rt_s1_s2.get() && serial2_speed.get() ){
			if( forwardMsg( s1, s2_tx_q )){
				Serial::setRxTxNotifier( TX2_REQ );
				// ESP_LOGI(FNAME,"S1 RX %d bytes looped to S2", s1.length() );
			}
		}
		if( serial1_rxloop.get() ){  // only 0=DISABLE | 1=ENABLE
			if( forwardMsg( s1, s1_tx_q )){
				Serial::setRxTxNotifier( TX1_REQ );
				// ESP_LOGI(FNAME,"S1 RX bytes %d looped to s1_tx_q", s1.length() );
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

		if( rt_s2_wl.get() && (wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE) )
			if( forwardMsg( s2, wl_aux_tx_q )){
				// ESP_LOGI(FNAME,"S2 RX bytes %d forward to WiFi port 8882", s2.length() );
			}
		if( rt_s2_wl.get() && ((wireless == WL_BLUETOOTH) || (wireless == WL_BLUETOOTH_LE)) ){
			if( forwardMsg( s2, bt_tx_q )){
				// ESP_LOGI(FNAME,"S2 RX %d bytes forwarded to BT", s2.length() );
			}
		}
		if( rt_s2_can.get() && can_speed.get() ){
			if( forwardMsg( s2, can_tx_q )){
				// ESP_LOGI(FNAME,"S2 RX %d bytes forwarded to CAN bus", s2.length() );
			}
		}
		if( rt_s1_s2.get() && serial1_speed.get() ){
			if( forwardMsg( s2, s1_tx_q )){  // This might connect XCSoar on S2 with Flarm on S1
				Serial::setRxTxNotifier( TX1_REQ );
				// ESP_LOGI(FNAME,"S2 RX %d bytes forwarded to S1", s2.length() );
			}
		}
		Protocols::parseNMEA( s2.c_str() );
	}
}

// route messages from WLAN
void Router::routeWLAN(){
	SString wlmsg;
	if( wireless == WL_WLAN_MASTER || wireless == WL_WLAN_CLIENT || wireless == WL_WLAN_STANDALONE  ){
		// Route received data from any of the WLAN ports
		while( pullMsg( wl_vario_rx_q, wlmsg) ){   // Port 8880 received data
			// ESP_LOGI(FNAME,"From WLAN port 8880 RX NMEA %s", wlmsg.c_str() );
			if( rt_s1_wl.get()  && serial1_speed.get() ){
				if( forwardMsg( wlmsg, s1_tx_q ) ){
					Serial::setRxTxNotifier( TX1_REQ );
					// ESP_LOGI(FNAME,"Send to S1 device, TCP port 8880 received %d bytes", wlmsg.length() );
				}
			}
			if( rt_s2_wl.get() && serial2_speed.get() ){
				if( forwardMsg( wlmsg, s2_tx_q ) ){
					Serial::setRxTxNotifier( TX2_REQ );
					// ESP_LOGI(FNAME,"Send to S2 device, TCP port 8880 received %d bytes", wlmsg.length() );
				}
			}
			if( rt_wl_can.get() ){
				if( forwardMsg( wlmsg, can_tx_q ) ){
					// ESP_LOGI(FNAME,"Send to CAN XCV processing, TCP port 8880 received %d bytes", wlmsg.length() );
				}
			}
			Protocols::parseNMEA( wlmsg.c_str() );
		}
		while( pullMsg( wl_flarm_rx_q, wlmsg ) ){   // Port 8881 received data
			if( rt_s1_wl.get() && serial1_speed.get() ){
				if( forwardMsg( wlmsg, s1_tx_q ) ){
					Serial::setRxTxNotifier( TX1_REQ );
					// ESP_LOGI(FNAME,"Send to S1 device, TCP port 8881 received %d bytes", wlmsg.length() );
				}
			}
			Protocols::parseNMEA( wlmsg.c_str() );
		}
		while( pullMsg( wl_aux_rx_q, wlmsg ) ){  // Port 8882 received data
			if( rt_s2_wl.get() && serial2_speed.get() ){
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
	if( (wireless != WL_BLUETOOTH) && (wireless != WL_BLUETOOTH_LE) )
		return;
	SString bt;
	while( pullMsg( bt_rx_q, bt ) ){
		// ESP_LOGI(FNAME,"BT RX %s, len: %d bytes", bt.c_str(), bt.length() );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,bt.c_str(),bt.length(), ESP_LOG_INFO);
		if( rt_s1_wl.get() && serial1_speed.get() ){  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s1_tx_q ) ){
				Serial::setRxTxNotifier( TX1_REQ );
				// ESP_LOGI(FNAME,"Send to S1 device, BT received %d bytes", bt.length() );
			}
		}
		if( rt_s2_wl.get() && serial2_speed.get() ){  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s2_tx_q ) ){
				Serial::setRxTxNotifier( TX2_REQ );
				// ESP_LOGI(FNAME,"Send to S2 device, BT received %d bytes", bt.length() );
			}
		}
		if( rt_wl_can.get() && can_speed.get() ){
			// ESP_LOGI(FNAME,"Send to CAN bus, BT received %d bytes", bt.length() );
			CAN->sendNMEA( bt );
		}
		Protocols::parseNMEA( bt.c_str() );
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
			// ESP_LOGI(FNAME,"can rx route.. wl-can:%d wl=bt:%d", rt_wl_can.get(), (wireless == WL_BLUETOOTH) );
			if( (rt_s1_can.get() && serial1_speed.get()) ) {
				if (forwardMsg(can, s1_tx_q)) {
					Serial::setRxTxNotifier( TX1_REQ );
					// ESP_LOGI(FNAME, "Send to S1 device, CAN link received %d NMEA bytes", can.length());
				}
			}
			if( rt_s2_can.get() && serial2_speed.get()) {
				if (forwardMsg(can, s2_tx_q)) {
					Serial::setRxTxNotifier( TX2_REQ );
					// ESP_LOGI(FNAME, "Send to S2 device, can link received %d NMEA bytes", can.length());
				}
			}
			if( rt_wl_can.get() && ((wireless == WL_BLUETOOTH) || (wireless == WL_BLUETOOTH_LE)) ) {
				if( forwardMsg( can, bt_tx_q )) {
					// ESP_LOGI(FNAME,"Send to BT device, can link received %d NMEA bytes", can.length() );
				}
			}
			if( rt_wl_can.get() && (wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE) ) {
				if( forwardMsg( can, wl_vario_tx_q )) {  // CAN received data sent to XCVario protocol 8880, maybe in future add other options if applicable
					// ESP_LOGI(FNAME,"Send to WiFi device, can link received %d NMEA bytes", can.length() );
				}
			}
			Protocols::parseNMEA( can.c_str() );
		}
		else{ // got !xs frame
			Protocols::parseXS( can.c_str() );
		}

	}
}
