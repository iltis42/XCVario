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

class QMCMagCAN: public MagnetSensor, public CompassSink_I
{
public:
	/*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. Note if i2cBus is not set in the constructor,
    you have to set it by calling method setBus(). The default address of the
    chip is 0x0D.
	 */
	QMCMagCAN();
	~QMCMagCAN() = default;

	// Returns true, if the self test has been passed successfully, otherwise
	bool haveSensor() {  return initialized; }
	// operation related methods
	void age_incr() { age++; }

	esp_err_t initialize() override { return ESP_OK; };

	// Check for CAN magsens stream data
	esp_err_t selfTest() override;

    bool overflowFlag() override { return false; }  // no support

	// Read in raw format into variables, return true if success
	bool readRaw( t_magn_axes &mag ) override;
	// In micro Tesla unit, bias and scale corrected, return true if success
	// bool readBiased( vector_ijk &mag );

	// If device is connected via CAN, just get X,Y,Z data from there
    void fromExternal(const t_magn_axes *magaxes) override;

	int curX() override { return (int)can.x; }
	int curY() override { return (int)can.y; }
	int curZ() override { return (int)can.z; }

private:
	bool initialized = false;
	t_magn_axes can = {};
	// vector_ijk calib, stage;
	int age = 100;
};
