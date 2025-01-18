#ifndef WIFI_APP_H
#define WIFI_APP_H

#include <esp_wifi.h>
#include <esp_event.h>
#include <cstring>

typedef struct str_wireless_id {
	char id[10];
	str_wireless_id() {};
	str_wireless_id( const char *val ) { strcpy( id, val ); };
	str_wireless_id(  const str_wireless_id &val ) { strcpy( id, val.id ); };
	bool operator == ( const struct str_wireless_id &other ) const {
		return( strcmp( id, other.id ) == 0 );
    };
	struct str_wireless_id operator = ( const struct str_wireless_id &other ) {
			strcpy( id, other.id );
			return *this;
	};
	struct str_wireless_id operator = ( const char * other ) {
			strcpy( id, other );
			return *this;
	};
}t_wireless_id;

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
