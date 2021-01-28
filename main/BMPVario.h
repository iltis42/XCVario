#ifndef __BMPVario_H__
#define __BMPVario_H__

#include "ESP32NVS.h"
#include "PressureSensor.h"

#include "Setup.h"
#include "S2F.h"
#include <stdio.h>
/*
     Implementation of a stable Vario with flight optimized Iltis-Kalman filter

 */

#define SPS 10                   // samples per second
#define FILTER_LEN 51            // Max Filter length
#define ALPHA 0.2   			 // Kalman Gain alpha
#define ERRORVAL 1.6             // damping Factor for values off the weeds
#define STANDARD 1013.25         // ICAO standard pressure
#define abs(x)  ((x) < 0.0 ? -(x) : (x))

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
		_test = false;
		_init = false;
		_bmpTE = 0;
		_avgTE = 0;
		averageAlt = 0;
		bmpTemp = 0;
		_damping = 1.0;
		_currentAlt = 0;
		_S2FTE = 0;
		_analog_adj = 0;
		myS2F = 0;
	}

	void begin( PressureSensor *bmp, S2F* s2f );

	static void setHolddown( int hold ) { holddown = hold; }
	void setQNH( float qnh ) { _qnh = qnh; };
	void setup();

	double   readTE( float tas );   // get TE value im m/s


	double   readAVGTE();   // get TE value im m/s
	float    readS2FTE();   // get TE value im m/s for S2F
	inline double   readAVGalt() { return averageAlt; };   // get average Altitude
	inline double   readCuralt() { return _currentAlt; };   // get average Altitude
	void setTE( double te ); // for testing purposes


private:
	gpio_num_t _negative;
	gpio_num_t _positive;
	double _alpha;
	int   _filter_len;
	double _errorval;
	double _qnh;
	PressureSensor *_bmpTE;
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
	float _damping;
	float _S2FTE;
	double _currentAlt;

	static int   holddown;
	S2F * myS2F;
};

extern BMPVario bmpVario;
#endif
