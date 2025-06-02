#pragma once

#include "comm/InterfaceCtrl.h"
#include "comm/DataLink.h"

#include <esp_wifi.h>
#include <esp_event.h>
#include <lwip/sockets.h>

#include <string>
#include <list>

extern bool netif_initialized;

class WifiApSta;

#define NUM_TCP_PORTS 5

typedef struct client_record {
	int peer;
	int retries;
	struct sockaddr_in clientAddress;
}peer_record_t;

struct sock_server_t {
	sock_server_t(int p) : port(p) {};
	int create_ap_socket();
	int connect_sta_socket(esp_netif_ip_info_t *ip_info);
	// This should store the handle to send data of the port
	const int port;
	bool is_ap = true;
	int sock_hndl = -1;
	// int idle = 0;
	std::list<peer_record_t> peers;
	// bool connected = false;
	bool full = true;
};

class WifiApSta final : public InterfaceCtrl
{
	friend class WIFI_EVENT_HANDLER;

private:
	WifiApSta();
public:
	static WifiApSta *createWifiApSta();
	~WifiApSta();

public:
	// Ctrl
	InterfaceId getId() const override { return WIFI_APSTA; }
	const char *getStringId() const override { return "WiFi"; }
	void ConfigureIntf(int port) override; // 8880, 8881, 8882, 8883, 8884, 80
	virtual int Send(const char *msg, int &len, int port = 0) override;

	// returns 1 if queue is full, changes color of WiFi symbol, connection is stuck
	int queueFull();
	bool isConnected(int p) const { return true; } // fixme
	bool scanMaster(int master_xcv_num);

private:
	sock_server_t *_socks[NUM_TCP_PORTS];
	TaskHandle_t socket_server_task_pid = nullptr;
	bool _terminte_sock_server = false;
	esp_netif_t *_ap_netif = nullptr;
	esp_netif_t *_sta_netif = nullptr;
	esp_event_handler_instance_t _evnt_handler = nullptr;

	// internal functionality
	void initialize_wifi(bool ap_mode, int maxcon, const char* ssid);
	void client_connect();
};

extern WifiApSta *WIFI;
