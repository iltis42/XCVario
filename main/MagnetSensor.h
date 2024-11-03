/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

Magnetic Sensor Abstraction.

 ****************************************************************************/

#pragma once

#include "vector_3d.h"
#include "esp_system.h"


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
	bool isCalibrated() { return calibrated; }
	virtual void tick() = 0;
	virtual esp_err_t selfTest() = 0;
	virtual bool overflowFlag() = 0;
	virtual bool readRaw( t_magn_axes &mag ) = 0;
	virtual bool readBiased( vector_ijk &mag ) { return false; }
	virtual int curX() = 0;
	virtual int curY() = 0;
	virtual int curZ() = 0;
protected:
	bool calibrated = false;
};
