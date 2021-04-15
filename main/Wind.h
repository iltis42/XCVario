/*
 * Wind.h
 *
 *  Created on: Mar 21, 2021
 *
 *  Author: Eckhard Völlm, Axel Pauli
 *
 *  Last update: 2021-04-13
 */
#pragma once

#include <sys/time.h>

class Wind
{
public:
	Wind();
	virtual ~Wind() {};
	void drift( double truehead, double gndhead ) { _drift = truehead - gndhead; };

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
	bool getWind( int* direction, float* speed )
	{
		*direction = int( windDir + 0.5 );
		*speed = float( windSpeed );
		return ( windDir != -1.0 && windSpeed != -1.0 );
	}

	void calculateWind( double tc, double gs, double th, double tas  );
	void test();
	double meanAngle( double angle, double average );
	double normAngle( double angle );

private:

	/**
	 * Return the elapsed time in milliseconds
	 */
	uint64_t elapsed()
	{
		return getMsTime() - measurementStart;
	}

	double _drift;

	uint16_t nunberOfSamples;      // current number of samples
	uint64_t measurementStart;     // measurement start in milliseconds
	double tas;                    // TAS in km/h
	double groundSpeed;            // GS in km/h
	double trueCourse;             // GPS course
	double trueHeading;            // Compass heading
	double sumTas;                 // TAS in km/h
	double sumGroundSpeed;         // sum of GS in km/h
	double averageTH;         // sum of Compass true heading
	double averageTC;         // sum of GPS heading (true course)
	double hMin;                   // lower limit of heading observation window
	double hMax;                   // upper limit of heading observation window
	double hMin_magn;              // lower limit of magnetic heading observation window
	double hMax_magn;              // upper limit of magnetic heading observation window
	double windDir;                // calculated wind direction
	double windSpeed;              // calculated wind speed in Km/h
};
