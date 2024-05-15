#ifndef _SPL06_007_
#define _SPL06_007_

#include "Arduino.h"
#include <inttypes.h>
#include <stdio.h>
#include "I2C.h"
#include <string.h>
#include "I2Cbus.hpp"
#include "PressureSensor.h"




class SPL06_007: public PressureSensor {
public:
	SPL06_007( char slave_adr );
	virtual ~SPL06_007() {};
	bool  begin();
	bool  selfTest( float &t, float &p );
	bool  setBus( I2C_t *theBus ) {  bus = theBus; return true; };
	bool  setSPIBus(gpio_num_t _sclk, gpio_num_t _mosi, gpio_num_t _miso, gpio_num_t _cs, uint32_t _freq ) { return true; };
	double get_altitude(double pressure, double seaLevelhPa);	// get altitude in meters
	inline double calcAltitudeSTD( double p ) { return get_altitude( p, 1013.25 ); };
	inline double calcAltitude( double sl, double p ) { return get_altitude( p, sl ); };
	double readAltitude( double qnh, bool &ok );
	double get_temp_c( bool &ok );
	double get_temp_f();
	double readTemperature( bool& success );

	double get_pcomp( bool &ok );
	double get_pressure(bool &ok);
	inline double readPressure(bool &ok){ return get_pressure(ok); };

private:
	int32_t get_praw( bool &ok );
	double get_praw_sc( bool &ok );

	int32_t get_traw( bool &ok );
	double get_traw_sc( bool &ok );

	double get_scale_factor( int reg );

	inline uint8_t get_spl_id(){ return i2c_read_uint8( 0x0D ); }		    // Get ID Register 		0x0D
	inline uint8_t get_spl_prs_cfg(){ return i2c_read_uint8( 0x06 ); };	// Get PRS_CFG Register	0x06
	inline uint8_t get_spl_tmp_cfg(){ return i2c_read_uint8( 0x07 ); };	// Get TMP_CFG Register	0x07
	inline uint8_t get_spl_meas_cfg(){ return i2c_read_uint8( 0x08 ); };	// Get MEAS_CFG Register	0x08
	inline uint8_t get_spl_cfg_reg(){ return i2c_read_uint8( 0x09 ); };	// Get CFG_REG Register	0x09
	inline uint8_t get_spl_int_sts(){ return i2c_read_uint8( 0x0A ); };	// Get INT_STS Register	0x0A
	inline uint8_t get_spl_fifo_sts(){ return i2c_read_uint8( 0x0B ); };	// Get FIFO_STS Register	0x0B


	int16_t get_16bit( uint8_t addr );
	int16_t get_c0();
	int16_t get_c1();
	int32_t get_c00();
	int32_t get_c10();


	int32_t c00,c10;
	int16_t c0,c1;
	int16_t c01,c11,c20,c21,c30;

	void i2c_write_uint8( uint8_t eeaddress, uint8_t data );
	uint8_t i2c_read_uint8( uint8_t eeaddress );
	bool i2c_read_bytes( uint8_t eeaddress, int num, uint8_t *data );

	I2C_t *bus;
	char   address;
	double _scale_factor_p;
	double _scale_factor_t;
	int    errors;
	int32_t _praw;
	int32_t last_praw;
	int32_t _traw;
	int32_t last_traw;
	uint32_t tick;
	double last_p;
};

#endif

