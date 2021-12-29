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
#include "Protocols.h"
#include <logdef.h>
#include "Switch.h"
#include "sensor.h"
#include "Router.h"
#include "Flarm.h"
#include "BluetoothSerial.h"
#include "DataMonitor.h"

static TaskHandle_t pid = NULL;

bool BTSender::selfTest(){
	ESP_LOGI(FNAME,"SerialBT::selfTest");
	if( !SerialBT ){
		ESP_LOGI(FNAME,"SerialBT not initialized");
		return false;
	}
	if( SerialBT->isReady( false ) ) {
		ESP_LOGI(FNAME,"SerialBT::selfTest: PASSED");
		return true;
	}
	ESP_LOGI(FNAME,"SerialBT::selfTest: FAILED");
	return false;
}

BluetoothSerial *BTSender::SerialBT = 0;

int BTSender::queueFull() {
	if( wireless == WL_BLUETOOTH ){
		if(bt_tx_q.isFull())
			return 1;
	}
	return 0;
}

void BTSender::btTask(void *pvParameters){
	while(1) {
		progress();
		Router::routeBT();
		if( uxTaskGetStackHighWaterMark( pid ) < 256 )
			ESP_LOGW(FNAME,"Warning BT task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid ) );
		vTaskDelay( 20/portTICK_PERIOD_MS );
	}
}

void BTSender::progress(){
	if (wireless != WL_BLUETOOTH )
		return;
	if( !SerialBT ){
		ESP_LOGI(FNAME,"SerialBT not initialized");
		return;
	}
	if (SerialBT->available() ) {
		ESP_LOGI(FNAME,"BT RFCOMM RX");
		SString rx;
		while (SerialBT->available() && (rx.length() < SSTRLEN-1) ){
			Router::pullMsg( bt_rx_q , rx );
			char byte = (char)SerialBT->read();
			// ESP_LOGI(FNAME,"BT RFCOMM RX %c", byte );
			rx.append( &byte, 1 );
		}
		Router::forwardMsg( rx, bt_rx_q );
		DM.monitorString( MON_BLUETOOTH, DIR_RX, rx.c_str() );
	}

	if( SerialBT->hasClient() ) {
		SString msg;
		if ( Router::pullMsg( bt_tx_q, msg ) ){
			// ESP_LOGI(FNAME,"data avail for BT send %d bytes: %s", msg.length(), msg.c_str() );
			SerialBT->write( (const uint8_t *)msg.c_str(), msg.length() );
			DM.monitorString( MON_BLUETOOTH, DIR_TX, msg.c_str() );
		}
	}
}


void BTSender::begin(){
	ESP_LOGI(FNAME,"BTSender::begin()" );
	if( wireless == WL_BLUETOOTH ) {
		ESP_LOGI(FNAME,"BT on, create BT master object" );
		SerialBT = new BluetoothSerial();
		SerialBT->begin( SetupCommon::getID() );
		xTaskCreatePinnedToCore(&btTask, "btTask", 3072, NULL, 10, &pid, 0);  // stay below compass task
	}
}


// dummy, we don't implement BLE right now
extern "C" void btsnd_hcic_ble_update_adv_report_flow_control( int ignore ) {};



