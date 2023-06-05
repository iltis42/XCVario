/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

I2C driver for the chip QMC5883L, 3-Axis Magnetic Sensor.

File: QMC5883L.h

Author: Axel Pauli, January 2021
		Eckhard Völlm, September 2021 (rework for can bus device and gyro fusion)

Last update: 2021-03-28

 ****************************************************************************/

#pragma once

#include <sys/time.h>
#include "esp_system.h"
#include "logdef.h"
//#include "mpu/types.hpp"
#include "esp_log.h"
#include "I2Cbus.hpp"
#include "average.h"
#include "WString.h"
#include "MagnetSensor.h"


/* The default I2C address of this chip */
#define QMC5883L_ADDR 0x0D

typedef enum e_oversampling { OSR_512=0, OSR_256=1, OSR_128=2, OSR_64=3 } e_oversampling_t;
typedef enum e_datarate { ODR_10Hz=0, ODR_50Hz=1, ODR_100Hz=2, ODR_200Hz=3 } e_datarate_t;
typedef enum e_magn_range { RANGE_2GAUSS=0, RANGE_8GAUSS=1 } e_mag_range_t;

class QMC5883L: public MagnetSensor
{
public:
	/*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. Note if i2cBus is not set in the constructor,
    you have to set it by calling method setBus(). The default address of the
    chip is 0x0D.
	 */
	QMC5883L( const uint8_t addr, const uint8_t odr, const uint8_t range, const uint16_t osr, I2C_t *i2cBus );
	~QMC5883L();

	// Returns true, if the self test has been passed successfully, otherwise
	bool haveSensor() {  return m_sensor; }
	// operation related methods
	void tick() { age++; }
	// Check for reply with I2C bus address
	esp_err_t selfTest();

<<<<<<< HEAD
	/**
	 * Configure the device with the set parameters and set the mode to continuous.
	 * That means, the device starts working.
	 * you max give output datarate (odr) and oversampling rate (osr)
	 * if not given defaults are as from constructor
	 */
	esp_err_t initialize( int a_odr=1, int a_osr=0 );

	/**
	 * Reads the heading in degrees of 0...359. Ok is set to true,
	 * if heading data is valid, otherwise it is set to false.
	 */
	float heading( bool *ok );

	/**
	 * Read temperature in degree Celsius. Ok is set to true,
	 * if temperature data is valid, otherwise it is set to false.
	 */
	short temperature( bool *ok );

	/**
	 * Read out the registers X, Y, Z (0...5) in raw format.
	 * Returns true in case of success otherwise false.
	 */
	bool rawHeading();

	/**
	 * Calibrate compass by using the read x, y, z raw values. The calibration is
	 * stopped by the reporter function which displays intermediate results of the
	 * calibration action.
	 */
	bool calibrate( bool (*reporter)( float x, float y, float z, float xbias, float ybias, float zbias ) );

	/**
	 * Resets the whole compass calibration, also the saved configuration.
	 */
	void resetCalibration();

	/**
	 * Saves a done compass calibration.
	 */
	void saveCalibration();

	/**
	 * Loads a stored compass calibration. Returns true, if valid calibration
	 * data could be loaded, otherwise false.
	 */
	bool loadCalibration();

	/** Write with data part. */
	esp_err_t writeRegister( const uint8_t addr,
			const uint8_t reg,
			const uint8_t value );

	/**
	 * Return the overflow status flag. It is set to true, if any data of three
	 * axis magnetic sensor channels is out of range.
	 */
	static bool overflowFlag()
	{
	  return overflowWarning;
	}

	/**
	 * Return the calibration flag. Set to true, if a calibration is running.
	 */
	static bool calibrationIsRunning()
	{
	  return calibrationRunning;
	}

	/**
	 * Read bytes from the chip.
	 * Return the number of read bytes or 0 in error case.
	 */
	uint8_t readRegister( const uint8_t addr,
			const uint8_t reg,
			const uint8_t count,
			uint8_t *data );
	//    bool getMagnet(raw_axes_t* mag);

	// modif gfm
	static double mag_vector[3] ;
	static double mag_raw[3] ;

	void getMagnet(double mag_raw[],double mag_vector[], bool *ok);
	//fin modif gfm
=======
	esp_err_t initialize();

	//  Write with data part
	bool overflowFlag()	{ return overflowWarning; }
	// Read out the registers X, Y, Z (0...5) in raw format into variables, return true if success
	bool rawAxes( t_magn_axes &axes );
>>>>>>> 6d4fdd1baa8561b1de0632823deccb549c2dcd9b

private:
	// Configure the device with the set parameters and set the mode to continuous.
	esp_err_t initialize2( int a_odr=0, int a_osr=0 );

	/** Check, if the bus pointer is valid. */
	bool checkBus();

	// Read temperature in degree Celsius.
	short temperature( bool *ok );

	// Low level device access methods
	esp_err_t writeRegister( const uint8_t addr, const uint8_t reg, const uint8_t value );
	// Return the overflow status flag. It is set to true, if any data of three * axis magnetic sensor channels is out of range.
	uint8_t readRegister( const uint8_t addr, const uint8_t reg, const uint8_t count, uint8_t *data );

	bool m_sensor;
	I2C_t* m_bus;
	uint8_t addr; // chip adress
	uint8_t odr;  // output data rate
	uint8_t range; // magnetic resolution of sensor
	uint8_t osr; // over sample ratio
	bool overflowWarning;
	Average<20, int16_t> filterX;
	Average<20, int16_t> filterY;
	Average<20, int16_t> filterZ;
	int age;
};
