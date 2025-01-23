#include "BTspp.h"
#include "SetupNG.h"

#include "comm/DataLink.h"
#include "comm/InterfaceCtrl.h"

#include "logdefnone.h"

#include <driver/uart.h>
#include <esp_bt.h>
#include <esp_bt_main.h>
#include <esp_bt_device.h>
#include <esp_gap_bt_api.h>


constexpr int RFCOMM_SERVER_CHANNEL = 1;
// #define HEARTBEAT_PERIOD_MS 50
// #define SPP_SERVICE_BUFFER_SIZE 1024


BTSender *BTspp = nullptr;

class BT_EVENT_HANDLER
{
public:

// SPP Callback function
static void spp_event_handler(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
	// assert(BTspp) this is 
    switch (event) {
    case ESP_SPP_INIT_EVT:
		ESP_LOGI(FNAME, "SPP initialized");
		// Start listening for incoming connections
		esp_spp_start_srv(ESP_SPP_SEC_AUTHENTICATE, ESP_SPP_ROLE_SLAVE, RFCOMM_SERVER_CHANNEL, SetupCommon::getID());
		esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
		BTspp->_server_running = true;
		break;

	case ESP_SPP_SRV_STOP_EVT:
		ESP_LOGI(FNAME, "SPP server stoped");
		BTspp->_server_running = true;
		break;

	case ESP_SPP_SRV_OPEN_EVT:
		ESP_LOGI(FNAME, "SPP rcomm opened, handle: %d", param->open.handle);
		BTspp->_client_handle = param->open.handle;
		esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
		break;

	// never received this for rfcomm channels
	// case ESP_SPP_OPEN_EVT:
	// 	ESP_LOGI(FNAME, "SPP connection opened, handle: %d", param->open.handle);
	// 	BTspp->_client_handle = param->open.handle;
	// 	esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_NON_DISCOVERABLE);
	// 	break;

	case ESP_SPP_CLOSE_EVT:
		ESP_LOGI(FNAME, "SPP connection closed, handle: %d", param->close.handle);
		BTspp->_client_handle = 0;
		esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
		break;

	case ESP_SPP_DATA_IND_EVT:
	{
		ESP_LOGI(FNAME, "Received data, handle: %d, length: %d", param->data_ind.handle, param->data_ind.len);
		// Process received data
		int count = param->data_ind.len;
		char *rxBuf = (char *)param->data_ind.data;
		if (count > 0)
		{
			rxBuf[count] = '\0';

			auto dlit = BTspp->_dlink.begin();
			if ( dlit != BTspp->_dlink.end() ) {
				dlit->second->process(rxBuf, count);
			}
		}
		// esp_spp_write(param->data_ind.handle, param->data_ind.len, param->data_ind.data);
		break;
	}
	default:
		break;
	}
}
};

BTSender::~BTSender()
{
	stop();
}

bool BTSender::selfTest()
{
	return isRunning();
}

void BTSender::ConfigureIntf(int cfg)
{
	// maybe fine like this
}

int BTSender::Send(const char *msg, int &len, int port)
{
	if ( _client_handle ) {
		// ESP_LOGI(FNAME,"Send BTspp, len %d", len);
		esp_err_t err = esp_spp_write(_client_handle, len, (uint8_t *)msg);
		if ( err == ESP_OK ) {
			return 0;
		}
		return 4; // wild guess
	}
	return 0;
}

bool BTSender::start()
{
    ESP_LOGI(FNAME,"BTSender::begin()" );

	if ( ! _initialized ) {
		// Initialize Bluetooth
		esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
		bt_cfg.mode = 2;
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

		// Set the Bluetooth device name
		// ret = esp_bt_gap_set_device_name("XCVario");
		ret = esp_bt_dev_set_device_name("XCVario");
		if (ret != ESP_OK)
		{
			ESP_LOGE(FNAME, "Failed to set device name: %s", esp_err_to_name(ret));
		}
		_initialized = true;
	}

	// Register SPP callback function
	if ( esp_spp_register_callback(BT_EVENT_HANDLER::spp_event_handler) ) {
		ESP_LOGE(FNAME, "SPP callback registration failed");
		return false;
	}

	// Initialize SPP
	if ( esp_spp_init(ESP_SPP_MODE_CB) ) {
		ESP_LOGE(FNAME, "SPP initialization failed");
		return false;
	}
	ESP_LOGI(FNAME, "SPP server started and waiting for connections...");

	return true;
}

void BTSender::stop()
{
	if ( _initialized ) {
		esp_spp_deinit();
		esp_bt_controller_deinit();
		_initialized = false;
		_server_running = false;
	}
}
