#include <esp_log.h>
#include "BLESender.h"
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
#include "DataMonitor.h"
#include "DataLink.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;

static TaskHandle_t pid = nullptr;
static DataLink *dlb;

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"


class MyServerCallbacks: public BLEServerCallbacks {
	void onConnect(BLEServer* pServer) {
		deviceConnected = true;
	};

	void onDisconnect(BLEServer* pServer) {
		deviceConnected = false;
	}
};

class MyCallbacks: public BLECharacteristicCallbacks {
	void onWrite(BLECharacteristic *pCharacteristic) {
		std::string rx = pCharacteristic->getValue();
		if (rx.length()) {
			dlb->process( rx.c_str(), rx.length(), 7 );
			DM.monitorString( MON_BLUETOOTH, DIR_RX, rx.c_str(), rx.length() );
			ESP_LOGI(FNAME,">BT LE RX: %d bytes",  rx.length()  );
			ESP_LOG_BUFFER_HEXDUMP(FNAME,rx.c_str(), rx.length() , ESP_LOG_INFO);
		}
	}
};

bool BLESender::selfTest(){
	ESP_LOGI(FNAME,"SerialBLE::selfTest");
	return true;
}

int BLESender::queueFull() {
	if( wireless == WL_BLUETOOTH_LE ){
		if(bt_tx_q.isFull())
			return 1;
	}
	return 0;
}

void BLESender::btTask(void *pvParameters){
	while(1) {
		progress();
		Router::routeBT();
		if( uxTaskGetStackHighWaterMark( pid ) < 256 )
			ESP_LOGW(FNAME,"Warning BT task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid ) );
		vTaskDelay( 10/portTICK_PERIOD_MS );
	}
}

void BLESender::progress(){
	char buf[256];
	if (deviceConnected) {
		int len = Router::pullBlock( bt_tx_q, buf, 20 );
		ESP_LOGI(FNAME,"BLE device connected, data len=%d",len);
		if( len ){
			int pos=0;
			while( len > 0 ){
				int sent=min( len, 20 );
				pTxCharacteristic->setValue((uint8_t*)&buf[pos], (size_t)sent);
				pTxCharacteristic->notify();
				ESP_LOGI(FNAME,"<BT LE TX %d bytes", sent );
				ESP_LOG_BUFFER_HEXDUMP(FNAME,&buf[pos],len, ESP_LOG_INFO);
				DM.monitorString( MON_BLUETOOTH, DIR_TX, &buf[pos], sent );
				pos+=sent;
				len-=sent;
			}
		}
	}
	// disconnecting
	if (!deviceConnected && oldDeviceConnected) {
		delay(500); // give the bluetooth stack the chance to get things ready
		pServer->startAdvertising(); // restart advertising
		oldDeviceConnected = deviceConnected;
	}
	// connecting
	if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
		oldDeviceConnected = deviceConnected;
	}
}

void BLESender::begin(){
	ESP_LOGI(FNAME,"BLESender::begin()" );
	ESP_LOGI(FNAME,"BT LE on, create BT master object" );
	dlb = new DataLink();
	// Create the BLE Device
	std::string ble_id( SetupCommon::getID() );
	ble_id += std::string("-LE");
	BLEDevice::init(ble_id.c_str());

	// Create the BLE Server
	pServer = BLEDevice::createServer();
	pServer->setCallbacks(new MyServerCallbacks());

	// Create the BLE Service
	BLEService *pService = pServer->createService(SERVICE_UUID);

	// Create a BLE Characteristic
	pTxCharacteristic = pService->createCharacteristic(
			CHARACTERISTIC_UUID_TX,
			BLECharacteristic::PROPERTY_WRITE
	);

	pTxCharacteristic->addDescriptor(new BLE2902());

	BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
			CHARACTERISTIC_UUID_RX,
			BLECharacteristic::PROPERTY_WRITE
	);

	pRxCharacteristic->setCallbacks(new MyCallbacks());

	// Start the service
	pService->start();

	// Start advertising
	pServer->getAdvertising()->start();

	xTaskCreatePinnedToCore(&btTask, "bleTask", 4096, NULL, 12, &pid, 0);  // stay below compass task
}
