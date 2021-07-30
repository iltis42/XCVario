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
	 * Starts a new wind measurment cycle.
	 */
	void start();

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
	float getAsCorrection() { return airspeedCorrection; }
	float getAngle() { return windDir; };
	float getSpeed() { return windSpeed; };
	float getAsJitter() { return airspeed_jitter; }
	float getGsJitter() { return groundspeed_jitter; }
	float getDeviation() { return deviation_cur; }
	bool  getGpsStatus() { return gpsStatus; }
	float getMH() { return magneticHeading; }
	const char *getStatus() { return status; }

private:

	/**
	 * Return the elapsed time in milliseconds
	 */
	uint64_t elapsed()
	{
		return getMsTime() - measurementStart;
	}

	uint16_t nunberOfSamples;  // current number of samples
	uint64_t measurementStart; // measurement start in milliseconds
	double groundSpeed;        // GS in km/h
	double trueCourse;         // GPS course
	double trueHeading;        // Compass heading
	double averageTas;             // TAS in km/h
	double averageTH;          // sum of Compass true heading
	double averageTC;          // sum of GPS heading (true course)
	double averageGS;		   // average ground speed
	double tcStart;            // start value of true course observation window
	double mhStart;			   // magnetic heading start value for observation window
	double tasStart;
	double gsStart;
	double windDir;            // calculated wind direction
	double windSpeed;          // calculated wind speed in Km/h
	bool   lowAirspeed;
	float  circlingWindDir;
	float  circlingWindSpeed;
	int    circlingWindAge;
	float  airspeedCorrection;
	int    _age;
	int    _tick;
	float  airspeed_jitter;
	float  groundspeed_jitter;
	float  airspeed_jitter_tmp;
	float  groundspeed_jitter_tmp;
	bool   gpsStatus;
	float  deviation_cur;
	float  magneticHeading;
	const char *status;
};
