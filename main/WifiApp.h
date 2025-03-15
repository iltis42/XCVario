#ifndef WIFI_APP_H
#define WIFI_APP_H

#include <esp_wifi.h>
#include <esp_event.h>
#include <cstring>
#include "comm/InterfaceCtrl.h"
#include "RingBufCPP.h"
#include <list>
#include "DataLink.h"

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

typedef struct client_record {
	int client;
	int retries;
}client_record_t;

typedef struct xcv_sock_server {
	RingBufCPP<SString>* txq;
	int port;
	int idle;
	TaskHandle_t pid;
	std::list<client_record_t>  clients;
	std::map<int, DataLink*> *dl;
}sock_server_t;

class WifiApp final : public InterfaceCtrl  {
	WifiApp();
	~WifiApp();

public:
    // Ctrl
    InterfaceId getId() const override { return _intfid; }
    const char* getStringId() const override { return _id_memo; }
    void ConfigureIntf(int cfg) override;                              // 0:8880, 1:8881, 2:8882, 3:8883
	virtual int Send(const char *msg, int &len, int port=0) override;

	// returns 1 if queue is full, changes color of WiFi symbol, connection is stuck
	static int queueFull();

	// WiFi Task
	static void socket_server(void *setup);

private:
	static bool full[4];
	const InterfaceId   _intfid;
	const char*         _id_memo;
	sock_server_t       _socks;
	char taskname[20];

	// internal functionality
	void wifi_init_softap();
	static int  create_socket( int port );
	static void on_client_connect( int port, int msg );
	static void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
};

#endif
