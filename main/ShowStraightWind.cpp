/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: ShowStraightWind.cpp

Class to display the compass status overview.

Author: Eckhard Völlm, May 2021

Last update: 2021-04-18

 ****************************************************************************/

#include "logdef.h"
#include "esp_log.h"
#include "Wind.h"
#include "ShowStraightWind.h"
#include "SetupNG.h"
#include "Units.h"
#include "sensor.h"
#include "Compass.h"

ShowStraightWind::ShowStraightWind( String title ) :
SetupMenuDisplay( title, nullptr )
{
	ESP_LOGI(FNAME, "ShowStraightWind(): title='%s'", title.c_str() );
}



void ShowStraightWind::display( int mode )
{
	if( (selected != this) || !_menu_enabled )
		return;

	ESP_LOGI(FNAME, "display() mode=%d", mode );
	if( mode != 5 )
		clear();
	ucg->setFont( ucg_font_fur14_hf );
	uprintf( 5, 25, selected->_title.c_str() );

	uint16_t y = 70;
	char buffer[32];

	semaphoreTake();

	ucg->setPrintPos( 0, y );
	sprintf( buffer, "Straight Wind enabled: %s", (wind_enable.get() & 1) ? "Yes" : "No  "  );
	ucg->printf( "%s", buffer );
	y += 25;

	ucg->setPrintPos( 0, y );
	sprintf( buffer, "Status: %s     ", theWind.getStatus() );
	ucg->printf( "%s", buffer );
	y += 25;

	ucg->setPrintPos( 0, y );
	sprintf( buffer, "GPS Status : %s", (theWind.getGpsStatus() == true ) ? "Good" : "Bad  "  );
	ucg->printf( "%s", buffer );
	y += 25;

	ucg->setPrintPos( 0, y );
	sprintf( buffer, "AS cor : %+3.3f %%    ", (theWind.getAsCorrection()-1.0)*100 );
	ucg->printf( "%s", buffer );
	y += 25;

	ucg->setPrintPos( 0, y );
	sprintf( buffer, "Last Wind : %3.1f\xb0/%2.1f   ", theWind.getAngle(), Units::Airspeed( theWind.getSpeed()) );
	ucg->printf( "%s", buffer );
	y += 25;

	ucg->setPrintPos( 0, y );
	sprintf( buffer, "AS/GS Jitter : %3.2f/%3.2f   ", theWind.getAsJitter(),  theWind.getGsJitter() );
	ucg->printf( "%s", buffer );
	y += 25;

	ucg->setPrintPos( 0, y );
	sprintf( buffer, "MH/Dev: %3.2f/%+3.2f   ", theWind.getMH(), theWind.getDeviation() );
	ucg->printf( "%s", buffer );
	y += 25;

	float minDev = 400.0;
	float maxDev = 0.0;
	for( int i=0; i<360; i++ ){
		float d = Compass::getDeviation((float)i);
		if( minDev > d )
			minDev = d;
		if( maxDev < d )
			maxDev = d;
	}

	ucg->setPrintPos( 0, y );
	sprintf( buffer, "Min/Max Dev: %+3.2f/%+3.2f   ", minDev, maxDev );
	ucg->printf( "%s", buffer );
	y += 25;

	ucg->setPrintPos( 0, y );
	sprintf( buffer, "Wind Age : %d sec   ", theWind.getAge() );
	ucg->printf( "%s", buffer );
	y += 25;


	ucg->setPrintPos( 5, 310 );
	ucg->printf( "Press button to exit" );

	semaphoreGive();

}
