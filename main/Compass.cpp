/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

File: Compass.cpp

Class to handle compass data access.

Author: Axel Pauli, January 2021

Last update: 2021-02-26

 **************************************************************************/

#include <cmath>
#include "esp_log.h"
#include "esp_system.h"

#include "Compass.h"

// Initialise static members
SetupNG<float>* Compass::deviations[8] = { &compass_dev_0,
    &compass_dev_45,
		&compass_dev_90,
		&compass_dev_135,
		&compass_dev_180,
		&compass_dev_225,
		&compass_dev_270,
		&compass_dev_335 };

float Compass::m_magn_heading = 0;
bool Compass::m_headingValid = false;
CompassFilter Compass::m_cfmh;
struct Compass::IPD Compass::ipd[8];

/*
  Creates instance for I2C connection with passing the desired parameters.
  No action is done at the bus. Note if i2cBus is not set in the constructor,
  you have to set it by calling method setBus(). The default address of the
  chip is 0x0D.
 */
Compass::Compass( const uint8_t addr,
		const uint8_t odr,
		const uint8_t range,
		const uint16_t osr,
		I2C_t *i2cBus ) :
		 QMC5883L( addr, odr, range, osr, i2cBus )
{
}

Compass::~Compass()
{
}

/**
 * This method must be called periodically in a fixed time raster. It Reads
 * the current heading from the sensor and apply a low pass filter
 * to it. It Returns the low pass filtered magnetic heading without applying
 * any corrections to it as declination or deviation.
 * If ok is passed, it is set to true, if heading data is valid, otherwise
 * it is set to false.
 */
float Compass::calculateHeading( bool *okIn )
{
	// ESP_LOGI(FNAME,"magneticHeading()");
	bool ok = false;
	if( okIn == nullptr )  // mandatory, return if zero
		return 0.0;

	float new_heading = QMC5883L::heading( &ok );
	// ESP_LOGI( FNAME, "calculateHeading H: %3.1f  OK: %d", new_heading, ok );
	if( ok == false )
	{
		*okIn = false;
		m_headingValid = false;
		ESP_LOGW( FNAME, "magneticHeading() error return from heading()");
		return 0.0;
	}
	m_magn_heading = m_cfmh.filter( new_heading );
	m_headingValid = true;

	*okIn = true;
	// ESP_LOGI(FNAME,"magneticHeading ret=%3.1f", m_magn_heading );
	return m_magn_heading;
}

/**
 * Returns the low pass filtered magnetic heading by considering
 * deviation, if argument withDeviation is set to true.
 * If ok is passed, it is set to true, if heading data is valid, otherwise
 * it is set to false.
 */
float Compass::magnHeading( bool *okIn, bool withDeviation )
{
  if( okIn == nullptr )
	  return 0.0;
  *okIn = m_headingValid;

  if( withDeviation == false )
    {
      // Return pure magnetic heading
      return m_magn_heading;
    }
  // Return magnetic heading by considering deviation data.
  if( m_headingValid )
	  return m_magn_heading + getDeviation( m_magn_heading );
  else
	  return 0.0;
}

/**
 * Compute heading deviation by using linear interpolation.
 *
 * @param heading Heading value between 0...359
 */
float Compass::getDeviation( float heading )
{
  static bool idLoaded = false;

  if( idLoaded == false )
    {
      // Load deviation interpolation data once.
      setupInterpolationData();
      idLoaded = true;
    }

  // Reduce heading to interval 0...45.
  int iv = (static_cast<int>(heading) % 360) / 45;

  // Apply linear interpolation
  float deviation = ( ipd[iv].m * (heading - ipd[iv].hi) ) + ipd[iv].n;

  // ESP_LOGI( FNAME, "RawHeading=%.1f : deviation=%0.1f", heading, deviation );
  return deviation;
}

/**
 * Setup the deviation interpolation data.
 */
void Compass::setupInterpolationData()
{
  // Deviation data 0...360
  SetupNG<float>* devData[9] = { &compass_dev_0,
      &compass_dev_45,
      &compass_dev_90,
      &compass_dev_135,
      &compass_dev_180,
      &compass_dev_225,
      &compass_dev_270,
      &compass_dev_335,
      &compass_dev_0 };

  for( int i = 0; i < 8; i++ )
    {
      ipd[i].m = ( devData[i+1]->get() - devData[i]->get() ) / 45.0;
      ipd[i].n = devData[i]->get();
      ipd[i].hi = float(i) * 45.0;
      ESP_LOGI( FNAME, "I=%d dev1=%f dev2=%f m=%f n=%f hi=%f",
                i, devData[i]->get(), devData[i+1]->get(),
                ipd[i].m, ipd[i].n, ipd[i].hi );
    }
}

/**
 * Returns the low pass filtered magnetic heading by considering deviation and
 * declination.
 * If ok is passed, it is set to true, if heading data is valid, otherwise
 * it is set to false.
 */
float Compass::trueHeading( bool *okIn )
{
  if( okIn == nullptr )
	  return 0.0;

  *okIn = m_headingValid;
	// Calculate and return true heading
  if( m_headingValid )
	  return m_magn_heading + compass_declination.get();
  else
	  return 0.0;
}

//------------------------------------------------------------------------------

CompassFilter::CompassFilter( const float coefficientIn ) :
		  coefficient( coefficientIn ),
		  turns( 0 ),
		  oldValue ( 0.0 ),
		  filteredValue( 0.0 )
{
}

float CompassFilter::filter( float newValue )
{
#if 0
	ESP_LOGI( FNAME, "Filter: H=%f, FV=%f, Coeff=%f",
			newValue, filteredValue, coefficient );
#endif

	// Check in which direction North was passed
	if( (oldValue < 90.) && (newValue > 270.) )
		turns--;
	else if( (oldValue > 270.) && (newValue < 90.) )
		turns++;

	// Save new value as old
	oldValue = newValue;
	// Correct angle's north overflow or underflow
	newValue += float( turns * 360 );
	// Low pass filtering
	filteredValue += ( newValue - filteredValue ) * coefficient;
	// Correct angle areas and turns
	if( filteredValue < 0.0 )
	{
		filteredValue += 360.;
		turns++;
	}
	else if( filteredValue >= 360. )
	{
		filteredValue -= 360.;
		turns--;
	}
	// ESP_LOGI( FNAME,"F-Heading=%.1f", filteredValue );

	return filteredValue;
}
