#ifndef _SPL06_007_
#define _SPL06_007_

#include "Arduino.h"
#include <inttypes.h>
#include <stdio.h>
#include "I2C.h"
#include <string.h>
#include "I2Cbus.hpp"

uint8_t SPL06_007_CHIP_ADDRESS = 0x76;

class SPL06_007 {

public:
	SPL06_007();
	bool  begin( char slave_adr = SPL06_007_CHIP_ADDRESS );
	bool  selfTest( int& adval );

	void  setBus( I2C_t *theBus ) {  bus = theBus; };
	double get_altitude(double pressure, double seaLevelhPa);	// get altitude in meters
	double get_altitude_f(double pressure, double seaLevelhPa);	// get altitude in feet

	int32_t get_traw();
	double get_traw_sc();
	double get_temp_c();
	double get_temp_f();
	double get_temperature_scale_factor();

	int32_t get_praw();
	double get_praw_sc();
	double get_pcomp();
	double get_pressure_scale_factor();
	double get_pressure();

private:
	inline uint8_t get_spl_id(){ return i2c_read_uint8( 0x0D ); }		    // Get ID Register 		0x0D
	inline uint8_t get_spl_prs_cfg(){ return i2c_read_uint8( 0x06 ); };	// Get PRS_CFG Register	0x06
	inline uint8_t get_spl_tmp_cfg(){ return i2c_read_uint8( 0x07 ); };	// Get TMP_CFG Register	0x07
	inline uint8_t get_spl_meas_cfg(){ return i2c_read_uint8( 0x08 ); };	// Get MEAS_CFG Register	0x08
	inline uint8_t get_spl_cfg_reg(){ return i2c_read_uint8( 0x09 ); };	// Get CFG_REG Register	0x09
	inline uint8_t get_spl_int_sts(){ return i2c_read_uint8( 0x0A ); };	// Get INT_STS Register	0x0A
	inline uint8_t get_spl_fifo_sts(){ return i2c_read_uint8( 0x0B ); };	// Get FIFO_STS Register	0x0B

	int16_t get_c0();
	int16_t get_c1();
	int32_t get_c00();
	int32_t get_c10();
	int16_t get_c01();
	int16_t get_c11();
	int16_t get_c20();
	int16_t get_c21();
	int16_t get_c30();

	void i2c_write_uint8( uint8_t eeaddress, uint8_t data );
	uint8_t i2c_read_uint8( uint8_t eeaddress );

	I2C_t *bus;
	char   address;
};

#endif

