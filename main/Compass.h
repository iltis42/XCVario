/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

File: Compass.h

Class to handle compass data access.

Author: Axel Pauli, January 2021

Last update: 2021-03-07

 **************************************************************************/

#pragma once

// #include "SetupNG.h"
#include "QMC5883L.h"
#include "spline.h"
#include <map>


typedef struct float_axes {
	float x;
	float y;
	float z;
}t_float_axes;

typedef enum e_oversampling { OSR_512=0, OSR_256=1, OSR_128=2, OSR_64=3 } e_oversampling_t;
typedef enum e_datarate { ODR_10Hz=0, ODR_50Hz=1, ODR_100Hz=2, ODR_200Hz=3 } e_datarate_t;
typedef enum e_magn_range { RANGE_2GAUSS=0, RANGE_8GAUSS=1 } e_mag_range_t;

typedef struct bitfield_compass{
	bool xmax_green :1;
	bool xmin_green :1;
	bool ymax_green :1;
	bool ymin_green :1;
	bool zmax_green :1;
	bool zmin_green :1;
	// type_t& operator[](int i) { return xyz[i]; }
	struct bitfield_compass operator = ( const struct bitfield_compass &other ) {
		xmax_green = other.xmax_green; xmin_green = other.xmin_green;
		ymax_green = other.ymax_green; ymin_green = other.ymin_green;
		zmax_green = other.zmax_green; zmin_green = other.zmin_green;
	    return *this;
	};
	bool operator == ( const struct bitfield_compass &other ) const {
		return( xmax_green == other.xmax_green && xmin_green == other.xmin_green &&
			    ymax_green == other.ymax_green && ymin_green == other.ymin_green &&
			    zmax_green == other.zmax_green && zmin_green == other.zmin_green  );
	};
	// bool isZero() { return bool( !(x|y|z) ); }
	operator String  () {
	   	return( String(xmax_green) + " " + String(xmin_green) +
	   			String(ymax_green) + " " + String(ymin_green) +
				String(zmax_green) + " " + String(zmin_green) );
	 };
}t_bitfield_compass;

class Compass
{
public:
	/*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. The default address of the chip is 0x0D.
	 */
	Compass( const uint8_t addr, const uint8_t odr=0,	const uint8_t range=0, const uint16_t osr=0, I2C_t *i2cBus=0 );
	~Compass();

	esp_err_t selfTest();

	// system related methods
	void begin();
	void start();
	void tick();

	// sensor related interface
	bool haveSensor();
	bool overflowFlag();

	// Heading related methods
	float cur_heading( bool *ok );
	float rawHeading( bool *okIn );      //  Returns the low pass filtered magnetic heading without deviation
	float filteredHeading( bool *okIn );
	float filteredTrueHeading( bool *okIn );
	void setGyroHeading( float hd );
	float getGyroHeading( bool *ok, bool addDeclination=true );
	inline bool headingValid() {	return m_headingValid;	}
	void setHeading( float h );

	// Deviatiation related methods
	float getDeviation( float heading );
	void saveDeviation();
	bool newDeviation( float for_heading, float steer, float airspeedCalibration );
	// Return the calibration flag. Set to true, if a calibration is running.
	void loadDeviationMap();
	void deviationReload();

	// Calibration releated methods
	// Calibrate compass by using the read x, y, z raw values.
	bool calibrate( bool (*reporter)( t_magn_axes raw, t_float_axes scale, t_float_axes bias, bitfield_compass b, bool print ), bool only_show );
	 // Resets the whole compass calibration, also the saved configuration.
	void resetCalibration();
	bool calibrationIsRunning() {  return calibrationRunning; }
	// Returns total number of read errors
	int getReadError(){ return totalReadErrors; };


private:
	// Calculates tilt compensated heading in degrees of 0...359. The ok flag is set to true if fine, else false
	float heading( bool *ok );

	// internal task management
	void progress();
	static void compassT(void* arg );  // task for compass reading

	// Saves a done compass calibration.
	void saveCalibration();
	// Loads a stored compass calibration. Returns true if all okay
	bool loadCalibration();

	 // Setup the deviation interpolation data.
	void readInterpolationData();
	 // Rebuild spline function
	void recalcInterpolationSpline();

	// fully gyro fused heading
	float m_gyro_fused_heading;
	/** Pure averaged magnetic heading */
	float m_magn_heading;
	/** Control flag of filtered heading. */
	bool m_headingValid;
	std::vector<double> X;
	std::vector<double> Y;
	static xSemaphoreHandle splineMutex;
	tk::spline *deviationSpline;
	std::map< double, double> devmap;
	int _tick;
	int _devHolddown;
	int _external_data;
	float _heading_average;
	int gyro_age;

	/** Variables used by calibration. */
	t_float_axes bias;
	t_float_axes scale;
	bool calibrationRunning;
	float _heading;
	bool holddown;
	// Error counters
	int errors;
	int totalReadErrors;
	Average<20, float, float> filterRoll;
	Average<20, float, float> filterPitch;
	int age;
	MagnetSensor *sensor;
	t_magn_axes can;
};
