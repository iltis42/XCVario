#ifndef __MP5004DP_H__
#define __MP5004DP_H__

#include "ESP32NVS.h"
#include "mcp3221.h"
#include "Setup.h"
#include <math.h>
#include "AirspeedSensor.h"


const float correction    = 5000.0/4096.0;  // according to above formula, this is the relation between adc readout and Pascal

const float alpha = 0.4;
const float min_pascal = 10.0;

class MP5004DP: public AirspeedSensor {
public:
	MP5004DP() {
		_offset = 0;
	    _alpha = alpha;
	    _haveDevice = false;
	    _correction = correction;
	    changeConfig();
	}
	bool    doOffset( bool force=false );
	float   readPascal( float minimum, bool &ok );
	bool    selfTest( int& adval );
	bool    offsetPlausible( uint32_t offset );
	void    setBus( I2C_t *_theBus );
	void    changeConfig();

private:
	float _offset;
	float _alpha;
	bool _haveDevice;
	float _correction;
};

#endif


