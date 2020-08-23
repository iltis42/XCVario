#pragma once

#define AP_SSID_HIDDEN 0
#define AP_MAX_CONNECTIONS 1
#define AP_AUTHMODE	WIFI_AUTH_OPEN		// WIFI_AUTH_WPA2_PSK // the passpharese should be atleast 8 chars long
#define AP_BEACON_INTERVAL 100			// in milli seconds
#define STATION_MAXIMUM_RETRY	10


void init_wifi_softap(void *arg);
void init_wifi_station(void *arg);
void start_dhcp_server(void);
void printStationList(void);
void print_sta_info(void *pvParam);




