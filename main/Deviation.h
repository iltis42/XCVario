/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

File: Deviation.h

Class to handle compass deviation data

Author: Eckhard Völlm, December 2021


 **************************************************************************/

#pragma once

#include "QMC5883L.h"
#include "spline.h"
#include <map>
#include "Compass.h"

class Deviation
{
public:
	/*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. The default address of the chip is 0x0D.
	 */
	Deviation();
	~Deviation();

	// system related methods
	void begin();
	// Deviatiation related methods
	float getDeviation( float heading );
	void saveDeviation();
	bool newDeviation( float for_heading, float steer, float airspeedCalibration );
	// Return the calibration flag. Set to true, if a calibration is running.
	void loadDeviationMap();
	void deviationReload();
	void tick() { _devHolddown--; };

private:
	 // Setup the deviation interpolation data.
	void readInterpolationData();
	 // Rebuild spline function
	void recalcInterpolationSpline();

	std::vector<double> X;
	std::vector<double> Y;

	static xSemaphoreHandle splineMutex;
	tk::spline *deviationSpline;
	std::map< double, double> devmap;
	int _devHolddown;
	int samples;
};
