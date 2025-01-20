#include "BTSender.h"

#include "RingBufCPP.h"
#include "Protocols.h"
#include "Switch.h"
#include "sensor.h"
#include "Router.h"
#include "Flarm.h"
#include "DataMonitor.h"
#include "DataLink.h"
#include "comm/InterfaceCtrl.h"

#include "logdef.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

#include <esp_task_wdt.h>
#include <driver/uart.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_log.h>

#include <cstdio>
#include <string>
#include <algorithm>


BTSender *BTspp = nullptr;

static DataLinkOld *dlb;
uint32_t BTSender::_client_handle = 0;


// SPP Callback function
static void spp_event_handler(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    switch (event) {
    // case ESP_SPP_INIT_EVT:
    //     ESP_LOGI(FNAME, "SPP initialized");
    //     // Start listening for incoming connections
    //     esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, RFCOMM_SERVER_CHANNEL, SetupCommon::getID());
    //     break;

    case ESP_SPP_OPEN_EVT:
        ESP_LOGI(FNAME, "SPP connection opened, handle: %d", param->open.handle);
		BTSender::_client_handle = param->open.handle;
        break;

    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(FNAME, "SPP connection closed, handle: %d", param->close.handle);
        break;

    case ESP_SPP_DATA_IND_EVT:
        ESP_LOGI(FNAME, "Received data, handle: %d, length: %d", param->data_ind.handle, param->data_ind.len);
        // Process received data
        esp_spp_write(param->data_ind.handle, param->data_ind.len, param->data_ind.data);
        break;

    default:
        break;
    }
}

// Selftest event handler
const char TEST_DATA[] = "ESP32 SPP Self-Test";
static bool selftest_ok = false;
static void selftest_handler(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
	static uint32_t loopback_handle = 0;

	switch (event)
	{
	case ESP_SPP_INIT_EVT:
		ESP_LOGI(FNAME, "SPP initialized");

		// Start the SPP server
		esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, RFCOMM_SERVER_CHANNEL, SetupCommon::getID());
		break;

	case ESP_SPP_START_EVT:
		ESP_LOGI(FNAME, "SPP server started, handle: %d", param->start.handle);

		// Start connecting to itself (loopback test)
		esp_bd_addr_t self_address;
		memcpy(&self_address, esp_bt_dev_get_address(), sizeof(esp_bd_addr_t));
		ESP_LOGI(FNAME, "Connecting to self: %02X:%02X:%02X:%02X:%02X:%02X",
				 self_address[0], self_address[1], self_address[2],
				 self_address[3], self_address[4], self_address[5]);
		esp_spp_connect(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_MASTER, 1, self_address);
		break;

	case ESP_SPP_OPEN_EVT:
		ESP_LOGI(FNAME, "SPP connection opened, handle: %d", param->open.handle);
		loopback_handle = param->open.handle;

		// Send test data
		ESP_LOGI(FNAME, "Sending test data: %s", TEST_DATA);
		esp_spp_write(loopback_handle, strlen(TEST_DATA), (uint8_t *)TEST_DATA);
		break;

	case ESP_SPP_DATA_IND_EVT:
		ESP_LOGI(FNAME, "Data received, handle: %d, length: %d", param->data_ind.handle, param->data_ind.len);
		ESP_LOGI(FNAME, "Received data: %.*s", param->data_ind.len, param->data_ind.data);

		// Verify received data
		if (strncmp((char *)param->data_ind.data, TEST_DATA, param->data_ind.len) == 0)
		{
			ESP_LOGI(FNAME, "Self-test PASSED: Data integrity verified");
			selftest_ok = true;
		}
		else
		{
			ESP_LOGE(FNAME, "Self-test FAILED: Data mismatch");
		}

		// Close the connection after the test
		esp_spp_disconnect(loopback_handle);
		break;

	case ESP_SPP_CLOSE_EVT:
		ESP_LOGI(FNAME, "SPP connection closed, handle: %d", param->close.handle);
		break;

	case ESP_SPP_DISCOVERY_COMP_EVT:
	case ESP_SPP_CL_INIT_EVT:
	case ESP_SPP_CONG_EVT:
	case ESP_SPP_WRITE_EVT:
	case ESP_SPP_SRV_OPEN_EVT:
	case ESP_SPP_SRV_STOP_EVT:
	case ESP_SPP_VFS_REGISTER_EVT:
	case ESP_SPP_VFS_UNREGISTER_EVT:
	default:
		break;
	}
}

bool BTSender::selfTest()
{
	ESP_LOGI(FNAME,"SerialBT::selfTest");
	if( ! start(true) ){
		ESP_LOGI(FNAME,"Failed to start BTspp");
		return false;
	}
	int wait = 0;
	while ( ! selftest_ok ) {
		vTaskDelay(200 / portTICK_PERIOD_MS);
		if ( ++wait > 15 ) {
			break;
		}
	}
	esp_spp_deinit();

	if ( ! selftest_ok ) {
		ESP_LOGI(FNAME,"SerialBT::selfTest: FAILED");
	}
	return false;
}

int BTSender::queueFull()
{
	if( wireless == WL_BLUETOOTH ){
		if(bt_tx_q.isFull())
			return 1;
	}
	return 0;
}

// void BTSender::btTask(void *pvParameters){
// 	while(1) {
// 		progress();
// 		Router::routeBT();
// 		if( uxTaskGetStackHighWaterMark( pid ) < 256 )
// 			ESP_LOGW(FNAME,"Warning BT task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid ) );
// 		vTaskDelay( 20/portTICK_PERIOD_MS );
// 	}
// }

void BTSender::progress(){
	if (wireless != WL_BLUETOOTH )
		return;
	// if( !SerialBT ){
	// 	ESP_LOGI(FNAME,"SerialBT not initialized");
	// 	return;
	// }
	// char buf[400];
	// int pos = 0;
	// while(SerialBT->available() && (pos < 256) ) {
	// 	char byte = (char)SerialBT->read();
	// 	buf[pos] = byte;
	// 	pos++;
	// }
	// if( pos ){
	// 	SString rx;
	// 	rx.set( buf, pos );
	// 	dlb->process( buf, pos, 7 );
	// 	DM.monitorString( ItfTarget(BT_SPP), DIR_RX, rx.c_str(), pos );
	// 	// ESP_LOGI(FNAME,">BT RX: %d bytes", pos );
	// 	// ESP_LOG_BUFFER_HEXDUMP(FNAME,rx.c_str(),pos, ESP_LOG_INFO);
	// }
	// if( SerialBT->hasClient() ) {
	// 	int len = Router::pullBlock( bt_tx_q, buf, 400 );
	// 	if( len ){
	// 		// ESP_LOGI(FNAME,"<BT TX %d bytes", msg.length() );
	// 		// ESP_LOG_BUFFER_HEXDUMP(FNAME,msg.c_str(),msg.length(), ESP_LOG_INFO);
	// 		SerialBT->write( (const uint8_t *)buf, len );
	// 		DM.monitorString( ItfTarget(BT_SPP), DIR_TX, buf, len );
	// 	}
	// }
}

void BTSender::ConfigureIntf(int cfg)
{
}

int BTSender::Send(const char *msg, int &len, int port)
{
	if ( _client_handle ) {
		esp_err_t err = esp_spp_write(_client_handle, len, (uint8_t *)msg);
		if ( err == ESP_OK ) {
			return 0;
		}
		return 4; // wild guess
	}
	return 0;
}

bool BTSender::start(bool test)
{
    ESP_LOGI(FNAME,"BTSender::begin()" );

	if ( ! _initialized ) {
		// Initialize Bluetooth
		esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
		esp_err_t ret = esp_bt_controller_init(&bt_cfg);
		if (ret) {
			ESP_LOGE(FNAME, "Bluetooth controller initialize failed");
			return false;
		}
		ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
		if (ret) {
			ESP_LOGE(FNAME, "Bluetooth controller enable failed");
			return false;
		}

		// Initialize Bluedroid stack
		ret = esp_bluedroid_init();
		if (ret) {
			ESP_LOGE(FNAME, "Bluedroid stack initialize failed");
			return false;
		}

		ret = esp_bluedroid_enable();
		if (ret) {
			ESP_LOGE(FNAME, "Bluedroid stack enable failed");
			return false;
		}
		_initialized = true;
	}

	// Register SPP callback function
	if ( esp_spp_register_callback(test ? selftest_handler : spp_event_handler) ) {
		ESP_LOGE(FNAME, "SPP callback registration failed");
		return false;
	}

	// Initialize SPP
	if ( esp_spp_init(ESP_SPP_MODE_CB) ) {
		ESP_LOGE(FNAME, "SPP initialization failed");
		return false;
	}

	ESP_LOGI(FNAME, "SPP server started and waiting for connections...");

	// ESP_LOGI(FNAME,"BT on, create BT master object" );
	// dlb = new DataLinkOld();
	// SerialBT = new BluetoothSerial();
	// SerialBT->begin( SetupCommon::getID() );
	// xTaskCreatePinnedToCore(&btTask, "btTask", 4096, NULL, 12, &pid, 0);  // stay below compass task
	return true;
}

void BTSender::stop()
{
	if ( _initialized ) {
		esp_bt_controller_deinit();
		esp_spp_deinit();
	}
}
