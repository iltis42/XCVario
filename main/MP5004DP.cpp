#include "MP5004DP.h"
#include "sdkconfig.h"
#include "math.h"
#include "string"
#include "esp_task_wdt.h"
#include "DallasRmt.h"
#include <logdef.h>
#include "sensor.h"

// MP5004DP::MP5004DP(){
//
// }

bool MP5004DP::begin(gpio_num_t sda, gpio_num_t scl ){
	ESP_LOGI(FNAME,"MP5004DP::begin");
	bool ret = NVS.begin();
	if ( ret == false ){
		ESP_LOGE(FNAME,"MP5004DP: Error NVS init");
		return ret;
	}
	 else
	 	ESP_LOGI(FNAME,"MP5004DP: NVS init ok");
	ret = MCP.begin( sda, scl);
		if ( ret == false ){
			ESP_LOGE(FNAME,"MP5004DP: Error MCP init");
			return ret;
		}
	return true;
}

bool MP5004DP::selfTest(int& val)
{
	if( MCP.selfTest() != ESP_OK ){
		return false;
	}
	val = MCP.readVal();
	if( MCP.readVal() >= 0 ) {
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
	if( hardwareRevision == 3 ){  // XGZ sensor, measured 252 typical
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
	if ( !MCP.haveDevice() ){
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

	int adcval = MCP.readVal();
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
	 		MCP.readRaw(raw);
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

float MP5004DP::readPascal( float minimum ){
	if( !_haveDevice ) {
		return 0.0;
	}
	float corr;
	if( hardwareRevision < 3 )
		corr = correction;
	else{
		corr = correctionXZG;
		// ESP_LOGI(FNAME,"pressure correction for XZG %f", corr );
	}
	float val = MCP.readVal();
	float _pascal = (val - _offset) * corr * ((100.0 + speedcal.get()) / 100.0);
    if ( (_pascal < minimum) && (minimum != 0) ) {
	  _pascal = 0.0;
	};
	return _pascal;
}


