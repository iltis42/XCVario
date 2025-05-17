#pragma once

#include <driver/twai.h>

#include "InterfaceCtrl.h"

#include <string>

class DataLink;


// There is no temperature control for XCV hardware < 23, GPIO Pin there is wired to CAN slope control
#define HAS_MPU_TEMP_CONTROL (CAN && !CAN->hasSlopeSupport())

typedef enum
{ // never change, nvs item (!)
	CAN_SPEED_250KBIT,
	CAN_SPEED_500KBIT,
	CAN_SPEED_1MBIT
} CanSpeed;

class CANbus final : public InterfaceCtrl
{
private:
	CANbus();
public:
	static CANbus *createCAN();
	~CANbus() = default;
	bool begin();
	void stop();
	bool isInitialized() { return _initialized; };
	bool hasSlopeSupport() { return _slope_support; };

	// Ctrl
    InterfaceId getId() const override { return CAN_BUS; }
    const char* getStringId() const override { return "CAN"; }
    void ConfigureIntf(int cfg) override;
    int Send(const char*, int&, int) override;
    bool selfTest();

private:
	friend void TransmitTask(void *arg);
	friend void canRxTask(void *arg);

private:
    void recover();
	bool sendData(int id, const char *msg, int length, int self = 0);
	void driverInstall(twai_mode_t mode);
	void driverUninstall();
	gpio_num_t _tx_io;
	gpio_num_t _rx_io;
	gpio_num_t _slope_ctrl;
	bool _initialized = false;
	bool _slope_support = false;
	TickType_t _tx_timeout = 2; // [msec] about two times the time for 111 bit to send
};

extern CANbus *CAN;
