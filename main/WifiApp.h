#ifndef WIFI_APP_H
#define WIFI_APP_H

#include "esp_wifi.h"
#include "esp_event.h"


class WifiApp {
	WifiApp() {};
	~WifiApp() {};

public:
	static int  queueFull();
	static int  create_socket( int port );
	static void on_client_connect( int port, int msg );
	static void socket_server(void *setup);
	static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
	static void wifi_init_softap();

private:

};

#endif
