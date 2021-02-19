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

Author: Axel Pauli, January 2021

Last update: 2021-02-19

 ****************************************************************************/

#ifndef QMC5883L_H
#define QMC5883L_H

#include "esp_system.h"
#include "I2Cbus.hpp"

/* The default I2C address of this chip */
#define QMC5883L_ADDR 0x0D

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
	 * Define SET/RESET period. Should be set to 1 after a reset.
	 */
	esp_err_t setPeriodRegister();

	/**
	 *  Set the device in standby mode.
	 */
	esp_err_t modeStandby();

	/**
	 * Configure the device with the set parameters and set the mode to continuous.
	 * That means, the device starts working.
	 */
	esp_err_t modeContinuous();

	/**
	 * Reads the heading in degrees of 1...360. If ok is passed, it is set to true,
	 * if heading data is valid, otherwise it is set to false.
	 */
	float heading( bool *ok=nullptr );

	/**
	 * Read temperature in degree Celsius. If ok is passed, it is set to true,
	 * if temperature data is valid, otherwise it is set to false.
	 */
	short temperature( bool *ok=nullptr );

	/**
	 * Read out the registers X, Y, Z (0...5) in raw format.
	 * Returns true in case of success otherwise false.
	 */
	bool rawHeading();

	/**
	 * Calibrate compass by using the read x, y, z raw values. The calibration
	 * duration is passed as seconds. Additionally a reporter function can be
	 * passed to get intermediate results of the calibration action.
	 */
	bool calibrate( const uint16_t seconds,
	                void (*reporter)( float x, float y, float z ) = nullptr );

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

	/** Set ODR output data rate, accepted values are: [10, 50, 100, 200] */
	void setOutputDataRate( const uint8_t odrIn );

	/** Set magnetic range for measurement, accepted values are: [2, 8]. */
	void setRange( const uint8_t rangeIn );

	/** Set over sample ratio OSR, accepted values are: [64, 128, 256, 512]. */
	void setOverSampleRatio( const uint16_t osrIn );

	/** Write with data part. */
	esp_err_t writeRegister( const uint8_t addr,
			const uint8_t reg,
			const uint8_t value );

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
   * Resets the class calibration variables.
   */
  void resetClassCalibration();

	/** Check, if the bus pointer is valid. */
	bool checkBus();

	static bool m_sensor;
	I2C_t* m_bus;

	/** Variables used by calibration. */
	float xbias, ybias, zbias;
	float xscale, yscale, zscale;
	int16_t xmax, xmin;
	int16_t ymax, ymin;
	int16_t zmax, zmin;
	uint32_t samples;

	/** Read raw values from the chip */
	int xraw, yraw, zraw;

	uint8_t addr; // chip adress
	uint8_t odr;  // output data rate
	uint8_t range; // magnetic resolution of sensor
	uint8_t osr; // over sample ratio
	bool overflowWarning;
	bool calibrationRunning;
};

#endif
