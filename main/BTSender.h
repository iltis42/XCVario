
#pragma once

#include "comm/InterfaceCtrl.h"

#include <esp_spp_api.h>


#define RFCOMM_SERVER_CHANNEL 1
#define HEARTBEAT_PERIOD_MS 50
#define SPP_SERVICE_BUFFER_SIZE 1024

class BTSender final : public InterfaceCtrl
{
public:
	BTSender() : InterfaceCtrl(true) {};
	virtual ~BTSender() {};

	// Ctrl
	InterfaceId getId() const override { return BT_SPP; }
	const char *getStringId() const override { return "BTser"; }
	void ConfigureIntf(int cfg) override;
	int Send(const char *msg, int &len, int port = 0) override;

	bool start(bool test=false);
	void stop();
	int queueFull();
	bool isConnected() { return _client_handle != 0; }
	void progress(); // progress loop
	bool selfTest(); // call 3 seconds after begin

// private:
	// Receiving data
	// friend void spp_event_handler(esp_spp_cb_event_t, esp_spp_cb_param_t);
	// friend void selftest_handler(esp_spp_cb_event_t, esp_spp_cb_param_t);
	static uint32_t _client_handle;
	bool _initialized = false;

};

extern BTSender *BTspp;

