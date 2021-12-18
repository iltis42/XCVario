/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: DisplayDeviations.cpp

Class to display the compass deviations overview.

Author: Axel Pauli, February 2021

Last update: 2021-02-25

 ****************************************************************************/

#include "DisplayDeviations.h"
#include "SetupNG.h"
#include "sensor.h"

#include <AdaptUGC.h>
#include <esp_log.h>


DisplayDeviations::DisplayDeviations( const char * title ) :
  SetupMenuDisplay( title, nullptr )
{
	ESP_LOGI(FNAME, "DisplayDeviations(): title='%s'", title );
}

void DisplayDeviations::display( int mode )
{
  if( (selected != this) || !inSetup )
    return;

  ESP_LOGI(FNAME, "display() mode=%d", mode );

  clear();
  ucg->setFont( ucg_font_ncenR14_hr );
  uprintf( 5, 25, selected->_title );

  const char* skydirdev[8] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };

  // Stored deviation data
  SetupNG<float>* deviations[8] = { &compass_dev_0,
      &compass_dev_45,
      &compass_dev_90,
      &compass_dev_135,
      &compass_dev_180,
      &compass_dev_225,
      &compass_dev_270,
      &compass_dev_315 };

  uint16_t y = 25;
  semaphoreTake();

  for( int i = 0; i < 8; i++ )
    {
      uint16_t x = 0; y += 20;
      ucg->setPrintPos( x, y );
      ucg->setColor( COLOR_HEADER_LIGHT );
      ucg->printf( "%s",  skydirdev[i] );
      x += 42;
      ucg->setPrintPos( x, y );
      ucg->printf( "%03d°", i * 45 );
      x += 50;
      ucg->setColor( COLOR_WHITE );
      ucg->setPrintPos( x, y );
      ucg->printf( "Deviation %3.1f°",  deviations[i]->get() );
    }

  ucg->setPrintPos( 5, 315 );
  ucg->printf( "Press button to exit" );
  semaphoreGive();
}
