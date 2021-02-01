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

Last update: 2021-02-01

**************************************************************************/

#include "esp_log.h"
#include "esp_system.h"

#include "CompassMenu.h"

// Initialise static members
SetupNG<float>* CompassMenu::deviations[8] = { &compass_dev_45,
                                               &compass_dev_90,
                                               &compass_dev_135,
                                               &compass_dev_180,
                                               &compass_dev_225,
                                               &compass_dev_270,
                                               &compass_dev_335,
                                               &compass_dev_360 };
/**
 * Creates a compass menu instance with an active compass object.
 */
CompassMenu::CompassMenu( Compass& compassIn ) :
 RotaryObserver(),
 compass( compassIn )
{
}

CompassMenu::~CompassMenu()
{
}

// Compass Menu Action Routine
int CompassMenu::calibrateAction( SetupMenuSelect *p )
{
  ESP_LOGI( FNAME, "Compass calibration ( %d ) ", p->getSelect() );

  if( p->getSelect() == 0 )
    {
      // Cancel is requested
      delay( 500 );
      ESP_LOGI( FNAME, "Cancel Button pressed" );
      return 0;
    }

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

  if( p->getSelect() == 1 )
    {
      // Calibration menu is requested
      const unsigned short skydirs[8] =
        { 45, 90, 135, 180, 225, 270, 335, 360 };

      for( int i = 0; i < 8; i++ )
        {
          p->clear();
          p->ucg->setFont( ucg_font_fur14_hf );
          p->ucg->setPrintPos( 1, 60 );
          p->ucg->printf( "Turn airplain to %d\260  ", skydirs[i] );
          p->ucg->setPrintPos( 1, 90 );
          p->ucg->printf( "and push button when done" );
          delay( 500 );

          float heading = 0.0;

          while( !p->_rotary->readSwitch() )
            {
              bool ok = false;
              float heading = compass.heading( &ok );

              if( ok == false )
                {
                  // in case of error deviation is set to 0
                  heading = static_cast<float>(skydirs[i]);
                }

              p->ucg->setFont( ucg_font_fur20_hf );
              p->ucg->setPrintPos( 1, 180 );
              p->ucg->printf( "Heading: %.1f\260  ", heading );
              delay( 250 );
            }

          // Save deviation value
          deviations[i]->set( static_cast<float>(skydirs[i]) - heading );
        }

      ESP_LOGI( FNAME, "Compass calibration is finished" );
    }
  else if( p->getSelect() == 2 )
    {
      p->clear();
      p->ucg->setFont( ucg_font_fur14_hf );
      p->ucg->setPrintPos( 1, 60 );
      p->ucg->printf( "Reset compass calibration" );

      // Reset calibration
      for( int i = 0; i < 8; i++ )
        {
          deviations[i]->set( 0.0 );
        }

      ESP_LOGI( FNAME, "Compass calibration was reset" );
      delay( 1000 );
    }

  p->clear();
  p->ucg->setFont( ucg_font_fur14_hf );
  p->ucg->setPrintPos( 1, 60 );
  p->ucg->printf( "Saved        " );
  delay( 2000 );

  return 0;
}

int CompassMenu::declinationAction( SetupMenuValFloat *p )
{
  if( p->pressed )
    {
      ESP_LOGI( FNAME, "Compass declination set to valid" );
      comp_decl_valid.set( true );
    }

  return 0;
}

void CompassMenu::up( int count )
{
  ESP_LOGI( FNAME, "CompassMenu::up" );
}

void CompassMenu::down( int count )
{
  ESP_LOGI( FNAME, "CompassMenu::down" );
}

void CompassMenu::press()
{
  ESP_LOGI( FNAME, "CompassMenu::press" );
}

void CompassMenu::release()
{
  ESP_LOGI( FNAME, "CompassMenu::release" );
}

void CompassMenu::longPress()
{
  ESP_LOGI( FNAME, "CompassMenu::longPress" );
}
