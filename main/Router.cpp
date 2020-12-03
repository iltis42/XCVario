
#include <esp_log.h>
#include "BTSender.h"
#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include <freertos/semphr.h>
#include "RingBufCPP.h"
#include "OpenVario.h"
#include <logdef.h>
#include "Switch.h"
#include "sensor.h"
#include "Router.h"

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
RingBufCPP<SString, QUEUE_SIZE> xcv_tx_q;

portMUX_TYPE btmux = portMUX_INITIALIZER_UNLOCKED;

// Utility methods to push and pull data into/from queues

// checks if Queue is full, otherwise appends SString
bool forwardMsg( SString &s, RingBufCPP<SString, QUEUE_SIZE>& q ){
	if( !q.isFull() ) {
		portENTER_CRITICAL_ISR(&btmux);
		q.add( s );
		portEXIT_CRITICAL_ISR(&btmux);
		return true;
	}
	return false;
}

// gets last message from ringbuffer FIFO
bool pullMsg( RingBufCPP<SString, QUEUE_SIZE>& q, SString& s ){
	if( !q.isEmpty() ){
		portENTER_CRITICAL_ISR(&btmux);
		q.pull( &s );
		portEXIT_CRITICAL_ISR(&btmux);
		return true;
	}
	return false;
}

// XCVario Router

// Route XCVario messages
void routeXCV( SString &xcv ){
	if( blue_enable.get() )
		if( forwardMsg( xcv, bt_tx_q ) )
			ESP_LOGI(FNAME,"Send to BT device, XCV received %d bytes", xcv.length() );
	if( !blue_enable.get() )  // tbd: Extra config option for WLAN
		if( forwardMsg( xcv, wl_vario_tx_q ) )
			ESP_LOGI(FNAME,"Send to WLAN port 8880, XCV received %d bytes", xcv.length() );
	if( serial1_tx.get() & RT_XCVARIO )
		if( forwardMsg( xcv, s1_tx_q ) )
			ESP_LOGI(FNAME,"Send to ttyS2 device, XCV received %d bytes", xcv.length() );
	if( serial2_tx.get() & RT_XCVARIO )
		if( forwardMsg( xcv, s2_tx_q ) )
			ESP_LOGI(FNAME,"Send to ttyS2 device, XCV received %d bytes", xcv.length() );

}

// Route messages from serial interface S1
void routeS1(){
	SString s1;
	if( pullMsg( s1_rx_q, s1) ){
		ESP_LOGD(FNAME,"Serial 1 RX len: %d bytes, Q:%d", s1.length(), bt_tx_q.isFull() );
		ESP_LOG_BUFFER_HEXDUMP(FNAME,s1.c_str(),s1.length(), ESP_LOG_DEBUG);
		if( !blue_enable.get() )   // tbd extra config.
			if( forwardMsg( s1, wl_flarm_tx_q ))
				ESP_LOGI(FNAME,"Serial 1 RX bytes %d forward to wl_flarm_tx_q port 8881", s1.length() );
		if( blue_enable.get() )
			if( forwardMsg( s1, bt_tx_q ))
				ESP_LOGI(FNAME,"Serial 1 RX bytes %d forward to bt_tx_q", s1.length() );
		if( serial1_rxloop.get() )  // only 0=DISABLE | 1=ENABLE
			if( forwardMsg( s1, s1_tx_q ))
				ESP_LOGI(FNAME,"Serial 1 RX bytes %d looped to s1_tx_q", s1.length() );
		if( serial2_tx.get() & RT_S1 )
			if( forwardMsg( s1, s2_tx_q ))
				ESP_LOGI(FNAME,"Serial 1 RX bytes %d looped to s2_tx_q", s1.length() );
	}
}

// Route messages from serial interface S2
void routeS2(){
	SString s2;
	if( pullMsg( s2_rx_q, s2) ){
		ESP_LOGD(FNAME,"Serial 2 RX len: %d bytes, Q:%d", s2.length(), bt_tx_q.isFull() );
		ESP_LOG_BUFFER_HEXDUMP(FNAME,s2.c_str(),s2.length(), ESP_LOG_DEBUG);
		if( !blue_enable.get() )
			if( forwardMsg( s2, wl_aux_tx_q ))
				ESP_LOGI(FNAME,"Serial 2 RX bytes %d forward to wl_aux_tx_q port 8882", s2.length() );
		if( blue_enable.get() )
			if( forwardMsg( s2, bt_tx_q ))
				ESP_LOGI(FNAME,"Serial 2 RX bytes %d forward to bt_tx_q", s2.length() );
		if( serial2_tx.get() & RT_S1 )
			if( forwardMsg( s2, s1_tx_q ))
				ESP_LOGI(FNAME,"Serial 2 RX bytes %d looped to s1_tx_q", s2.length() );
	}
}

// route messages from WLAN
void routeWLAN(){
	if( blue_enable.get() )  // tbd: extra config to switch of WLAN
		return;
	// Route received data from WLAN ports
	SString wlmsg;
	if( pullMsg( wl_vario_rx_q, wlmsg) ){
		if( strncmp( wlmsg.c_str(), "!g,", 3 )  == 0 ) {
			ESP_LOGI(FNAME,"From WLAN port 8880 RX matched a Borgelt command %s", wlmsg.c_str() );
			OpenVario::parseNMEA( wlmsg.c_str() );
		}
	}
	if( pullMsg( wl_flarm_rx_q, wlmsg ) ){
		if( serial1_tx.get() & RT_WIRELESS )
			if( forwardMsg( wlmsg, s1_tx_q ) )
				ESP_LOGI(FNAME,"Send to ttyS1 device, TCP port 8881 received %d bytes", wlmsg.length() );
		if( serial2_tx.get() & RT_WIRELESS )
			if( forwardMsg( wlmsg, s2_tx_q ) )
				ESP_LOGI(FNAME,"Send to ttyS2 device, TCP port 8881 received %d bytes", wlmsg.length() );
	}
	if( pullMsg( wl_aux_rx_q, wlmsg ) ){
		if( serial1_tx.get() & RT_WIRELESS )
			if( forwardMsg( wlmsg, s1_tx_q ) )
				ESP_LOGI(FNAME,"Send to ttyS1 device, TCP port 8882 received %d bytes", wlmsg.length() );
		if( serial2_tx.get() & RT_WIRELESS )
			if( forwardMsg( wlmsg, s2_tx_q ) )
				ESP_LOGI(FNAME,"Send to ttyS2 device, TCP port 8882 received %d bytes", wlmsg.length() );
	}
}

// route messages from Bluetooth
void routeBT(){
	if( !blue_enable.get() )
		return;
	SString bt;
	if( pullMsg( bt_rx_q, bt ) ){
		if( serial1_tx.get() & RT_XCVARIO )  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s1_tx_q ) )
				ESP_LOGI(FNAME,"Send to ttyS1 device, BT received %d bytes", bt.length() );
		if( serial2_tx.get() & RT_XCVARIO )  // Serial data TX from bluetooth enabled ?
			if( forwardMsg( bt, s2_tx_q ) )
				ESP_LOGI(FNAME,"Send to ttyS2 device, BT received %d bytes", bt.length() );
		if( strncmp( bt.c_str(), "!g,", 3 )  == 0 ) {
			ESP_LOGI(FNAME,"BT RX Matched a Borgelt command %s", bt.c_str() );
			OpenVario::parseNMEA( bt.c_str() );
		}
	}
}


