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

RingBufCPP<SString, QUEUE_SIZE> client_rx_q;
RingBufCPP<SString, QUEUE_SIZE> client_tx_q;

portMUX_TYPE btmux = portMUX_INITIALIZER_UNLOCKED;

// Utility methods to push and pull data into/from queues

#undef ESP_LOGV
#define ESP_LOGV(x,y,z);  ;

// checks if Queue is full, otherwise appends SString
bool Router::forwardMsg( SString &s, RingBufCPP<SString, QUEUE_SIZE>& q ){
	if( !q.isFull() ) {
		portENTER_CRITICAL_ISR(&btmux);
		q.add( s );
		portEXIT_CRITICAL_ISR(&btmux);
		return true;
	}
	// ESP_LOGW(FNAME,"+++ WARNING +++ dropped msg %s", s.c_str() );
	return false;
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
	ESP_LOGV( FNAME,"XCVario message %s",s);
	if(  !Flarm::bincom  ){
		SString xcv( s );
		if( forwardMsg( xcv, xcv_rx_q ) )
			ESP_LOGV(FNAME,"Received %d bytes from XCV", xcv.length() );
	}
}

// XCVario Router
void Router::sendAUX(char * s){
    ESP_LOGI( FNAME,"AUX message %s",s);
	if(  !Flarm::bincom  ){
		SString xcv( s );
		if( forwardMsg( xcv, wl_aux_tx_q ) )
			ESP_LOGV(FNAME,"Sent %d bytes to 8882 port", xcv.length() );
	}
}

// Route XCVario messages
void Router::routeXCV(){
	SString xcv;
	if( pullMsg( xcv_rx_q, xcv ) ){
		if ( strncmp( xcv.c_str(), "!xs", 3 ) != 0 ){  // !xs messages are XCV specific and must not go to BT,WiFi or serial Navi's
			if( wireless == WL_BLUETOOTH ) {
				if( forwardMsg( xcv, bt_tx_q ) )
					ESP_LOGV(FNAME,"Send to BT device, XCV received %d bytes", xcv.length() );
			}
			if( wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE  ){
				if( forwardMsg( xcv, wl_vario_tx_q ) )
					ESP_LOGV(FNAME,"Send to WLAN port 8880, XCV %d bytes", xcv.length() );
			}
			if( (serial1_tx.get() & RT_XCVARIO) && serial1_speed.get() )
				if( forwardMsg( xcv, s1_tx_q ) )
				  Serial::setRxTxNotifier( TX1_REQ );
					ESP_LOGV(FNAME,"Send to ttyS1 device, XCV %d bytes", xcv.length() );
			if( (serial2_tx.get() & RT_XCVARIO) && serial2_speed.get() )
				if( forwardMsg( xcv, s2_tx_q ) )
				  Serial::setRxTxNotifier( TX2_REQ );
					ESP_LOGV(FNAME,"Send to ttyS2 device, XCV %d bytes", xcv.length() );
		}
	}
}

// Route messages from serial interface S1
void Router::routeS1(){
	SString s1;
	if( pullMsg( s1_rx_q, s1) ){
		// ESP_LOGD(FNAME,"ttyS1 RX len: %d bytes, Q:%d", s1.length(), bt_tx_q.isFull() );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,s1.c_str(),s1.length(), ESP_LOG_DEBUG);
		if( wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE )
			if( forwardMsg( s1, wl_flarm_tx_q ))
				ESP_LOGV(FNAME,"ttyS1 RX bytes %d forward to wl_flarm_tx_q port 8881", s1.length() );
		if( wireless == WL_BLUETOOTH )
			if( forwardMsg( s1, bt_tx_q ))
				ESP_LOGV(FNAME,"ttyS1 RX bytes %d forward to bt_tx_q", s1.length() );
		if( serial1_rxloop.get() )  // only 0=DISABLE | 1=ENABLE
			if( forwardMsg( s1, s1_tx_q ))
			  Serial::setRxTxNotifier( TX1_REQ );
				ESP_LOGV(FNAME,"ttyS1 RX bytes %d looped to s1_tx_q", s1.length() );
		if( (can_tx.get() & RT_XCVARIO) && can_speed.get() )
			if( forwardMsg( s1, client_tx_q ))
				ESP_LOGV(FNAME,"ttyS1 RX bytes %d forward to client_tx_q", s1.length() );
        if( (serial2_tx.get() & RT_S1) && serial2_speed.get() )
			if( forwardMsg( s1, s2_tx_q ))
			  Serial::setRxTxNotifier( TX2_REQ );
				ESP_LOGV(FNAME,"ttyS1 RX bytes %d looped to s2_tx_q", s1.length() );
		Protocols::parseNMEA( s1.c_str() );
	}
}

// Route messages from serial interface S2
void Router::routeS2(){
	SString s2;
	if( pullMsg( s2_rx_q, s2) ){
		// ESP_LOGD(FNAME,"ttyS2 RX len: %d bytes, Q:%d", s2.length(), bt_tx_q.isFull() );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,s2.c_str(),s2.length(), ESP_LOG_DEBUG);
		if(wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE )
			if( forwardMsg( s2, wl_aux_tx_q ))
				ESP_LOGV(FNAME,"ttyS2 RX bytes %d forward to wl_aux_tx_q port 8882", s2.length() );
		if( wireless == WL_BLUETOOTH )
			if( forwardMsg( s2, bt_tx_q ))
				ESP_LOGV(FNAME,"ttyS2 RX bytes %d forward to bt_tx_q", s2.length() );
		if( (can_tx.get() & RT_XCVARIO) && can_speed.get() )
			if( forwardMsg( s2, client_tx_q ))
				ESP_LOGV(FNAME,"ttyS2 RX bytes %d forward to client_tx_q", s2.length() );
		if( (serial1_tx.get() & RT_S1) && serial1_speed.get() ) // RT_S1 could be renamed to RT_SERIAL
			if( forwardMsg( s2, s1_tx_q )) // This might connect XCSoar on S2 with Flarm on S1
			  Serial::setRxTxNotifier( TX1_REQ );
				ESP_LOGV(FNAME,"ttyS2 RX bytes %d forward to s1_tx_q", s2.length() );
		Protocols::parseNMEA( s2.c_str() );
	}
}

// route messages from WLAN
void Router::routeWLAN(){
	SString wlmsg;
	if( wireless != WL_DISABLE  ){
		// Route received data from WLAN ports
		if( pullMsg( wl_vario_rx_q, wlmsg) ){
			ESP_LOGV(FNAME,"From WLAN port 8880 RX NMEA %s", wlmsg.c_str() );
			if( (serial1_tx.get() & RT_WIRELESS)  && serial1_speed.get() )
				if( forwardMsg( wlmsg, s1_tx_q ) )
				  Serial::setRxTxNotifier( TX1_REQ );
					ESP_LOGV(FNAME,"Send to  device, TCP port 8880 received %d bytes", wlmsg.length() );
			if( (serial2_tx.get() & RT_WIRELESS) && serial2_speed.get() )
				if( forwardMsg( wlmsg, s2_tx_q ) )
				  Serial::setRxTxNotifier( TX2_REQ );
					ESP_LOGV(FNAME,"Send to ttyS2 device, TCP port 8880 received %d bytes", wlmsg.length() );
			Protocols::parseNMEA( wlmsg.c_str() );
		}
		if( pullMsg( wl_flarm_rx_q, wlmsg ) ){
			if( (serial1_tx.get() & RT_WIRELESS) && serial1_speed.get() )
				if( forwardMsg( wlmsg, s1_tx_q ) )
				  Serial::setRxTxNotifier( TX1_REQ );
					ESP_LOGV(FNAME,"Send to  device, TCP port 8881 received %d bytes", wlmsg.length() );
			if( (serial2_tx.get() & RT_WIRELESS) && serial2_speed.get() )
				if( forwardMsg( wlmsg, s2_tx_q ) )
				  Serial::setRxTxNotifier( TX2_REQ );
					ESP_LOGV(FNAME,"Send to ttyS2 device, TCP port 8881 received %d bytes", wlmsg.length() );
			Protocols::parseNMEA( wlmsg.c_str() );
		}
		if( pullMsg( wl_aux_rx_q, wlmsg ) ){
			if( (serial1_tx.get() & RT_WIRELESS) && serial1_speed.get() )
				if( forwardMsg( wlmsg, s1_tx_q ) )
				  Serial::setRxTxNotifier( TX1_REQ );
					ESP_LOGV(FNAME,"Send to  device, TCP port 8882 received %d bytes", wlmsg.length() );
			if( (serial2_tx.get() & RT_WIRELESS) && serial2_speed.get() )
				if( forwardMsg( wlmsg, s2_tx_q ) )
				  Serial::setRxTxNotifier( TX2_REQ );
					ESP_LOGV(FNAME,"Send to ttyS2 device, TCP port 8882 received %d bytes", wlmsg.length() );
		Protocols::parseNMEA( wlmsg.c_str() );
		}
	}
}


// route messages from Bluetooth
void Router::routeBT(){
	if( wireless != WL_BLUETOOTH )
		return;
	SString bt;
	if( pullMsg( bt_rx_q, bt ) ){
		if( (serial1_tx.get() & RT_WIRELESS) && serial1_speed.get() )  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s1_tx_q ) )
			  Serial::setRxTxNotifier( TX1_REQ );
				ESP_LOGV(FNAME,"Send to S1 device, BT received %d bytes", bt.length() );
		if( (serial2_tx.get() & RT_WIRELESS) && serial2_speed.get() )  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s2_tx_q ) )
			  Serial::setRxTxNotifier( TX2_REQ );
				ESP_LOGV(FNAME,"Send to S2 device, BT received %d bytes", bt.length() );
		// always check if it is a command to ourselves
		if( !strncmp( bt.c_str(), "!g,", 3 ) || !strncmp( bt.c_str(), "$g,", 3 ) ) {
			ESP_LOGV(FNAME,"BT RX Matched a Borgelt command %s", bt.c_str() );
			Protocols::parseNMEA( bt.c_str() );
		}
		bt.clear();
	}
}

// route messages from master or client via CAN or Wifi
void Router::routeClient(){
	SString client;
	if( pullMsg( client_rx_q, client ) ){
		// ESP_LOGI(FNAME,"Client received %d bytes %s", client.length(), client.c_str());
		if (strncmp(client.c_str(), "!xs", 3) != 0)
		{
			if ((serial1_tx.get() & RT_XCVARIO) && serial1_speed.get()) {
				if (forwardMsg(client, s1_tx_q)) {
				  Serial::setRxTxNotifier( TX1_REQ );
					ESP_LOGV(FNAME, "Send to S1 device, client link received %d bytes NMEA", client.length());
				}
			}
			if ((serial2_tx.get() & RT_XCVARIO) && serial2_speed.get()) {
				if (forwardMsg(client, s2_tx_q)) {
				  Serial::setRxTxNotifier( TX2_REQ );
					ESP_LOGV(FNAME, "Send to S2 device, client link received %d bytes NMEA", client.length());
				}
			}
			if( wireless == WL_BLUETOOTH ) {
    			if( forwardMsg( client, bt_tx_q )) {
    				ESP_LOGV(FNAME,"Send to BT device, client link received %d bytes NMEA", client.length() );
    			}
			}
		}
		Protocols::parseNMEA( client.c_str() );
	}
}
