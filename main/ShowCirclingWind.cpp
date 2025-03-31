/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: ShowCirclingWind.cpp

Class to display the compass status overview.

Author: Eckhard Völlm, May 2021

Last update: 2021-04-18

 ****************************************************************************/

#include "ShowCirclingWind.h"

#include "CircleWind.h"
#include "SetupNG.h"
#include "Units.h"
#include "sensor.h"
#include "logdef.h"

#include "AdaptUGC.h"

ShowCirclingWind::ShowCirclingWind( const char* title ) :
SetupMenuDisplay( title, nullptr )
{
	ESP_LOGI(FNAME, "ShowCirclingWind(): title='%s'", title );
}



void ShowCirclingWind::display( int mode )
{
	ESP_LOGI(FNAME, "display() mode=%d", mode );
	if( mode != 5 )
		clear();
	MYUCG->setFont( ucg_font_ncenR14_hr  );
	uprintf( 5, 25, _title );

	uint16_t y = 55;
	char buffer[32];

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "Circling Wind enabled: %s", (wind_enable.get() & 2) ? "Yes" : "No  "  );
	MYUCG->printf( "%s", buffer );
	y += 25;

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "GPS Status : %s", (CircleWind::getGpsStatus() == true ) ? "Good" : "Bad  "  );
	MYUCG->printf( "%s", buffer );
	y += 25;

	if( CircleWind::getGpsStatus() ){
		MYUCG->setPrintPos( 0, y );
		sprintf( buffer, "GPS Satellites : %d  ", CircleWind::getSatCnt() );
		MYUCG->printf( "%s", buffer );
		y += 25;
	}

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "Number of Circles : %2.2f  ", CircleWind::getNumCircles() );
	MYUCG->printf( "%s", buffer );
	y += 25;

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "Last Wind : %3.1f°/%2.1f  ", CircleWind::getAngle(), Units::Airspeed( CircleWind::getSpeed()) );
	MYUCG->printf( "%s", buffer );
	y += 25;

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "Wind Age : %d sec   ", CircleWind::getAge() );
	MYUCG->printf( "%s", buffer );
	y += 25;

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "Quality : %d %%  ", CircleWind::getQuality() );
	MYUCG->printf( "%s", buffer );
	y += 25;

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "Status : %s    ", CircleWind::getStatus() );
	MYUCG->printf( "%s", buffer );
	y += 25;

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "Flight Mode : %s    ", CircleWind::getFlightModeStr() );
	MYUCG->printf( "%s", buffer );
	y += 25;


	MYUCG->setPrintPos( 5, 290 );
	MYUCG->printf( "Press button to exit" );

}
