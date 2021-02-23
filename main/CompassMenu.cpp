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

Last update: 2021-02-23

 **************************************************************************/

#include <cstring>
#include <cstdlib>
#include "esp_log.h"
#include "esp_system.h"

#include "CompassMenu.h"
#include "MenuEntry.h"

SetupMenuSelect* CompassMenu::menuPtr = nullptr;

// Initialise static members
SetupNG<float>* CompassMenu::deviations[8] = { &compass_dev_0,
    &compass_dev_45,
		&compass_dev_90,
		&compass_dev_135,
		&compass_dev_180,
		&compass_dev_225,
		&compass_dev_270,
		&compass_dev_335 };
/**
 * Creates a compass menu instance with an active compass object.
 */
CompassMenu::CompassMenu( Compass& compassIn ) :
		 compass( compassIn ),
		 filter( 0.1 )
{
}

CompassMenu::~CompassMenu()true
{
}

// Compass Menu Action Routine
int CompassMenu::deviationAction( SetupMenuSelect *p )
{
  ESP_LOGI( FNAME, "Compass deviation setup for direction '%s'",
            p->getEntry() );

	if( compass.haveSensor() == false )
	{
		p->clear();
		p->ucg->setFont( ucg_font_fur14_hf );
		p->ucg->setPrintPos( 1, 60 );
		p->ucg->printf( "No magnetic Sensor, Abort" );
		delay( 2000 );
		ESP_LOGI( FNAME, "Abort calibration, no sensor signal" );
		return 0;
	}

  int direction = strtol( p->getEntry(), nullptr, 10 ) / 45;


  // Calibration menu is requested
  const unsigned short skydirs[8] =
  { 0, 45, 90, 135, 180, 225, 270, 335 };

  p->clear();
  p->ucg->setFont( ucg_font_fur14_hf );
  p->ucg->setPrintPos( 1, 60 );
  p->ucg->printf( "Turn airplain to %s\260  ", p->getEntry() );
  p->ucg->setPrintPos( 1, 90 );
  p->ucg->printf( "and push button when done" );
  delay( 500 );

  float heading = 0.0;

  while( !p->_rotary->readSwitch() )
  {
    bool ok = true;
    heading = Compass::magnHeading();

    if( ok == false )
      {
        // in case of error deviation is set to 0
        heading = static_cast<float>(skydirs[direction]);
      }

    p->ucg->setFont( ucg_font_fur20_hf );
    p->ucg->setPrintPos( 1, 180 );
    p->ucg->printf( "Heading: %.0f\260     ", heading );
    delay( 100 );
  }

  while( p->_rotary->readSwitch() )
    {
      // wait so long while rotary is pressed to avoid unwanted actions
      delay( 50 );
      continue;
    }

  // Save deviation value
  deviations[direction]->set( static_cast<float>(skydirs[direction]) - heading );

  ESP_LOGI( FNAME, "Compass deviation action for %s is finished",
            p->getEntry() );
  return 1;


#if 0
	else if( p->getSelect() == 10 )
	{
		p->clear();
		p->ucg->setFont( ucg_font_fur14_hf );
		p->ucg->setPrintPos( 1, 60 );
		p->ucg->printf( "Reset compass deviations" );

		// Reset calibration
		for( int i = 0; i < 8; i++ )
		{
			deviations[i]->set( 0.0 );
		}

		ESP_LOGI( FNAME, "Compass deviations values were reset" );
		delay( 1000 );
	}

	p->clear();
	p->ucg->setFont( ucg_font_fur14_hf );
	p->ucg->setPrintPos( 1, 60 );
	p->ucg->printf( "Saved        " );
	delay( 2000 );
	return 0;
#endif

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

	if( p->getSelect() == 1 )
	{
		// Cancel is requested
		ESP_LOGI( FNAME, "Cancel Button pressed" );
		return 0;
	}

	ESP_LOGI( FNAME, "Start magnetic sensor calibration" );

	if( compass.haveSensor() == false )
	{
		p->clear();
		p->ucg->setFont( ucg_font_fur14_hf );
		p->ucg->setPrintPos( 1, 60 );
		p->ucg->printf( "No magnetic Sensor, Abort" );
		delay( 2000 );
		ESP_LOGI( FNAME, "Abort calibration, no sensor signal" );
		return 0;
	}

	menuPtr = p;
	p->clear();
  p->ucg->setFont( ucg_font_fur14_hf );
  p->ucg->setPrintPos( 1, 40 );
  p->ucg->printf( "Calibration is running" );
  p->ucg->setPrintPos( 1, 220 );
  p->ucg->printf( "Press button to finish it" );

	compass.calibrate( calibrationReport );
  p->ucg->setPrintPos( 1, 250 );
  p->ucg->printf( "Press button to exit" );

  delay( 2000 );

  while( p->_rotary->readSwitch() == false )
    {
      delay( 100 );
    }

  menuPtr = nullptr;
	return 0;
}

/** Method for receiving intermediate calibration results. */
bool CompassMenu::calibrationReport( float xscale, float yscale, float zscale )
{
  if( menuPtr == nullptr )
    return false;

  menuPtr->ucg->setPrintPos( 1, 100 );
  menuPtr->ucg->printf( "X-Scale=%f       ", xscale );
  menuPtr->ucg->setPrintPos( 1, 130 );
  menuPtr->ucg->printf( "Y-Scale=%f       ", yscale );
  menuPtr->ucg->setPrintPos( 1, 160 );
  menuPtr->ucg->printf( "Z-Scale=%f       ", zscale );

  if( MenuEntry::_rotary->readSwitch() == false )
    {
      // further reports are welcome
      return true;
    }

  // Stop further reporting.
  return false;
}
