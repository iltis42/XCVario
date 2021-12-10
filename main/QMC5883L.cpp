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
#include "sensor_processing_lib.h"
#include "canbus.h"

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

// Sensor state
bool QMC5883L::m_sensor = false;
// Sensor overflow flag.
bool QMC5883L::overflowWarning = false;
// Error counter
int QMC5883L::errors = 0;
// Calibration flag
bool QMC5883L::calibrationRunning = false;
float QMC5883L::_heading = 0;
int QMC5883L::totalReadErrors = 0;
Average<20> QMC5883L::filterX;
Average<20> QMC5883L::filterY;
Average<20> QMC5883L::filterZ;
Average<20, float, float> QMC5883L::filterRoll;
Average<20, float, float> QMC5883L::filterPitch;

float QMC5883L::pitch = 0;
float QMC5883L::roll = 0;

int16_t QMC5883L::can_x = 0;
int16_t QMC5883L::can_y = 0;
int16_t QMC5883L::can_z = 0;
int QMC5883L::age = 100;

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

	overflowWarning = false;
	resetClassCalibration();
}


QMC5883L::~QMC5883L()
{
}

float QMC5883L::cur_heading( bool *ok ) {
	*ok = true;
	if( age > 10 )
		*ok = false;
	return _heading;
};


/** Write with data part. */
esp_err_t QMC5883L::writeRegister( const uint8_t addr,
		const uint8_t reg,
		const uint8_t value )
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
uint8_t QMC5883L::readRegister( const uint8_t addr,
		const uint8_t reg,
		const uint8_t count,
		uint8_t *data  )
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
	if( !checkBus() )	{
		m_sensor = false;
		return ESP_FAIL;
	}
	// load last known calibration.
	loadCalibration();
	if( compass_enable.get() == CS_I2C || compass_enable.get() == CS_I2C_NO_TILT ){
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
	else if( compass_enable.get() == CS_CAN ){
		for( int i=0; i<300; i++ ){ // give 30 second's chance for module to send data with the corresponding CAN data rate
			if( age < 5 ){          // CAN bus sensor probes next speed after 13 seconds and starts with new speed, so max 26 seconds all 3 speeds are probed
				return ESP_OK;
			}
			delay(100);
		}
	}
	return ESP_FAIL;
}

/**
 * Configure the device with the set parameters and set the mode to continuous.
 * That means, the device starts working.
 */
esp_err_t QMC5883L::initialize( int a_odr, int a_osr )
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

/**
 * Read out the registers X, Y, Z (0...5) in raw format.
 *
 * The status register bits are checked, if the heading data available, if no
 * overflow has occurred and if no data locking is shown.
 *
 * Returns true in case of success otherwise false.
 */

void QMC5883L::fromCAN( const char * msg ){

	can_x = (msg[0] & 0xff) | ((msg[1] << 8) & 0xff00 );
	can_y = (msg[2] & 0xff) | ((msg[3] << 8) & 0xff00 );
	can_z = (msg[4] & 0xff) | ((msg[5] << 8) & 0xff00 );
	// ESP_LOGI(FNAME,"from CAN bus magn X=%d Y=%d Z=%d", can_x, can_y, can_z );
	age = 0;
}

bool QMC5883L::rawHeading()
{
	if( compass_enable.get() == CS_I2C || compass_enable.get() == CS_I2C_NO_TILT ){
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

			xraw = filterX( x );
			yraw = filterY( y );
			zraw = filterZ( z );
			age = 0;
			m_sensor = true;
			// ESP_LOGI( FNAME, "X:%d Y:%d Z:%d  RDY:%d DOR:%d", xraw, yraw,zraw, status & STATUS_DRDY, status & STATUS_DOR );
			return true;
		}
		ESP_LOGE( FNAME, "read Register REG_X_LSB returned count != 6, count: %d", count );
		if( age > 10 )
			m_sensor = false;
		return false;
	}
	else if( compass_enable.get() == CS_CAN ){  // we get compass raw data via CAN interface
		if( age < 10 ){
			xraw = can_x;
			yraw = can_y;
			zraw = can_z;
			// ESP_LOGI( FNAME, "X:%d Y:%d Z:%d  Age:%d", xraw, yraw, zraw, age );
			m_sensor = true;
			return true;
		}
		else{
			// ESP_LOGE( FNAME, "Magnet sensor data from CAN missing");
			// CANbus::restart();
			m_sensor = false;
			return false;
		}
	}
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

/**
 * Resets the class calibration variables.
 */
void QMC5883L::resetClassCalibration()
{
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
 * Calibrate compass by using the read x, y, z raw values. The calibration is
 * stopped by the reporter function which displays intermediate results of the
 * calibration action.
 */
bool QMC5883L::calibrate( bool (*reporter)( float xc, float yc, float zc, float xscale, float yscale, float zscale, float xb, float yb, float zb ) )
{
	// reset all old calibration data
	ESP_LOGI( FNAME, "calibrate magnetic sensor" );
	calibrationRunning = true;
	resetCalibration();

	ESP_LOGI( FNAME, "calibrate min-max xyz");

	int i = 0;
	uint64_t lastReport = 0;

	// #define MAX_MIN_LOGGING

#ifdef MAX_MIN_LOGGING
	int xmin_old = 0;
	int xmax_old = 0;
	int ymin_old = 0;
	int ymax_old = 0;
	int zmin_old = 0;
	int zmax_old = 0;
#endif

	while( true )
	{
		i++;
		if( rawHeading() == false )
		{
			errors++;
			if( errors > 10 ){
				initialize();
				errors = 0;
			}
			continue;
		}
		// uint64_t start = getMsTime();

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
		xbias = ( (float)xmax + xmin ) / 2;
		ybias = ( (float)ymax + ymin ) / 2;
		zbias = ( (float)zmax + zmin ) / 2;

		// Calculate soft-iron scale factors
		// calculate average x, y, z axis max chord length in counts
		float xchord = ( (float)xmax - xmin ) / 2;
		float ychord = ( (float)ymax - ymin ) / 2;
		float zchord = ( (float)zmax - zmin ) / 2;

		float cord_avgerage = ( xchord + ychord + zchord ) / 3.;

		xscale = cord_avgerage / xchord;
		yscale = cord_avgerage / ychord;
		zscale = cord_avgerage / zchord;

		if( !(i%4) )
		{
			// Send a calibration report to the subscriber every 500ms
			reporter( xraw,yraw,zraw, xscale, yscale, zscale, xbias, ybias, zbias );
		}
		if( MenuEntry::_rotary->readSwitch() == true  )  // more responsive to query every loop
			break;
	}

	ESP_LOGI( FNAME, "Read Cal-Samples=%d, OK=%d, NOK=%d",
			i, i-errors, errors );

	if( i < 2 )
	{
		// Too less samples to start calibration
		ESP_LOGI( FNAME, "calibrate min-max xyz not enough samples");
		calibrationRunning = false;
		return false;
	}

	// save calibration
	saveCalibration();

	ESP_LOGI( FNAME, "Compass: xmin=%d xmax=%d, ymin=%d ymax=%d, zmin=%d zmax=%d",
			xmin, xmax, ymin, ymax, zmin, zmax );

	ESP_LOGI( FNAME, "Compass hard-iron: xbias=%.3f, ybias=%.3f, zbias=%.3f",
			xbias, ybias, zbias );

	ESP_LOGI( FNAME, "Compass soft-iron: xscale=%.3f, yscale=%.3f, zscale=%.3f",
			xscale, yscale, zscale );

	calibrationRunning = false;


	ESP_LOGI( FNAME, "calibration end" );

	return true;
}


int N=0;
bool holddown=false;
/**
 * Reads the heading in degrees of 0...359. Ok is set to true,
 * if heading data is valid, otherwise it is set to false.
 */
float QMC5883L::heading( bool *ok )
{
	assert( (ok != nullptr) && "Passing of NULL pointer is forbidden" );

	if( calibrationRunning == true )
	{
		*ok = false;
		// ESP_LOGI(FNAME,"Calibration running, return 0");
		return 0.0;
	}
	// ESP_LOGI(FNAME,"QMC5883L::heading() errors:%d, N:%d", errors, N );
	// Holddown processing and throwing errors once sensor is gone
	if( errors > 100 && errors % 100 )
	{
		*ok = false;
		errors++;
		// ESP_LOGI(FNAME,"Errors overrun, return 0");
		return 0.0;
	}
	if( errors > 100 ){
		if( !holddown ){
			holddown = true;
			ESP_LOGI(FNAME,"100 sensor read errors: Start Holddown");
		}
	}else{
		if( holddown ){
			holddown = false;
		}
	}
	bool state = rawHeading();
	// ESP_LOGI(FNAME,"state %d", state );
	N++;
	if( state == false )
	{
		errors++;
		totalReadErrors++;
		//if( !(totalReadErrors%100) )
		// ESP_LOGI(FNAME,"Magnetic sensor error Reads:%d, Total Errors:%d  Init: %d", N, totalReadErrors, errors );
		if( errors > 10 )
		{
			// ESP_LOGI(FNAME,"Magnetic sensor errors > 10: init mag sensor" );
			//  reinitialize once crashed, one retry
			if( compass_enable.get() != CS_CAN ){
				if( initialize() != ESP_OK )
					initialize();
			}
			*ok=false;
			return 0.0;
		}
		if( errors > 100 ){  // survive 100 samples with constant heading if no valid reading
			*ok = false;
			return 0.0;
		}
		*ok = true;
		return _heading;
	}
	errors = 0;

	// Check if calibration data are available
	if( compass_calibrated.get() == 0 )
	{
		// No calibration data available, return error because to return
		// the raw heading is not meaningful.
		*ok = false;
		// ESP_LOGI(FNAME,"Not calibrated" );
		return 0.0;
	}

	/* Apply corrections to the measured values. Note, due to mounting of chip
	 * turned clockwise by 90 degrees the X-axis and the Y-axis are moved and
	 * have to be handled in this way.
	 */
	// ESP_LOGI( FNAME, "heading: X:%d Y:%d Z:%d xs:%f ys:%f zs:%f", xraw, yraw, zraw, xscale, yscale, zscale);

	double fy = (double) ((float( xraw ) - xbias) * xscale);
	double fx = -(double) ((float( yraw ) - ybias) * yscale);
	double fz = (double) ((float( zraw ) - zbias) * zscale);

	roll = filterRoll( IMU::getRollRad() );      // equal filter with equal delay to raw compass data for pitch and roll
	pitch = filterPitch( IMU::getPitchRad() );

	double tcx = fx * cos( -pitch ) + fy * sin( -roll ) * sin( -pitch) - fz * cos( -roll) * sin( -pitch);
	double tcy = fy * cos( -roll) + fz * sin( -roll);
	if( isnan(tcx) || isnan(tcy) ){
		*ok = true;
		return _heading;  // deliver old value in case of gimbal lock
	}

	if( compass_enable.get() == CS_CAN || compass_enable.get() == CS_I2C ){
		_heading = -RAD_TO_DEG * atan2( tcy, tcx );
		// ESP_LOGI(FNAME,"tcy %03.2f tcx %03.2f  heading:%03.1f pi:%.1f ro:%.1f", tcy, tcx, _heading, pitch, roll );
	}
	else if ( compass_enable.get() == CS_I2C_NO_TILT )
		_heading = -RAD_TO_DEG * atan2( fy, fx );

    // float heading_nt = -RAD_TO_DEG * atan2( fy, fx );
	// heading_nt = Vector::normalizeDeg( heading_nt );

	_heading = Vector::normalizeDeg( _heading );

	// ESP_LOGI(FNAME,"CR: %.1f TC:%.1f Roll:%0.1f Pitch:%.1f", heading_nt, _heading, R2D(roll*2)+180, R2D(pitch*2)+180  );
#if 0
	if( wind_logging.get() ){
		char log[120];
		sprintf( log, "$COMPASS;%d;%d;%d;%.1f;%.1f;%.1f;%d\n", xraw, yraw, zraw, IMU::getPitch(), IMU::getRoll(),  _heading,  totalReadErrors );
		Router::sendXCV( log );
	}
#endif

	*ok = true;
	return _heading;
}
