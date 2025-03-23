#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#include <esp_wifi.h>
#include <freertos/event_groups.h>
#include <string>


const int CONNECTED_BIT = BIT0;


#define PASSPHARSE_KOBO "xcvario21"
#define TCPServerIP "192.168.4.1"

class WifiClient {
	WifiClient() {};
	~WifiClient() {};

public:
	static std::string scan(int master_xcv_num);
	static bool client_connected();
	static void start();
	static bool isConnected( int port=8884 );
	static void wifi_connect();
	static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data);
	static int getScannedMasterXCV() { return master_xcvario_scanned; };

private:
	static void tcp_client(void *pvParam);
	static void initialise_wifi(void);
	static EventGroupHandle_t wifi_event_group;

	static esp_netif_t *sta_netif;
	static std::string SSID;
	static bool cl_connected;
	static int master_xcvario_scanned; // (XCVario-)  >1234<
	// static int sock;


};

#endif
