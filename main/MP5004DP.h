#ifndef __MP5004DP_H__
#define __MP5004DP_H__

#include "ESP32NVS.h"
#include "mcp3221.h"
#include "Setup.h"
#include <math.h>


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
Vout = VS*0.2*Pkpa
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


XZG:
1)
V = 2.5V * Pascal/5000;
adc = 4096 * Vout/3.3V

   Vout = (adc/4096) * 3.3V
2)
Pkpa = Vout / (2.5V * 0.2)

   Pkpa = Vout / 0.5V

1) in 2):
Pkpa = (adc/4096) *3.3V / 0.5V
or
P = (adc/4096) * 2*3.3    in kPa
P = (adc/4096) * 6.6      in kPa
or
P = adc * (6600/4096) in Pascal
P = 1.611328125 * adc



*/


const float correction    = 5000.0/4096.0;  // according to above formula, this is the relation between adc readout and Pascal
const float correctionXZG = (6600.0/4096.0) * (141.0/147.0);

const float alpha = 0.4;
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
	bool    selfTest( int& adval );
	bool    offsetPlausible( uint16_t offset );

private:
	float _offset;
	float _alpha;
	bool _haveDevice;
};

#endif


