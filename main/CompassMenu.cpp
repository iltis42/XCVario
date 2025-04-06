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

#include "CompassMenu.h"
 
#include "SetupMenu.h"
#include "sensor.h"  // we need spiMutex
#include "vector.h"
#include "Colors.h"
#include "MagnetSensor.h"
#include "logdef.h"

#include <cstring>
#include <cstdlib>
#include <cmath>
#include <esp_system.h>


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
	ESP_LOGI( FNAME, "Compass deviation setup for direction '%s'",	p->value() );

	if( !compass || !(compass->haveSensor()) )
	{
		p->clear();
		MYUCG->setFont( ucg_font_ncenR14_hr );
		MYUCG->setPrintPos( 1, 30 );
		MYUCG->printf( "No magnetic Sensor, Abort" );
		ESP_LOGI( FNAME, "Abort calibration, no sensor signal" );
		return 0;
	}
	short direction = strtol( p->value(), nullptr, 10 );
	// Calibration menu is requested
	p->clear();
	MYUCG->setFont( ucg_font_ncenR14_hr );
	MYUCG->setPrintPos( 1, 60 );
	MYUCG->printf( "Turn airplane to %s  ", p->value() );
	MYUCG->setPrintPos( 1, 90 );
	MYUCG->printf( "and push button when done" );
	delay( 500 );

	static float heading = 0.0;
	float deviation = 0;
	static float last_heading = 0.0;

	while( !Rotary->readSwitch() )
	{
		bool ok = true;
		heading += Vector::angleDiffDeg( compass->rawHeading( &ok ), last_heading )*0.05;
		heading = Vector::normalizeDeg( heading );
		last_heading = heading;
		if( ok == false )
		{
			continue;
		}
		MYUCG->setPrintPos( 1, 150 );
		MYUCG->setFont( ucg_font_fub25_hf );
		MYUCG->printf( "Heading: %.1f°  ", heading );
		MYUCG->setPrintPos( 1, 200 );
		deviation = Vector::angleDiffDeg( direction, heading );
		MYUCG->printf( "Deviation: %.1f°  ", deviation );
		delay( 50 );
	}
	while( Rotary->readSwitch() )
	{
		// wait so long while rotary is active to avoid unwanted actions
		delay( 50 );
		continue;
	}

	// Save and update deviation value
	compass->newDeviation( heading, direction, true );
	MYUCG->setPrintPos( 1, 270 );
	MYUCG->setFont( ucg_font_ncenR14_hr );
	MYUCG->printf( "Saved" );
	delay(500);
	MYUCG->setPrintPos( 1, 300 );
	p->getParent()->incHighlight();
	if(p->getParent()->getHighlight() > 7 )
		p->getParent()->highlightTop();
	MYUCG->printf( "Press key for next" );
	ESP_LOGI( FNAME, "Compass deviation action for %s is finished",	p->value() );
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
		MYUCG->setFont( ucg_font_ncenR14_hr );
		MYUCG->setPrintPos( 1, 60 );
		MYUCG->printf( "Reset all compass" );
		MYUCG->setPrintPos( 1, 90 );
		MYUCG->printf( "deviation data" );
		// Reset deviation
		for( int i = 0; i < 8; i++ )
		{
			deviations[i]->set( 0.0 );
		}
		ESP_LOGI( FNAME, "All compass deviations values were reset" );
		delay( 1000 );
	}
	// Reload compass interpolation data


	p->clear();
	MYUCG->setFont( ucg_font_ncenR14_hr );
	MYUCG->setPrintPos( 1, 300 );
	if( compass ){
		compass->resetDeviation();
		MYUCG->printf( "Saved        " );
	}
	else
		MYUCG->printf( "Compass not configured" );
	delay( 2000 );

	return 0;
}

int CompassMenu::declinationAction( SetupMenuValFloat *p )
{
	ESP_LOGI( FNAME, "declinationAction()" );

	ESP_LOGI( FNAME, "Compass declination set to valid" );

	return 0;
}

float tesla=0;
float tesla_cal=0;

bool CompassMenu::showSensorRawData(SetupMenuSelect *p)
{
	if( compass == 0 ){
		ESP_LOGI( FNAME, "showSensorRawData(): no compass" );
		return false;
	}
	t_magn_axes raw = compass->getRawAxes();
	// ESP_LOGI( FNAME, "showSensorRawData() %d %d %d", raw.x, raw.y, raw.z );
	MYUCG->setColor( COLOR_WHITE );
	MYUCG->setPrintPos( 1, 60 );
	MYUCG->printf( "X = %d  ", raw.x );
	MYUCG->setPrintPos( 1, 90 );
	MYUCG->printf( "Y = %d  ", raw.y );
	MYUCG->setPrintPos( 1, 120 );
	MYUCG->printf( "Z = %d  ", raw.z );
	MYUCG->setPrintPos( 1, 150 );
	float t = sqrt( compass->curX()*compass->curX() + compass->curY()*compass->curY() + compass->curZ()*compass->curZ() )/150.0;
	if( abs(t-tesla) > 5 )
		tesla += (t-tesla)*0.2;
	else if ( abs(t-tesla) > 1 )
		tesla += (t-tesla)*0.07;
	else
		tesla += (t-tesla)*0.01;
	MYUCG->printf( "Raw magn H= %.1f uT", tesla );
	if( compass_calibrated.get() ){
		MYUCG->setPrintPos( 1, 180 );
		float t= sqrt( compass->calX()*compass->calX() + compass->calY()*compass->calY() + compass->calZ()*compass->calZ() )/150.0;
		if( abs(t-tesla_cal) > 5 )
			tesla_cal += (t-tesla_cal)*0.2;
		else if ( abs(t-tesla_cal) > 1 )
			tesla_cal += (t-tesla_cal)*0.07;
		else
			tesla_cal += (t-tesla_cal)*0.01;
		MYUCG->printf( "Cal magn H= %.1f uT", tesla_cal );
	}
	return true;
}

int CompassMenu::sensorCalibrationAction( SetupMenuSelect *p )
{
	ESP_LOGI( FNAME, "sensorCalibrationAction() selected: %d", p->getSelect());
	bool only_show = (p->getSelect() == 2);  // Show
	bool show_raw_data = (p->getSelect() == 3);  // Show X,Y,Z
	MYUCG->setFont( ucg_font_ncenR14_hr, true );
	if( show_raw_data ){
		p->clear();
		while( !Rotary->readSwitch() ){
				delay( 100 );
				showSensorRawData(p);
		}
		return 0;
	}

	if( p->getSelect() == 0 )
	{
		// Cancel is requested
		ESP_LOGI( FNAME, "Cancel Button pressed" );
		return 0;
	}
	if( !compass ){
		p->clear();
		MYUCG->setFont( ucg_font_ncenR14_hr, true );
		MYUCG->setPrintPos( 1, 30 );
		MYUCG->printf( "Compass not configured" );
		delay(2000);
		return 0;
	}

	ESP_LOGI( FNAME, "Start magnetic sensor calibration" );

	menuPtr = p;

	p->clear();
	if( only_show ){
		compass->calibrate( calibrationReport, true );
		while( !Rotary->readSwitch() )
			delay( 100 );
	}else{
		MYUCG->setFont( ucg_font_ncenR14_hr, true );
		MYUCG->setPrintPos( 1, 30 );
		MYUCG->printf( "Calibration is started" );
		MYUCG->setPrintPos( 1, 220 );
		MYUCG->printf( "Now rotate sensor until" );
		MYUCG->setPrintPos( 1, 245 );
		MYUCG->printf( "all numbers are green" );
		MYUCG->setPrintPos( 1, 270 );
		MYUCG->printf( "Press button to finish" );
		compass->calibrate( calibrationReport, false);
		MYUCG->setPrintPos( 1, 250 );
		delay( 1000 );
		p->clear();
	}

	menuPtr = nullptr;
	return 0;
}



/** Method for receiving intermediate calibration results. */
bool CompassMenu::calibrationReport( t_magn_axes raw, t_float_axes scale, t_float_axes bias, bitfield_compass b, bool print )
{
	static t_magn_axes  raw_back = {0, 0, 0};
	static t_float_axes scale_back = {0., 0., 0.};
	static t_float_axes bias_back = {0., 0., 0.};
	static t_magn_axes mins = { 20000, 20000, 20000 };
	static t_magn_axes maxs = { 0,0,0 };
	static bitfield_compass bits_old;// = { 0, 0, 0, 0, 0, 0 };

	if( menuPtr == nullptr )
		return false;

	// X := -y
	if( b.ymax_green && b.ymin_green )
		MYUCG->setColor( COLOR_GREEN );
	else
		MYUCG->setColor( COLOR_WHITE );
	if(scale.y != scale_back.y || print || !(bits_old == b) ){
		MYUCG->setPrintPos( 1, 60 );
		MYUCG->printf( "X-Scale=%3.1f  ", -scale.y * 100);
		scale_back.y = scale.y;
	}
	if( (raw_back.y != raw.y || !(bits_old == b) ) && !print  ){
		MYUCG->setPrintPos( 160, 60 );
		MYUCG->printf( "(%.1f)  ", (float)(-raw.y)/32768 *100 );
		raw_back.y = raw.y;
	}
	if( bias.y != bias_back.y || print || !(bits_old == b) ){
		MYUCG->setPrintPos( 1, 135 );
		MYUCG->printf( "X-Bias=%3.1f  ", -bias.y/32768 *100 );
		bias_back.y = bias.y;
	}
	// Y := -x
	if( b.xmax_green && b.xmin_green )
		MYUCG->setColor( COLOR_GREEN );
	else
		MYUCG->setColor( COLOR_WHITE );
	if( scale.x != scale_back.x || print || !(bits_old == b) ){
		MYUCG->setPrintPos( 1, 85 );
		MYUCG->printf( "Y-Scale=%3.1f  ", -scale.x * 100 );
		scale_back.x = scale.x;
	}
	if( (raw_back.x != raw.x || !(bits_old == b)) && !print ){
		MYUCG->setPrintPos( 160, 85 );
		MYUCG->printf( "(%.1f)  ", (float)(-raw.x)/32768 *100 );
		raw_back.x = raw.x;
	}
	if( bias.x != bias_back.x || print || !(bits_old == b) ){
		MYUCG->setPrintPos( 1, 160 );
		MYUCG->printf( "Y-Bias=%3.1f  ", -bias.x/32768 *100 );
		bias_back.x = bias.x;
	}
	// Z := -z
	if( b.zmax_green && b.zmin_green )
		MYUCG->setColor( COLOR_GREEN );
	else
		MYUCG->setColor( COLOR_WHITE );
	if( scale.z != scale_back.z || print || !(bits_old == b) ){
		MYUCG->setPrintPos( 1, 110 );
		MYUCG->printf( "Z-Scale=%3.1f  ", -scale.z * 100 );
		scale_back.z = scale.z;
	}
	if( (raw_back.z != raw.z || !(bits_old == b)) && !print ){
		MYUCG->setPrintPos( 160, 110 );
		MYUCG->printf( "(%.1f)  ", (float)(-raw.z)/32768 *100 );
		raw_back.z = raw.z;
	}
	if( bias.z != bias_back.z || print || !(bits_old == b) ){
		MYUCG->setPrintPos( 1, 185 );
		MYUCG->printf( "Z-Bias=%3.1f  ", -bias.z/32768 *100 );
		bias_back.z = bias.z;
	}
	// ESP_LOGI(FNAME,"Max X: %d Y: %d Z: %d  Min P: %d %d %d", xm, ym, zm, xi, yi, mins.z  );

	if( !print ){
		const uint16_t X = 180;
		const uint16_t Y = 155;
		t_magn_axes peak;

		peak.x = int16_t((float)raw.x*160.f/32768);
		peak.y = int16_t((float)raw.y*114.f/32768);
		peak.z = int16_t((float)raw.z*160.f/32768);

		maxs.x = maxs.x > peak.x ? maxs.x : peak.x;
		maxs.y = maxs.y > peak.y ? maxs.y : peak.y;
		maxs.z = maxs.z > peak.z ? maxs.z:  peak.z;
		mins.x = mins.x < peak.x ? mins.x : peak.x;
		mins.y = mins.y < peak.y ? mins.y : peak.y;
		mins.z = mins.z < peak.z ? mins.z : peak.z;

		// draw mag X alias -Y 45° as the glider axes -y/y
		if( b.ymax_green )
			MYUCG->setColor( COLOR_GREEN );
		else
			MYUCG->setColor( COLOR_RED );
		MYUCG->drawLine( X, Y, X-maxs.y, Y+maxs.y);    // 45 degree

		if( b.ymin_green )
			MYUCG->setColor( COLOR_GREEN );
		else
			MYUCG->setColor( COLOR_RED );
		MYUCG->drawLine( X, Y, X-mins.y, Y+mins.y  );    // 45 degree

		// draw mag Y alias -X along -x screen axes
		if( b.xmax_green )
			MYUCG->setColor( COLOR_GREEN );
		else
			MYUCG->setColor( COLOR_RED );
		MYUCG->drawLine( X, Y, X-maxs.x, Y );

		if( b.xmin_green )
			MYUCG->setColor( COLOR_GREEN );
		else
			MYUCG->setColor( COLOR_RED );
		MYUCG->drawLine( X, Y, X-mins.x, Y );

		// draw mag Z alias -Z along -y screen axes
		if( b.zmax_green )
			MYUCG->setColor( COLOR_GREEN );
		else
			MYUCG->setColor( COLOR_RED );
		MYUCG->drawLine( X, Y, X, Y+maxs.z);

		if( b.zmin_green )
			MYUCG->setColor( COLOR_GREEN );
		else
			MYUCG->setColor( COLOR_RED );
		MYUCG->drawLine( X, Y, X, Y+mins.z  );

		static t_magn_axes old = { 0,0,0 };
		MYUCG->setColor( COLOR_BLACK );
		MYUCG->drawCircle( X-old.y, Y+old.y, 2 );
		MYUCG->drawCircle( X-old.x, Y, 2 );
		MYUCG->drawCircle( X, Y+old.z,2 );
		MYUCG->setColor( COLOR_WHITE );
		MYUCG->drawCircle( X-peak.y, Y+peak.y, 2 );
		MYUCG->drawCircle( X-peak.x, Y, 2);
		MYUCG->drawCircle( X, Y+peak.z, 2 );
		old = peak;
		bits_old = b;
	}


	// Stop further reporting.
	return true;
}
