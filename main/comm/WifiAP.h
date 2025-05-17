#pragma once

#include "comm/InterfaceCtrl.h"
#include "comm/DataLink.h"

#include <esp_wifi.h>
#include <esp_event.h>
#include <lwip/sockets.h>

#include <cstring>
#include <list>

extern bool netif_initialized;

class WifiAP;

#define NUM_TCP_PORTS 4

typedef struct client_record {
	int client;
	int retries;
	struct sockaddr_in clientAddress;
}client_record_t;

struct sock_server_t {
	sock_server_t(int p) : port(p) {};
	// This should store the handle to send data to the port
	const int port;
	int socket;
	int idle = 0;
	std::list<client_record_t> clients;
};

class WifiAP final : public InterfaceCtrl
{
	friend class WIFI_EVENT_HANDLER;

private:
	WifiAP();
public:
	static WifiAP *createWifiAP();
	~WifiAP();

public:
    // Ctrl
    InterfaceId getId() const override { return WIFI_AP; }
    const char* getStringId() const override { return "WiFi"; }
    void ConfigureIntf(int port) override;                              // 8880, 8881, 8882, 8883
	virtual int Send(const char *msg, int &len, int port=0) override;

	// returns 1 if queue is full, changes color of WiFi symbol, connection is stuck
	int queueFull();

private:
	bool full[NUM_TCP_PORTS];
	sock_server_t *_socks[NUM_TCP_PORTS];
	TaskHandle_t socket_server_task_pid = nullptr;
	bool _terminte_sock_server = false;
	esp_netif_t *_netif = nullptr;
	esp_event_handler_instance_t _evnt_handler = nullptr;

	// internal functionality
	void initialize_wifi(bool ap_mode, bool ota, const char* ssid);
};

extern WifiAP *Wifi;
