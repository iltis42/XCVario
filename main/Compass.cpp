/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************

File: Compass.cpp

Class to handle compass data access.

Author: Axel Pauli, deviation and refactoring by Eckhard Völlm Dec 2021


 **************************************************************************/

#include <cassert>
#include <cmath>
#include "esp_log.h"
#include "esp_system.h"
#include "sensor.h"
#include "Compass.h"
#include "KalmanMPU6050.h"
#include "QMCMagCAN.h"
#include "QMC5883L.h"

TaskHandle_t ctid = 0;

/*
  Creates instance for I2C connection with passing the desired parameters.
  No action is done at the bus. The default address of the chip is 0x0D.
 */
Compass::Compass( const uint8_t addr, const uint8_t odr, const uint8_t range, const uint16_t osr , I2C_t *i2cBus  )
{
	ESP_LOGI(FNAME,"Compass() I2C addr=%02x", addr );
	if( addr == 0 ){

		sensor = new QMCMagCAN();
	}
	else{
		sensor = new QMC5883L( addr, odr, range, osr, i2cBus );  // tbd. base class and QMC5883CAN class
	}
	m_magn_heading = 0;
	m_gyro_fused_heading = 0;
	m_headingValid = false;
	_tick = 0;
	gyro_age = 0;
	_external_data = 0;
	_heading_average = -1000;
	calibrationRunning = false;
	_heading = 0;
	holddown = false;
	errors = 0;
	totalReadErrors = 0;
	bias = { 0,0,0 };
	scale = { 0,0,0 };
	age = 100;
}

Compass::~Compass()
{
}

void Compass::tick(){
	// ESP_LOGI( FNAME, "Compass::tick()");
	Deviation::tick();
	age++;
	_tick++;
	sensor->tick();
	gyro_age++;
}

esp_err_t Compass::selfTest(){
	if( sensor )
		return sensor->selfTest();
	return ESP_FAIL;
}

bool Compass::haveSensor() {
	if( sensor )
		return sensor->haveSensor();
	return false;
}

bool Compass::overflowFlag(){
	if( sensor )
		return sensor->overflowFlag();
	return false;
}

void Compass::setGyroHeading( float hd ){
	// ESP_LOGI( FNAME, "setGyroHeading(): %3.2f ext_d:%d", hd, _external_data );
	if( !_external_data ){
		m_gyro_fused_heading = hd;
		gyro_age = 0;
	}
}

float Compass::getGyroHeading( bool *ok, bool addDecl ){
	*ok = true;
	if( gyro_age > 10 )
		*ok = false;
	if( _external_data ){  // Simulation data
		*ok = true;
		_external_data--;  // age external data
	}
	// ESP_LOGI( FNAME, "Heading: %3.2f age: %d ext: %d", m_gyro_fused_heading, gyro_age, _external_data );
	return m_gyro_fused_heading;
}

/**
 * This is the compass task called periodically in a fixed time raster. It Reads
 * the current heading from the sensor and apply a low pass filter, deviation and more
 */
void Compass::compassT(void* arg ){
	while(1){
		TickType_t lastWakeTime = xTaskGetTickCount();
		if( compass != 0){
			if( !compass->calibrationIsRunning() ){
				compass->progress();
			}
			vTaskDelayUntil(&lastWakeTime, 50/portTICK_PERIOD_MS);
		}
	}
}

void Compass:: progress(){
	if( _external_data ){  // Simulation data
		_external_data--;  // age external data
	}
	if( compass_enable.get() ){
		if( !_external_data ){
			bool rok;
			float hd = heading( &rok );
			if( rok == false ){
				m_headingValid = false;
			}else{
				m_magn_heading = hd;
				m_headingValid = true;
			}
		}
		if( uxTaskGetStackHighWaterMark( ctid  ) < 256 )
			ESP_LOGW(FNAME,"Warning Compass task stack low: %d bytes", uxTaskGetStackHighWaterMark( ctid ) );
		float diff = Vector::angleDiffDeg( m_gyro_fused_heading, _heading_average );
		if( _heading_average == -1000 )
			_heading_average = m_gyro_fused_heading;
		else
			_heading_average += diff * (1/(20*compass_damping.get()));
		_heading_average = Vector::normalizeDeg( _heading_average );
		// ESP_LOGI(FNAME,"average hd=%.1f mag:%.1f gfh:%.1f", _heading_average, m_magn_heading, m_gyro_fused_heading );
	}
}

void Compass::begin(){
	Deviation::begin();
	loadCalibration();
	if( (compass_enable.get() == CS_I2C) || (compass_enable.get() == CS_I2C_NO_TILT) ){
		i2c_0.begin(GPIO_NUM_4, GPIO_NUM_18, GPIO_PULLUP_DISABLE, GPIO_PULLUP_DISABLE, (int)(compass_i2c_cl.get()*1000) );
		if( serial2_speed.get() )
			serial2_speed.set(0);  // switch off serial interface, we can do only alternatively
	}
	mag_hdm.set( -1 );
	mag_hdt.set( -1 );
}

void Compass::start(){
	xTaskCreatePinnedToCore(&compassT, "compassT", 2600, NULL, 12, &ctid, 0);
}

float Compass::filteredHeading( bool *okIn )
{
	*okIn = m_headingValid;
	return _heading_average;
}

float Compass::rawHeading( bool *okIn )
{
	*okIn = m_headingValid;
	return m_magn_heading;
}

float Compass::filteredTrueHeading( bool *okIn ){ // consider deviation table
	float deviation_cur = getDeviation(  _heading_average );
	float fth = Vector::normalizeDeg( _heading_average + deviation_cur );
	*okIn = m_headingValid;
	// ESP_LOGI(FNAME,"filteredTrueHeading dev=%.1f head=%.1f hddev=%.1f ok=%d", deviation_cur, _heading_average, fth, *okIn   );
	return fth;
}

// for simulation purposes
void Compass::setHeading( float h ) {
	m_gyro_fused_heading = h;
	m_magn_heading = h;
	m_headingValid=true;
	_external_data=100;
	// ESP_LOGI( FNAME, "NEW external heading %.1f", h );
};


/**
 * Calibrate compass by using the read x, y, z raw values. The calibration is
 * stopped by the reporter function which displays intermediate results of the
 * calibration action.
 */
bool Compass::calibrate( bool (*reporter)( t_magn_axes raw, t_float_axes scale, t_float_axes bias, bitfield_compass b, bool print ), bool only_show  )
{
	// reset all old calibration data
	ESP_LOGI( FNAME, "calibrate/show magnetic sensor, only_show=%d ", only_show );
	calibrationRunning = true;
	if( !only_show )
		resetCalibration();

	ESP_LOGI( FNAME, "calibrate min-max xyz");
	int i = 0;
	t_magn_axes raw;
	t_magn_axes min = { 20000,20000,20000 } ;
	t_magn_axes max = { 0,0,0 };
	t_bitfield_compass bits = { false, false, false, false, false, false };
	if( !only_show ){
		while( true )
		{
			i++;
			if( sensor->rawAxes( raw ) == false )
			{
				errors++;
				if( errors > 10 ){
					sensor->initialize();
					errors = 0;
				}
				continue;
			}
			/* Find max/min peak values */
			min.x = ( raw.x < min.x ) ? raw.x : min.x;
			min.y = ( raw.y < min.y ) ? raw.y : min.y;
			min.z = ( raw.z < min.z ) ? raw.z : min.z;
			max.x = ( raw.x > max.x ) ? raw.x : max.x;
			max.y = ( raw.y > max.y ) ? raw.y : max.z;
			max.z = ( raw.z > max.z ) ? raw.z : max.z;

			const int16_t minval = (32768/100)*1; // 1%
			if( abs(raw.x) < minval && abs(raw.y) < minval && raw.z > 0  )
				bits.zmax_green = true;
			if( abs(raw.x) < minval && abs(raw.y) < minval && raw.z < 0  )
				bits.zmin_green = true;
			if( abs(raw.x) < minval && abs(raw.z) < minval && raw.y < 0  )
				bits.ymin_green = true;
			if( abs(raw.x) < minval && abs(raw.z) < minval && raw.y > 0  )
				bits.ymax_green = true;
			if( abs(raw.y) < minval && abs(raw.z) < minval && raw.x < 0  )
				bits.xmin_green = true;
			if( abs(raw.y) < minval && abs(raw.z) < minval && raw.x > 0  )
				bits.xmax_green = true;

			if( i < 2 )
				continue;

			// Calculate hard iron correction
			// calculate average x, y, z magnetic bias.x in counts
			bias.x = ( (float)max.x + min.x ) / 2;
			bias.y = ( (float)max.y + min.y ) / 2;
			bias.z = ( (float)max.z + min.z ) / 2;

			// Calculate soft-iron scale factors
			// calculate average x, y, z axis max chord length in counts
			float xchord = ( (float)max.x - min.x ) / 2;
			float ychord = ( (float)max.y - min.y ) / 2;
			float zchord = ( (float)max.z - min.z ) / 2;
			float cord_avgerage = ( xchord + ychord + zchord ) / 3.;
			scale.x = cord_avgerage / xchord;
			scale.y = cord_avgerage / ychord;
			scale.z = cord_avgerage / zchord;

			if( !(i%4) )
			{
				// Send a calibration report to the subscriber every 500ms
				reporter( raw, scale, bias, bits, false );
			}
			if( ESPRotary::readSwitch() == true  )  // more responsive to query every loop
				break;
		}
	}else{
		ESP_LOGI( FNAME, "Show Calibration");
		t_bitfield_compass b = calibration_bits.get();
		reporter( raw, scale, bias, b, true );
		while( ESPRotary::readSwitch() == true  )
			delay(100);
	}
	if( !only_show ){
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
		calibration_bits.set( bits );

		ESP_LOGI( FNAME, "Compass: min.x=%d max.x=%d, min.y=%d max.y=%d, min.z=%d max.z=%d",
				min.x, max.x, min.y, max.y, min.z, max.z );

		ESP_LOGI( FNAME, "Compass hard-iron: bias.x=%.3f, bias.y=%.3f, bias.z=%.3f", bias.x, bias.y, bias.z );
		ESP_LOGI( FNAME, "Compass soft-iron: scale.x=%.3f, scale.y=%.3f, scale.z=%.3f",	scale.x, scale.y, scale.x );
		ESP_LOGI( FNAME, "calibration end" );
	}
	calibrationRunning = false;
	return true;
}

/**
 * Resets the whole compass calibration, also the saved configuration.
 */
void Compass::resetCalibration()
{
	bias = { 0,0,0 };
	scale = { 0,0,0 };

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
void Compass::saveCalibration()
{
	compass_x_bias.set( bias.x );
	compass_y_bias.set( bias.y );
	compass_z_bias.set( bias.z );
	compass_x_scale.set( scale.x );
	compass_y_scale.set( scale.y );
	compass_z_scale.set( scale.z );
	compass_calibrated.set( 1 );
}

/**
 * Loads a stored compass calibration. Returns true, if valid calibration
 * data could be loaded, otherwise false.
 */
bool Compass::loadCalibration()
{
	if( compass_calibrated.get() == 0 )
	{
		ESP_LOGI( FNAME, "Compass calibration is not valid" );
		// no stored calibration available
		return false;
	}

	bias.x = compass_x_bias.get();
	bias.y = compass_y_bias.get();
	bias.z = compass_z_bias.get();
	scale.x = compass_x_scale.get();
	scale.y = compass_y_scale.get();
	scale.z = compass_z_scale.get();

	ESP_LOGI( FNAME, "Read calibration: %f, %f, %f, %f, %f, %f, valid=%d",
			bias.x, bias.y, bias.z, scale.x, scale.y, scale.z, compass_calibrated.get() );

	return true;
}

float Compass::cur_heading( bool *ok ) {
	// ESP_LOGI( FNAME, "current heading %.1f, age:%d", _heading, age );
	*ok = true;
	if( age > 10 )
		*ok = false;
	return _heading;
};

/**
 * Reads the heading in degrees of 0...359. Ok is set to true,
 * if heading data is valid, otherwise it is set to false.
 */
float Compass::heading( bool *ok )
{
	if( calibrationRunning == true )
	{
		*ok = false;
		// ESP_LOGI(FNAME,"Calibration running, return 0");
		return 0.0;
	}
	// Check if calibration data are available
	if( compass_calibrated.get() == 0 )
	{
		// No calibration data available, return error
		*ok = false;
		// ESP_LOGI(FNAME,"Not calibrated" );
		return 0.0;
	}
	t_magn_axes raw;
	// ESP_LOGI(FNAME,"QMC5883L::heading() errors:%d, N:%d", errors, N );
	if( errors > 100 && errors % 100 )  // Holddown processing and throwing errors once sensor is gone
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
	bool state = sensor->rawAxes( raw );
	// ESP_LOGI(FNAME,"state %d  x:%d y:%d z:%d", state, raw.x, raw.y, raw.z );
	if( !state )
	{
		errors++;
		age++;
		totalReadErrors++;
		// ESP_LOGI(FNAME,"Magnetic sensor error Reads:%d, Total Errors:%d  Init: %d", N, totalReadErrors, errors );
		if( errors > 10 )
		{
			// ESP_LOGI(FNAME,"Magnetic sensor errors > 10: init mag sensor" );
			if( compass_enable.get() != CS_CAN ){
				if( sensor->initialize() != ESP_OK ) //  reinitialize once crashed, one retry
					sensor->initialize();
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
	age = 0;

	/* Apply tilt corrections to the measured values. Note, due to mounting of chip
	 * turned clockwise by 90 degrees the X-axis and the Y-axis are moved and
	 * have to be handled in this way.
	 */
	// ESP_LOGI( FNAME, "heading: X:%d Y:%d Z:%d xs:%f ys:%f zs:%f", raw.x, raw.y, raw.z, scale.x, scale.y, scale.z);

	double fy = (double) ((float( raw.x ) - bias.x) * scale.x);
	double fx = -(double) ((float( raw.y ) - bias.y) * scale.y);
	double fz = (double) ((float( raw.z ) - bias.z) * scale.z);

	double roll = filterRoll( IMU::getRollRad() );      // equal filter with equal delay to raw compass data for pitch and roll
	double pitch = filterPitch( IMU::getPitchRad() );

	double tcx = fx * cos( -pitch ) + fy * sin( -roll ) * sin( -pitch) - fz * cos( -roll) * sin( -pitch);
	double tcy = fy * cos( -roll) + fz * sin( -roll);
	if( isnan(tcx) || isnan(tcy) ){
		*ok = true;
		ESP_LOGI(FNAME,"gimbal lock, cannot determine heading");
		return _heading;  // deliver old value in case of gimbal lock
	}

	if( compass_enable.get() == CS_CAN || compass_enable.get() == CS_I2C ){
		_heading = -RAD_TO_DEG * atan2( tcy, tcx );
		// ESP_LOGI(FNAME,"tcy %03.2f tcx %03.2f  heading:%03.1f pi:%.1f ro:%.1f", tcy, tcx, _heading, pitch, roll );
	}
	else if ( compass_enable.get() == CS_I2C_NO_TILT )
		_heading = -RAD_TO_DEG * atan2( fy, fx );

	_heading = Vector::normalizeDeg( _heading );

	// ESP_LOGI(FNAME,"CR: %.1f TC:%.1f Roll:%0.1f Pitch:%.1f", heading_nt, _heading, R2D(roll*2)+180, R2D(pitch*2)+180  );
#if 0
	if( wind_logging.get() ){
		char log[120];
		sprintf( log, "$COMPASS;%d;%d;%d;%.1f;%.1f;%.1f;%d\n", raw.x, raw.y, raw.z, IMU::getPitch(), IMU::getRoll(),  _heading,  totalReadErrors );
		Router::sendXCV( log );
	}
#endif

	*ok = true;
	return _heading;
}

