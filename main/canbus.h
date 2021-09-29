#pragma once
#include "driver/gpio.h"
#include "driver/twai.h"


#define CAN_BUS_TX_PIN GPIO_NUM_26
#define CAN_BUS_RX_PIN GPIO_NUM_33

class SString;

class CANbus {
public:
	CANbus(){};
	~CANbus(){};
	static void begin();
	static void restart();
	static bool sendNMEA( const char* msg );
	static void tick();
	static bool selfTest();
	static int _tick;
	static bool connectedXCV() { return _connected_xcv; };
	static bool connectedMagSens() { return _connected_magsens; };

	static bool isOkay() { return _ready_initialized; };

private:
	static bool sendData( int id, const char* msg, int length, int self=0 );
	static int receive(  int *id, SString& msg, int timeout=5);
	static void driverInstall( twai_mode_t mode );
	static void on_can_connect( int msg );
	static void driverUninstall();
	static bool _ready_initialized;
	static gpio_num_t _tx_io;
	static gpio_num_t _rx_io;
	static bool _connected_magsens;
	static bool _connected_xcv;
	static int _connected_timeout_magsens;
	static int _connected_timeout_xcv;
    static bool _master_present;
    static int dataIndex;
};
