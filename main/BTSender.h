
#pragma once

#include "comm/InterfaceCtrl.h"

#include <esp_spp_api.h>


class BTSender final : public InterfaceCtrl
{
public:
	BTSender() : InterfaceCtrl(true) {};
	virtual ~BTSender();

	bool start();
	void stop();
	bool selfTest();
	bool isInitialized() const { return _initialized; }
	bool isRunning() const { return _server_running; }
	bool isConnected() const { return _client_handle != 0; }

	// Ctrl
	InterfaceId getId() const override { return BT_SPP; }
	const char *getStringId() const override { return "BTspp"; }
	void ConfigureIntf(int cfg) override;
	int Send(const char *msg, int &len, int port = 0) override;

private:
	// Receiving data
	friend class BT_EVENT_HANDLER;
	uint32_t _client_handle = 0;
	bool _initialized = false;
	bool _server_running = false;
};

extern BTSender *BTspp;

