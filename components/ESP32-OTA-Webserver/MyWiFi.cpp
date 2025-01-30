
#include "MyWiFi.h"

#include <esp_wifi.h>
#include <esp_mac.h>
#include <esp_log.h>
#include <esp_random.h>

#include <cstring>


extern bool netif_initialized;


void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_id == WIFI_EVENT_AP_START) {
        ESP_LOGI("Wifi", "SoftAP started");
    } else if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
        ESP_LOGI("Wifi", "Station connected: " MACSTR, MAC2STR(event->mac));
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
        ESP_LOGI("Wifi", "Station disconnected: " MACSTR, MAC2STR(event->mac));
    }
}


void init_wifi_softap(void *arg)
{
	esp_log_level_set("wifi", ESP_LOG_INFO);    // disable wifi driver logging

	if ( ! netif_initialized ) {
		ESP_ERROR_CHECK(esp_netif_init()); // can only be called once
	}
	ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_ap();

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));

	// configure the wifi connection and start the interface
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
	wifi_config_t ap_config;
	strcpy( (char *)(ap_config.ap.ssid), "ESP32 OTA" );
	strcpy( (char*)ap_config.ap.password, "xcvario-21" );
	ap_config.ap.ssid_len = 0;
	uint32_t channel = esp_random()%11;
	ap_config.ap.channel = channel;
	ap_config.ap.authmode = AP_AUTHMODE;
	ap_config.ap.ssid_hidden = AP_SSID_HIDDEN;
	ap_config.ap.max_connection = AP_MAX_CONNECTIONS;
	ap_config.ap.beacon_interval = AP_BEACON_INTERVAL;
	ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
	

	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_LOGI("Wifi", "ESP WiFi started in AP mode, channel: %d \n", (int)channel );
	
	// This can only be placed after esp_wifi_start();
	ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(80));
	
}
