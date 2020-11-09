#include "MS4525DO.h"
#include <math.h>
#include <logdef.h>

MS4525DO::MS4525DO() : I2C()
{
	address = I2C_ADDRESS_MS4525DO;
	psi = 0;
	temperature = 0;
	airspeed = 0;
	P_dat = 0;  // 14 bit pressure data
	T_dat = 0;  // 11 bit temperature data
	error = ESP_OK;
}

bool MS4525DO::begin(gpio_num_t sda, gpio_num_t scl, char slave_adr) {
	init( sda, scl );
	// exponential_average = 0;
	address = slave_adr;
	return true;
}


MS4525DO::~MS4525DO()
{

}


/* public functions
        void initialize(void);
        int measure(void);              // returns status of measurement
        float getPSI(void);             // returns the PSI of last measurement
        float getTemperature(void);     // returns temperature of last measurement
        float getAirSpeed(void);        // calculates and returns the airspeed
        int calibrate(void);            // attempts to calibrate and returns a status code
 */

void MS4525DO::initialize()
{
	// i2c writes to set up addresses, etc.

}

// combine measure and collect into one function that returns the status code only, and holds the results in variables
// other subroutines for returning clean values should be get functions


int MS4525DO::measure()
{
	char ret;
	ret = fetch_pressure(P_dat, T_dat);
	// ESP_LOGI(FNAME,"MS4525DO::fetch_pressure: %d", P_dat );
	return ret;
}


char MS4525DO::fetch_pressure(uint16_t &P_dat, uint16_t &T_dat)
{
	// ESP_LOGI(FNAME,"MS4525DO::fetch_pressure");
	char _status;
	char Press_H;
	char Press_L;
	char Temp_H;
	char Temp_L;

	uint8_t data[4];
	error = read32bit( address, data );
	if( error != ESP_OK ) {
		_status = 5;   // i2c error detected
		return _status;
	}

	Press_H = data[0];
	Press_L = data[1];
	Temp_H = data[2];
	Temp_L = data[3];
	// ESP_LOG_BUFFER_HEXDUMP(FNAME,data,4, ESP_LOG_INFO);

	_status = (Press_H >> 6) & 0x03;
	Press_H = Press_H & 0x3f;
	P_dat = (((uint16_t)Press_H) << 8) | Press_L;

	Temp_L = (Temp_L >> 5);
	T_dat = (((uint16_t)Temp_H) << 3) | Temp_L;

	return _status;
}

float   MS4525DO::readPascal( float minimum, bool &ok ){
	measure();
	if( _status == 0 )
		ok=true;
	else{
		ESP_LOGI(FNAME,"Retry measure, status :%d  p=%d", _status, P_dat );
		measure();
		if( _status == 0 )
			ok=true;
		else{
			ESP_LOGI(FNAME,"Warning, status :%d  p=%d, bad even retry", _status, P_dat );
			ok=false;
		}
	}

	float _pascal = (_offset - P_dat) * multiplier * ((100.0 + speedcal.get()) / 100.0);
	    if ( (_pascal < minimum) && (minimum != 0) ) {
		  _pascal = 0.0;
		};
	return( _pascal );
}

bool    MS4525DO::selfTest( int& adval ){
	if( scan( I2C_ADDRESS_MS4525DO ) != ESP_OK ){
		ESP_LOGI(FNAME,"MS4525DO selftest, scan for I2C address %02x FAILED",I2C_ADDRESS_MS4525DO );
		return false;
	}
	ESP_LOGI(FNAME,"MS4525DO selftest, scan for I2C address %02x PASSED",I2C_ADDRESS_MS4525DO );
	measure();
	adval = P_dat;
	if( error != ESP_OK )
		return false;
	else
		return true;
}

float MS4525DO::getPSI(void){             // returns the PSI of last measurement
	// convert and store PSI
	psi=( static_cast<float>(static_cast<int16_t>(P_dat)-MS4525ZeroCounts))  / static_cast<float>(MS4525Span)* static_cast<float>(MS4525FullScaleRange);

return psi;
}             

float MS4525DO::getTemperature(void){     // returns temperature of last measurement
	temperature= (static_cast<float>(static_cast<int16_t>(T_dat)));
	temperature = (temperature / 10);   // now in deg F
	temperature = ((temperature -32) / 1.8f);   // now in deg C
	return temperature;
}

float MS4525DO::getAirSpeed(void){        // calculates and returns the airspeed
	/* Velocity calculation from a pitot tube explanation */
	/* +/- 1PSI, approximately 100 m/s */
	float rho = 1.225; // density of air
	// velocity = squareroot( (2*differential) / rho )
	float velocity;
	if (psi<0) {
		velocity = -sqrt(-(2*psi) / rho);
	}else{
		velocity = sqrt((2*psi) / rho);
	}
	velocity = velocity*10;

	return velocity;
}


bool MS4525DO::offsetPlausible(uint16_t aoffset )
{
	ESP_LOGI(FNAME,"MS4525DO offsetPlausible( %d )", aoffset );
	int lower_val = 7700;
	int upper_val = 8300;
	if( (aoffset > lower_val ) && (aoffset < upper_val )  )
		return true;
	else
		return false;
}

bool MS4525DO::doOffset( bool force ){
	ESP_LOGI(FNAME,"MS4525DO doOffset()");

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


