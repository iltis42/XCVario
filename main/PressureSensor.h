#pragma once

#include "I2Cbus.hpp"
#include <hal/gpio_types.h>

class PressureSensor {
public:
	virtual bool  setSPIBus(gpio_num_t sclk, gpio_num_t mosi, gpio_num_t miso, gpio_num_t cs, uint32_t freq ) = 0;
	virtual bool  setBus( I2C_t *theBus ) = 0;
	virtual bool  begin() = 0;
	virtual bool  selfTest( float& p, float &t ) = 0;
	virtual float readPressure(bool &success) = 0;
	virtual float readTemperature( bool& success ) = 0;
	virtual float readAltitude( float qnh, bool &success ) = 0;
};

