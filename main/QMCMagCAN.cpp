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

#include "QMCMagCAN.h"

#include "sensor.h"
#include "SetupNG.h"
#include "MenuEntry.h"
#include "Router.h"
#include "vector.h"
#include "comm/CanBus.h"

#include <cassert>
#include <cmath>

/*
  Creates instance for CAN bus magnet sensor
 */

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
	// Fixme outdated
	// give 30 second's chance for module to send data with the corresponding CAN data rate
	// CAN bus sensor probes next speed after 13 seconds and starts with new speed, so max 26 seconds all 3 speeds are probed
	// Currently a fix 1MBit rate is considert

	// Wait max. 1 sec for arriving magsens stream data
	ESP_LOGI( FNAME, "QMCMagCAN selftest");
	for( int i=0; i<100; i++ ) {
		if( age < 5 ) {
			return ESP_OK;
		}
		delay(10);
	}
	return ESP_FAIL;
}

void QMCMagCAN::fromCAN( const t_magn_axes *magaxes )
{
	can = *magaxes;
	// Reset age
	age = 0;
}

void QMCMagCAN::fromCAN( const char * msg, int len )
{
	// Read raw int mag values at once
	if ( len == 6 ) {
		memcpy( ((char *)&can.x), msg, 6);
		// ESP_LOGI(FNAME,"from CAN bus magn X=%d Y=%d Z=%d", can.x, can.y, can.z );
	}
	// Stage biased float mag values from two successive messages
	else if ( len == 8 ) {
		memcpy( ((char *)&stage.a), msg, 8);
	}
	else if ( len == 4 ) {
		memcpy( ((char *)&stage.c), msg, 4);
		if ( stage.a != 0.f ) {
			calib = stage;
			stage.a = 0.f;
		}
	}
	
	// Reset age
	age = 0;
}

bool QMCMagCAN::readRaw( t_magn_axes &mag )
{
	if ( age < 5 ) {
		mag.x = can.x; // filterX( can.x );
		mag.y = can.y; // filterY( can.y );
		mag.z = can.z; // filterZ( can.z );
		// if ( age == 0 ) {
		// 	ESP_LOGI( FNAME, "Mag Average: X:%d Y:%d Z:%d  Raw: X:%d Y:%d Z:%d", mag.x, mag.y, mag.z, can.x, can.y, can.z );
		ESP_LOGI( FNAME, "X:%d Y:%d Z:%d  Age:%d", can.x, can.y, can.z, age );
		// }
		m_sensor = true;
	}
	else {
		// ESP_LOGE( FNAME, "Magnet sensor data from CAN missing");
		// CANbus::restart();
		m_sensor = false;
	}
	return m_sensor;
}

bool QMCMagCAN::readBiased( vector_ijk &axes )
{
	if ( age < 5 ) {
		axes = calib;
		// if ( age == 0 ) {
		// 	ESP_LOGI( FNAME, "Mag calibrated: X:%.5f Y:%.5f Z:%.5f", axes.a, axes.b, axes.c);
		// }
		m_sensor = true;
	}
	else {
		m_sensor = false;
	}
	return m_sensor;
}
