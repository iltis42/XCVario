#pragma once
#include "driver/gpio.h"

class CANbus {
public:
	CANbus(){};
	~CANbus(){};
	static void begin( gpio_num_t tx, gpio_num_t rx );
	static void send( char* msg);
	static void receive();
};
