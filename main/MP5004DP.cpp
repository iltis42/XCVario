#include "MP5004DP.h"
#include "sdkconfig.h"
#include "math.h"
#include "string"
#include "esp_task_wdt.h"
#include "DallasRmt.h"
#include <logdef.h>
#include "sensor.h"



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


bool MP5004DP::begin(gpio_num_t sda, gpio_num_t scl, char slave_adr ){
	ESP_LOGI(FNAME,"MP5004DP::begin");
	bool ret = NVS.begin();
	if ( ret == false ){
		ESP_LOGE(FNAME,"MP5004DP: Error NVS init");
		return ret;
	}
	 else
	 	ESP_LOGI(FNAME,"MP5004DP: NVS init ok");
	ret = MCP->begin();
		if ( ret == false ){
			ESP_LOGE(FNAME,"MP5004DP: Error MCP init");
			return ret;
		}
	return true;
}

bool MP5004DP::selfTest(int& val)
{
	if( MCP->selfTest() != ESP_OK ){
		return false;
	}
	val = MCP->readVal();
	if( MCP->readVal() >= 0 ) {
		return( true);
	}
	else
		return(false);
}

/*
 * Offset Voltage according to datasheet:
 *  0.45 0.6 0.75  V  @ 3V Vs
 *  15% min 20% (typ) 25% (max)
 *  @ 4096 full swing Vs adc val:
 *  614   812   1024 + add +-1% for ADC
 */

bool MP5004DP::offsetPlausible(uint16_t aoffset )
{
	ESP_LOGI(FNAME,"MP5004DP offsetPlausible( %d )", aoffset );
	int lower_val = 608;
	int upper_val = 1067;
	if( hardwareRevision.get() >= 4 ){  // XGZ sensor, measured 252 typical
		lower_val = 150;
		upper_val = 500;
	}
	if( (aoffset > lower_val ) && (aoffset < upper_val )  )
		return true;
	else
		return false;
}

bool MP5004DP::doOffset( bool force ){
	ESP_LOGI(FNAME,"MP5004DP doOffset()");
	if ( !MCP->haveDevice() ){
		ESP_LOGI(FNAME,"Dont see a device, skip offset");
		_haveDevice=false;
		return true;
	}
	_haveDevice=true;
	ESP_LOGI(FNAME,"looks like have device");
	_offset = as_offset.get();
	if( _offset < 0 )
		ESP_LOGI(FNAME,"offset not yet done: need to recalibrate" );
	else
		ESP_LOGI(FNAME,"offset from NVS: %0.1f", _offset );

	int adcval = MCP->readVal();
	ESP_LOGI(FNAME,"offset from ADC %d", adcval );

	bool plausible = offsetPlausible( adcval );
	if( plausible )
		ESP_LOGI(FNAME,"offset from ADC is plausible");
	else
		ESP_LOGI(FNAME,"offset from ADC is NOT plausible");

	int deviation = abs( _offset - adcval );
	if( deviation < 20 )
		ESP_LOGI(FNAME,"Deviation in bounds");
	else
		ESP_LOGI(FNAME,"Deviation out of bounds");

	// Long term stability of Sensor as from datasheet 0.5% per year -> 4000 * 0.005 = 20
	if( (_offset < 0 ) || ( plausible && (deviation < 20 ) ) || autozero.get() )
	{
	 	ESP_LOGI(FNAME,"Airspeed OFFSET correction ongoing, calculate new _offset...");
	 	if( autozero.get() )
	 		autozero.set(0);
	 	uint32_t rawOffset=0;
	 	for( int i=0; i<100; i++){
	 		uint16_t raw;
	 		MCP->readRaw(raw);
	 		rawOffset += raw;
	 		vTaskDelay(10 / portTICK_PERIOD_MS);
	 	}
   	    _offset = rawOffset / 100;
	   	if( offsetPlausible( _offset ) )
	   	{
	   	   ESP_LOGI(FNAME,"Offset procedure finished, offset: %f", _offset);
	   	   if( as_offset.get() != _offset ){
	          as_offset.set( _offset );
  		      ESP_LOGI(FNAME,"Stored new offset in NVS");
	   	   }
	   	   else
	   		   ESP_LOGI(FNAME,"New offset equal to value from NVS");
	    }
	   	else{
	   		ESP_LOGW(FNAME,"Offset out of tolerance, ignore odd offset value");
	   	}
	}
	else
	{
		ESP_LOGI(FNAME,"No new Calibration: flying with plausible pressure");
	}
	return true;
}

float MP5004DP::readPascal( float minimum, bool &ok ){
	ok=true;
	if( !_haveDevice ) {
		ok = false;
		return 0.0;
	}
	float corr;
	if( hardwareRevision.get() < 3 )
		corr = correction;
	else{
		corr = correctionXZG;
		// ESP_LOGI(FNAME,"pressure correction for XZG %f", corr );
	}
	float val = MCP->readVal();

	float _pascal = (val - _offset) * corr * ((100.0 + speedcal.get()) / 100.0);
    if ( (_pascal < minimum) && (minimum != 0) ) {
	  _pascal = 0.0;
	};
    // ESP_LOGI(FNAME,"pressure read %f", _pascal );
	return _pascal;
}


