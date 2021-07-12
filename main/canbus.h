#pragma once
#include "driver/gpio.h"
#include "driver/twai.h"

class CANbus {
public:
	CANbus(){};
	~CANbus(){};
	static void begin( gpio_num_t tx, gpio_num_t rx );
	static bool sendData( int id, const char* msg, int length, int self=0 );
	static bool sendNMEA( const char* msg );
	static int receive(  int *id, char *msg, int timeout=5);
	static void tick();
	static bool selfTest();
	static int _tick;
	static bool connected() { return _connected; };
private:
	static void driverInstall( twai_mode_t mode, bool reinstall=false );
	static bool can_ready;
	static gpio_num_t _tx_io;
	static gpio_num_t _rx_io;
	static bool _connected;
	static int _connected_timeout;
};
