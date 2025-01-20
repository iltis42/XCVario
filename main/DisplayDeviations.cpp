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
  if( (selected != this) || !gflags.inSetup )
	  return;
  if( !compass )
  {
	  clear();
	  ucg->setFont( ucg_font_ncenR14_hr );
	  ucg->setPrintPos( 1, 30 );
	  ucg->printf( "No magnetic Sensor, Abort" );
	  return;
  }
  ESP_LOGI(FNAME, "display() mode=%d", mode );
  clear();
  ucg->setFont( ucg_font_ncenR14_hr );
  uprintf( 5, 25, selected->_title );
  const char* skydirdev[8] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };
  uint16_t y = 25;
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
      ucg->printf( "Deviation %3.1f°",  compass->getDeviation(i*45) );
    }

  // draw graph
  const int Y=245;
  float max=0;
   for( int x=0; x<360; x++ ){
  	  float dev=compass->getDeviation((float)(x));
  	  if( abs(dev) > max ){
  		  max = abs(dev);
  	  }
  }
  max = max*1.1;
  ucg->drawHLine( 10, Y, 215 );   // X-Axis
  ucg->drawHLine( 15, Y-50, 5 );  // Scale
  ucg->drawHLine( 15, Y+50, 5 );
  ucg->drawVLine( 20, Y-50, 100 );
  for( int x=20; x<=220; x+=25 ){
	  if( !((x-20)%100) )
		  ucg->drawVLine( x, Y-6, 12 );
	  else
		  ucg->drawVLine( x, Y-3, 6 );
  }
  ucg->setColor( COLOR_GREEN );

  float avg=0;
  for( int x=0; x<360; x++ ){
	  float dev=compass->getDeviation((float)(x));
 	  ucg->drawPixel( 20+(x*200./360.), Y-(int)(dev*50/max +0.5) );
 	  avg +=dev;
  }
  avg = avg/360;
  ucg->setColor( COLOR_WHITE );
  for( int x=0; x<360; x++ ){
	  if( !(x%10) )
		  ucg->drawPixel( 20+(x*200./360.), Y-(int)(avg *50/max +0.5) );
  }
  ucg->setFontPosCenter();
  ucg->setColor( COLOR_HEADER_LIGHT );
  ucg->drawHLine( 15, Y-(int)avg, 5 );  // average
  ucg->setPrintPos( 23, Y-(int)avg+7 );
  ucg->printf("%d", (int)(avg+0.5));
  ucg->setFont(ucg_font_fub11_hr, true);
  ucg->setPrintPos(23,Y-50+7);  // Scale Labels
  ucg->printf("%2d", (int)rint(max));
  ucg->setPrintPos(23,Y+50+7);
  ucg->printf("%2d", -(int)rint(max) );
  ucg->setPrintPos( 40, 317 );
  ucg->setColor( COLOR_WHITE );
  ucg->setPrintPos( 40, 317 );
  ucg->printf( "Press button to exit" );
}
