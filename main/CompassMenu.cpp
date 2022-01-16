/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

File: CompassMenu.cpp

Class to handle compass data and actions.

Author: Axel Pauli, February 2021

Last update: 2021-12-30

 **************************************************************************/

#include <cstring>
#include <cstdlib>
#include <cmath>
#include "esp_log.h"
#include "esp_system.h"

#include "CompassMenu.h"
#include "MenuEntry.h"
#include "sensor.h"  // we need spiMutex
#include "vector.h"
#include "Colors.h"
#include "logdef.h"

SetupMenuSelect* CompassMenu::menuPtr = nullptr;

// Initialise static members
SetupNG<float>* CompassMenu::deviations[8] = {
		&compass_dev_0,
		&compass_dev_45,
		&compass_dev_90,
		&compass_dev_135,
		&compass_dev_180,
		&compass_dev_225,
		&compass_dev_270,
		&compass_dev_315 };
/**
 * Creates a compass menu instance with an active compass object.
 */
CompassMenu::CompassMenu()
{
}

CompassMenu::~CompassMenu()
{
}

// Compass Menu Action Routine
int CompassMenu::deviationAction( SetupMenuSelect *p )
{
	ESP_LOGI( FNAME, "Compass deviation setup for direction '%s'",	p->getEntry() );

	if( !compass || !(compass->haveSensor()) )
	{
		p->clear();
		p->ucg->setFont( ucg_font_ncenR14_hr );
		p->ucg->setPrintPos( 1, 30 );
		p->ucg->printf( "No magnetic Sensor, Abort" );
		ESP_LOGI( FNAME, "Abort calibration, no sensor signal" );
		return 0;
	}
	short direction = strtol( p->getEntry(), nullptr, 10 );
	// Calibration menu is requested
	p->clear();
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setFont( ucg_font_ncenR14_hr );
	p->ucg->setPrintPos( 1, 60 );
	p->ucg->printf( "Turn airplane to %s  ", p->getEntry() );
	p->ucg->setPrintPos( 1, 90 );
	p->ucg->printf( "and push button when done" );
	xSemaphoreGive(spiMutex);
	delay( 500 );

	static float heading = 0.0;
	float deviation = 0;
	static float last_heading = 0.0;

	while( !p->readSwitch() )
	{
		bool ok = true;
		heading += Vector::angleDiffDeg( compass->rawHeading( &ok ), last_heading )*0.05;
		heading = Vector::normalizeDeg( heading );
		last_heading = heading;
		if( ok == false )
		{
			continue;
		}
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		p->ucg->setPrintPos( 1, 150 );
		p->ucg->setFont( ucg_font_fub25_hf );
		p->ucg->printf( "Heading: %.1f°  ", heading );
		p->ucg->setPrintPos( 1, 200 );
		deviation = Vector::angleDiffDeg( direction, heading );
		p->ucg->printf( "Deviation: %.1f°  ", deviation );
		xSemaphoreGive(spiMutex);
		delay( 50 );
	}
	while( p->readSwitch() )
	{
		// wait so long while rotary is pressed to avoid unwanted actions
		delay( 50 );
		continue;
	}

	// Save and update deviation value
	compass->newDeviation( heading, direction, true );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->setPrintPos( 1, 270 );
	p->ucg->setFont( ucg_font_ncenR14_hr );
	p->ucg->printf( "Saved" );
	delay(500);
	p->ucg->setPrintPos( 1, 300 );
	p->_parent->highlight++;
	if(p->_parent->highlight > 7 )
		p->_parent->highlight = -1;
	p->ucg->printf( "Press key for next" );
	xSemaphoreGive(spiMutex);
	ESP_LOGI( FNAME, "Compass deviation action for %s is finished",	p->getEntry() );
	return 0;
}

/** Compass Menu Action method to reset all deviations to 0. */
int CompassMenu::resetDeviationAction( SetupMenuSelect *p )
{
	if( p->getSelect() == 0 )
	{
		// Cancel is selected
		return 0;
	}
	else if( p->getSelect() == 1 )
	{
		p->clear();
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		p->ucg->setFont( ucg_font_ncenR14_hr );
		p->ucg->setPrintPos( 1, 60 );
		p->ucg->printf( "Reset all compass" );
		p->ucg->setPrintPos( 1, 90 );
		p->ucg->printf( "deviation data" );
		xSemaphoreGive(spiMutex);
		// Reset deviation
		for( int i = 0; i < 8; i++ )
		{
			deviations[i]->set( 0.0 );
		}
		SetupCommon::commitNow();
		ESP_LOGI( FNAME, "All compass deviations values were reset" );
		delay( 1000 );
	}
	// Reload compass interpolation data


	p->clear();
	p->ucg->setFont( ucg_font_ncenR14_hr );
	p->ucg->setPrintPos( 1, 300 );
	if( compass ){
		compass->resetDeviation();
		p->ucg->printf( "Saved        " );
	}
	else
		p->ucg->printf( "Compass not configured" );
	delay( 2000 );

	return 0;
}

int CompassMenu::declinationAction( SetupMenuValFloat *p )
{
	ESP_LOGI( FNAME, "declinationAction()" );

	if( p->pressed )
	{
		ESP_LOGI( FNAME, "Compass declination set to valid" );
	}

	return 0;
}

int CompassMenu::sensorCalibrationAction( SetupMenuSelect *p )
{
	ESP_LOGI( FNAME, "sensorCalibrationAction()" );
	bool only_show = (p->getSelect() == 2);  // Show

	if( p->getSelect() == 0 )
	{
		// Cancel is requested
		ESP_LOGI( FNAME, "Cancel Button pressed" );
		return 0;
	}
	if( !compass ){
		p->clear();
		p->ucg->setFont( ucg_font_ncenR14_hr, true );
		p->ucg->setPrintPos( 1, 30 );
		p->ucg->printf( "Compass not configured" );
		delay(2000);
		return 0;
	}

	ESP_LOGI( FNAME, "Start magnetic sensor calibration" );

	menuPtr = p;

	p->clear();
	if( only_show ){
		compass->calibrate( calibrationReport, true );
		while( !p->readSwitch() )
			delay( 100 );
	}else{
		p->ucg->setFont( ucg_font_ncenR14_hr, true );
		p->ucg->setPrintPos( 1, 30 );
		p->ucg->printf( "Calibration is started" );
		p->ucg->setPrintPos( 1, 220 );
		p->ucg->printf( "Now rotate sensor until" );
		p->ucg->setPrintPos( 1, 245 );
		p->ucg->printf( "all numbers are green" );
		p->ucg->setPrintPos( 1, 270 );
		p->ucg->printf( "Press button to finish" );
		compass->calibrate( calibrationReport, false);
		p->ucg->setPrintPos( 1, 250 );
		delay( 1000 );
		p->clear();
	}

	menuPtr = nullptr;
	return 0;
}

t_magn_axes CompassMenu::raw_back =    { 0,0,0 };
t_float_axes CompassMenu::scale_back = { 0.0, 0.0, 0.0 };
t_float_axes CompassMenu::bias_back =  { 0.0, 0.0, 0.0 };

static t_magn_axes mins = { 20000, 20000, 20000 };
static t_magn_axes maxs = { 0,0,0 };

static bitfield_compass bits_old = { false, false, false, false, false, false };

/** Method for receiving intermediate calibration results. */
bool CompassMenu::calibrationReport( t_magn_axes raw, t_float_axes scale, t_float_axes bias, bitfield_compass b, bool print )
{
	if( menuPtr == nullptr )
		return false;
	xSemaphoreTake(spiMutex,portMAX_DELAY );

	// X
	if( b.xmax_green && b.xmin_green )
		menuPtr->ucg->setColor( COLOR_GREEN );
	else
		menuPtr->ucg->setColor( COLOR_WHITE );
	if( scale.x != scale_back.x || print || !(bits_old == b) ){
		menuPtr->ucg->setPrintPos( 1, 60 );
		menuPtr->ucg->printf( "X-Scale=%3.1f  ", scale.x * 100 );
		scale_back.x = scale.x;
	}
	if( (raw_back.x != raw.x || !(bits_old == b)) && !print ){
		menuPtr->ucg->setPrintPos( 160, 60 );
		menuPtr->ucg->printf( "(%.1f)  ", (float)(raw.x)/32768 *100 );
		raw_back.x = raw.x;
	}
	if( bias.x != bias_back.x || print || !(bits_old == b) ){
		menuPtr->ucg->setPrintPos( 1, 135 );
		menuPtr->ucg->printf( "X-Bias=%3.1f  ", bias.x/32768 *100 );
		bias_back.x = bias.x;
	}
	// Y
	if( b.ymax_green && b.ymin_green )
		menuPtr->ucg->setColor( COLOR_GREEN );
	else
		menuPtr->ucg->setColor( COLOR_WHITE );
	if(scale.y != scale_back.y || print || !(bits_old == b) ){
		menuPtr->ucg->setPrintPos( 1, 85 );
		menuPtr->ucg->printf( "Y-Scale=%3.1f  ", scale.y * 100);
		scale_back.y = scale.y;
	}
	if( (raw_back.y != raw.y || !(bits_old == b) ) && !print  ){
		menuPtr->ucg->setPrintPos( 160, 85 );
		menuPtr->ucg->printf( "(%.1f)  ", (float)(raw.y)/32768 *100 );
		raw_back.y = raw.y;
	}
	if( bias.y != bias_back.y || print || !(bits_old == b) ){
		menuPtr->ucg->setPrintPos( 1, 160 );
		menuPtr->ucg->printf( "Y-Bias=%3.1f  ", bias.y/32768 *100 );
		bias_back.y = bias.y;
	}
	// Z
	if( b.zmax_green && b.zmin_green )
		menuPtr->ucg->setColor( COLOR_GREEN );
	else
		menuPtr->ucg->setColor( COLOR_WHITE );
	if( scale.z != scale_back.z || print || !(bits_old == b) ){
		menuPtr->ucg->setPrintPos( 1, 110 );
		menuPtr->ucg->printf( "Z-Scale=%3.1f  ", scale.z * 100 );
		scale_back.z = scale.z;
	}
	if( (raw_back.z != raw.z || !(bits_old == b)) && !print ){
		menuPtr->ucg->setPrintPos( 160, 110 );
		menuPtr->ucg->printf( "(%.1f)  ", (float)(raw.z)/32768 *100 );
		raw_back.z = raw.z;
	}
	if( bias.z != bias_back.z || print || !(bits_old == b) ){
		menuPtr->ucg->setPrintPos( 1, 185 );
		menuPtr->ucg->printf( "Z-Bias=%3.1f  ", bias.z/32768 *100 );
		bias_back.z = bias.z;
	}
	// ESP_LOGI(FNAME,"Max X: %d Y: %d Z: %d  Min P: %d %d %d", xm, ym, zm, xi, yi, mins.z  );

	if( !print ){
		const uint16_t X = 180;
		const uint16_t Y = 155;
		t_magn_axes peak;

		peak.x = int16_t((float)raw.x*160/32768);
		peak.y = int16_t((float)raw.y*160/32768);
		peak.z = int16_t((float)raw.z*114/32768);

		maxs.x = maxs.x > peak.x ? maxs.x : peak.x;
		maxs.y = maxs.y > peak.y ? maxs.y : peak.y;
		maxs.z = maxs.z > peak.z ? maxs.z:  peak.z;
		mins.x = mins.x < peak.x ? mins.x : peak.x;
		mins.y = mins.y < peak.y ? mins.y : peak.y;
		mins.z  = mins.z < peak.z ? mins.z : peak.z;

		if( b.ymax_green )
			menuPtr->ucg->setColor( COLOR_GREEN );
		else
			menuPtr->ucg->setColor( COLOR_RED );
		menuPtr->ucg->drawLine( X, Y, X, Y+maxs.y );

		if( b.ymin_green )
			menuPtr->ucg->setColor( COLOR_GREEN );
		else
			menuPtr->ucg->setColor( COLOR_RED );
		menuPtr->ucg->drawLine( X, Y, X, Y+mins.y );

		if( b.xmax_green )
			menuPtr->ucg->setColor( COLOR_GREEN );
		else
			menuPtr->ucg->setColor( COLOR_RED );
		menuPtr->ucg->drawLine( X, Y, X+maxs.x, Y );

		if( b.xmin_green )
			menuPtr->ucg->setColor( COLOR_GREEN );
		else
			menuPtr->ucg->setColor( COLOR_RED );
		menuPtr->ucg->drawLine( X, Y, X+mins.x, Y );

		if( b.zmax_green )
			menuPtr->ucg->setColor( COLOR_GREEN );
		else
			menuPtr->ucg->setColor( COLOR_RED );
		menuPtr->ucg->drawLine( X, Y, X+maxs.z, Y-maxs.z);    // 45 degree

		if( b.zmin_green )
			menuPtr->ucg->setColor( COLOR_GREEN );
		else
			menuPtr->ucg->setColor( COLOR_RED );
		menuPtr->ucg->drawLine( X, Y, X+mins.z, Y-mins.z  );    // 45 degree

		static t_magn_axes old = { 0,0,0 };
		menuPtr->ucg->setColor( COLOR_BLACK );
		menuPtr->ucg->drawCircle( X, Y+old.y, 2 );
		menuPtr->ucg->drawCircle( X+ old.x, Y, 2 );
		menuPtr->ucg->drawCircle( X+ old.z, Y-old.z,2 );
		menuPtr->ucg->setColor( COLOR_WHITE );
		menuPtr->ucg->drawCircle( X, Y+peak.y, 2 );
		menuPtr->ucg->drawCircle( X+peak.x, Y, 2);
		menuPtr->ucg->drawCircle( X+peak.z, Y-peak.z, 2 );
		old = peak;
		bits_old = b;
	}


	xSemaphoreGive(spiMutex);
	// Stop further reporting.
	return true;
}
