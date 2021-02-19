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

Author: Axel Pauli, January 2021

Last update: 2021-02-18

 ***************************************************************************/

// Activate/deactivate debug messages
// #define DEBUG_COMP 1

#include <cmath>
#include <ctime>
#include <logdef.h>
#include "QMC5883L.h"
#include "KalmanMPU6050.h"
#include "SetupNG.h"

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

/* Flags for Control Register #1. */
#define ODR_10HZ   0b00000000 // Output Data Rate in Hz.
#define ODR_50HZ   0b00000100
#define ODR_100HZ  0b00001000
#define ODR_200HZ  0b00001100

#define RNG_2G 0b00000000  // Range 2 Gauss: for magnetic-clean environments.
#define RNG_8G 0b00010000  // Range 8 Gauss: for strong magnetic fields.

#define OSR_512 0b00000000  // Over Sample Rate 512: less noise, more power.
#define OSR_256 0b01000000
#define OSR_128 0b10000000
#define OSR_64  0b11000000  // Over Sample Rate 64: more noise, less power.

// Sensor state
bool QMC5883L::m_sensor = false;
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
		I2C_t *i2cBus ) :
						  m_bus( i2cBus ),
						  addr( addrIn ),
						  odr( odrIn ),
						  range( rangeIn ),
						  osr( osrIn ),
						  overflowWarning( false ),
						  calibrationRunning( false )
{
	if( addrIn == 0 )
	{
		// set address to default value of chip, if it is zero.
		addr = QMC5883L_ADDR;
	}

	setOutputDataRate( odrIn );
	setRange( rangeIn );
	setOverSampleRatio( osrIn );
}

QMC5883L::~QMC5883L()
{
}

/** Write with data part. */
esp_err_t QMC5883L::writeRegister( const uint8_t addr,
		const uint8_t reg,
		const uint8_t value )
{
	if( checkBus() == false )
	{
		return ESP_FAIL;
	}

	esp_err_t err = m_bus->writeByte( addr, reg, value );

	if( err != ESP_OK )
	{
		ESP_LOGE( FNAME,
				"QMC5883L writeRegister( 0x%02X, 0x%02X, 0x%02X ) FAILED",
				addr, reg, value );
		return ESP_FAIL;
	}
	return err;
}

/**
 * Read bytes from the chip.
 * Return the number of read bytes or 0 in error case.
 */
uint8_t QMC5883L::readRegister( const uint8_t addr,
		const uint8_t reg,
		const uint8_t count,
		uint8_t *data  )
{
	if( checkBus() == false )
		return 0;
	// read bytes from chip
	esp_err_t err = m_bus->readBytes( addr, reg, count, data );
	if( err != ESP_OK ){
		ESP_LOGE( FNAME,"QMC5883L readRegister( 0x%02X, 0x%02X, %d ) FAILED", addr, reg, count );
		return 0;
	}
	return count;
}

/** Check, if the bus pointer is valid. */
bool QMC5883L::checkBus()
{
	if( m_bus == nullptr )	{
		ESP_LOGE( FNAME, "QMC5883L bus pointer is zero" );
		return false;
	}
	return true;
}

// scan bus for I2C address
esp_err_t QMC5883L::selfTest()
{
	if( checkBus() == false )	{
		m_sensor = false;
		return ESP_FAIL;
	}
	uint8_t chipId = 0;
	// Try to read Register 0xD, it delivers the chip id 0xff for a QMC5883L
	esp_err_t err = m_bus->readByte( QMC5883L_ADDR, REG_CHIP_ID, &chipId );
	if( err != ESP_OK ){
		m_sensor = false;
		ESP_LOGE( FNAME,"QMC5883L self-test, scan for I2C address 0x%02X FAILED", QMC5883L_ADDR );
		return ESP_FAIL;
	}
	if( chipId != 0xff ){
		m_sensor = false;
		ESP_LOGE( FNAME, "QMC5883L self-test, chip ID 0x%02X is unsupported, expected 0xFF",	chipId );
		return ESP_FAIL;
	}
	ESP_LOGI( FNAME, "QMC5883L selftest, scan for I2C address 0x%02X and chip ID 0x%02X PASSED",	QMC5883L_ADDR, chipId );
	m_sensor = true;

	// load last known calibration.
	loadCalibration();
	return ESP_OK;
}

/**
 * Configure the device with the set parameters and set the mode to continuous.
 * That means, the device starts working.
 */
esp_err_t QMC5883L::modeContinuous()
{
	esp_err_t e1, e2, e3, e4;
	e1 = e2 = e3 = e4 = 0;

	// Soft Reset
	e1 = writeRegister( addr, REG_CONTROL2, SOFT_RST );

	// Enable ROL_PTN, Pointer roll over function.
	e2 = writeRegister( addr, REG_CONTROL2, POL_PNT );

	// Define SET/RESET period. Should be set to 1
	e3 = writeRegister( addr, REG_RST_PERIOD, 1 );

	// Set mesaurement data and start it in dependency of mode bit.
	e4 = writeRegister( addr,
			REG_CONTROL1,
			osr | range | odr | MODE_CONTINUOUS );

	if( (e1 + e2 + e3 + e4) == 0 )
		return ESP_OK;

	return ESP_FAIL;
}

/**
 * Set the device in standby mode.
 */
esp_err_t QMC5883L::modeStandby()
{
	// Soft reset, device goes after that in the standby mode.
	return writeRegister( addr, REG_CONTROL2, SOFT_RST );
}

/**
 * Define SET/RESET period. Should be set to 1 after a reset.
 */
esp_err_t QMC5883L::setPeriodRegister()
{
	return writeRegister( addr, REG_RST_PERIOD, 0x01 );
}

/** Set oversampling rate OSR_64 ... OSR_512. */
void QMC5883L::setOverSampleRatio( const uint16_t osrIn )
{
	switch( osrIn )
	{
	case 512:
		osr = OSR_512;
		break;
	case 256:
		osr = OSR_256;
		break;
	case 128:
		osr = OSR_128;
		break;
	case 64:
		osr = OSR_64;
		break;
	default:
		ESP_LOGE( FNAME, "Wrong Over Sample Ration value %d passed",
				osrIn );
		break;
	}
}

/** Set magnetic range for measurement RNG_2G, RNG_8G. */
void QMC5883L::setRange( const uint8_t rangeIn )
{
	switch( rangeIn )
	{
	case 2:
		range = RNG_2G;
		break;
	case 8:
		range = RNG_8G;
		break;
	default:
		ESP_LOGE( FNAME, "Wrong Gauss Range value %d passed",rangeIn );
		break;
	}
}

/** Set ODR output data rate. */
void QMC5883L::setOutputDataRate( const uint8_t odrIn )
{
	switch( odrIn )
	{
	case 10:
		odr = ODR_10HZ;
		break;
	case 50:
		odr = ODR_50HZ;
		break;
	case 100:
		odr = ODR_100HZ;
		break;
	case 200:
		odr = ODR_200HZ;
		break;
	default:
		ESP_LOGE( FNAME, "Wrong Output Data Rate value %d passed", odrIn );
		break;
	}
}

/**
 * Read out the registers X, Y, Z (0...5) in raw format.
 *
 * The status register bits are checked, if the heading data available, if no
 * overflow has occurred and if no data locking is shown.
 *
 * Returns true in case of success otherwise false.
 */
bool QMC5883L::rawHeading()
{
	// Check, if data are available
	uint8_t data[6];
	uint8_t status = 0;

	// Read status register
	uint8_t count = readRegister( addr, REG_STATUS, 1, &status );

	if( count != 1 )
	{
		ESP_LOGE( FNAME, "read REG_STATUS FAILED, count=%d, status=%d", count, status );
		return false;
	}
	// ESP_LOGI( FNAME, "REG_STATUS: %02x", status );

	if( ( status & STATUS_OVL ) == true &&
			range == RNG_2G && overflowWarning == false )
	{
		// Overflow has occurred, give out a warning only once
		overflowWarning = true;
		ESP_LOGE( FNAME, "readRawHeading detected a gauss overflow." );
		return false;;
	}

	if( ( status & STATUS_DOR ) == true )
	{
		// Previous measure was read partially, sensor in Data Lock.
		// Read all data again to overcome lock.
		readRegister( addr, REG_X_LSB, 6, data );
		ESP_LOGE( FNAME, "read REG_X_LSB FAILED" );
		return false;
	}

	if( ( status & STATUS_DRDY ) == true  )
	{
		// Data ready for reading
		if( readRegister( addr, REG_X_LSB, 6, data ) > 0 )
		{
			xraw = (int)( (int16_t)(( data[1] << 8 ) | data[0]) );
			yraw = (int)( (int16_t)(( data[3] << 8 ) | data[2]) );
			zraw = (int)( (int16_t)(( data[5] << 8 ) | data[4]) );
			return true;
		}
		ESP_LOGE( FNAME, "read Register returned <= 0" );
	}
	ESP_LOGE( FNAME, "STATUS_DRDY FAILED" );

	return false;
}

/**
 * Read temperature in degree Celsius. If ok is passed, it is set to true,
 * if temperature data is valid, otherwise it is set to false.
 */
int16_t QMC5883L::temperature( bool *ok )
{
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

/**
 * Resets the class calibration variables.
 */
void QMC5883L::resetClassCalibration()
{
	samples = 0;
	xraw = yraw = zraw = 0;
	xbias = ybias = zbias = 0.0;
	xscale = yscale = zscale = 0.0;
	xmax = ymax = zmax = -32767;
	xmin = ymin = zmin = 32767;
}

/**
 * Resets the whole compass calibration, also the saved configuration.
 */
void QMC5883L::resetCalibration()
{
	resetClassCalibration();

	// reset nonvolatile configuration data
	compass_x_bias.set( 0 );
	compass_y_bias.set( 0 );
	compass_z_bias.set( 0 );
	compass_x_scale.set( 0 );
	compass_y_scale.set( 0 );
	compass_z_scale.set( 0 );
	compass_calibrated.set( 0 );
	// commit() is implicitely done in set()
}

/**
 * Saves a done compass calibration.
 */
void QMC5883L::saveCalibration()
{
	compass_x_bias.set( xbias );
	compass_y_bias.set( ybias );
	compass_z_bias.set( zbias );
	compass_x_scale.set( xscale );
	compass_y_scale.set( yscale );
	compass_z_scale.set( zscale );
	compass_calibrated.set( 1 );
}

/**
 * Loads a stored compass calibration. Returns true, if valid calibration
 * data could be loaded, otherwise false.
 */
bool QMC5883L::loadCalibration()
{
	if( compass_calibrated.get() == 0 )
	{
		ESP_LOGI( FNAME, "Compass calibration is not valid" );
		// no stored calibration available
		resetClassCalibration();
		return false;
	}

	xbias = compass_x_bias.get();
	ybias = compass_y_bias.get();
	zbias = compass_z_bias.get();
	xscale = compass_x_scale.get();
	yscale = compass_y_scale.get();
	zscale = compass_z_scale.get();

	ESP_LOGI( FNAME, "Read calibration: %f, %f, %f, %f, %f, %f, valid=%d",
			xbias, ybias, zbias, xscale, yscale, zscale, compass_calibrated.get() );

	return true;
}

/**
 * Calibrate compass by using the read x, y, z raw values. The calibration
 * duration is passed as seconds. Additionally a reporter function can be
 * passed to get intermediate results of the calibration action.
 */
bool QMC5883L::calibrate( const uint16_t seconds,
                          void (*reporter)( float x, float y, float z ) )
{
	// reset all old calibration data
	ESP_LOGI( FNAME, "calibrate( %d sec )", seconds);
	resetCalibration();

	// The calibration is done with data output rate of 100 Hz.
	samples = seconds * 1000 / 15;

	// Save current used ODR and OSR
	uint8_t usedOdr = odr;
	uint8_t usedOsr = osr;

	// Set ODR to 100 Hz
	setOutputDataRate( 100 );

	// Set OSR to 5125
	setOverSampleRatio( 512 );

	calibrationRunning = true;

	// Switch on continues mode
	if( modeContinuous() != ESP_OK )
	{
		odr = usedOdr;
		osr = usedOsr;
		calibrationRunning = false;
		return false;
	}

	// wait a moment after measurement start.
	delay( 100 );

	ESP_LOGI( FNAME, "calibrate min-max xyz");

	int i = 0;
	int errors = 0;

// #define MAX_MIN_LOGGING

#ifdef MAX_MIN_LOGGING
	int xmin_old = 0;
	int xmax_old = 0;
	int ymin_old = 0;
	int ymax_old = 0;
	int zmin_old = 0;
	int zmax_old = 0;
#endif

	for( i=0 ; i < samples; i++ )
	{
		if( rawHeading() == false )
		{
			errors++;
		}

		/* Find max/min xyz values */
		xmin = ( xraw < xmin ) ? xraw : xmin;
		ymin = ( yraw < ymin ) ? yraw : ymin;
		zmin = ( zraw < zmin ) ? zraw : zmin;
		xmax = ( xraw > xmax ) ? xraw : xmax;
		ymax = ( yraw > ymax ) ? yraw : ymax;
		zmax = ( zraw > zmax ) ? zraw : zmax;

#ifdef MAX_MIN_LOGGING
		if( xmin_old != xmin ){
			ESP_LOGI( FNAME, "New X-Min: %d", xmin );
			xmin_old = xmin;
		}
		if( xmax_old != xmax ){
			ESP_LOGI( FNAME, "New X-Max: %d", xmax );
			xmax_old = xmax;
		}
		if( ymin_old != ymin ){
			ESP_LOGI( FNAME, "New Y-Min: %d", ymin );
			ymin_old = ymin;
		}
		if( ymax_old != ymax ){
			ESP_LOGI( FNAME, "New Y-Max: %d", ymax );
			ymax_old = ymax;
		}
		if( zmin_old != zmin ){
			ESP_LOGI( FNAME, "New Z-Min: %d", zmin );
			zmin_old = zmin;
		}
		if( zmax_old != zmax ){
			ESP_LOGI( FNAME, "New Z-Max: %d", zmax );
			zmax_old = zmax;
		}
#endif

		if( i < 2 )
		  continue;

	  // Calculate hard iron correction
	  // calculate average x, y, z magnetic bias in counts
	  xbias = static_cast<float>( xmax + xmin ) / 2.;
	  ybias = static_cast<float>( ymax + ymin ) / 2.;
	  zbias = static_cast<float>( zmax + zmin ) / 2.;

	  // Calculate soft-iron scale factors
	  // calculate average x, y, z axis max chord length in counts
	  float xchord = static_cast<float>( xmax - xmin ) / 2.;
	  float ychord = static_cast<float>( ymax - ymin ) / 2.;
	  float zchord = static_cast<float>( zmax - zmin ) / 2.;

	  float cord_avgerage = ( xchord + ychord + zchord ) / 3.;

	  xscale = cord_avgerage / xchord;
	  yscale = cord_avgerage / ychord;
	  zscale = cord_avgerage / zchord;

	  if( i > 0 && i % 30 == 0 && reporter != nullptr )
	    {
	      // Send every second a calibration report to the subscriber
	      reporter( xscale, yscale, zscale );
	    }

    // The sensor seems to have sometimes problems to deliver all 10ms new data
    delay( 15 );
	}

	ESP_LOGI( FNAME, "Read Cal-Samples=%d, OK=%d, NOK=%d",
	          samples, samples-errors, errors );

	if( i < 2 )
	{
		// Too less samples to start calibration
		ESP_LOGI( FNAME, "calibrate min-max xyz not enough samples");
		odr = usedOdr;
		osr = usedOsr;
		calibrationRunning = false;
		return false;
	}

	// save calibration
	saveCalibration();

	// Set used before ODR and OSR values
	odr = usedOdr;
	osr = usedOsr;

	ESP_LOGI( FNAME, "Compass: xmin=%d xmax=%d, ymin=%d ymax=%d, zmin=%d zmax=%d",
	          xmin, xmax, ymin, ymax, zmin, zmax );

	ESP_LOGI( FNAME, "Compass hard-iron: xbias=%.3f, ybias=%.3f, zbias=%.3f",
	          xbias, ybias, zbias );

	ESP_LOGI( FNAME, "Compass soft-iron: xscale=%.3f, yscale=%.3f, zscale=%.3f",
	          xscale, yscale, zscale );

	// restart previous continuous mode
	modeContinuous();
	calibrationRunning = false;
	ESP_LOGI( FNAME, "calibration end" );
	return true;
}

/**
 * Reads the heading in degrees of 1...360. If ok is passed, it is set to true,
 * if heading data is valid, otherwise it is set to false.
 */
float QMC5883L::heading( bool *ok )
{
	static short error = 0;

	if( ok != nullptr )
		*ok = false;

	// Holddown processing and throwing errors once sensor is gone
	if( error > 100 && error%100 ){
		*ok = false;
		error++;
		return 0;
	}


	if( calibrationRunning == true )
	{
		// Calibration is running, don't disturb it.
		return 0.0;
	}

	if( rawHeading() == false )
	{
		ESP_LOGE(FNAME,"rawHeading() returned false" );
		error++;

		if( error > 10 ) {
			modeContinuous();  // reinitialize once crashed
		}

		return 0.0;
	}

	error = 0;

	// Check if calibration data are available
  if( compass_calibrated.get() == 0 )
    {
      // No calibration data available, return error because to return
      // the raw heading is not meaningful.
      return 0.0;
    }

	/* Apply corrections to the measured values. */
	double fx = -(double) ((float( xraw ) - xbias) * xscale);
	double fy = -(double) ((float( yraw ) - ybias) * yscale);
	double fz = (double) ((float( zraw ) - zbias) * zscale);

#if 0
	double headingc = RAD_TO_DEG * atan2( fx, fy );

  if( headingc <= 0.0 )
    headingc += 360.0;

	static uint8_t out = 0;

	if( out % 10 )
	  ESP_LOGI( FNAME, "fX=%f fY=%f fZ=%f C-Heading=%.1f", fx, fy, fz, headingc );
#endif

	// ESP_LOGI(FNAME,"RANGE XH:%d YH:%d ZH:%d  XL:%d YL:%d ZL:%d OX:%d OY:%d OZ:%d", xmax,ymax,zmax, xmin,ymin,zmin, xmax + xmin, ymax + ymin,zmax + zmin);
	// ESP_LOGI(FNAME,"RAW NORM Flux, fx:%f fy:%f fz:%f", fx,fy,fz);

	// 	Xhorizontal = X*cos(pitch) + Y*sin(roll)*sin(pitch) – Z*cos(roll)*sin(pitch)
	double tcx = fx * cos( IMU::getPitchRad() ) + fy * sin( IMU::getRollRad() ) * sin( IMU::getPitchRad()) - fz * cos( IMU::getRollRad()) * sin( IMU::getPitchRad());
	// ESP_LOGI(FNAME,"RR:%f, PR:%f tcx 1:%f tcx2:%f tcx3:%f", IMU::getPitchRad(), IMU::getRollRad(), fx*cos( IMU::getPitchRad() ),   fy*sin( IMU::getRollRad() )*sin( IMU::getPitchRad()), fz*cos( IMU::getRollRad())*sin( IMU::getPitchRad() ) );
	// 	Yhorizontal = Y*cos(roll) + Z*sin(roll)
	double tcy = fy * cos( IMU::getRollRad()) + fz * sin( IMU::getRollRad());

	double heading = RAD_TO_DEG * atan2( tcx, tcy );

	if( heading <= 0.0 )
		heading += 360.0;

	if( ok != nullptr )
		*ok = true;

#ifdef DEBUG_COMP1
  ESP_LOGI( FNAME,
            "rawHeading, x:%d y:%d z:%d, roll: %f  pitch: %f  tcx:%f tcy:%f mh:%f ",
            xraw, yraw, zraw, IMU::getRoll(), IMU::getPitch(), tcx, tcy, heading );
#endif

  return float( heading );
}
