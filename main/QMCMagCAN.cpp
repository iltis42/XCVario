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

#include "vector.h"
#include "logdefnone.h"

#include <freertos/FreeRTOS.h>

#include <cmath>

/*
  Creates instance for CAN bus magnet sensor
 */

QMCMagCAN::QMCMagCAN()
{
	ESP_LOGI( FNAME, "QMCMagCAN()");
}

// check if messages arrive
esp_err_t QMCMagCAN::selfTest()
{
	// Wait max. 1 sec for arriving magsens stream data
	ESP_LOGI( FNAME, "QMCMagCAN selftest");
	for( int i=0; i<100; i++ ) {
		if( age < 5 ) {
			initialized = true;
			return ESP_OK;
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	return ESP_FAIL;
}

void QMCMagCAN::fromExternal( const t_magn_axes *magaxes )
{
	// fixme, use a queue
	can = *magaxes;
	// Reset age
	age = 0;
}

bool QMCMagCAN::readRaw( t_magn_axes &mag )
{
	if ( age < 5 ) {
		mag.x = can.x;
		mag.y = can.y;
		mag.z = can.z;
		// if ( age == 0 ) {
		// ESP_LOGI( FNAME, "Mag Average: X:%d Y:%d Z:%d  Raw: X:%d Y:%d Z:%d", mag.x, mag.y, mag.z, can.x, can.y, can.z );
		// }
		return true;
	}
	else {
		ESP_LOGE( FNAME, "Magnet sensor data from CAN missing");
		return false;
	}
}

// bool QMCMagCAN::readBiased( vector_ijk &axes )
// {
// 	if ( age < 5 ) {
// 		axes = calib;
// 		// if ( age == 0 ) {
// 		// 	ESP_LOGI( FNAME, "Mag calibrated: X:%.5f Y:%.5f Z:%.5f", axes.a, axes.b, axes.c);
// 		// }
// 		return true;
// 	}
// 	else {
// 		return false;
// 	}
// }
