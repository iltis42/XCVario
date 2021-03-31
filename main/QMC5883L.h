/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ****************************************************************************

I2C driver for the chip QMC5883L, 3-Axis Magnetic Sensor.

QMC5883L data sheet:

https://datasheetspdf.com/pdf-file/1309218/QST/QMC5883L/1

File: QMC5883L.h

Author: Axel Pauli, January 2021

Last update: 2021-03-28

 ****************************************************************************/

#pragma once

#include <sys/time.h>
#include "esp_system.h"
#include "logdef.h"
#include "esp_log.h"
#include "I2Cbus.hpp"

/* The default I2C address of this chip */
#define QMC5883L_ADDR 0x0D

typedef enum e_oversampling { OSR_512=0, OSR_256=1, OSR_128=2, OSR_64=3 } e_oversampling_t;
typedef enum e_datarate { ODR_10Hz=0, ODR_50Hz=1, ODR_100Hz=2, ODR_200Hz=3 } e_datarate_t;
typedef enum e_magn_range { RANGE_2GAUSS=0, RANGE_8GAUSS=1 } e_mag_range_t;


class QMC5883L
{
public:
	/*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. Note if i2cBus is not set in the constructor,
    you have to set it by calling method setBus(). The default address of the
    chip is 0x0D.
	 */
	QMC5883L( const uint8_t addr,
			      const uint8_t odr,
			      const uint8_t range,
			      const uint16_t osr,
			      I2C_t *i2cBus=nullptr );

	~QMC5883L();

	/**
	 * Returns true, if the self test has been passed successfully, otherwise
	 * false.
	 */
	static bool haveSensor()
	{
	  return m_sensor;
	}

	/** Set the I2C bus used for the connection. */
	void setBus( I2C_t *theBus ) { m_bus = theBus; }

	/** Check for reply with I2C bus address */
	esp_err_t selfTest();

	/**
	 * Configure the device with the set parameters and set the mode to continuous.
	 * That means, the device starts working.
	 * you max give output datarate (odr) and oversampling rate (osr)
	 * if not given defaults are as from constructor
	 */
	esp_err_t initialize( int a_odr=0, int a_osr=0 );

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

private:

	/**
	 * Get time in ms since 1.1.1970
	 */
	uint64_t getMsTime()
	{
	  struct timeval tv;
	  gettimeofday( &tv, nullptr );
	  return ( tv.tv_sec * 1000 ) + ( tv.tv_usec / 1000 );
	}

  /**
   * Resets the class calibration variables.
   */
  void resetClassCalibration();

	/** Check, if the bus pointer is valid. */
	inline bool checkBus()
	{
		if( m_bus == nullptr )	{
			// ESP_LOGE( FNAME, "QMC5883L bus pointer is zero" );
			return false;
		}
		return true;
	}

	static bool m_sensor;
	I2C_t* m_bus;

	/** Variables used by calibration. */
	float xbias, ybias, zbias;
	float xscale, yscale, zscale;
	int16_t xmax, xmin;
	int16_t ymax, ymin;
	int16_t zmax, zmin;

	/** Read raw values from the chip */
	int xraw, yraw, zraw;

	uint8_t addr; // chip adress
	uint8_t odr;  // output data rate
	uint8_t range; // magnetic resolution of sensor
	uint8_t osr; // over sample ratio
	static bool overflowWarning;
	static int errors;
	static bool calibrationRunning;
	static float _heading;
};
