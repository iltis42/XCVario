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

#include "wind/CircleWind.h"
#include "setup/SetupNG.h"
#include "Units.h"
#include "sensor.h"
#include "logdef.h"

#include "AdaptUGC.h"

ShowCirclingWind::ShowCirclingWind( const char* title ) :
SetupMenuDisplay( title, nullptr )
{
	ESP_LOGI(FNAME, "ShowCirclingWind(): title='%s'", title );
}



void ShowCirclingWind::display(int mode)
{
	ESP_LOGI(FNAME, "display() mode=%d", mode );
	if( mode != 5 ) {
		clear();
	}
	MYUCG->setFont( ucg_font_ncenR14_hr  );
	menuPrintLn(_title.c_str(), 0);

	char buffer[32];
	int i = 1;

	sprintf( buffer, "Circling Wind enabled: %s", (wind_enable.get() & 2) ? "Yes" : "No  "  );
	menuPrintLn(buffer, i++);

	if ( circleWind )
	{
		sprintf( buffer, "GPS Status : %s", (circleWind->getGpsStatus() == true ) ? "Good" : "Bad  "  );
		menuPrintLn(buffer, i++);

		if( circleWind->getGpsStatus() ){
			sprintf( buffer, "GPS Satellites : %d  ", circleWind->getSatCnt() );
			menuPrintLn(buffer, i++);
		}

		sprintf( buffer, "Number of Circles : %2.2f  ", circleWind->getNumCircles() );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "Last Wind : %3.1f°/%2.1f  ", circleWind->getAngle(), Units::Airspeed( circleWind->getSpeed()) );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "Wind Age : %d sec   ", circleWind->getAge() );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "Quality : %d %%  ", circleWind->getQuality() );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "Status : %s    ", circleWind->getStatus() );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "Flight Mode : %s    ", circleWind->getFlightModeStr() );
		menuPrintLn(buffer, i++);
	}
	menuPrintLn("Press button to exit", i+1, 5);

}
