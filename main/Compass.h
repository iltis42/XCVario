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

/**
 * Low pass filter for heading values.
 */
class CompassFilter
{
public:

	CompassFilter( const float coefficientIn=0.2 );

	~CompassFilter() {};

	void setCoefficient( const float value )
	{
		coefficient = value;
	}

	/**
	 * Low pass filter method. The filtering time constant can be set by using
	 * method setCoefficient():
	 */
	float filter( float value );

	/**
	 * Resets all used dynamic filter values.
	 */
	void reset()
	{
		turns = 0;
		oldValue = 0.0;
		filteredValue = 0.0;
	}

private:

	float coefficient;
	int turns;
	float oldValue;
	float filteredValue;
};


class Compass : public QMC5883L
{
public:

	/*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. Note if i2cBus is not set in the constructor,
    you have to set it by calling method setBus(). The default address of the
    chip is 0x0D.
	 */
	Compass( const uint8_t addr,
			const uint8_t odr,
			const uint8_t range,
			const uint16_t osr,
			I2C_t *i2cBus=nullptr );

	~Compass();

  void begin();

  /**
   * This method must be called periodically in a fixed time raster. It Reads
   * the current heading from the sensor and apply a low pass filter
   * to it. It Returns the low pass filtered magnetic heading without applying
   * any corrections to it as declination or deviation.
   * Ok is set to true, if heading data is valid, otherwise it is set to false.
   */
  float calculateHeading( bool *okIn );

  /**
	 * Returns the heading by considering deviation and declination.
	 * Ok is set to true, if heading data is valid, otherwise it is set to false.
	 */
	static float trueHeading( bool *okIn );

  /**
   * Returns the low pass filtered magnetic heading by considering
   * deviation
   * Ok is set to true, if heading data is valid, otherwise it is set to false.
   */
	static float magnHeading( bool *okIn );

	/*
	 * Returns the low pass filtered magnetic heading without deviation
	 */
	static float rawHeading( bool *okIn );

	/**
	 *  Returns the heading valid flag.
	 */

	static bool headingValid()
	{
	  return m_headingValid;
	}

	/**
	 * Returns the used compass filter object.
	 */
	static CompassFilter& compassFilter() { return m_cfmh; }

	/**
	 * Setup the deviation interpolation data.
	 */
	static void setupInterpolationData();

	/**
	 * Compute heading deviation by using linear interpolation.
	 * @param heading Heading value between 0...359
	 */
	static float getDeviation( float heading );


private:

	/** Pure low pass filtered magnetic heading */
	static float m_magn_heading;

	/** magnetic heading including deviation */
	static float m_magn_heading_dev;

	/** true heading including deviation and declination */
	static float m_true_heading_dev;

	/** Control flag of filtered heading. */
  static bool m_headingValid;

    /** Low pass filter used by compass. */
 	static CompassFilter m_cfmh;

 	/** Deviation table */
	// static float ipd[360];
};
