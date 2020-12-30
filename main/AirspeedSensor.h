#ifndef AirspeedSensor_H
#define AirspeedSensor_H


class AirspeedSensor {
public:
	virtual bool  begin(gpio_num_t sda, gpio_num_t scl, char slave_adr ) = 0;
	virtual bool  doOffset( bool force=false ) = 0;
	virtual float readPascal( float minimum, bool &ok ) = 0;
	virtual bool  selfTest( int& adval ) = 0;
	virtual bool  offsetPlausible( uint16_t offset ) = 0;
};


#endif
