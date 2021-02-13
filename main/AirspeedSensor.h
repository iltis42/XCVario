#ifndef AirspeedSensor_H
#define AirspeedSensor_H

#include "I2Cbus.hpp"

class AirspeedSensor {
public:
	virtual bool  doOffset( bool force=false ) = 0;
	virtual float readPascal( float minimum, bool &ok ) = 0;
	virtual bool  selfTest( int& adval ) = 0;
	virtual bool  offsetPlausible( uint16_t offset ) = 0;
    virtual void  setBus( I2C_t *theBus ) = 0;
};


#endif
