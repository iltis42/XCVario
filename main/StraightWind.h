/*
 * StraightWind.h
 *
 *  Created on: Mar 21, 2021
 *
 *  Author: Eckhard Völlm, Axel Pauli
 *
 *  Last update: 2021-04-18
 */
#pragma once

#include <sys/time.h>
#include "vector.h"

#define NUM_STRAIGHT_RESULTS 90

class StraightWind
{
public:
	StraightWind();
	virtual ~StraightWind() {};

	void begin();

	void tick();

	/**
	 * Get time in ms since 1.1.1970
	 */
	static uint64_t getMsTime()
	{
		struct timeval tv;
		gettimeofday( &tv, nullptr );
		return ( tv.tv_sec * 1000 ) + ( tv.tv_usec / 1000 );
	}

	/**
	 * Measurement cycle for wind calculation in straight flight. Should be
	 * triggered periodically, maybe once per second.
	 *
	 * Returns true, if a new wind was calculated.
	 */
	bool calculateWind();

	/**
	 * Return the last calculated wind. If return result is true, the wind data
	 * are valid otherwise false.
	 */
	bool getWind( int* direction, float* speed, int *age );

	void setWind( float direction, float speed ){
		windDir = direction;
		windSpeed = speed;
		_age = 0;
	}

	void calculateWind( float tc, float gs, float th, float tas  );
	static float calculateSpeed( float angle1, float speed1, float angle2, float speed2  );
	static float calculateAngle( float angle1, float speed1, float angle2, float speed2  );
	void newCirclingWind( float angle, float speed );
	void test();
	int getAge() { return _age; }
	static void resetAge() { _age = 0; }
	float getAsCorrection() { return airspeedCorrection; }
	float getAngle();
	float getSpeed();
	float getDeviation() { return deviation_cur; }
	bool  getGpsStatus() { return gpsStatus; }
	float getMH() { return magneticHeading; }
	const char *getStatus() { return status; }

private:

	int    nunberOfSamples;  // current number of samples
	float averageTas;             // TAS in km/h
	float averageTH;          // sum of Compass true heading
	float averageTC;          // sum of GPS heading (true course)
	float averageGS;		   // average ground speed
	float windDir;            // calculated wind direction
	float windSpeed;          // calculated wind speed in Km/h
	float lastWindSpeed;          // calculated wind speed in Km/h
	bool   lowAirspeed;
	float  circlingWindDir;
	float  circlingWindDirReverse;
	float  circlingWindSpeed;
	int    circlingWindAge;
	float  airspeedCorrection;
	static int    _age;
	int    _tick;
	bool   gpsStatus;
	float  deviation_cur;
	float  magneticHeading;
	const char *status;
	float  jitter;
	int curVectorNum;
	static Vector windVectors[NUM_STRAIGHT_RESULTS];
	float newWindSpeed;
	float newWindDir;
	float slipAverage;
};
