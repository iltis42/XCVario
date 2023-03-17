#include "BMPVario.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <atomic>
#include <cmath>
#include <logdef.h>
#include "S2F.h"
#include "AverageVario.h"
#include "sensor.h"

const double sigmaAdjust = 255 * 2.0/33;  // 2 Vss
int BMPVario::holddown = 0;

void BMPVario::begin( PressureSensor *te, PressureSensor *baro, S2F *aS2F  ) {
	_sensorTE = te;
	_sensorBARO = baro;
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

void BMPVario::configChange(){
	_damping = vario_delay.get();
	_damping_factor = (1.0/(_damping));
	int filter_len = rint(_damping*(10.0/3));
	TEavg.setLength( filter_len );
	ESP_LOGI(FNAME, "configChange damping:%f filter_len:%d", _damping, filter_len );
}

void BMPVario::setup() {
	_qnh = QNH.get();
	configChange();
	lastrts = esp_timer_get_time();
	vTaskDelay(100 / portTICK_PERIOD_MS);
	bool success;
	_currentAlt = _sensorTE->readAltitude(_qnh, success ) * 1.03; // we want have some beep when powerd on
	if( success ){
		lastAltitude = _currentAlt;
		predictAlt = _currentAlt;
		Altitude = _currentAlt;
		averageAlt = _currentAlt;
		ESP_LOGI(FNAME, "Initial Alt=%0.1f",Altitude );
	}else{
		ESP_LOGE(FNAME, "Initial Alt read error Alt=%0.1f",Altitude );
	}
}


double BMPVario::readTE( float tas ) {
	if ( _test )     // we are in testmode, just return what has been set
		return _TEF;
	bool success;
	// Latency supervision and correction
	uint64_t rts = esp_timer_get_time();
	float time_delta = (float)(rts - lastrts)/1000000.0;   // in seconds
	if( time_delta < 0.095 ) {   // ensure time_delta is 100 mS at least
		int addwait = (int)(100.0-time_delta*1000);
		// ESP_LOGW(FNAME, "Too short TE time time_delta <95 mS: %4.1f, add wait %d ", time_delta*1000, addwait );
		delay( addwait );
		rts = esp_timer_get_time();
		time_delta = (float)(rts - lastrts)/1000000.0;
	}
	lastrts = rts;
	if( time_delta < 0.090 || time_delta > 0.2 )
		ESP_LOGW(FNAME,"Vario time_delta=%2.3f sec", time_delta );

	bmpTemp = _sensorTE->readTemperature( success );
	ESP_LOGI(FNAME,"BMP temp=%0.1f", bmpTemp );
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
	averageAlt += (_currentAlt - averageAlt) * 0.1;
	double adiff = _currentAlt - Altitude;
	// ESP_LOGI(FNAME,"BMPVario new alt %0.1f err %0.1f", _currentAlt, err);
	double diff = (abs(adiff) * 1000) + 1;
	if(diff > 1000000){  // more than 100 m altitude diff in 0.1 second not plausible ( > 400 km/h vertical ) -> handled by Kalman filter
		 ESP_LOGW(FNAME,"TE sensor delta OOB: %f m", diff/10000 );
	}
	double err = (abs(_currentAlt - predictAlt) * 1000) + 1;
	averageAlt += (_currentAlt - averageAlt) * 0.1;
	double kg = (diff / (err*_errorval + diff)) * _alpha;
	Altitude += (adiff) * kg;
	double altDiff = Altitude - lastAltitude;
	// ESP_LOGI(FNAME," altDiff %0.1f diff %0.1f", TE, diff);
	lastAltitude = Altitude;
	double TEAVG = TEavg( altDiff / time_delta );
	predictAlt = Altitude + (TEAVG * time_delta);
	_TEF += ((TEAVG - _TEF)) * _damping_factor;
	_avgTE += (_TEF - _avgTE)* (1/(10*vario_av_delay.get()));   // _av_damping in seconds, we have 10 samples per second.
	if( holddown > 0 ) {
		holddown--;
	}
	else
	{
		AverageVario::newSample( _TEF );
	}
	// Bird catcher
	// if( (altDiff > 0.1) || (altDiff < -0.1) ){
	// ESP_LOGI(FNAME,"Vario alt: %f, Vario: %f, Vario-AVG: %f, t-delta=%2.3f sec", _currentAlt, _TEF, TEAVG, time_delta );
	//}
	return _TEF;
}

void BMPVario::setTE( double te ) {
	_test = true;
	_TEF = te;
	// calcAnalogOut();
}  // for testing purposes


BMPVario bmpVario;
