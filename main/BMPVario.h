#ifndef __BMPVario_H__
#define __BMPVario_H__

#include "ESP32NVS.h"
#include "BME280_ESP32_SPI.h"

extern "C" {
#include <driver/sigmadelta.h>
}
#include "Setup.h"
#include <stdio.h>
/*
     Implementation of a stable Vario with flight optimized Iltis-Kalman filter

 */

#define SPS 10                   // samples per second
#define FILTER_LEN 51            // Max Filter length
#define ALPHA 0.2   			 // Kalman Gain alpha
#define ERRORVAL 1.6             // damping Factor for values off the weeds
#define STANDARD 1013.25         // ICAO standard pressure
#define abs(x)  (x < 0.0 ? -x : x)

class BMPVario {
public:
	BMPVario() {
		_positive = GPIO_NUM_0;
		_negative = GPIO_NUM_0;
		_qnh = STANDARD;
		index = 0;
		_alpha = ALPHA;
		predictAlt = 0;
		Altitude = 0;
		lastAltitude = 0;
		_errorval = ERRORVAL;
		_filter_len = 10;
		memset( TEarr, 0, sizeof(TEarr) );
		_TEF = 0;
		_analog_adj = 0;
		_test = false;
		_init = false;
		_bmpTE = 0;
		_avgTE = 0;
		averageAlt = 0;
		bmpTemp = 0;
		_setup = 0;
		_damping = 1.0;
		_currentAlt = 0;
		samples = 0;
		averageClimb = 0.0;
		holddown = 0;
	}

	void begin( BME280_ESP32_SPI *bmp,
			    Setup* setup=0 );

	static void setHolddown( int hold ) { holddown = hold; }
	void setQNH( float qnh ) { _qnh = qnh; };
	void setup();

	double   readTE();   // get TE value im m/s
	float readAvgClimb() {
		float ac = 0;
		int ns=0;
		for( int i=avindexMin, j=(int)(_setup->get()->_core_climb_history); i>=0 && j>=0; i--, j-- ) {
			// printf("MST pM= %2.2f  %d\n", avClimbMin[i], i  );
			if( avClimbMin[i] > _setup->get()->_core_climb_min ) {
				ac += avClimbMin[i];
				ns++;
			}
		}
		if( ns ) {
			return ac/ns;
		}
		else
			return 0;
	}
	double   readAVGTE();   // get TE value im m/s
	inline double   readAVGalt() { return averageAlt; };   // get average Altitude
	inline double   readCuralt() { return _currentAlt; };   // get average Altitude

	// Analog Display
	void analogOut( gpio_num_t negative=GPIO_NUM_18, gpio_num_t positive=GPIO_NUM_19 );
	void calcAnalogOut();
	void setTE( double te ); // for testing purposes
	void setAdj( double adj ) { _analog_adj = adj; calcAnalogOut(); };
	void history( int idx );

private:
	gpio_num_t _negative;
	gpio_num_t _positive;
	double _alpha;
	int   _filter_len;
	double _errorval;
	double _qnh;
	BME280_ESP32_SPI *_bmpTE;
	double predictAlt;
	double Altitude;
	double lastAltitude;
	double averageAlt;
	double  TEarr[FILTER_LEN];
	double _analog_adj;
	int    index;
	double _TEF;
	double _avgTE;
	double bmpTemp;
	bool _test;
	bool _init;
	Setup *_setup;
	float _damping;
	double _currentAlt;
	double averageClimb;
	float avClimbSec[60];
	float avClimbMin[300];
	int avindexSec;
	int avindexMin;
	int    samples;
	static int   holddown;
};

extern BMPVario bmpVario;
#endif
