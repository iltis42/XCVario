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

Last update: 2021-02-18

 **************************************************************************/

#include <cmath>
#include "esp_log.h"
#include "esp_system.h"

#include "Compass.h"

// Initialise static members
SetupNG<float>* Compass::deviations[8] = { &compass_dev_45,
		&compass_dev_90,
		&compass_dev_135,
		&compass_dev_180,
		&compass_dev_225,
		&compass_dev_270,
		&compass_dev_335,
		&compass_dev_360 };
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

float Compass::magn_heading = 0;
float Compass::true_heading = 0;
CompassFilter Compass::cfmh;

Compass::~Compass()
{
}

/**
 * Returns the magnetic heading by considering deviation. If ok is passed,
 * it is set to true, if heading data is valid, otherwise it is set to false.
 */
float Compass::magneticHeading( bool *okIn )
{
	ESP_LOGI(FNAME,"magneticHeading()");
	bool ok = false;
	float new_heading =  QMC5883L::heading( &ok );
	if( ok == false )
	{
		if( okIn != nullptr )
		{
			*okIn = false;
			ESP_LOGW(FNAME,"magneticHeading() error return from heading()");
		}
		return 0.0;
	}
	magn_heading = cfmh.filter( new_heading );
	// consider deviation
	const float skydirs[8] = { 45, 90, 135, 180, 225, 270, 335, 360 };
	for( int i = 0; i < 8; i++ )
	{
		break;
		float lowlim = skydirs[i] - 22.5;
		float uplim  = skydirs[i] + 22.5;

		if( uplim > 360 )
		{
			uplim -= 360;
		}

		if( magn_heading >= lowlim && magn_heading <= uplim )
		{
			magn_heading += deviations[i]->get();
			break;
		}
	}
	if( okIn != nullptr )
	{
		*okIn = true;
	}
	ESP_LOGI(FNAME,"magneticHeading ret=%3.1f", magn_heading );
	return magn_heading;
}

/**
 * Returns the magnetic heading by considering deviation and declination.
 * If ok is passed, it is set to true, if heading data is valid, otherwise
 * it is set to false.
 */
float Compass::trueHeading( bool *okIn )
{
	bool ok = false;
	true_heading = magneticHeading( &ok );
	if( ok == false )
	{
		if( okIn != nullptr )
		{
			*okIn = false;
		}
		return 0.0;
	}
	// Calculate true heading
	true_heading = magn_heading + compass_declination.get();

	if( okIn != nullptr )
	{
		*okIn = true;
	}
	return true_heading;
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
	if( filteredValue <= 0.0 )
	{
		filteredValue += 360.;
		turns++;
	}
	else if( filteredValue > 360. )
	{
		filteredValue -= 360.;
		turns--;
	}
	// ESP_LOGI( FNAME,"F-Heading=%.1f", filteredValue );

	return filteredValue;
}
