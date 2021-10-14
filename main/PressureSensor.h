#ifndef PressureSensor_H
#define PressureSensor_H

#include "I2Cbus.hpp"
#include <cmath>
#include <hal/gpio_types.h>

class PressureSensor {
public:
	virtual bool  setSPIBus(gpio_num_t sclk, gpio_num_t mosi, gpio_num_t miso, gpio_num_t cs, uint32_t freq ) = 0;
	virtual bool  setBus( I2C_t *theBus ) = 0;
	virtual bool  begin() = 0;
	virtual bool  selfTest( float& p, float &t ) = 0;
	virtual double readPressure(bool &success) = 0;
	virtual double readTemperature( bool& success ) = 0;
	virtual double readAltitude( double qnh, bool &success ) = 0;
	virtual double calcAVGAltitudeSTD( double p ) = 0;
	virtual double calcAVGAltitude( double qnh, double p ) = 0;
    double calcPressure(double SeaLevel_Pres, double altitude) { return SeaLevel_Pres * pow(1.0 - (altitude / 44330.171), 5.255); }
};

#endif
