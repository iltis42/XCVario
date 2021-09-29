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

#define NUM_STRAIGHT_RESULTS 50

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
	bool getWind( int* direction, float* speed, int *age )
	{
		*direction = int( windDir + 0.5 );
		*speed = float( windSpeed );
		*age = _age;
		return ( windDir != -1.0 && windSpeed != -1.0 );
	}

	void setWind( float direction, float speed ){
		windDir = direction;
		windSpeed = speed;
		_age = 0;
	}

	void calculateWind( double tc, double gs, double th, double tas  );
	static double calculateSpeed( double angle1, double speed1, double angle2, double speed2  );
	static double calculateAngle( double angle1, double speed1, double angle2, double speed2  );
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
	double averageTas;             // TAS in km/h
	double averageTH;          // sum of Compass true heading
	double averageTC;          // sum of GPS heading (true course)
	double averageGS;		   // average ground speed
	double windDir;            // calculated wind direction
	double windSpeed;          // calculated wind speed in Km/h
	double lastWindSpeed;          // calculated wind speed in Km/h
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
};
