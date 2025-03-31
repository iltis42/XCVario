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
#include "AdaptUGC.h"

#include "logdef.h"


DisplayDeviations::DisplayDeviations( const char * title ) :
  SetupMenuDisplay( title, nullptr )
{
	ESP_LOGI(FNAME, "DisplayDeviations(): title='%s'", title );
}

void DisplayDeviations::display( int mode )
{
  if( !compass )
  {
	  clear();
	  MYUCG->setFont( ucg_font_ncenR14_hr );
	  MYUCG->setPrintPos( 1, 30 );
	  MYUCG->printf( "No magnetic Sensor, Abort" );
	  return;
  }
  ESP_LOGI(FNAME, "display() mode=%d", mode );
  clear();
  MYUCG->setFont( ucg_font_ncenR14_hr );
  uprintf( 5, 25, _title );
  const char* skydirdev[8] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };
  uint16_t y = 25;
  for( int i = 0; i < 8; i++ )
    {
      uint16_t x = 0; y += 20;
      MYUCG->setPrintPos( x, y );
      MYUCG->setColor( COLOR_HEADER_LIGHT );
      MYUCG->printf( "%s",  skydirdev[i] );
      x += 42;
      MYUCG->setPrintPos( x, y );
      MYUCG->printf( "%03d°", i * 45 );
      x += 50;
      MYUCG->setColor( COLOR_WHITE );
      MYUCG->setPrintPos( x, y );
      MYUCG->printf( "Deviation %3.1f°",  compass->getDeviation(i*45) );
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
  MYUCG->drawHLine( 10, Y, 215 );   // X-Axis
  MYUCG->drawHLine( 15, Y-50, 5 );  // Scale
  MYUCG->drawHLine( 15, Y+50, 5 );
  MYUCG->drawVLine( 20, Y-50, 100 );
  for( int x=20; x<=220; x+=25 ){
	  if( !((x-20)%100) )
		  MYUCG->drawVLine( x, Y-6, 12 );
	  else
		  MYUCG->drawVLine( x, Y-3, 6 );
  }
  MYUCG->setColor( COLOR_GREEN );

  float avg=0;
  for( int x=0; x<360; x++ ){
	  float dev=compass->getDeviation((float)(x));
 	  MYUCG->drawPixel( 20+(x*200./360.), Y-(int)(dev*50/max +0.5) );
 	  avg +=dev;
  }
  avg = avg/360;
  MYUCG->setColor( COLOR_WHITE );
  for( int x=0; x<360; x++ ){
	  if( !(x%10) )
		  MYUCG->drawPixel( 20+(x*200./360.), Y-(int)(avg *50/max +0.5) );
  }
  MYUCG->setFontPosCenter();
  MYUCG->setColor( COLOR_HEADER_LIGHT );
  MYUCG->drawHLine( 15, Y-(int)avg, 5 );  // average
  MYUCG->setPrintPos( 23, Y-(int)avg+7 );
  MYUCG->printf("%d", (int)(avg+0.5));
  MYUCG->setFont(ucg_font_fub11_hr, true);
  MYUCG->setPrintPos(23,Y-50+7);  // Scale Labels
  MYUCG->printf("%2d", (int)rint(max));
  MYUCG->setPrintPos(23,Y+50+7);
  MYUCG->printf("%2d", -(int)rint(max) );
  MYUCG->setPrintPos( 40, 317 );
  MYUCG->setColor( COLOR_WHITE );
  MYUCG->setPrintPos( 40, 317 );
  MYUCG->printf( "Press button to exit" );
}
