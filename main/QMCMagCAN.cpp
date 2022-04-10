/**************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

 If sensor is connected via CAN

 ***************************************************************************/

// Activate/deactivate debug messages
// #define DEBUG_COMP 1

#include <cassert>
#include <cmath>
#include "QMCMagCAN.h"
#include "SetupNG.h"
#include "MenuEntry.h"
#include "Router.h"
#include "vector.h"
#include "canbus.h"

/*
  Creates instance for CAN bus magnet sensor
 */

t_magn_axes QMCMagCAN::can = { 0,0,0 };
int QMCMagCAN::age = 100;

QMCMagCAN::QMCMagCAN()
{
	ESP_LOGI( FNAME, "QMCMagCAN()");
	age = 100;
	can = { 0,0,0 };
	m_sensor = false;
}

QMCMagCAN::~QMCMagCAN()
{
}

// check if messages arrive
esp_err_t QMCMagCAN::selfTest()
{
	ESP_LOGI( FNAME, "QMCMagCAN selftest");
	for( int i=0; i<300; i++ ){ // give 30 second's chance for module to send data with the corresponding CAN data rate
		if( age < 5 ){          // CAN bus sensor probes next speed after 13 seconds and starts with new speed, so max 26 seconds all 3 speeds are probed
			return ESP_OK;
		}
		delay(100);
	}
	return ESP_FAIL;
}

void QMCMagCAN::fromCAN( const char * msg ){

	can.x = (msg[0] & 0xff) | ((msg[1] << 8) & 0xff00 );
	can.y = (msg[2] & 0xff) | ((msg[3] << 8) & 0xff00 );
	can.z = (msg[4] & 0xff) | ((msg[5] << 8) & 0xff00 );
	// ESP_LOGI(FNAME,"from CAN bus magn X=%d Y=%d Z=%d", can.x, can.y, can.z );
	age = 0;
}

bool QMCMagCAN::rawAxes( t_magn_axes &axes )
{
	if( age < 10 ){
		axes.x = filterX( can.x );
		axes.y = filterY( can.y );
		axes.z = filterZ( can.z );
		ESP_LOGI( FNAME, "Mag Average: X:%d Y:%d Z:%d  Raw: X:%d Y:%d Z:%d", axes.x, axes.y, axes.z, can.x, can.y, can.z );
		// ESP_LOGI( FNAME, "X:%d Y:%d Z:%d  Age:%d", can.x, can.y, can.z, age );
		m_sensor = true;
		return true;
	}
	else{
		// ESP_LOGE( FNAME, "Magnet sensor data from CAN missing");
		// CANbus::restart();
		m_sensor = false;
		return false;
	}
}
