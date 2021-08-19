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
#include "sensor.h"

#include "Compass.h"

float Compass::m_magn_heading = 0;
float Compass::m_true_heading_dev = 0;
float Compass::m_magn_heading_dev = 0;
bool Compass::m_headingValid = false;
xSemaphoreHandle Compass::splineMutex = 0;
tk::spline *Compass::deviationSpline = 0;
std::vector<double>	Compass::X;
std::vector<double>	Compass::Y;
std::map< double, double> Compass::devmap;
int Compass::_tick = 0;
int Compass::_devHolddown = 0;
CompassFilter Compass::m_cfmh;
int Compass::_external_data = 0;
float Compass::_heading_average = -1000;

TaskHandle_t *ctid = 0;

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
	// ESP_LOGI( FNAME, "calculateHeading");
	assert( (okIn != nullptr) && "Passing of NULL pointer is forbidden" );
	if( _external_data ){
		_external_data--;
		*okIn = true;
		return m_magn_heading;
	}

	float new_heading = QMC5883L::heading( okIn );
	// ESP_LOGI( FNAME, "Mag Heading: %3.2f", new_heading  );
	if( *okIn == false )
	{
		m_headingValid = false;
		// ESP_LOGW( FNAME, "magneticHeading() error return from heading()");
		return 0.0;
	}
	 // Correct magnetic heading in case of over/underflow
	m_magn_heading = Vector::normalizeDeg( m_cfmh.filter( new_heading ) );
	m_headingValid = true;
	m_magn_heading_dev = Vector::normalizeDeg( _heading_average + getDeviation( _heading_average ) );

	// If declination is set, calculate true heading including deviation
	if(  compass_declination.get() != 0.0 )
	  {
	    m_true_heading_dev = Vector::normalizeDeg( m_magn_heading_dev + compass_declination.get() );  // Correct true heading in case of over/underflow
	  }
	else
		m_true_heading_dev = m_magn_heading_dev;

	*okIn = true;
	return m_magn_heading;
}

void Compass::deviationReload(){
	ESP_LOGI( FNAME, "deviationReload()");
	readInterpolationData();
	loadDeviationMap();
	recalcInterpolationSpline();
}

void Compass::compassT(void* arg ){
	while(1){
		TickType_t lastWakeTime = xTaskGetTickCount();
		if( !calibrationIsRunning() ){
			if( compass_enable.get() ){
				bool hok;
				compass.calculateHeading( &hok );
				// if( !hok )
				//	ESP_LOGI( FNAME, "warning compass heading calculation error");
			}
			if( uxTaskGetStackHighWaterMark( ctid  ) < 256 )
				ESP_LOGW(FNAME,"Warning Compass task stack low: %d bytes", uxTaskGetStackHighWaterMark( ctid ) );
			bool ok;
			float cth = (double)Compass::rawHeading( &ok );
			float diff = Vector::angleDiffDeg( cth, _heading_average );
			if( _heading_average == -1000 )
				_heading_average = cth;
			else
				_heading_average += diff * (1/(20*compass_damping.get()));

			_heading_average = Vector::normalizeDeg( _heading_average );
			// ESP_LOGI(FNAME,"Heading: %.1f %.1f ", cth, _heading_average );
		}
		vTaskDelayUntil(&lastWakeTime, 50/portTICK_PERIOD_MS);
	}
}

void Compass::begin(){
	_devHolddown = 1800;
	splineMutex = xSemaphoreCreateMutex();
	X.reserve(40);
	Y.reserve(40);
	deviationReload();
	i2c_0.begin(GPIO_NUM_4, GPIO_NUM_18, GPIO_PULLUP_DISABLE, GPIO_PULLUP_DISABLE, (int)(compass_i2c_cl.get()*1000) );
	if( serial2_speed.get() )
		serial2_speed.set(0);  // switch off serial interface, we can do only alternatively
	compass.initialize();
}

void Compass::start(){
	xTaskCreatePinnedToCore(&compassT, "compassT", 2600, NULL, 12, ctid, 0);
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

float Compass::filteredRawHeading( bool *okIn )
{
	assert( (okIn != nullptr) && "Passing of NULL pointer is forbidden" );
	*okIn = m_headingValid;
	return _heading_average;
}

float Compass::rawHeading( bool *okIn )
{
	assert( (okIn != nullptr) && "Passing of NULL pointer is forbidden" );
	*okIn = m_headingValid;
	return m_magn_heading;
}

/**
 * Compute heading deviation by using linear interpolation.
 *
 * @param heading Heading value between 0...359
 */
float Compass::getDeviation( float heading )
{
	if( !deviationSpline )
		return 0.0;
	xSemaphoreTake(splineMutex,portMAX_DELAY );
	float dev=(*deviationSpline)((double)heading);
	// ESP_LOGI( FNAME, "RawHeading=%.1f : deviation=%0.2f", heading, dev );
	xSemaphoreGive(splineMutex);
	return( dev );
}

static int samples = 0;

// new Deviation from reverse calculated TAWC Wind measurement
bool Compass::newDeviation( float measured_heading, float desired_heading, float airspeedCalibration ){
	double deviation = Vector::angleDiffDeg( desired_heading , measured_heading );
	// ESP_LOGI( FNAME, "newDeviation Measured Head: %3.2f Desired Head: %3.2f => Deviation=%3.2f, Samples:%d", measured_heading, desired_heading, deviation, samples );
	if( abs(deviation) > wind_max_deviation.get() ){ // data is not plausible/useful
		ESP_LOGI( FNAME, "new Deviation out of bounds: %3.3f: Drop this deviation", deviation );
		return false;
	}
	// we implement one point every 45 degrees, so each point comes with a guard band of 22.5 degree
	xSemaphoreTake(splineMutex,portMAX_DELAY );
	// for(auto itx = std::begin(devmap); itx != std::end(devmap); ++itx ){
	//	ESP_LOGI( FNAME, "Cur Dev MAP Head: %3.2f Dev: %3.2f", itx->first, itx->second );
	//}
	for(auto it = std::begin(devmap); it != std::end(devmap); ){
#ifdef VERBOSE_LOG
		ESP_LOGI( FNAME, "Main X/Y Vector Head: %3.2f Dev: %3.2f", it->first, it->second );
#endif
		float diff = Vector::angleDiffDeg( measured_heading, (float)it->first );
		if( diff < 22.5 && diff > -22.5  ){
			// ESP_LOGI( FNAME, "Diff<22.5 @ head %3.2f, diff %3.3f Erase", it->first, diff );
			devmap.erase( it++ );
		}
		else
			it++;
	}
	double old_dev = 0;
	if( deviationSpline ){
		old_dev = (*deviationSpline)((double)measured_heading);
		// ESP_LOGI( FNAME, "OLD Spline Deviation for mesured heading: %3.2f Dev: %3.2f", measured_heading, old_dev );
	}
	double delta = (deviation - old_dev);
	ESP_LOGI( FNAME, "Deviation Delta: %f old dev: %f, new dev: %f", delta, old_dev, old_dev + (delta * 0.15) );
	float k=wind_dev_filter.get();
	if(old_dev == 0.0) {
		ESP_LOGI( FNAME, "We are starting so provide initial value");
		k=1.0;
	}

	devmap[ measured_heading ] = old_dev + (delta * k);  // insert the new low pass filtered element
#ifdef VERBOSE_LOG
	for(auto itx = std::begin(devmap); itx != std::end(devmap); ++itx ){
		ESP_LOGI( FNAME, "NEW Dev MAP Head: %3.2f Dev: %3.2f", itx->first, itx->second );
	}
#endif
	xSemaphoreGive(splineMutex);
	recalcInterpolationSpline();
	ESP_LOGI( FNAME, "NEW Spline Deviation for mesured heading: %3.2f Dev: %3.2f", measured_heading, (*deviationSpline)((double)measured_heading) );
	samples++;
	if( (samples > 50) && (_devHolddown <= 0) ){
		saveDeviation();
		_devHolddown = 1800; // maximum every half hour
		samples = 0;
		if( abs( wind_as_calibration.get() - airspeedCalibration )*100 > 0.5 )
			wind_as_calibration.set( airspeedCalibration );
	}
	return true;
}

/**
 * Setup the deviation interpolation data.
 */
void Compass::recalcInterpolationSpline()
{
	ESP_LOGI( FNAME, "recalcInterpolationSpline()");
	xSemaphoreTake(splineMutex,portMAX_DELAY );
#ifdef VERBOSE_LOG
	if( deviationSpline ){
		for( int dir=0; dir <= 360; dir+=45 ){
			// ipd[dir] = (float)( deviationSpline((double)dir) );
			ESP_LOGI( FNAME, "OLD DEV Heading=%d  dev=%f", dir, (float)( (*deviationSpline)((double)dir) ));
		}
	}
#endif
	if( deviationSpline )
		delete deviationSpline;

	X.clear();
	Y.clear();
	// take care for head of spline by extrapolation
	for(auto it = std::begin(devmap); it != std::end(devmap); ++it ){
		if( it->first >= 260 ){
			// ESP_LOGI( FNAME, "Pre  X/Y Vector Head: %3.2f Dev: %3.2f", it->first-360, it->second );
			X.push_back( it->first-360.0 );
			Y.push_back( it->second );
		}
	}
	for(auto it = std::begin(devmap); it != std::end(devmap); ++it ){
		// ESP_LOGI( FNAME, "Main X/Y Vector Head: %3.2f Dev: %3.2f", it->first, it->second );
		X.push_back( it->first );
		Y.push_back( it->second );
	}
	// take care for tail of spline by extrapolation
	for(auto it = std::begin(devmap); it != std::end(devmap); ++it ){
		if( it->first <= 140 ){
			// ESP_LOGI( FNAME, "Post  X/Y Vector Head: %3.2f Dev: %3.2f", it->first+360, it->second );
			X.push_back( it->first+360.0 );
			Y.push_back( it->second );
		}
	}

	deviationSpline  = new tk::spline(X,Y, tk::spline::cspline_hermite );
	xSemaphoreGive(splineMutex );
#ifdef VERBOSE_LOG
	for( int dir=0; dir <= 360; dir+=45 ){
		// ipd[dir] = (float)( deviationSpline((double)dir) );
		ESP_LOGI( FNAME, "NEW DEV Heading=%d  dev=%f", dir, (float)( (*deviationSpline)((double)dir) ));
	}
#endif

}

void Compass::readInterpolationData()
{
	ESP_LOGI( FNAME, "readInterpolationData()");
	// Setup cubic spline interpolation lookup table for dedicated angles 0...360
	X = {  	0-compass_dev_0.get(),   45-compass_dev_45.get(),   90-compass_dev_90.get(),  135-compass_dev_135.get(),
			180-compass_dev_180.get(), 225-compass_dev_225.get(), 270-compass_dev_270.get(), 315-compass_dev_315.get()
	};
	Y = { 	compass_dev_0.get(),   compass_dev_45.get(),  compass_dev_90.get(),  compass_dev_135.get(),
			compass_dev_180.get(), compass_dev_225.get(), compass_dev_270.get(), compass_dev_315.get()
	};
}

void Compass::loadDeviationMap(){
	xSemaphoreTake(splineMutex,portMAX_DELAY );
	auto ity = std::begin(Y);
	for(auto itx = std::begin(X); itx != std::end(X); ++itx, ++ity ){
		ESP_LOGI( FNAME, "Initial MAP Heading: %3.2f Deviation: %3.2f", *itx, *ity );
		devmap[ *itx ] = *ity;
	}
	xSemaphoreGive(splineMutex );
}

void Compass::saveDeviation(){
	ESP_LOGI( FNAME, "+++++++++++++ saveDeviation ++++++++++++++");
	xSemaphoreTake(splineMutex,portMAX_DELAY );
#ifdef VERBOSE_LOG
	for( int dir=0; dir < 360; dir+=45 ){
		// ipd[dir] = (float)( deviationSpline((double)dir) );
		ESP_LOGI( FNAME, "OLD DEV Heading=%d  dev=%f", dir, (float)( (*deviationSpline)((double)dir) ));
	}
#endif
	ESP_LOGI( FNAME, "DEV Flash(0) - Spline(0) %3.3f",  abs( compass_dev_0.get() - (*deviationSpline)(0)) );
	if( abs( compass_dev_0.get() - (*deviationSpline)(0)) >1 )
		compass_dev_0.set( (*deviationSpline)(0) );
	if( abs( compass_dev_45.get() - (*deviationSpline)(45)) >1 )
		compass_dev_45.set( (*deviationSpline)(45) );
	if( abs( compass_dev_90.get() - (*deviationSpline)(90)) >1 )
		compass_dev_90.set( (*deviationSpline)(90) );
	if( abs( compass_dev_135.get() - (*deviationSpline)(135)) >1 )
		compass_dev_135.set( (*deviationSpline)(135) );
	if( abs( compass_dev_180.get() - (*deviationSpline)(180)) >1 )
		compass_dev_180.set( (*deviationSpline)(180) );
	if( abs( compass_dev_225.get() - (*deviationSpline)(225)) >1 )
		compass_dev_225.set( (*deviationSpline)(225) );
	if( abs( compass_dev_270.get() - (*deviationSpline)(270)) >1 )
		compass_dev_270.set( (*deviationSpline)(270) );
	if( abs( compass_dev_315.get() - (*deviationSpline)(315)) >1 )
		compass_dev_315.set( (*deviationSpline)(315) );

	for( int dir=0; dir < 360; dir+=45 ){
		// ipd[dir] = (float)( deviationSpline((double)dir) );
		ESP_LOGI( FNAME, "NEW DEV Heading=%d  dev=%f", dir, (float)( (*deviationSpline)((double)dir) ));
	}
	xSemaphoreGive(splineMutex);
}

/**
 * Returns the low pass filtered magnetic heading by considering deviation and
 * declination.
 * Ok is set to true, if heading data is valid, otherwise it is set to false.
 */
float Compass::trueHeading( bool *okIn )
{
	assert( (okIn != nullptr) && "Passing of NULL pointer is forbidden" );
	if( _external_data ){  // Simulation data
			*okIn = true;
			_external_data--;  // age external data
			return m_true_heading_dev;
	}
	if( compass_enable.get() ){
		*okIn = m_headingValid;
		return m_true_heading_dev;
	}
	else{
		*okIn = false;
		return 0;
	}
}

// for simulation purposes
void Compass::setHeading( float h ) {
	m_magn_heading = h;
	m_headingValid=true;
	m_true_heading_dev=h;
	_external_data=100;
};

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
