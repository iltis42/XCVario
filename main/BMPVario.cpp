#include "BMPVario.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <atomic>

const double sigmaAdjust = 255 * 2.0/33;  // 2 Vss

int BMPVario::holddown = 0;

void BMPVario::begin( BME280_ESP32_SPI *bmp, Setup* setup ) {
	_bmpTE = bmp;
	_init = true;
	_setup = setup;
}

double BMPVario::readAVGTE() {
	return _avgTE;
}

uint64_t lastrts = 0;
void BMPVario::setup() {
	_qnh = _setup->get()->_QNH;
	_analog_adj = _setup->get()->_analog_adj;
	_damping = _setup->getVarioDelay();
	_filter_len = 10;
	lastrts = esp_timer_get_time();
}


double BMPVario::readTE() {
	if ( _test )     // we are in testmode, just return what has been set
		return _TEF;
	bool success;
	bmpTemp = _bmpTE->readTemperature( success );
	// printf("BMP temp=%0.1f", bmpTemp );
	_currentAlt = _bmpTE->readAltitude(_qnh);
	uint64_t rts = esp_timer_get_time();
	float delta = (float)(rts - lastrts)/1000000.0;   // in seconds
	if( delta < 0.1 )
		return _TEF;

	// printf("Vario delta=%f\n", delta );
	lastrts = rts;
	// printf( "TE-Alt %0.1f  NM:", _currentAlt );
	if( _init  ){
		vTaskDelay(100 / portTICK_PERIOD_MS);
		_currentAlt = _bmpTE->readAltitude(_qnh) * 1.03; // we want have some beep when powerd on
		lastAltitude = _currentAlt;
		predictAlt = _currentAlt;
		Altitude = _currentAlt;
		averageAlt = _currentAlt;
		printf("Initial Alt=%0.1f\n",Altitude);
		// analogOut();  // set defaults
		_init = false;
	}
	averageAlt += (_currentAlt - averageAlt) * 0.1;
	double err = (abs(_currentAlt - predictAlt) * 1000) + 1;
	// printf("BMPVario new alt %0.1f err %0.1f\n", _currentAlt, err);
	double diff = (abs(_currentAlt - Altitude) * 1000) + 1;
	// if(diff > 1000000)  // more than 10 m alt diff in 0.1 second not plausible ( > 400 km/h vertical )
	// 	return _TEF;  // fancy errored value ignored, return last TE
	double kg = (diff / (err*_errorval + diff)) * _alpha;
	Altitude += (_currentAlt - Altitude) * kg;
	double TE = Altitude - lastAltitude;
	// printf(" TE %0.1f diff %0.1f\n", TE, diff);
	lastAltitude = Altitude;

	// printf("++++++ DELTA %0.4f\n", delta );

	TEarr[index++] = TE / delta;
	if (index >= _filter_len ) {
		index = 0;
	}
	double TEFR = 0;
	for ( int i = 0; i < _filter_len; i++) {
		TEFR += TEarr[i];
	}
	TEFR = TEFR / _filter_len;

	predictAlt = Altitude + (TEFR * delta);
	_TEF = _TEF + ((TEFR - _TEF)/ delta ) /(_damping/delta);
	// calcAnalogOut();
	_avgTE = (_TEF - _avgTE)*0.02 +_avgTE;
	if( holddown > 0 ) {
		holddown--;
	}
	else
	{
		if( _avgTE > 0.1 )  // add core climb here
		{
			averageClimb += _avgTE;
			samples++;
		}
	}
	return _TEF;
}

/*
   The average output voltage will be Vdc = VDDIO / 256 * duty + VDDIO/2,
   where VDDIO is the power supply voltage.
   VDDIO = 3.3V

   TEF = 5 m/s : out = +2.5 V
   Full swing sigma = 3.3 V  = 127
   need 2.5 = 2.5/3.3 * 255

   sigma = 255 * 2.5/3.3 * 1/2 * TE/5.0 = 2.5*255 / 33  ~ 19.31

   There is a correction of +7% applied for the current model of analog display used.
   TBD: make this configurable
   SIGMADELTA_CHANNEL_0  hardcoded GPIO_PIN18
   SIGMADELTA_CHANNEL_1  hardcoded GPIO_PIN19


 */

void BMPVario::setTE( double te ) {
	_test = true;
	_TEF = te;
   // calcAnalogOut();
}  // for testing purposes

void BMPVario::calcAnalogOut()
{
	double aTE = _TEF;
	if (aTE > 5.0) {aTE = 5.0; } // hardware limit
	if (aTE < -5.0) { aTE = -5.0; } // hardware limit
	// int8_t sigma = (int8_t)(aTE*sigmaAdjust) * ((_analog_adj+100.0)/100.0);
	// sigmadelta_set_duty( SIGMADELTA_CHANNEL_0, -sigma );
	// sigmadelta_set_duty( SIGMADELTA_CHANNEL_1, +sigma );
}

void BMPVario::analogOut( gpio_num_t negative, gpio_num_t positive ){
	_negative = negative;
	_positive = positive;
	// sigmadelta_set_pin( SIGMADELTA_CHANNEL_0, _negative );
	// sigmadelta_set_pin( SIGMADELTA_CHANNEL_1, _positive );
	// sigmadelta_set_prescale( SIGMADELTA_CHANNEL_0, 255 );
	// sigmadelta_set_prescale( SIGMADELTA_CHANNEL_1, 255 );
	// sigmadelta_set_duty( SIGMADELTA_CHANNEL_0, 127 );
	// sigmadelta_set_duty( SIGMADELTA_CHANNEL_1, 127 );
}

BMPVario bmpVario;
