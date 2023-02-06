#include "ABPMRR.h"
#include <math.h>
#include <logdef.h>


ABPMRR::ABPMRR()
{
	address = I2C_ADDRESS_ABPMRR;
	psi = 0;
	temperature = 0;
	airspeed = 0;
	P_dat = 0;  // 14 bit pressure data
	T_dat = 0;  // 11 bit temperature data
	error = ESP_OK;
	_status = 0;
	_offset = 0;
	bus = 0;
}

ABPMRR::~ABPMRR()
{
}

// combine measure and collect into one function that returns the status code only, and holds the results in variables
// other subroutines for returning clean values should be get functions


int ABPMRR::measure()
{
	char ret;
	ret = fetch_pressure(P_dat, T_dat);
	// ESP_LOGI(FNAME,"ABPMRR::fetch_pressure: %d", P_dat );
	return ret;
}

// #define RANDOM_TEST

char ABPMRR::fetch_pressure(uint16_t &P_dat, uint16_t &T_dat)
{
	// ESP_LOGI(FNAME,"ABPMRR::fetch_pressure");
	char _status;
	char Press_H;
	char Press_L;
	char Temp_H;
	char Temp_L;

	uint8_t data[4];
	esp_err_t err = bus->readBytes(address, 0, 4, data );
	if( err != ESP_OK ) {
		_status = 5;   // i2c error detected
		ESP_LOGW(FNAME,"fetch_pressure() I2C error");
		return _status;
	}

	Press_H = data[0];
	Press_L = data[1];
	Temp_H = data[2];
	Temp_L = data[3];

#ifdef RANDOM_TEST
	Press_H = esp_random() % 255;
	Press_L = esp_random() % 255;
	Temp_L = esp_random() % 255;
#endif

	// ESP_LOG_BUFFER_HEXDUMP(FNAME,data,4, ESP_LOG_INFO);

	_status = (Press_H >> 6) & 0x03;
	Press_H = Press_H & 0x3f;
	P_dat = (((uint16_t)Press_H) << 8) | Press_L;

	Temp_L = (Temp_L >> 5);
	T_dat = (((uint16_t)Temp_H) << 3) | Temp_L;

	// ESP_LOGI(FNAME,"fetch_pressure() status: %d, err %d,  P:%04x T: %04x",  _status, err, P_dat, T_dat );
	return _status;
}

float   ABPMRR::readPascal( float minimum, bool &ok ){
	measure();
	if( _status == 0 )
		ok=true;
	else{
		ESP_LOGE(FNAME,"Retry measure, status :%d  p=%d", _status, P_dat );
		measure();
		if( _status == 0 )
			ok=true;
		else{
			ESP_LOGE(FNAME,"Warning, status :%d  p=%d, bad even retry", _status, P_dat );
			ok=false;
		}
	}

	float _pascal = (P_dat - _offset) * ABPMRRmultiplier * ((100.0 + speedcal.get()) / 100.0);
	    if ( (_pascal < minimum) && (minimum != 0) ) {
		  _pascal = 0.0;
		};

	// ESP_LOGI(FNAME,"pressure: %f offset: %d raw: %d  raw-off:%f m:%f", _pascal, (int)_offset, P_dat,  (_offset - P_dat),  ABPMRRmultiplier );
	return( _pascal );
}

bool    ABPMRR::selfTest( int& adval ){
	uint8_t data[4];
	esp_err_t err = ESP_FAIL;
	for( int i=0; i<4; i++ ){
		err = bus->readBytes(address, 0, 4, data );
		if( err == ESP_OK )
			break;
	}
	if( err != ESP_OK ){
		ESP_LOGI(FNAME,"ABPMRR selftest, scan for I2C address %02x FAILED",I2C_ADDRESS_ABPMRR );
		return false;
	}
	ESP_LOGI(FNAME,"ABPMRR selftest, scan for I2C address %02x PASSED",I2C_ADDRESS_ABPMRR );
	measure();
	adval = P_dat;
	if( error != ESP_OK )
		return false;
	else
		return true;
}

float ABPMRR::getPSI(void){             // returns the PSI of last measurement
	// convert and store PSI
	psi=( static_cast<float>(static_cast<int16_t>(P_dat)-ABPMRRZeroCounts))  / static_cast<float>(ABPMRRSpan)* static_cast<float>(ABPMRRFullScaleRange);

return psi;
}             

float ABPMRR::getTemperature(void){     // returns temperature of last measurement
	temperature= (static_cast<float>(static_cast<int16_t>(T_dat)));
	temperature = (temperature / 10);   // now in deg F
	temperature = ((temperature -32) / 1.8f);   // now in deg C
	return temperature;
}

float ABPMRR::getAirSpeed(void){        // calculates and returns the airspeed in m/s IAS
	/* Velocity calculation from a pitot tube explanation */
	/* +/- 1PSI, approximately 100 m/s */
	const float rho = 1.225/(2.0*100); // density of air plus multiplier
	// velocity = sqrt( (2*psi) / rho )   or sqt( psi /
	float velocity sqrt(psi/rho);
	if (psi<0)
		velocity = -velocity;
	return velocity;
}


bool ABPMRR::offsetPlausible(uint16_t aoffset )
{
	ESP_LOGI(FNAME,"ABPMRR offsetPlausible( %d )", aoffset );
	int lower_val = 8192-200;
	int upper_val = 8192+200;
	if( (aoffset > lower_val ) && (aoffset < upper_val )  )
		return true;
	else
		return false;
}

bool ABPMRR::doOffset( bool force ){
	ESP_LOGI(FNAME,"ABPMRR doOffset()");

	_offset = as_offset.get();
	if( _offset < 0 )
		ESP_LOGI(FNAME,"offset not yet done: need to recalibrate" );
	else
		ESP_LOGI(FNAME,"offset from NVS: %0.1f", _offset );

	uint16_t adcval,T;
	fetch_pressure( adcval, T );

	ESP_LOGI(FNAME,"offset from ADC %d", adcval );

	bool plausible = offsetPlausible( adcval );
	if( plausible )
		ESP_LOGI(FNAME,"offset from ADC is plausible");
	else
		ESP_LOGI(FNAME,"offset from ADC is NOT plausible");

	int deviation = abs( _offset - adcval );
	if( deviation < MAX_AUTO_CORRECTED_OFFSET )
		ESP_LOGI(FNAME,"Deviation in bounds");
	else
		ESP_LOGI(FNAME,"Deviation out of bounds");

	// Long term stability of Sensor as from datasheet 0.5% per year -> 4000 * 0.005 = 20
	if( (_offset < 0 ) || ( plausible && (deviation < MAX_AUTO_CORRECTED_OFFSET ) ) || autozero.get() )
	{
	 	ESP_LOGI(FNAME,"Airspeed OFFSET correction ongoing, calculate new _offset...");
	 	if( autozero.get() )
	 		autozero.set(0);
	 	uint32_t rawOffset=0;
	 	for( int i=0; i<100; i++){
	 		fetch_pressure( adcval, T );
	 		rawOffset += adcval;
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


