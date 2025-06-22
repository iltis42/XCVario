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

#include "wind/StraightWind.h"
#include "ShowStraightWind.h"
#include "setup/SetupNG.h"
#include "Units.h"
#include "sensor.h"
#include "Compass.h"

#include "logdef.h"


StraightWind *straightWind = nullptr;

ShowStraightWind::ShowStraightWind( const char* title ) :
SetupMenuDisplay( title, nullptr )
{
	ESP_LOGI(FNAME, "ShowStraightWind(): title='%s'", title );
}



void ShowStraightWind::display(int mode)
{
	ESP_LOGI(FNAME, "display() mode=%d", mode );
	if( mode != 5 ) {
		clear();
	}
	MYUCG->setFont( ucg_font_ncenR14_hr );
	menuPrintLn(_title.c_str(), 0, 5);

	char buffer[32];
	int i = 1;

	sprintf( buffer, "Straight Wind enabled: %s", (wind_enable.get() & 1) ? "Yes" : "No  "  );
	menuPrintLn(buffer, i++);

	if ( straightWind )
	{
		sprintf( buffer, "Status: %s     ", straightWind->getStatus() );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "GPS Status : %s", (straightWind->getGpsStatus() == true ) ? "Good" : "Bad  "  );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "AS C/F: %+3.3f %%/%3.3f %%  ", (straightWind->getAsCorrection()-1.0)*100, (wind_as_calibration.get()-1.0)*100 );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "Last Wind : %3.1f°/%2.1f   ", straightWind->getAngle(), Units::Airspeed( straightWind->getSpeed()) );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "MH/Dev: %3.2f/%+3.2f   ", straightWind->getMH(), straightWind->getDeviation() );
		menuPrintLn(buffer, i++);

		sprintf( buffer, "Wind Age : %d sec   ", straightWind->getAge() );
		menuPrintLn(buffer, i++);
	}


	menuPrintLn("Press button to exit", i+1, 5);
}
