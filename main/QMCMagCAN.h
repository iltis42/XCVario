/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

I2C driver for the chip QMCMagCAN, 3-Axis Magnetic Sensor.

QMCMagCAN data sheet:

https://datasheetspdf.com/pdf-file/1309218/QST/QMCMagCAN/1

File: QMCMagCAN.h

Author: Axel Pauli, January 2021
		Eckhard Völlm, September 2021 (rework for can bus device and gyro fusion)

Last update: 2021-03-28

 ****************************************************************************/

#pragma once

#include "MagnetSensor.h"

#include "vector_3d.h"
#include "average.h"


class QMCMagCAN: public MagnetSensor
{
public:
	/*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. Note if i2cBus is not set in the constructor,
    you have to set it by calling method setBus(). The default address of the
    chip is 0x0D.
	 */
	QMCMagCAN();
	~QMCMagCAN();

	// Returns true, if the self test has been passed successfully, otherwise
	bool haveSensor() {  return m_sensor; }
	// operation related methods
	void tick() { age++; }

	esp_err_t initialize() { return ESP_OK; };

	// Check for reply with I2C bus address
	esp_err_t selfTest();

	bool overflowFlag()	{ return false; }  // no support

	// Read in raw format into variables, return true if success
	bool readRaw( t_magn_axes &mag );
	// In micro Tesla unit, bias and scale corrected, return true if success
	bool readBiased( vector_ijk &mag );

	// If device is connected via CAN, just get X,Y,Z data from there
	static void fromCAN( const char * msg, int len );

	int curX() { return (int)can.x; }
	int curY() { return (int)can.y; }
	int curZ() { return (int)can.z; }

private:
	bool m_sensor;
	Average<5, float> filterX;
	Average<5, float> filterY;
	Average<5, float> filterZ;
	static t_magn_axes can;
	static vector_ijk calib, stage;
	static int age;
};
