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
#include "DataLink.h"

static TaskHandle_t pid = nullptr;

DataLink *dlb;

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
	char buf[256];
	int pos = 0;
	while(SerialBT->available() && (pos < 256) ) {
		char byte = (char)SerialBT->read();
		buf[pos] = byte;
		pos++;
	}
	if( pos ){
		SString rx;
		rx.set( buf, pos );
		dlb->process( buf, pos, 7 );
		DM.monitorString( MON_BLUETOOTH, DIR_RX, rx.c_str(), pos );
		// ESP_LOGI(FNAME,">BT RX: %d bytes", pos );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME,rx.c_str(),pos, ESP_LOG_INFO);
	}
	if( SerialBT->hasClient() ) {
		SString msg;
		if ( Router::pullMsg( bt_tx_q, msg ) ){
			// ESP_LOGI(FNAME,"<BT TX %d bytes", msg.length() );
			// ESP_LOG_BUFFER_HEXDUMP(FNAME,msg.c_str(),msg.length(), ESP_LOG_INFO);
			SerialBT->write( (const uint8_t *)msg.c_str(), msg.length() );
			DM.monitorString( MON_BLUETOOTH, DIR_TX, msg.c_str(), msg.length() );
		}
	}
}

void BTSender::begin(){
	ESP_LOGI(FNAME,"BTSender::begin()" );
	if( wireless == WL_BLUETOOTH ) {
		ESP_LOGI(FNAME,"BT on, create BT master object" );
		dlb = new DataLink();
		SerialBT = new BluetoothSerial();
		SerialBT->begin( SetupCommon::getID() );
		xTaskCreatePinnedToCore(&btTask, "btTask", 4096, NULL, 12, &pid, 0);  // stay below compass task
	}
}

// dummy, we don't implement BLE right now
extern "C" void btsnd_hcic_ble_update_adv_report_flow_control( int ignore ) {};
