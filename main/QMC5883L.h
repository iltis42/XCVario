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
#include "I2Cbus.hpp"
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

	esp_err_t initialize();

	//  Write with data part
	bool overflowFlag()	{ return overflowWarning; }
	// Read out the registers X, Y, Z (0...5) in raw format into variables, return true if success
	bool readRaw( t_magn_axes &mag );
	int curX(){ return axes.x; };
	int curY(){ return axes.y; };
	int curZ(){ return axes.z; };
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
	t_magn_axes axes;
	int age;
};
