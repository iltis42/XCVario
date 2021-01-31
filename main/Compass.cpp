/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************

File: Compass.cpp

Class to handle compass data and actions.

Author: Axel Pauli, January 2021

**************************************************************************/

#include <stdio.h>
#include <wchar.h>

#include "esp_log.h"
#include "esp_system.h"

#include "Compass.h"

// Initialise static members
SetupNG<float>* Compass::deviations[8] = { &compass_deviation_45,
                                           &compass_deviation_90,
                                           &compass_deviation_135,
                                           &compass_deviation_180,
                                           &compass_deviation_225,
                                           &compass_deviation_270,
                                           &compass_deviation_335,
                                           &compass_deviation_360 };
/*
  Creates instance for I2C connection with passing the desired parameters.
  No action is done at the bus. Note if i2cBus is not set in the constructor,
  you have to set it by calling method setBus(). The default address of the
  chip is 0x0D.
*/
Compass::Compass(const uint8_t addr,
                 const uint8_t odr,
                 const uint8_t range,
                 const uint8_t osr,
                 I2C_t *i2cBus ) :
 QMC5883L( addr, odr, range, osr, i2cBus )
{
}

Compass::~Compass()
{
}

/** Returns the heading by considering deviation. If ok is passed, it is set
 * to true, if heading data is valid, otherwise it is set to false.*/
float Compass::trueHeading( bool *okIn )
{
  bool ok = false;

  float heading = QMC5883L::heading( &ok );

  if( ok == false )
    {
      if( okIn != nullptr )
        {
          *okIn = false;
        }

      return 0.0;
    }

  // consider deviation
  const float skydirs[8] =
    { 45, 90, 135, 180, 225, 270, 335, 360 };

  for( int i = 0; i < 8; i++ )
    {
      float lowlim = skydirs[i] - 22.5;
      float uplim  = skydirs[i] + 22.5;

      if( uplim > 360 )
        {
          uplim -= 360;
        }

      if( heading >= lowlim && heading <= uplim )
        {
          heading += deviations[i]->get();
          break;
        }
    }

  if( okIn != nullptr )
    {
      *okIn = true;
    }

  return heading;
}

// Compass Menu Action Routine
int Compass::calibrateAction( SetupMenuSelect *p )
{
  ESP_LOGI( FNAME, "Compass calibration ( %d ) ", p->getSelect() );

  if( p->getSelect() == 0 )
    {
      // Cancel is requested
      delay( 500 );
      ESP_LOGI( FNAME, "Cancel Button pressed" );
      return 0;
    }

  if( QMC5883L::haveSensor() == false )
    {
      p->clear();
      p->ucg->setFont( ucg_font_ncenR14_hr );
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
          p->ucg->setFont( ucg_font_ncenR14_hr );
          p->ucg->setPrintPos( 1, 60 );
          p->ucg->printf( "Turn airplain to %d degrees  ", skydirs[i] );
          p->ucg->setPrintPos( 1, 90 );
          p->ucg->printf( "and push button when done" );
          delay( 500 );

          float heading = 0.0;

          while( !p->_rotary->readSwitch() )
            {
              bool ok = false;
              float heading = QMC5883L::heading( &ok );

              if( ok == false )
                {
                  // in case of error deviation is set to 0
                  heading = static_cast<float>(skydirs[i]);
                }

              p->ucg->setFont( ucg_font_fub20_hr );
              p->ucg->setPrintPos( 1, 180 );
              p->ucg->printf( "Heading: %.1f \260  ", heading );
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
      p->ucg->setFont( ucg_font_ncenR14_hr );
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
  p->ucg->setFont( ucg_font_ncenR14_hr );
  p->ucg->setPrintPos( 1, 60 );
  p->ucg->printf( "Saved        " );
  delay( 2000 );

  return 0;
}
