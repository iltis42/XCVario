#include "BMPVario.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <atomic>
#include <logdef.h>
#include "S2F.h"
#include "AverageVario.h"
#include "sensor.h"

const double sigmaAdjust = 255 * 2.0/33;  // 2 Vss
int BMPVario::holddown = 0;

void BMPVario::begin( PressureSensor *te, PressureSensor *baro, S2F *aS2F  ) {
	_sensorTE = te;
	_sensorBARO = baro;
	_init = true;

	_S2FTE = 0.0;
	myS2F = aS2F;
}

double BMPVario::readAVGTE() {
	return _avgTE;
}

float BMPVario::readS2FTE() {
	return _TEF;
}


uint64_t lastrts = 0;
void BMPVario::setup() {
	_qnh = Units::Qnh( QNH.get() );
	_damping = vario_delay.get();
	_filter_len = 10;
	lastrts = esp_timer_get_time();
}


double BMPVario::readTE( float tas ) {
	if ( _test )     // we are in testmode, just return what has been set
		return _TEF;
	bool success;
	// Latency supervision and correction
	uint64_t rts = esp_timer_get_time();
	float delta = (float)(rts - lastrts)/1000000.0;   // in seconds
	if( delta < 0.095 ) {   // ensure delta is 100 mS at least
		int addwait = (int)(100.0-delta*1000);
		// ESP_LOGW(FNAME, "Too short TE time delta <95 mS: %4.1f, add wait %d ", delta*1000, addwait );
		delay( addwait );
		rts = esp_timer_get_time();
		delta = (float)(rts - lastrts)/1000000.0;
	}
	lastrts = rts;
	if( delta < 0.090 || delta > 0.2 )
		ESP_LOGW(FNAME,"Vario delta=%2.3f sec", delta );

	bmpTemp = _sensorTE->readTemperature( success );
	// ESP_LOGI(FNAME,"BMP temp=%0.1f", bmpTemp );
	if( te_comp_enable.get() ) {
		_currentAlt = altitude.get(); // already read
		if( !success )
			_currentAlt = lastAltitude;  // ignore readout when failed
		float mps = tas / 3.6;  // m/s
		float cw  = myS2F->cw( mps );
		float ealt = (((  (mps*mps)/19.62) * (1+(te_comp_adjust.get()/100.0) ))) * ( 1 - cw );  // Ekin ~ h = v²/2g  * adjust * (1-cw)
		_currentAlt += ealt;
		ESP_LOGD(FNAME,"Energiehöhe @%0.1f km/h: %0.1f cw: %f", tas, ealt, cw );
	}
	else{
		_currentAlt = _sensorTE->readAltitude(_qnh, success );
		if( !success )
			_currentAlt = lastAltitude;  // ignore readout when failed
	}

	// ESP_LOGI(FNAME,"TE alt: %4.3f m", _currentAlt );
	if( _init  ){
		vTaskDelay(100 / portTICK_PERIOD_MS);
		_currentAlt = _sensorTE->readAltitude(_qnh, success ) * 1.03; // we want have some beep when powerd on
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
	double adiff = _currentAlt - Altitude;
	// ESP_LOGI(FNAME,"BMPVario new alt %0.1f err %0.1f", _currentAlt, err);
	double diff = (abs(adiff) * 1000) + 1;
	if(diff > 1000000){  // more than 100 m alt diff in 0.1 second not plausible ( > 400 km/h vertical )
		 ESP_LOGW(FNAME,"TE alt erratic reading: %f m", _currentAlt );
	  	 return _TEF;  // looks like noise on the bus, fancy errored value ignored, return last TE
	}
	double err = (abs(_currentAlt - predictAlt) * 1000) + 1;
	averageAlt += (_currentAlt - averageAlt) * 0.1;
	double kg = (diff / (err*_errorval + diff)) * _alpha;
	Altitude += (adiff) * kg;
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

	_avgTE += (_TEF - _avgTE)* (1/(10*vario_av_delay.get()));   // _av_damping in seconds, we have 10 samples per second.
	if( holddown > 0 ) {
		holddown--;
	}
	else
	{
		AverageVario::newSample( _TEF );
	}
	// Bird catcher
	// if( (TE > 0.1) || (TE < -0.1) ){
	// ESP_LOGI(FNAME,"Vario alt: %f, Vario: %f, t-delta=%2.3f sec", _currentAlt, TE, delta );
	//}
	return _TEF;
}

void BMPVario::setTE( double te ) {
	_test = true;
	_TEF = te;
	// calcAnalogOut();
}  // for testing purposes


BMPVario bmpVario;
