#include "Router.h"
#include <esp_log.h>
#include "comm/BTspp.h"
#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/semphr.h>
#include "Protocols.h"
#include <logdef.h>
#include "Switch.h"
#include "sensor.h"
#include "Flarm.h"
#include "UbloxGNSSdecode.h"

// RingBufCPP<SString> wl_vario_tx_q;
// RingBufCPP<SString> wl_flarm_tx_q;
// RingBufCPP<SString> wl_aux_tx_q;
// RingBufCPP<SString> wl_vario_rx_q;
// RingBufCPP<SString> wl_flarm_rx_q;
// RingBufCPP<SString> wl_aux_rx_q;

// RingBufCPP<SString> xcv_tx_q;

// RingBufCPP<SString> can_rx_q;
// RingBufCPP<SString> can_tx_q;

portMUX_TYPE btmux = portMUX_INITIALIZER_UNLOCKED;

// Utility methods to push and pull data into/from queues

static SemaphoreHandle_t qMutex=NULL;

void Router::begin(){
	qMutex = xSemaphoreCreateMutex();
}


int Router::pullBlock( RingBufCPP<SString>& q, char *block, int size ){
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
// 	// ESP_LOGI( FNAME,"XCVario message %s",s);
// 	SString xcv( s );
// 	if( forwardMsg( xcv, xcv_tx_q ) ){
// 		// ----------------(FNAME,"Received %d bytes from XCV", xcv.length() );
// 	}
}

