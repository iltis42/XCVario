#ifndef AirspeedSensor_H
#define AirspeedSensor_H

#include "I2Cbus.hpp"

class AirspeedSensor {
public:
	virtual ~AirspeedSensor() {};
	virtual bool  doOffset( bool force=false ) = 0;
	virtual float readPascal( float minimum, bool &ok ) = 0;
	virtual bool  selfTest( int& adval ) = 0;
	virtual bool  offsetPlausible( uint32_t offset ) = 0;
    virtual void  setBus( I2C_t *theBus ) = 0;
    virtual void  changeConfig() = 0;
};


#endif
