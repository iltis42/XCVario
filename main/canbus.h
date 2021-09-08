#pragma once
#include "driver/gpio.h"
#include "driver/twai.h"


#define CAN_BUS_TX_PIN GPIO_NUM_26
#define CAN_BUS_RX_PIN GPIO_NUM_33

class CANbus {
public:
	CANbus(){};
	~CANbus(){};
	static void begin();
	static bool sendData( int id, const char* msg, int length, int self=0 );
	static bool sendNMEA( const char* msg );
	static int receive(  int *id, char *msg, int timeout=5);
	static void tick();
	static bool selfTest();
	static int _tick;
	static bool connected() { return _connected; };
	static bool isOkay() { return can_ready; };

private:
	static void driverInstall( twai_mode_t mode, bool reinstall=false );
	static bool can_ready;
	static gpio_num_t _tx_io;
	static gpio_num_t _rx_io;
	static bool _connected;
	static int _connected_timeout;
};
