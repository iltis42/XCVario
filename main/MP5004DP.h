#ifndef __MP5004DP_H__
#define __MP5004DP_H__

#include "ESP32NVS.h"
#include "mcp3221.h"
#include "Setup.h"


/*
Analog / Digital conversion MP3V5004DP:
- MP3V5004DP Vout -> (0.2*VS) - VS = 0.66..3.3V
- MCP3221    0...4095 (12 bit)
- VS = 3.3 V

1)
V = VS*[(0.2*P) +0.2]
adc = 4096* Vout/VS
Vout = (adc/4096) * VS

2)
after offset compensation:  ( P in kPA )
Vout = VS*0.2*P
P = Vout / VS*0.2


1) in 2)
P = (adc / 4096)*VS / VS*0.2
VS surplus:
P = (adc /4096) / 0.2
or
P = adc * 5/4096     (kPa)
or in Pascal:
P = 5000/4096 * adc

100 mm H2O:  P = 1000 Pascal o. 0.6 V o. 0.2*4096 = 819.2

*/


const float correction = 5000.0/4096.0;  // according to above formula, this is the relation between adc readout and Pascal
														 // 1000/1026 is a factory correction.
const float alpha = 0.2;
const float min_pascal = 10.0;

class MP5004DP {
public:
	MP5004DP() {
		_offset = 0;
	    _alpha = alpha;
	    _haveDevice = false;
	}
	bool    begin(gpio_num_t sda, gpio_num_t scl );
	bool    doOffset( bool force=false );
	float   readPascal( float minimum=min_pascal );
	float   pascal2km( float pascal );
	bool    selfTest(uint16_t& adval );
	bool    offsetPlausible( uint16_t offset );

private:
	float _offset;
	float _alpha;
	bool _haveDevice;
};

#endif


