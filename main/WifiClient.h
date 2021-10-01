#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include <string>
#include "esp_wifi.h"


const int CONNECTED_BIT = BIT0;

#define PASSPHARSE "xcvario-21"
#define PASSPHARSE_KOBO "xcvario21"
#define TCPServerIP "192.168.4.1"

class WifiClient {
	WifiClient() {};
	~WifiClient() {};

public:
	static std::string scan();
	static bool client_connected();
	static void start();
	static bool isConnected( int port=8884 );
	static void wifi_connect();
	static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);

private:
	static void tcp_client(void *pvParam);
	static void initialise_wifi(void);
	static EventGroupHandle_t wifi_event_group;

	static esp_netif_t *sta_netif;
	static std::string SSID;
	static bool cl_connected;
	// static int sock;


};

#endif
