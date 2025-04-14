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
#include "DataLink.h"
// #include <BLEDevice.h>
// #include <BLEServer.h>
// #include <BLEUtils.h>
// #include <BLE2902.h>
#include <esp_bt.h>
#include <esp_gatts_api.h>
#include <esp_gap_ble_api.h>
#include <esp_bt_main.h>

static esp_gatt_if_t my_gatts_if = 0;
static uint16_t service_handle = 0;
// BLEServer *pServer = NULL;
// BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;

static TaskHandle_t pid = nullptr;
static DataLinkOld *dlb;
// static int tick=0;
static uint16_t peer_mtu;
static int congestion=0;

#define MAX_CONGESTION 300

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define UUID_AS_HEX    {0x6E, 0x40, 0x00, 0x02, 0xB5, 0xA3, 0xF3, 0x93, 0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9E}

static uint8_t char_value[] = "Hello BLE!";
static esp_gatt_char_prop_t char_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;

// BLE server event handler
static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if,
								esp_ble_gatts_cb_param_t *param)
{
	switch (event)
	{
	case ESP_GATTS_MTU_EVT:
		peer_mtu = param->mtu.mtu;
		ESP_LOGI(FNAME, "Peer MTU: %d", peer_mtu);
		break;

	case ESP_GATTS_CONNECT_EVT: // Client connected
		ESP_LOGI(FNAME, "Client connected, conn_id: %d", param->connect.conn_id);
		deviceConnected = true;
		break;

	case ESP_GATTS_DISCONNECT_EVT: // Client disconnected
	{
		ESP_LOGI(FNAME, "Client disconnected, reason: %d", param->disconnect.reason);
		deviceConnected = false;
		// Restart advertising
		esp_ble_adv_params_t tmp_params = {
			.adv_int_min = 0x20,
			.adv_int_max = 0x40,
			.adv_type = ADV_TYPE_IND,
			.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
			.peer_addr = { 0, 0, 0, 0, 0, 0 },
			.peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
			.channel_map = ADV_CHNL_ALL,
			.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
		};
		esp_ble_gap_start_advertising(&tmp_params);
		break;
	}
	case ESP_GATTS_READ_EVT:
		ESP_LOGI(FNAME, "Characteristic read, handle: %d", param->read.handle);
		// Optionally, modify the value here before sending it to the client.
		break;

	case ESP_GATTS_WRITE_EVT:
		ESP_LOGI(FNAME, "Characteristic write, handle: %d", param->write.handle);
		if (param->write.is_prep)
		{
			// Handle prepare write (for large data writes)
		}
		else
		{
			// Check if the write is to the CCCD for notifications/indications
			if (param->write.len == 2)
			{
				uint16_t cccd_value = param->write.value[1] << 8 | param->write.value[0];
				if (cccd_value == 0x0001)
				{
					ESP_LOGI(FNAME, "Notifications enabled");
				}
				else if (cccd_value == 0x0002)
				{
					ESP_LOGI(FNAME, "Indications enabled");
				}
				else if (cccd_value == 0x0000)
				{
					ESP_LOGI(FNAME, "Notifications/indications disabled");
				}
			}
			else
			{
				// Handle normal writes
				ESP_LOGI(FNAME, "Written value: %.*s", param->write.len, param->write.value);
				std::string rx((char *)param->write.value, param->write.len);
				if (rx.length()) {
					dlb->process( rx.c_str(), rx.length(), 7 );
					ESP_LOGI(FNAME,">BT LE RX: %d bytes",  rx.length()  );
					ESP_LOG_BUFFER_HEXDUMP(FNAME,rx.c_str(), rx.length() , ESP_LOG_INFO);
				}
			}
		}
		break;

	case ESP_GATTS_CONF_EVT:
		if (param->conf.status != ESP_GATT_OK) {
			ESP_LOGE(FNAME, "Notification/Indication status: %d", param->conf.status);
			if (param->conf.status == ESP_GATT_CONGESTED) {
				// ESP_LOGI(FNAME, "Congested, pacing: %d", congestion );
				if (congestion < 300) congestion++;
			}
		}
		else {
			ESP_LOGI(FNAME, "Send OK, pacing: %d", congestion );
			if (congestion)
				congestion--;
		}
		break;

	case ESP_GATTS_REG_EVT:
	{
		ESP_LOGI(FNAME, "ESP_GATTS_REG_EVT, registering service...");
		my_gatts_if = gatts_if; // Store the GATT server interface

		// esp_bt_uuid_t service_uuid = {
		// 	.len = ESP_UUID_LEN_16,
		// 	.uuid = {.uuid16 = SERVICE_UUID}};
		esp_gatt_srvc_id_t service_id = {
			.id = {
				.uuid = {
					.len = ESP_UUID_LEN_128,
					.uuid = {.uuid128 = UUID_AS_HEX},
				},
				.inst_id = 0,
			},
			.is_primary = true,
		};
		esp_ble_gatts_create_service(gatts_if, &service_id, 4); // Create service with max 4 handles
		break;
	}
	case ESP_GATTS_CREATE_EVT:
	{
		ESP_LOGI(FNAME, "ESP_GATTS_CREATE_EVT, service created with handle %d", param->create.service_handle);
		service_handle = param->create.service_handle; // Store the service handle

		// Add characteristic to the service
		esp_bt_uuid_t char_uuid = {
			.len = ESP_UUID_LEN_128,
			.uuid = {.uuid128 = UUID_AS_HEX},
		};
		esp_attr_value_t tmp_attr = {
			.attr_max_len = sizeof(char_value),
			.attr_len = sizeof(char_value),
			.attr_value = char_value,
		};
		esp_ble_gatts_add_char(service_handle, &char_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, char_property, &tmp_attr, NULL);
		break;
	}
	case ESP_GATTS_EXEC_WRITE_EVT:
	case ESP_GATTS_UNREG_EVT:
	case ESP_GATTS_ADD_INCL_SRVC_EVT:
	case ESP_GATTS_ADD_CHAR_EVT:
	case ESP_GATTS_ADD_CHAR_DESCR_EVT:
	case ESP_GATTS_DELETE_EVT:
	case ESP_GATTS_START_EVT:
	case ESP_GATTS_STOP_EVT:
	case ESP_GATTS_OPEN_EVT:
	case ESP_GATTS_CANCEL_OPEN_EVT:
	case ESP_GATTS_CLOSE_EVT:
	case ESP_GATTS_LISTEN_EVT:
	case ESP_GATTS_CONGEST_EVT:
	case ESP_GATTS_RESPONSE_EVT:
	case ESP_GATTS_CREAT_ATTR_TAB_EVT:
	case ESP_GATTS_SET_ATTR_VAL_EVT:
	case ESP_GATTS_SEND_SERVICE_CHANGE_EVT:
	default:
		ESP_LOGI(FNAME, "Unhandled GATTS event: %d", event);
		break;
	}
}

// GAP Event Handler
static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
	switch (event)
	{
	// case ESP_GAP_BLE_ADV_REPORT_EVT:
	// {
	// 	// When a device is found during a scan
	// 	ESP_LOGI(FNAME, "Advertise report received");

	// 	// You can filter devices by address or advertisement data here
	// 	ESP_LOGI(FNAME, "Device found: %s", param->adv_report.bda);

	// 	// Example: Stop scanning after finding a device
	// 	esp_ble_gap_stop_scanning();
	// 	break;
	// }
	case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT:
	{
		// Scanning parameters were successfully set
		ESP_LOGI(FNAME, "Scan parameters set successfully");
		break;
	}
	case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT:
	{
		// Scanning started
		if (param->scan_start_cmpl.status == ESP_BT_STATUS_SUCCESS)
		{
			ESP_LOGI(FNAME, "Scan started successfully");
		}
		else
		{
			ESP_LOGE(FNAME, "Scan start failed, status: %d", param->scan_start_cmpl.status);
		}
		break;
	}
	// case ESP_GAP_BLE_CONNECT_EVT:
	// {
	// 	// When a connection is established with a client
	// 	ESP_LOGI(FNAME, "Device connected, conn_id: %d", param->connect.conn_id);
	// 	break;
	// }
	// case ESP_GAP_BLE_DISCONNECT_EVT:
	// {
	// 	// When a client disconnects from the server
	// 	ESP_LOGI(FNAME, "Device disconnected, conn_id: %d", param->disconnect.conn_id);
	// 	break;
	// }
	case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
	{
		// Connection parameters have been updated
		ESP_LOGI(FNAME, "Connection parameters updated: min_int=%d, max_int=%d, latency=%d, timeout=%d",
				 param->update_conn_params.min_int,
				 param->update_conn_params.max_int,
				 param->update_conn_params.latency,
				 param->update_conn_params.timeout);
		break;
	}
	
	default:
		ESP_LOGI(FNAME, "Unhandled GAP event: %d", event);
		break;
	}
}

// class MyTxCallbacks: public BLECharacteristicCallbacks {
// 	void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) override {
// 		// ESP_LOGI(FNAME, "onStatus Status %d code: %d", s, code );
// 		if (s == BLECharacteristicCallbacks::ERROR_GATT && code == -1) {  // upon congestion, when using notify, the status logs ERROR_GATT
// 			// ESP_LOGI(FNAME, "Congested, pacing: %d", congestion );
// 			if( congestion < MAX_CONGESTION )
// 				congestion++;
// 		}
// 		if (s == BLECharacteristicCallbacks::SUCCESS_NOTIFY && code == 0) {
// 			// ESP_LOGI(FNAME, "Send OK, pacing: %d", congestion );
// 			if( congestion )
// 				congestion--;
// 		}
// 	}
// };

// class MyServerCallbacks: public BLEServerCallbacks {
// 	void onConnect(BLEServer* pServer) {
// 		ESP_LOGI(FNAME,"BT LE device connected" );
// 		deviceConnected = true;
// 		peer_mtu = pServer->getPeerMTU(pServer->getConnId());
// 		ESP_LOGI(FNAME, "Peer MTU: %d", peer_mtu);
// 	};

// 	void onDisconnect(BLEServer* pServer) {
// 		ESP_LOGI(FNAME,"BT LE device disconnected" );
// 		deviceConnected = false;
// 		pServer->startAdvertising(); // restart advertising
// 	}
// };

// class MyRxCallbacks: public BLECharacteristicCallbacks {
// 	void onWrite(BLECharacteristic *pCharacteristic) {
// 		std::string rx = pCharacteristic->getValue();
// 		if (rx.length()) {
// 			dlb->process( rx.c_str(), rx.length(), 7 );
// 			ESP_LOGI(FNAME,">BT LE RX: %d bytes",  rx.length()  );
// 			ESP_LOG_BUFFER_HEXDUMP(FNAME,rx.c_str(), rx.length() , ESP_LOG_INFO);
// 		}
// 	}
// };

// class MyTxCallbacks: public BLECharacteristicCallbacks {
// 	void onStatus(BLECharacteristic* pCharacteristic, Status s, uint32_t code) override {
// 		// ESP_LOGI(FNAME, "onStatus Status %d code: %d", s, code );
// 		if (s == 4 && code == -1) {
// 			// ESP_LOGI(FNAME, "Congested, pacing: %d", congestion );
// 			if( congestion < 300 )
// 				congestion++;
// 		}
// 		if (s == 1 && code == 0) {
// 			// ESP_LOGI(FNAME, "Send OK, pacing: %d", congestion );
// 			if( congestion )
// 				congestion--;
// 		}
// 	}
// };

bool BLESender::selfTest(){
	ESP_LOGI(FNAME,"SerialBLE::selfTest");
	return true;
}

int BLESender::queueFull() {
	// if( wireless == WL_BLUETOOTH_LE ){
	// 	if(bt_tx_q.isFull())
	// 		return 1;
	// }
	return 0;
}

void BLESender::btTask(void *pvParameters){
	while(1) {
		// progress();
		// Router::routeBT();
		// if( uxTaskGetStackHighWaterMark( pid ) < 256 )
		// 	ESP_LOGW(FNAME,"Warning BT task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid ) );
		// vTaskDelay( 100/portTICK_PERIOD_MS );
		// tick++;
		// if( !(tick%300) ){  // keep somehow visible, a true disconnect may not take place
		// 	pServer->getAdvertising()->start();
		// }
	}
}

void BLESender::progress(){
	// char buf[256];
	// int max_packet_size = std::min(peer_mtu - 3, 250);
	// if (deviceConnected) {
	// 	int len = Router::pullBlock( bt_tx_q, buf, max_packet_size );
	// 	if( len ){
	// 		// ESP_LOGI(FNAME,"BLE len=%d P:%d, %s",len, congestion, buf);
	// 		int sent=std::min( len, 250 );
	// 		// pTxCharacteristic->setValue((uint8_t*)buf, (size_t)sent);
	// 		// pTxCharacteristic->notify(); // No return value
	// 		// ESP_LOGI(FNAME,"<BT LE TX %d bytes (pending: %d)", sent, indicationPending );
	// 		// ESP_LOG_BUFFER_HEXDUMP(FNAME,&buf[pos],len, ESP_LOG_INFO);
	// 		delay( congestion );  // slow down sender upon congestion
	// 	}
	// }
}

void BLESender::begin()
{
	ESP_LOGI(FNAME, "BLESender::begin()");
	ESP_LOGI(FNAME, "BT LE on, create BT master object");

	// esp_log_level_set("GATTS", ESP_LOG_DEBUG);
	// esp_log_level_set("BT_BTM", ESP_LOG_DEBUG);

	dlb = new DataLinkOld();
	// Create the BLE Device
	std::string ble_id(SetupCommon::getID());
	ble_id += std::string("-LE");
	// BLEDevice::init(ble_id.c_str());

	// Initialize BT
	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_bt_controller_init(&bt_cfg));
	ESP_ERROR_CHECK(esp_bt_controller_enable(ESP_BT_MODE_BLE));
	ESP_ERROR_CHECK(esp_bluedroid_init());
	ESP_ERROR_CHECK(esp_bluedroid_enable());

	// Register GAP and GATT events
	esp_ble_gatts_register_callback(gatts_event_handler);
	esp_ble_gap_register_callback(gap_event_handler);

	// // Start scanning (for example purposes, scanning for 30 seconds)
	// esp_ble_gap_set_scan_params(&(esp_ble_scan_params_t){
	// 	.scan_type = BLE_SCAN_TYPE_ACTIVE,
	// 	.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
	// 	.scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL,
	// 	.scan_interval = 0x50, // 50 ms
	// 	.scan_window = 0x30	   // 30 ms
	// });

	// ESP_ERROR_CHECK(esp_ble_gap_start_scanning(30)); // Start scanning for 30 seconds

	esp_ble_gap_set_device_name(ble_id.c_str());

	// // Create service and characteristic
	// esp_gatt_srvc_id_t service_id = {
	// 	.id = {
	// 		.uuid = {
	// 			.len = ESP_UUID_LEN_128,
	// 			.uuid = {.uuid128 = UUID_AS_HEX},
	// 		},
	// 		.inst_id = 0,
	// 	},
	// 	.is_primary = true,
	// };
	// esp_ble_gatts_create_service(gatts_if, &service_id, service_handle);

	// esp_bt_uuid_t char_uuid = {
	// 	.len = ESP_UUID_LEN_128,
	// 	.uuid = {.uuid128 = UUID_AS_HEX},
	// };
	// esp_attr_value_t tmp_attr = {
	// 	.attr_max_len = sizeof(char_value),
	// 	.attr_len = sizeof(char_value),
	// 	.attr_value = char_value,
	// };
	// esp_ble_gatts_add_char(service_handle, &char_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, char_property, &tmp_attr, NULL);
	// esp_err_t err = esp_ble_gatts_add_char(service_handle, &char_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, char_property, &char_val, NULL);

	// Start advertising
	esp_ble_adv_params_t tmp_params = {
		.adv_int_min = 0x20,
		.adv_int_max = 0x40,
		.adv_type = ADV_TYPE_IND,
		.own_addr_type = BLE_ADDR_TYPE_PUBLIC,
		.peer_addr = { 0, 0, 0, 0, 0, 0 },
		.peer_addr_type = BLE_ADDR_TYPE_PUBLIC,
		.channel_map = ADV_CHNL_ALL,
		.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
	};
	esp_ble_gap_start_advertising(&tmp_params);

	// // Create the BLE Server
	// pServer = BLEDevice::createServer();
	// pServer->setCallbacks(new MyServerCallbacks());

	// // Create the BLE Service
	// BLEService *pService = pServer->createService(SERVICE_UUID);

	// // Create a BLE Characteristic
	// pTxCharacteristic = pService->createCharacteristic(
	// 		CHARACTERISTIC_UUID_TX,
	// 		BLECharacteristic::PROPERTY_NOTIFY
	// );

	// pTxCharacteristic->addDescriptor(new BLE2902());

	// BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
	// 		CHARACTERISTIC_UUID_RX,
	// 		BLECharacteristic::PROPERTY_WRITE
	// );
	// pTxCharacteristic->setCallbacks(new MyTxCallbacks());
	// pRxCharacteristic->setCallbacks(new MyRxCallbacks());

	// BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

	// // Tune advertising parameters
	// pAdvertising->setMinInterval(0x20);  // Minimum advertising interval (32 * 0.625 ms = 20 ms)
	// pAdvertising->setMaxInterval(0x40);  // Maximum advertising interval (64 * 0.625 ms = 40 ms)

	// // Start the service
	// pService->start();

	// // Start advertising
	// pServer->getAdvertising()->start();

	xTaskCreate(&btTask, "bleTask", 4096, NULL, 12, &pid);  // stay below compass task
}
