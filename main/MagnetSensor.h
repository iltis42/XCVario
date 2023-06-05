/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

I2C driver for the chip MagnetSensor, 3-Axis Magnetic Sensor.

MagnetSensor data sheet:

https://datasheetspdf.com/pdf-file/1309218/QST/MagnetSensor/1

File: MagnetSensor.h

Author: Axel Pauli, January 2021
		Eckhard Völlm, September 2021 (rework for can bus device and gyro fusion)

Last update: 2021-03-28

 ****************************************************************************/

#pragma once

#include <sys/time.h>
#include "esp_system.h"
#include "logdef.h"
#include "esp_log.h"
#include "I2Cbus.hpp"

typedef struct magn_axes {
	int16_t x;
	int16_t y;
	int16_t z;
}t_magn_axes;


// Class Template
class MagnetSensor
{
public:
	// virtual ~MagnetSensor();
	virtual esp_err_t initialize() = 0;
	virtual bool haveSensor() = 0;
	virtual void tick() = 0;
	virtual esp_err_t selfTest() = 0;
	virtual bool overflowFlag() = 0;
	virtual bool rawAxes( t_magn_axes &axes ) = 0;
};
