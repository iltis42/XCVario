/**************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

I2C driver for the chip QMC5883L, 3-Axis Magnetic Sensor.

QMC5883L data sheet:

https://datasheetspdf.com/pdf-file/1309218/QST/QMC5883L/1

File: QMC5883L.cpp

Author: Axel Pauli, January 2021

Last update: 2021-04-05

 ***************************************************************************/

// Activate/deactivate debug messages
// #define DEBUG_COMP 1

#include <cassert>
#include <cmath>
#include "QMC5883L.h"
#include "KalmanMPU6050.h"
#include "SetupNG.h"
#include "MenuEntry.h"
#include "Router.h"
#include "vector.h"
// #include "sensor_processing_lib.h"
// #include "canbus.h"

/* Register numbers */
#define REG_X_LSB 0         // Output Data Registers for magnetic sensor.
#define REG_X_MSB 1
#define REG_Y_LSB 2
#define REG_Y_MSB 3
#define REG_Z_LSB 4
#define REG_Z_MSB 5
#define REG_STATUS 6        // Status Register.
#define REG_TEMP_LSB 7      // Output Data Registers for temperature.
#define REG_TEMP_MSB 8
#define REG_CONTROL1 9      // Control Register #1.
#define REG_CONTROL2 0xA    // Control Register #2.
#define REG_RST_PERIOD 0xB  // ET/RESET Period Register.
#define REG_RESERVED 0xC    // Reserved.
#define REG_CHIP_ID 0xD     // Chip ID register.

/* Flags for Status Register #1. */
#define STATUS_DRDY 1 // Data Ready.
#define STATUS_OVL 2  // Overflow flag.
#define STATUS_DOR 4  // Data skipped for reading.

/* Flags for Status Register #2. */
#define INT_ENB 0b00000001   // Interrupt Pin Enabling.
#define POL_PNT 0b01000000   // Pointer Roll-over.
#define SOFT_RST 0b10000000  // Soft Reset.

/* Flags for Control Register 1. */
#define MODE_STANDBY    0b00000000  // Standby mode.
#define MODE_CONTINUOUS 0b00000001  // Continuous read mode.

/*
  Creates instance for I2C connection with passing the desired parameters.
  No action is done at the bus. Note if i2cBus is not set in the constructor,
  you have to set it by calling method setBus(). The default address of the
  chip is 0x0D.
 */


QMC5883L::QMC5883L( const uint8_t addrIn,
		const uint8_t odrIn,
		const uint8_t rangeIn,
		const uint16_t osrIn,
		I2C_t *i2cBus ) : m_bus( i2cBus ), addr( addrIn ), odr( odrIn ), range( rangeIn ), osr( osrIn )
{
	ESP_LOGI( FNAME, "QMC5883L( %02X )", addrIn );

	if( addrIn == 0 )
	{
		// set address to default value of chip, if it is zero.
		addr = QMC5883L_ADDR;
	}
	age = 0;
	overflowWarning = false;
	m_sensor = false;
}


QMC5883L::~QMC5883L()
{
}

bool QMC5883L::checkBus()
{
	if( m_bus == nullptr )	{
		// ESP_LOGE( FNAME, "QMC5883L bus pointer is zero" );
		return false;
	}
	return true;
}

/** Write with data part. */
esp_err_t QMC5883L::writeRegister( const uint8_t addr,	const uint8_t reg,	const uint8_t value )
{
	if( checkBus() == false )
		return ESP_FAIL;

	esp_err_t err = m_bus->writeByte( addr, reg, value );

	if( err != ESP_OK )	{
		// ESP_LOGE( FNAME, "QMC5883L writeRegister( 0x%02X, 0x%02X, 0x%02X ) FAILED",	addr, reg, value );
		return ESP_FAIL;
	}
	return err;
}

/**
 * Read bytes from the chip.
 * Return the number of read bytes or 0 in error case.
 */
uint8_t QMC5883L::readRegister( const uint8_t addr,	const uint8_t reg,	const uint8_t count, uint8_t *data  )
{
	if( checkBus() == false )
		return 0;
	// read bytes from chip
	for( int i=0; i<=100; i++ ){
		esp_err_t err = m_bus->readBytes( addr, reg, count, data );
		if( err == ESP_OK && count == 6 ){
			return count;
		}
		else
		{
			// ESP_LOGW( FNAME,"readRegister( 0x%02X, 0x%02X, %d ) FAILED N:%d", addr, reg, count, i );
			if( i == 100 ){
				ESP_LOGW( FNAME,"100 retries read mag sensor failed also, now try to reinitialize chip");
				if( initialize() != ESP_OK )
					initialize();  // one retry
				err = m_bus->readBytes( addr, reg, count, data );
				if( err == ESP_OK && count == 6 ){
					// ESP_LOGI( FNAME,"Read after reinit ok");
					return count;
				}
			}
			// ESP_LOGW( FNAME,"retry #%d reg=%02X err=%d", i, reg, err );
			delay(2);
		}
	}
	ESP_LOGW( FNAME,"Read after init and retry failed also, return with no data, len=0");
	return 0;
}

// scan bus for I2C address
esp_err_t QMC5883L::selfTest()
{
	ESP_LOGI( FNAME, "QMC5883L selftest");
	// load last known calibration.
	if( !checkBus() )	{
		m_sensor = false;
		return ESP_FAIL;
	}
	uint8_t chipId = 0;
	// Try to read Register 0xD, it delivers the chip id 0xff for a QMC5883L
	m_sensor = false;
	for( int i=0; i< 10; i++ ){
		esp_err_t err = m_bus->readByte( QMC5883L_ADDR, REG_CHIP_ID, &chipId );
		if( err == ESP_OK ){
			m_sensor = true;
			break;
		}
		delay(20);
	}
	if( !m_sensor ){
		ESP_LOGE( FNAME,"Scan for I2C address 0x%02X FAILED", QMC5883L_ADDR );
		return ESP_FAIL;
	}
	if( chipId != 0xff ){
		m_sensor = false;
		ESP_LOGE( FNAME, "QMC5883L self-test, detected chip ID 0x%02X is unsupported, expected 0xFF",	chipId );
		return ESP_FAIL;
	}
	ESP_LOGI( FNAME, "QMC5883L selftest PASSED");
	m_sensor = true;
	return ESP_OK;

}

esp_err_t QMC5883L::initialize() {
	return initialize2( odr, osr );
}


/**
 * Configure the device with the set parameters and set the mode to continuous.
 * That means, the device starts working.
 */
esp_err_t QMC5883L::initialize2( int a_odr, int a_osr )
{
	esp_err_t e1, e2, e3, e4;
	e1 = e2 = e3 = e4 = 0;

	// Soft Reset
	e1 = writeRegister( addr, REG_CONTROL2, SOFT_RST );
	delay(2);

	// Enable ROL_PTN, Pointer roll over function.
	e2 = writeRegister( addr, REG_CONTROL2, POL_PNT );
	// Define SET/RESET period. Should be set to 1
	e3 = writeRegister( addr, REG_RST_PERIOD, 1 );
	// Set mesaurement data and start it in dependency of mode bit.
	int used_osr = a_osr;
	if( used_osr == 0 )
		used_osr = osr;

	int used_odr = a_odr;
	if( used_odr == 0 )
		used_odr = odr;

	// ESP_LOGI( FNAME, "initialize() dataRate: %d Oversampling: %d", used_odr, used_osr );

	e4 = writeRegister( addr, REG_CONTROL1,	(used_osr << 6) | (range <<4) | (used_odr <<2) | MODE_CONTINUOUS );
	if( e1 == ESP_OK || e2 == ESP_OK || e3 == ESP_OK || e4 == ESP_OK ) {
		// ESP_LOGI( FNAME, "initialize() OK");
		return ESP_OK;
	}
	else{
		ESP_LOGE( FNAME, "initialize() ERROR %d %d %d %d", e1,e2,e3,e4 );
		return ESP_FAIL;
	}
	return ESP_FAIL;
}


bool QMC5883L::rawAxes( t_magn_axes &axes )
{
	uint8_t data[6];
	uint8_t status = 0;
	// Check, if data are available
	// as sensor is outside the housing, there may be I2C noise, so we need retries
	bool okay = false;
	// Poll status until RDY or DOR
	esp_err_t ret = ESP_OK;
	for( int i=1; i<30; i++ ){
		ret = m_bus->readByte( addr, REG_STATUS, &status );
		if( ret == ESP_OK ){
			if( (status & STATUS_DRDY) || (status & STATUS_DOR )  ){
				okay = true;
				break;
			}
			// else
			// 	ESP_LOGW( FNAME, "No new data,  N=%d  RDY%d  DOR%d REG:%02X", i, status & STATUS_DRDY, status & STATUS_DOR, status );
		}
		else{
			delay( 2 );
			// ESP_LOGW( FNAME, "read REG_STATUS failed, N=%d  RDY%d  DOR%d", i, status & STATUS_DRDY, status & STATUS_DOR );
		}
	}
	if( okay == false )
	{
		// ESP_LOGE( FNAME, "read REG_STATUS FAILED %d", ret );
		return false;
	}

	if( ( status & STATUS_OVL ) == true )
	{
		// Magnetic X-Y-Z data overflow has occurred, give out a warning only once
		if( overflowWarning == false ){
			ESP_LOGW( FNAME, "read rawHeading detected a X-Y-Z data overflow." );
			overflowWarning = true;
			return false;
		}
	}
	// Reset overflow warning, to get a current status of it.

	overflowWarning = false;

	// Precondition already checked in loop before, point only reached if there is RDY or DOR
	int count = readRegister( addr, REG_X_LSB, 6, data );
	// Data can be read in every case
	if( count == 6 )
	{
		int x = (int)( (int16_t)(( data[1] << 8 ) | data[0]) );
		int y = (int)( (int16_t)(( data[3] << 8 ) | data[2]) );
		int z = (int)( (int16_t)(( data[5] << 8 ) | data[4]) );

		axes.x = filterX( x );
		axes.y = filterY( y );
		axes.z = filterZ( z );
		age = 0;
		m_sensor = true;
		// ESP_LOGI( FNAME, "X:%d Y:%d Z:%d  RDY:%d DOR:%d", raw.x, raw.y,raw.z, status & STATUS_DRDY, status & STATUS_DOR );
		return true;
	}
	ESP_LOGE( FNAME, "read Register REG_X_LSB returned count != 6, count: %d", count );
	if( age > 10 )
		m_sensor = false;
	return false;
}

/**
 * Read temperature in degree Celsius. If ok is passed, it is set to true,
 * if temperature data is valid, otherwise it is set to false.
 */
int16_t QMC5883L::temperature( bool *ok )
{
	assert( (ok != nullptr) && "Passing of NULL pointer is forbidden" );

	uint8_t data[2];
	if( readRegister( addr, REG_TEMP_LSB, 2, data ) == 0 ){
		if( ok != nullptr )
			*ok = false;
		// Nothing has been read
		return 0.0;
	}
	int16_t t = ( data[1] << 8 ) | data[0];

	if( ok != nullptr )
		*ok = true;
	return t;
}
