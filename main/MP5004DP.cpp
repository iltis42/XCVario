#include "MP5004DP.h"
#include "sdkconfig.h"
#include "math.h"
#include "string"
#include "esp_task_wdt.h"
#include "DallasRmt.h"

// MP5004DP::MP5004DP(){
//
// }

bool MP5004DP::begin(gpio_num_t sda, gpio_num_t scl ){
	printf("MP5004DP::begin\n");
	bool ret = NVS.begin();
	if ( ret == false ){
		printf("MP5004DP: Error NVS init\n");
		return ret;
	}
	 else
	 	printf("MP5004DP: NVS init ok\n");
	ret = MCP.begin( sda, scl);
		if ( ret == false ){
			printf("MP5004DP: Error MCP init\n");
			return ret;
		}
	return true;
}

bool MP5004DP::selfTest(uint16_t& val)
{
	if( MCP.readRaw(val) == ESP_OK )
		return( true);
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
	printf("MP5004DP offsetPlausible( %d )\n", aoffset );
	if( (aoffset > 608 ) && (aoffset < 1067 )  )
		return true;
	else
		return false;
}

bool MP5004DP::doOffset( bool force ){
	printf("MP5004DP doOffset()\n");
	if ( !MCP.haveDevice() ){
		printf("Dont see a device, skip offset\n");
		_haveDevice=false;
		return true;
	}
	_haveDevice=true;
	printf("MP5004DP looks like have device\n");
	printf("MP5004DP key initialized\n");
	_offset = as_offset.get();

	printf("MP5004DP got offset from NVS: %0.1f\n", _offset );
	bool flying = false;
	float p;
	bool plausible = offsetPlausible( _offset );
    if( plausible ){
       printf("Offset is plausible\n");
	   p = readPascal();
	   printf("Pressure offset (Pascal): %0.1f\n",p);
       if( p > 124.0 ){   // or 50 km/h
    	  flying = true;
    	  printf("Flying state: P > 30\n");
       }
       else{
    	   printf("Not flying\n");
       }
    }

	if( force || (plausible && !flying) || !plausible )
	{
	 	printf("DP OFFSET correction ongoing, set new _offset...\n");
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
	   	   printf("Offset procedure finished, offset: %f\n", _offset);
	   	   if( as_offset.get() != _offset ){
	          as_offset.set( _offset );
  		      printf("Stored new offset in NVS\n");
	   	   }
	   	   else
	   		   printf("New offset equal old value\n");
	    }
	   	else{
	   		printf("Offset out of tolerance, ignore odd offset value\n");
	   	}
	}
	else
	{
		printf("No new Calibration: flying with plausible pressure\n");
	}
	return true;
}

float MP5004DP::readPascal( float minimum ){
	if( !_haveDevice ) {
		return 0.0;
	}
	float val = MCP.readAVG( _alpha );
	float _pascal = (val - _offset) * correction * ((100.0 + speedcal.get()) / 100.0);
    if ( (_pascal < minimum) && (minimum != 0) ) {
	  _pascal = 0.0;
	};
	return _pascal;
}

float   MP5004DP::pascal2km( float pascal )
{
    // p = 1/2 * rho * v^2
    // v = sqrt( 2*p / rho )
    float v = sqrt( 2*pascal / 1.225 );
	return v*3.6;
}

