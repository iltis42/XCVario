/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

File: ShowCompassSettings.cpp

Class to display the compass status overview.

Author: Axel Pauli, March 2021

Last update: 2021-04-18

 ****************************************************************************/

#include "ShowCompassSettings.h"

#include "SetupNG.h"
#include "QMC5883L.h"
#include "sensor.h"

#include <AdaptUGC.h>
#include <logdef.h>

ShowCompassSettings::ShowCompassSettings( const char* title) :
SetupMenuDisplay( title, nullptr )
{
	ESP_LOGI(FNAME, "ShowCompassSettings(): title='%s'", title );
}

void ShowCompassSettings::display( int mode )
{
	if( (selected != this) || !gflags.inSetup )
		return;
	if( !compass )
	{
		clear();
		MYUCG->setFont( ucg_font_ncenR14_hr );
		MYUCG->setPrintPos( 1, 30 );
		MYUCG->printf( "No magnetic Sensor, Abort" );
		delay( 2000 );
		clear();
		return;
	}
	ESP_LOGI(FNAME, "display() mode=%d", mode );

	clear();
	MYUCG->setFont( ucg_font_ncenR14_hr );
	uprintf( 5, 25, _title );

	uint16_t y = 75;
	uint16_t y1 = 75;
	char buffer[32];

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "Sensor enabled: %s",
			(compass_enable.get() == 0) ? "No" : "Yes"  );
	MYUCG->printf( "%s", buffer );
	y += 25;

	MYUCG->setPrintPos( 0, y );
	t_bitfield_compass state = calibration_bits.get();
	t_bitfield_compass target = { 1,1,1,1,1,1 };
	bool all_green = false;
	if( state == target )
		all_green = true;
	sprintf( buffer, "Sensor calibrated: %s",
			(compass_calibrated.get() == 0 || !all_green) ? "No" : "Yes"  );
	MYUCG->printf( "%s", buffer );
	y += 25;

	y1 = y;
	const char* soText = "Sensor overflow: ";
	int sotw = MYUCG->getStrWidth( soText );
	MYUCG->setPrintPos( 0, y );
	MYUCG->printf( "%s", soText );
	MYUCG->setPrintPos( sotw, y );
	MYUCG->printf( "%s", (compass->overflowFlag() == false) ? "No" : "Yes" );
	y += 25;

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "Compass declination: %d°",
			static_cast<int>(compass_declination.get()) );
	MYUCG->printf( "%s", buffer );
	y += 25;

  MYUCG->setPrintPos( 0, y );
  sprintf( buffer, "Display damping: %.02fs", (compass_damping.get()) );
  MYUCG->printf( "%s", buffer );
  y += 25;

  MYUCG->setPrintPos( 0, y );
  sprintf( buffer, "I2C Clock: %d KHz", static_cast<int>(compass_i2c_cl.get()) );
  MYUCG->printf( "%s", buffer );
  y += 25;

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "NMEA mag heading: %s",
			(compass_nmea_hdm.get() == 0) ? "No" : "Yes"  );
	MYUCG->printf( "%s", buffer );
	y += 25;

	MYUCG->setPrintPos( 0, y );
	sprintf( buffer, "NMEA true heading: %s",
			(compass_nmea_hdt.get() == 0) ? "No" : "Yes"  );
	MYUCG->printf( "%s", buffer );
	y += 25;

	MYUCG->setPrintPos( 5, 290 );
	MYUCG->printf( "Press button to exit" );

	uint32_t counter = 0;

	while( Rotary->readSwitch() == false )
	{
		counter++;

		if( counter % 100 != 0 )
		{
			delay( 10 );
			continue;
		}

		// Ca. after a second make an update of the overflow flag display.
		MYUCG->setPrintPos( sotw, y1 );
		MYUCG->printf( "%s", (compass->overflowFlag() == false) ? "No  " : "Yes" );
		continue;
	}

	press();
}
