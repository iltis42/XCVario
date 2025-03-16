#pragma once

#include <esp_wifi.h>
#include <esp_event.h>
#include <cstring>
#include "comm/InterfaceCtrl.h"
#include "comm/DataLink.h"
#include "RingBufCPP.h"
#include "SString.h"
#include <list>

class WifiApp;

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
	xcv_sock_server(int p, WifiApp* mw) : port(p), mywifi(mw) {};

	RingBufCPP<SString>* txq;
	// This should store the handle to send data to the port
	const int port;
	int idle = 0;
	TaskHandle_t pid = nullptr;
	std::list<client_record_t>  clients;
	WifiApp *mywifi;
}sock_server_t;

class WifiApp final : public InterfaceCtrl
{
	friend class WIFI_EVENT_HANDLER;

public:
	WifiApp();
	~WifiApp();

public:
    // Ctrl
    InterfaceId getId() const override { return WIFI; }
    const char* getStringId() const override { return "WiFi"; }
    void ConfigureIntf(int port) override;                              // 8880, 8881, 8882, 8883
	virtual int Send(const char *msg, int &len, int port=0) override;

	// returns 1 if queue is full, changes color of WiFi symbol, connection is stuck
	static int queueFull();

private:
	static bool full[4];
	sock_server_t *_socks[4];

	// internal functionality
	void wifi_init_softap();
};

extern WifiApp *Wifi;
