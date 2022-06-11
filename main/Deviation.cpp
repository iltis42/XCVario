/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

File: Deviation.cpp

Class to handle compass deviation data, auto deviation and deviation as been setup

Author: Eckhard Völlm January 2021

Last update: 2021-03-29

 **************************************************************************/

#include <cassert>
#include <cmath>
#include "esp_log.h"
#include "esp_system.h"
#include "sensor.h"
#include "Deviation.h"
#include "KalmanMPU6050.h"

xSemaphoreHandle Deviation::splineMutex = 0;

/*
  Creates instance for I2C connection with passing the desired parameters.
  No action is done at the bus. The default address of the chip is 0x0D.
 */
Deviation::Deviation()
{
	ESP_LOGI(FNAME,"Deviation()");
	deviationSpline = 0;
	_devHolddown = 1800;
	samples = 0;
}

Deviation::~Deviation()
{
}

void Deviation::deviationReload(){
	ESP_LOGI( FNAME, "deviationReload()");
	readInterpolationData();
	loadDeviationMap();
	recalcInterpolationSpline();
}

void Deviation::begin(){
	splineMutex = xSemaphoreCreateMutex();
	X.reserve(40);
	Y.reserve(40);
	deviationReload();
}

/**
 * Compute heading deviation by using linear interpolation.
 *
 * @param heading Heading value between 0...359
 */
float Deviation::getDeviation( float heading )
{
	if( !deviationSpline )
		return 0.0;
	xSemaphoreTake(splineMutex,portMAX_DELAY );
	float dev=(*deviationSpline)((double)heading);
	// ESP_LOGI( FNAME, "RawHeading=%.1f : deviation=%0.2f", heading, dev );
	xSemaphoreGive(splineMutex);
	return( dev );
}

void Deviation::resetDeviation()
{
	ESP_LOGI( FNAME,"resetDeviation()");
	X.clear();
	Y.clear();
	devmap.clear();
	deviationReload();
}

// new Deviation from reverse calculated TAWC Wind measurement
bool Deviation::newDeviation( float measured_heading, float desired_heading, bool force ){
	double deviation = Vector::angleDiffDeg( desired_heading , measured_heading );
	// ESP_LOGI( FNAME, "newDeviation Measured Head: %3.2f Desired Head: %3.2f => Deviation=%3.2f, Samples:%d", measured_heading, desired_heading, deviation, samples );
	if( (abs(deviation) > wind_max_deviation.get()) && !force ){ // data is not plausible/useful
		ESP_LOGW( FNAME, "new Deviation out of bounds: %3.3f: Drop this deviation", deviation );
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
			// ESP_LOGI( FNAME, "Erase from map dev for heading=%3.2f, diff=%3.3f", it->first, diff );
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
	// ESP_LOGI( FNAME, "Deviation Delta: %f old dev: %f, new dev: %f", delta, old_dev, old_dev + (delta * 0.15) );
	float k=wind_dev_filter.get();
	if(old_dev == 0.0) {
		ESP_LOGI( FNAME, "We are starting so provide initial value");
		k=1.0;
	}
	if( force )
		devmap[ measured_heading ] = deviation;  // insert as is from manual measurement
	else
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
	if( ((samples > 50) && (_devHolddown <= 0)) || force ){
		saveDeviation();
		_devHolddown = 1800; // maximum every half hour
		samples = 0;
	}
	return true;
}

/**
 * Setup the deviation interpolation data.
 */
void Deviation::recalcInterpolationSpline()
{
	ESP_LOGI( FNAME, "recalcInterpolationSpline()");
	xSemaphoreTake(splineMutex,portMAX_DELAY );
#ifdef VERBOSE_LOG
	if( deviationSpline ){
		for( int dir=0; dir <= 360; dir+=45 ){
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
		if( it->first >= 210 ){  // 360° - 140°
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
		if( it->first <= 140 ){  // 140°
			// ESP_LOGI( FNAME, "Post  X/Y Vector Head: %3.2f Dev: %3.2f", it->first+360, it->second );
			X.push_back( it->first+360.0 );
			Y.push_back( it->second );
		}
	}

	deviationSpline  = new tk::spline(X,Y, tk::spline::cspline_hermite );
	xSemaphoreGive(splineMutex );
#ifdef VERBOSE_LOG
	for( int dir=0; dir <= 360; dir+=45 ){
		ESP_LOGI( FNAME, "NEW DEV Heading=%d  dev=%f", dir, (float)( (*deviationSpline)((double)dir) ));
	}
#endif

}

void Deviation::readInterpolationData()
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

void Deviation::loadDeviationMap(){
	xSemaphoreTake(splineMutex,portMAX_DELAY );
	auto ity = std::begin(Y);
	for(auto itx = std::begin(X); itx != std::end(X); ++itx, ++ity ){
		ESP_LOGI( FNAME, "Initial MAP Heading: %3.2f Deviation: %3.2f", *itx, *ity );
		devmap[ *itx ] = *ity;
	}
	xSemaphoreGive(splineMutex );
}

void Deviation::saveDeviation(){
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

