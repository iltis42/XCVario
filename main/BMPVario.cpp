#include "BMPVario.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <atomic>
#include <logdef.h>
#include "S2F.h"

const double sigmaAdjust = 255 * 2.0/33;  // 2 Vss
int BMPVario::holddown = 0;

void BMPVario::begin( BME280_ESP32_SPI *bmp, S2F *aS2F  ) {
	_bmpTE = bmp;
	_init = true;
	for( int i=0; i<300; i++ )
		avClimbMin[i] = 0.0;
	for( int i=0; i<60; i++ )
		avClimbSec[i] = 0.0;
	avindexSec = 0;
	avindexMin = 0;
	_S2FTE = 0.0;
	myS2F = aS2F;
}

double BMPVario::readAVGTE() {
	return _avgTE;
}

float BMPVario::readS2FTE() {
	_S2FTE += ( (float)_TEF - _S2FTE ) * ( 1/(s2f_delay.get() * 10) );
	return _S2FTE;
}


uint64_t lastrts = 0;
void BMPVario::setup() {
	_qnh = QNH.get();
	// _analog_adj = analog_adj.get();
	_damping = vario_delay.get();
	_filter_len = 10;
	lastrts = esp_timer_get_time();
}


void BMPVario::recalcAvgClimb() {
	float ac = 0;
	int ns=1;
	for( int i=avindexMin, j=(int)(core_climb_history.get() * 60 )-1; i>=0 && j>=0; i--, j-- ) {
		// ESP_LOGI(FNAME,"MST pM= %2.2f  %d", avClimbMin[i], i  );
		if( avClimbMin[i] > core_climb_min.get() ) {
			ac += avClimbMin[i];
			ns++;
		}
	}
	float ac_sec = 0;
	int nss=0; // number of relevant samples per second
	// Look also to second values counted in last second
	for( int i=0; i<60; i++ ) {
		// ESP_LOGI(FNAME,"MST pM= %2.2f  %d", avClimbSec[i], i  );
		if( avClimbSec[i] > core_climb_min.get() ) {
			ac_sec += avClimbSec[i];
			nss++;
		}
	}
	if( nss ) {
		ac += ac_sec/nss;
		ns++;
	}
	if( ns )
		averageClimb = ac/ns;
	// ESP_LOGI(FNAME,"AVGsec:%2.2f  AVG:%2.2f", ac_sec, averageClimb );
}


double BMPVario::readTE( float tas ) {
	if ( _test )     // we are in testmode, just return what has been set
		return _TEF;
	bool success;
	bmpTemp = _bmpTE->readTemperature( success );
	// ESP_LOGI(FNAME,"BMP temp=%0.1f", bmpTemp );
	_currentAlt = _bmpTE->readAltitude(_qnh);
	if( te_comp_enable.get() ) {
		float mps = tas / 3.6;  // m/s
		float cw  = myS2F->cw( mps );
		float ealt = (((  (mps*mps)/19.62) * (1+(te_comp_adjust.get()/100.0) ))) * ( 1 - cw );  // Ekin ~ h = v²/2g  * adjust * (1-cw)
		_currentAlt += ealt;
		ESP_LOGD(FNAME,"Energiehöhe @%0.1f km/h: %0.1f cw: %f", tas, ealt, cw );
	}
	uint64_t rts = esp_timer_get_time();
	float delta = (float)(rts - lastrts)/1000000.0;   // in seconds
	if( delta < 0.075 )  // ensure every 100 mS one calculation
		return _TEF;

	// ESP_LOGI(FNAME,"Vario delta=%f", delta );
	lastrts = rts;
	// ESP_LOGI(FNAME, "TE-Alt %0.1f  NM:", _currentAlt );
	if( _init  ){
		vTaskDelay(100 / portTICK_PERIOD_MS);
		_currentAlt = _bmpTE->readAltitude(_qnh) * 1.03; // we want have some beep when powerd on
		lastAltitude = _currentAlt;
		predictAlt = _currentAlt;
		Altitude = _currentAlt;
		averageAlt = _currentAlt;
		// ESP_LOGI(FNAME,"Initial Alt=%0.1f",Altitude);
		ESP_LOGI(FNAME, "Initial Alt=%0.1f",Altitude );

		// analogOut();  // set defaults
		_init = false;
	}
	averageAlt += (_currentAlt - averageAlt) * 0.1;
	double err = (abs(_currentAlt - predictAlt) * 1000) + 1;
	// ESP_LOGI(FNAME,"BMPVario new alt %0.1f err %0.1f", _currentAlt, err);
	double diff = (abs(_currentAlt - Altitude) * 1000) + 1;
	// if(diff > 1000000)  // more than 10 m alt diff in 0.1 second not plausible ( > 400 km/h vertical )
	// 	return _TEF;  // fancy errored value ignored, return last TE
	double kg = (diff / (err*_errorval + diff)) * _alpha;
	Altitude += (_currentAlt - Altitude) * kg;
	double TE = Altitude - lastAltitude;
	// ESP_LOGI(FNAME," TE %0.1f diff %0.1f", TE, diff);
	lastAltitude = Altitude;

	// ESP_LOGI(FNAME,"++++++ DELTA %0.4f", delta );

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
	_avgTE += (_TEF - _avgTE)* (1/(10*vario_av_delay.get()));   // _av_damping in seconds, we have 10 samples per second.
	if( holddown > 0 ) {
		holddown--;
	}
	else
	{
		samples++;
		if( _TEF > 0 )
			avClimb100MSec[avindex100MSec] = _TEF;
		else
			avClimb100MSec[avindex100MSec] = 0;
		avindex100MSec++;
		if( avindex100MSec >= 10 ) {  // 0..9
			// every second
			avindex100MSec = 0;
			float ac=0;
			for( int i=0; i<10; i++ )
				if( avClimb100MSec[i] > 0 )
					ac += avClimb100MSec[i];
			avClimbSec[avindexSec] = ac/10;
			// ESP_LOGI(FNAME,"- MST pSEC= %2.2f %d", avClimbSec[avindexSec], avindexSec );
			avindexSec++;
			// every minute, or what is setup in mean climb period
			if( avindexSec >= 60 ) { // 0..59
				avindexSec = 0;
				ac=0;
				for( int i=0; i<60; i++ ) {
					ac +=  avClimbSec[i];
					avClimbSec[i] = 0;
				}
				avClimbMin[avindexMin] = ac/60;
				// ESP_LOGI(FNAME,"new MST pM= %2.2f", avClimbMin[avindexMin] );
				avindexMin++;
				if( avindexMin >= 300 ) { // overflowing, drop last h
					for( int i=60; i<300; i++ ) {
						avClimbMin[i-60] = avClimbMin[i]; // move by one h
					}
					avindexMin = 240; // one h free again
				}
			}
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
