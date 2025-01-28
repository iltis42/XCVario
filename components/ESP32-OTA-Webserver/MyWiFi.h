#pragma once

#define AP_SSID_HIDDEN 0
#define AP_MAX_CONNECTIONS 2
#define AP_AUTHMODE	WIFI_AUTH_WPA2_PSK		// WIFI_AUTH_WPA2_PSK // the passpharese should be atleast 8 chars long
#define AP_BEACON_INTERVAL 100			    // in milli seconds


void init_wifi_softap(void *arg);



