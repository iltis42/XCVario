#pragma once

#include <driver/twai.h>

#include "InterfaceCtrl.h"

#include <string>

class DataLink;

#define CAN_NMEA_ID_MASTER 0x20
#define CAN_CONFIG_ID_MASTER 0x21
#define CAN_KEEPALIVE_ID_MASTER 0x11

#define CAN_NMEA_ID_CLIENT 0x22
#define CAN_CONFIG_ID_CLIENT 0x23
#define CAN_KEEPALIVE_ID_CLIENT 0x12

#define CAN_MAGSENS_ID 0x31

typedef enum
{ // never change, nvs item (!)
	CAN_SPEED_OFF,
	CAN_SPEED_250KBIT,
	CAN_SPEED_500KBIT,
	CAN_SPEED_1MBIT
} CanSpeed;

class SString;

class CANbus final : public InterfaceCtrl
{
public:

	CANbus();
    ~CANbus();
	bool begin();
	void stop();
	bool GotNewClient() const { return _new_can_client_connected; }
	void ResetNewClient() { _new_can_client_connected = false; }
	bool connectedXCV() { return _connected_xcv; };
	bool connectedMagSens() { return _connected_magsens; };
	bool connected() { return (_connected_xcv | _connected_magsens); };
	bool isInitialized() { return _initialized; };
	bool hasSlopeSupport() { return _slope_support; };
	// Ctrl
    const char* getId() const  override { return "CAN"; }
	void ConfigureIntf(int cfg) override;
	bool Send(const char *, int, int) override;

private:
	friend void TransmitTask(void *arg);
	friend void canRxTask(void *arg);

private:
    void recover();
    bool selfTest();
	bool sendData(int id, const char *msg, int length, int self = 0);
	void driverInstall(twai_mode_t mode);
	void driverUninstall();
	gpio_num_t _tx_io;
	gpio_num_t _rx_io;
	gpio_num_t _slope_ctrl;
	bool _initialized = false;
	bool _connected_magsens = false;
	bool _connected_xcv = false;
	bool _slope_support = false;
	bool _new_can_client_connected = false;
	bool _keep_alive_fails = false;
	bool _send_nmea_fails = false;
	int _connected_timeout_magsens = 0;
	int _connected_timeout_xcv = 0;
	TickType_t _tx_timeout = 2; // [msec] about two times the time for 111 bit to send
};

extern CANbus *CAN;
