/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

File: Compass.h

Class to handle compass data access.

Author: Axel Pauli, January 2021

Last update: 2021-03-07

 **************************************************************************/

#pragma once

#include "SetupNG.h"
#include "QMC5883L.h"
#include "spline.h"
#include <map>

class Compass : public QMC5883L
{
public:
	/*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. The default address of the chip is 0x0D.
	 */
	Compass( const uint8_t addr,
			const uint8_t odr,
			const uint8_t range,
			const uint16_t osr,
			I2C_t *i2cBus=nullptr );

	~Compass();

	// system related methods
	void begin();
	void start();
	static void cur();  // ticker
	static void compassT(void* arg );  // task for compass reading

	// Heading related methods
	//  Returns the low pass filtered magnetic heading without deviation
	static float rawHeading( bool *okIn );
	static float filteredHeading( bool *okIn );
	static float filteredTrueHeading( bool *okIn );
	static void setGyroHeading( float hd );
	static float getGyroHeading( bool *ok, bool addDeclination=true );
	//  Returns the heading valid flag.
	static inline bool headingValid() {	return m_headingValid;	}
	static void setHeading( float h );

	// Deviatiation related methods
	static float getDeviation( float heading );
	static void saveDeviation();
	static bool newDeviation( float for_heading, float steer, float airspeedCalibration );
	static void loadDeviationMap();
	static void deviationReload();
	 // Setup the deviation interpolation data.
	static void readInterpolationData();
	 // Rebuild spline function
	static void recalcInterpolationSpline();

private:
	// fully gyro fused heading
	static float m_gyro_fused_heading;
	/** Pure averaged magnetic heading */
	static float m_magn_heading;
	/** Control flag of filtered heading. */
	static bool m_headingValid;
	static std::vector<double> X;
	static std::vector<double> Y;
	static xSemaphoreHandle splineMutex;
	static tk::spline *deviationSpline;
	static std::map< double, double> devmap;
	static int _tick;
	static int _devHolddown;
	static int _external_data;
	static float _heading_average;
	static int gyro_age;

};
