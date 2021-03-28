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

Last update: 2021-03-08

 **************************************************************************/

#include <cassert>
#include <cmath>
#include "esp_log.h"
#include "esp_system.h"
#include "spline.h"

#include "Compass.h"

float Compass::m_magn_heading = 0;
float Compass::m_true_heading_dev = 0;
float Compass::m_magn_heading_dev = 0;
bool Compass::m_headingValid = false;

CompassFilter Compass::m_cfmh;
float Compass::ipd[360];

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
 * Ok is set to true, if heading data is valid, otherwise it is set to false.
 */
float Compass::calculateHeading( bool *okIn )
{
	assert( (okIn != nullptr) && "Passing of NULL pointer is forbidden" );

	float new_heading = QMC5883L::heading( okIn );
	// ESP_LOGI( FNAME, "calculateHeading H: %3.1f  OK: %d", new_heading, ok );
	if( *okIn == false )
	{
		m_headingValid = false;
		// ESP_LOGW( FNAME, "magneticHeading() error return from heading()");
		return 0.0;
	}
	m_magn_heading = m_cfmh.filter( new_heading );
	m_headingValid = true;
	m_magn_heading_dev = m_magn_heading + getDeviation( m_magn_heading );

	// If declination is set, calculate true heading including deviation
	if(  compass_declination.get() != 0.0 )
		m_true_heading_dev = m_magn_heading_dev + compass_declination.get();
	else
		m_true_heading_dev = m_magn_heading_dev;

	// Correct heading in case of over/underflow
	if( m_magn_heading_dev >= 360.0 )
		m_magn_heading_dev -= 360.0;
	else if( m_magn_heading_dev < 0.0 )
		m_magn_heading_dev += 360.0;

	*okIn = true;
	// ESP_LOGI(FNAME,"magneticHeading ret=%3.1f", m_magn_heading );
	return m_magn_heading;
}

void Compass::begin(){
	setupInterpolationData();
}

/**
 * Returns the low pass filtered magnetic heading by considering
 * deviation, if argument withDeviation is set to true.
 * Ok is set to true, if heading data is valid, otherwise it is set to false.
 */
float Compass::magnHeading( bool *okIn, bool withDeviation )
{
	assert( (okIn != nullptr) && "Passing of NULL pointer is forbidden" );
	*okIn = m_headingValid;
	if( withDeviation == false )
	{
		// Return pure magnetic heading
		return m_magn_heading;
	}
	//else
	//  ESP_LOGI(FNAME,"magneticHeading NOT valid");
	return m_magn_heading_dev;
}

/**
 * Compute heading deviation by using linear interpolation.
 *
 * @param heading Heading value between 0...359
 */
float Compass::getDeviation( float heading )
{
	int iv = (static_cast<int>(heading + 0.5) % 360);
	float deviation = ipd[iv];
	// ESP_LOGI( FNAME, "RawHeading=%.1f : deviation=%0.1f", heading, deviation );
	return deviation;
}

/**
 * Setup the deviation interpolation data.
 */
void Compass::setupInterpolationData()
{
	// Setup cubic spline interpolation lookup table for dedicated angles 0...360
	std::vector<double> X = {    0-compass_dev_0.get(),   45-compass_dev_45.get(),   90-compass_dev_90.get(),  135-compass_dev_135.get(),
							   180-compass_dev_180.get(), 225-compass_dev_225.get(), 270-compass_dev_270.get(), 315-compass_dev_315.get(),
							   360-compass_dev_0.get()}; // close spline
	std::vector<double> Y = { compass_dev_0.get(),   compass_dev_45.get(),  compass_dev_90.get(),  compass_dev_135.get(),
			                  compass_dev_180.get(), compass_dev_225.get(), compass_dev_270.get(), compass_dev_315.get(),
							  compass_dev_0.get() };
	tk::spline s(X,Y);
	for( int dir=0; dir < 360; dir++ ){
		ipd[dir] = (float)( s((double)dir) );
		ESP_LOGI( FNAME, "DEV Heading=%d  dev=%f", dir, ipd[dir] );
	}
}


/**
 * Returns the low pass filtered magnetic heading by considering deviation and
 * declination.
 * Ok is set to true, if heading data is valid, otherwise it is set to false.
 */
float Compass::trueHeading( bool *okIn )
{
	assert( (okIn != nullptr) && "Passing of NULL pointer is forbidden" );
	*okIn = m_headingValid;
	return m_true_heading_dev;
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
	filteredValue += ( newValue - filteredValue ) * coefficient/compass_damping.get();  // default 1 sec ( 0.2 * 5 times/sec )
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
