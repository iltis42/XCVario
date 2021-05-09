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

Last update: 2021-03-29

 **************************************************************************/

#include <cassert>
#include <cmath>
#include "esp_log.h"
#include "esp_system.h"
#include "spline.h"
#include "sensor.h"

#include "Compass.h"

float Compass::m_magn_heading = 0;
float Compass::m_true_heading_dev = 0;
float Compass::m_magn_heading_dev = 0;
bool Compass::m_headingValid = false;

CompassFilter Compass::m_cfmh;
// float Compass::ipd[360];

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
	// ESP_LOGI( FNAME, "H: %3.2f", new_heading  );
	if( *okIn == false )
	{
		m_headingValid = false;
		// ESP_LOGW( FNAME, "magneticHeading() error return from heading()");
		return 0.0;
	}
	m_magn_heading = m_cfmh.filter( new_heading );
	m_headingValid = true;
	m_magn_heading_dev = m_magn_heading + getDeviation( m_magn_heading );

  // Correct magnetic heading in case of over/underflow
  if( m_magn_heading_dev >= 360.0 )
    m_magn_heading_dev -= 360.0;
  else if( m_magn_heading_dev < 0.0 )
    m_magn_heading_dev += 360.0;

	// If declination is set, calculate true heading including deviation
	if(  compass_declination.get() != 0.0 )
	  {
	    m_true_heading_dev = m_magn_heading_dev + compass_declination.get();

	    // Correct true heading in case of over/underflow
	    if( m_true_heading_dev >= 360.0 )
	      m_true_heading_dev -= 360.0;
	    else if( m_true_heading_dev < 0.0 )
	      m_true_heading_dev += 360.0;
	  }
	else
		m_true_heading_dev = m_magn_heading_dev;

	*okIn = true;
	return m_magn_heading;
}

void Compass::begin(){
	setupInterpolationData();
	i2c_0.begin(GPIO_NUM_4, GPIO_NUM_18, GPIO_PULLUP_DISABLE, GPIO_PULLUP_DISABLE, (int)(compass_i2c_cl.get()*1000) );
	if( serial2_speed.get() )
		serial2_speed.set(0);  // switch off serial interface, we can do only alternatively
	compass.initialize();
}

/**
 * Returns the low pass filtered magnetic heading by considering
 * deviation, if argument withDeviation is set to true.
 * Ok is set to true, if heading data is valid, otherwise it is set to false.
 */
float Compass::magnHeading( bool *okIn )
{
	assert( (okIn != nullptr) && "Passing of NULL pointer is forbidden" );
	*okIn = m_headingValid;
	return m_magn_heading_dev;
}

float Compass::rawHeading( bool *okIn )
{
	assert( (okIn != nullptr) && "Passing of NULL pointer is forbidden" );
	*okIn = m_headingValid;
	return m_magn_heading;
}

tk::spline *deviationSpline = 0;

/**
 * Compute heading deviation by using linear interpolation.
 *
 * @param heading Heading value between 0...359
 */
float Compass::getDeviation( float heading )
{
	if( !deviationSpline )
		setupInterpolationData();
	// float dev = (*deviationSpline)((double)heading);
	// ESP_LOGI( FNAME, "RawHeading=%.1f : deviation=%0.2f", heading, dev );
	return( (*deviationSpline)((double)heading) );
}

static int samples = 0;

void Compass::newDeviation( float measured_heading, float desired_heading ){
	double deviation = Vector::angleDiffDeg( desired_heading , measured_heading );
	ESP_LOGI( FNAME, "newDeviation Measured Head: %3.2f Desired Head: %3.2f => Deviation=%3.2f", measured_heading, desired_heading, deviation );
	// we implement one point every 45 degrees, so each point comes with a guard band of 22.5 degree
	auto ity = std::begin(Y);
	auto itx = std::begin(X);
	auto minxit = std::end(X);
	auto minyit = std::end(Y);
	float min_diff=360.0;
	for( itx = std::begin(X); itx != std::end(X); ++itx, ++ity ){
		ESP_LOGI( FNAME, "OLD Dev Head: %3.2f Dev: %3.2f", *itx, *ity );
		float head = *itx;
		float diff = abs( Vector::angleDiffDeg( measured_heading, head ));
		if( diff < min_diff ){
			min_diff = diff;
			minxit = itx;
			minyit = ity;
		}
	}
	ESP_LOGI( FNAME, "Closest Dev: %3.2f Dev: %3.2f", *minxit, *minyit );
	if( minxit != std::end(X) && min_diff < 30 ){  // maximum: at least all 30 deg one entry
		ESP_LOGI( FNAME, "Now erase %3.2f:%3.2f", *minxit, *minyit);
		minxit = X.erase(minxit);
		minyit = Y.erase(minyit);
	}
	X.insert( minxit, measured_heading );  // insert before the element
	Y.insert( minyit, deviation );
	ity = std::begin(Y);
	for(auto itx = std::begin(X); itx != std::end(X); ++itx, ++ity ){
		ESP_LOGI( FNAME, "NEW Dev Head: %3.2f Dev: %3.2f", *itx, *ity );
	}
	samples++;
	recalcInterpolationData();
	if( !(samples%10)  )
		saveDeviation();
}

std::vector<double>	Compass::X;
std::vector<double>	Compass::Y;

/**
 * Setup the deviation interpolation data.
 */


void Compass::recalcInterpolationData()
{
	if( deviationSpline )
		delete deviationSpline;
	deviationSpline  = new tk::spline(X,Y, tk::spline::cspline_hermite );

	for( int dir=0; dir < 360; dir+=30 ){
		// ipd[dir] = (float)( deviationSpline((double)dir) );
		ESP_LOGI( FNAME, "DEV Heading=%d  dev=%f", dir, (float)( (*deviationSpline)((double)dir) ));
	}

}

void Compass::setupInterpolationData()
{
	// Setup cubic spline interpolation lookup table for dedicated angles 0...360
	X = {  -135-compass_dev_225.get(), -90-compass_dev_270.get(), -45-compass_dev_315.get(), 0-compass_dev_0.get(),   45-compass_dev_45.get(),   90-compass_dev_90.get(),  135-compass_dev_135.get(),
							   180-compass_dev_180.get(), 225-compass_dev_225.get(), 270-compass_dev_270.get(), 315-compass_dev_315.get(),
							   360-compass_dev_0.get(), 405-compass_dev_45.get(), 450-compass_dev_90.get(), 495-compass_dev_135.get() }; // close spline
	Y = { compass_dev_225.get(), compass_dev_270.get(), compass_dev_315.get(), compass_dev_0.get(),   compass_dev_45.get(),  compass_dev_90.get(),  compass_dev_135.get(),
			                  compass_dev_180.get(), compass_dev_225.get(), compass_dev_270.get(), compass_dev_315.get(),
							  compass_dev_0.get(), compass_dev_45.get(), compass_dev_90.get(), compass_dev_135.get()  };

	recalcInterpolationData();
}

void Compass::saveDeviation(){
	ESP_LOGI( FNAME, "+++++++++++++ saveDeviation ++++++++++++++");
	for( int dir=0; dir < 360; dir+=45 ){
		// ipd[dir] = (float)( deviationSpline((double)dir) );
		ESP_LOGI( FNAME, "OLD DEV Heading=%d  dev=%f", dir, (float)( (*deviationSpline)((double)dir) ));
	}
	if( abs( compass_dev_0.get() - (*deviationSpline)(0)) >0.1 )
		compass_dev_0.set( (*deviationSpline)(0) );
	if( abs( compass_dev_45.get() - (*deviationSpline)(45)) >0.1 )
		compass_dev_45.set( (*deviationSpline)(45) );
	if( abs( compass_dev_90.get() - (*deviationSpline)(90)) >0.1 )
		compass_dev_90.set( (*deviationSpline)(90) );
	if( abs( compass_dev_135.get() - (*deviationSpline)(135)) >0.1 )
		compass_dev_135.set( (*deviationSpline)(135) );
	if( abs( compass_dev_180.get() - (*deviationSpline)(180)) >0.1 )
		compass_dev_180.set( (*deviationSpline)(180) );
	if( abs( compass_dev_225.get() - (*deviationSpline)(225)) >0.1 )
		compass_dev_225.set( (*deviationSpline)(225) );
	if( abs( compass_dev_270.get() - (*deviationSpline)(270)) >0.1 )
		compass_dev_270.set( (*deviationSpline)(270) );
	if( abs( compass_dev_315.get() - (*deviationSpline)(315)) >0.1 )
		compass_dev_315.set( (*deviationSpline)(315) );

	for( int dir=0; dir < 360; dir+=45 ){
		// ipd[dir] = (float)( deviationSpline((double)dir) );
		ESP_LOGI( FNAME, "NEW DEV Heading=%d  dev=%f", dir, (float)( (*deviationSpline)((double)dir) ));
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

	return filteredValue;
}
