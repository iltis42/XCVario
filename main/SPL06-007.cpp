#include "SPL06-007.h"
#include "logdef.h"

SPL06_007::SPL06_007( char _addr ){
	bus = 0;
	address = _addr;
	c00 = c10 = c01 = c11 = c20 = c21 = c30 = c0 = c1 = 0;
	_scale_factor_p = 0;
	_scale_factor_t = 0;
	errors = 0;
	_praw = 0;
	_traw = 0;
}

// Addr. 0x06 PM_RATE Bits 6-4:    110  - 64 measurements pr. sec.
//            PM_PRC  Bis  3-0:    0011 - 8 times oversampling

// Addr. 0x08 MEAS_CTRL Bits 2-0:  111  - Continuous pressure and temperature measurement

bool SPL06_007::begin() {
	// ---- Oversampling of >8x for temperature or pressuse requires FIFO operational mode which is not implemented ---
	// ---- Use rates of 8x or less until feature is implemented ---
	errors = 0;

	i2c_write_uint8( 0X06, 0x73);	// Pressure 7=128 samples per second & 3 = 8x oversampling
	i2c_write_uint8( 0X07, 0X83);	// Temperature 8x oversampling
	i2c_write_uint8( 0X08, 0B0111);	// continuous temp and pressure measurement
	i2c_write_uint8( 0X09, 0X00);	// FIFO Pressure measurement
	c00 = get_c00();
	if( errors )
		return false;
	c10 = get_c10();

	c01 = get_16bit( 0X18 );  // ok
	c11 = get_16bit( 0X1A );  //
	c20 = get_16bit( 0X1C );
	c21 = get_16bit( 0X1E );
	c30 = get_16bit( 0X20 );

	get_c0();
	get_c1();

	_scale_factor_p = get_scale_factor( 0x06 );
	_scale_factor_t = get_scale_factor( 0x07 );
	if( errors )
		return false;
	else
		return true;
}

bool SPL06_007::selfTest( float& p, float& t ){
	uint8_t rdata = 0xFF;
	esp_err_t err = bus->readByte(address, 0x0D, &rdata );  // ID
	if( err != ESP_OK ){
		ESP_LOGE(FNAME,"Error I2C read, status :%d", err );
		return false;
	}

	ESP_LOGI(FNAME,"SPL06_007 selftest, scan for I2C address %02x PASSED, Product ID: %d, Revision ID:%d", address, rdata>>4 , rdata&0x0F );
	bool ok;
	p = (float)get_pressure(ok);
	if( !ok ) {
		ESP_LOGI(FNAME,"SPL06_007 selftest failed, getPressure returnd false");
		return false;
	}
	t = (float)get_temp_c();
	ESP_LOGI(FNAME,"SPL06_007 selftest, p=%f t=%f", p, t );

	if( p < 1200 && p > 0  ) {
		ESP_LOGI(FNAME,"SPL06_007 selftest addr: %d PASSED, p=%f t=%f", address, p, t );
		return true;
	}
	else{
		ESP_LOGI(FNAME,"SPL06_007 selftest addr: %d FAILED, p=%f t=%f", address, p, t );
		return false;
	}
}

double SPL06_007::get_altitude(double pressure, double seaLevelhPa) {
	double altitude = 44330.0 * (1.0 - pow(pressure / seaLevelhPa, 0.19029495718));
	// ESP_LOGI(FNAME,"SPL06_007::get_altitude: %f, p: %f, qnh: %f",altitude, pressure, seaLevelhPa );
	return altitude;
}

double SPL06_007::get_traw_sc( bool &ok )
{
	get_traw( ok );
	return (double(_traw)/_scale_factor_t);
}

double SPL06_007::get_temp_c()
{
	bool ok;
	double traw_sc = get_traw_sc(ok);
	return (double(c0) * 0.5f) + (double(c1) * traw_sc);
}

double SPL06_007::get_temp_f()
{
	bool ok;
	double traw_sc = get_traw_sc(ok);
	return (((double(c0) * 0.5f) + (double(c1) * traw_sc)) * 9/5) + 32;
}

int32_t SPL06_007::get_traw( bool &ok )
{
	_traw = 0;
	uint8_t data[3];
	if ( !i2c_read_bytes( 0X03, 3, data ) )   // use correct method to read 3 bytes of volatile data
		ok = false;
	_traw = (data[0] << 8) | data[1];
	_traw = (_traw << 8) | data[2];

	if(_traw & (1 << 23))
		_traw = _traw | 0XFF000000; // Set left bits to one for 2's complement conversion of negitive number
	ok = true;
	return _traw;
}

double SPL06_007::get_praw_sc( bool &ok )
{
	get_praw( ok );
	return (double(_praw)/_scale_factor_p);
}



double SPL06_007::get_pcomp(bool &ok)
{
	bool ok_t, ok_p;
	double traw_sc = get_traw_sc( ok_t );
	double praw_sc = get_praw_sc( ok_p );
	if( !ok_t || !ok_t ){
		ok = false;
		ESP_LOGW(FNAME,"T %d or P %d reading returned false", ok_t, ok_p );
		return 0;
	}
	uint8_t status = i2c_read_uint8( 0x08 );
	if( !(status & 0x40) ){
		ok = false;
		ESP_LOGW(FNAME,"Sensor ready bit not set %02x", status );
		return 0;
	}
	double p = double(c00) + praw_sc * (double(c10) + praw_sc * (double(c20) + praw_sc * double(c30))) + traw_sc * double(c01) + traw_sc * praw_sc * ( double(c11) + praw_sc * double(c21));
	// if( address == 0x76 ) {
	// 	float t = (double(c0) * 0.5f) + (double(c1) * traw_sc);
	// 	ESP_LOGI(FNAME,"P:%06x,%d  T:%06x PC:%f T:%f I2C E:%d",_praw, _praw, _traw, p/100, t , errors );
	// }
	return p;
}

double SPL06_007::readAltitude( double qnh, bool &ok ) {

	double p =  get_pressure( ok );
	return get_altitude( p, qnh );
};

double SPL06_007::get_pressure( bool &ok )
{
	double pcomp = get_pcomp( ok );
	return pcomp / 100; // convert to mb
}

double SPL06_007::get_scale_factor( int reg )
{
	double k;
	uint8_t tmp_Byte;
	tmp_Byte = i2c_read_uint8( reg );   // MSB
	tmp_Byte = tmp_Byte & 0B00000111;   // Focus on 2-0 oversampling rate

	switch (tmp_Byte) // oversampling rate
	{
	case 0B000:
		k = 524288.0;
		break;

	case 0B001:
		k = 1572864.0;
		break;

	case 0B010:
		k = 3670016.0;
		break;

	case 0B011:
		k = 7864320.0;
		break;

	case 0B100:
		k = 253952.0;
		break;

	case 0B101:
		k = 516096.0;
		break;

	case 0B110:
		k = 1040384.0;
		break;

	case 0B111:
		k = 2088960.0;
		break;
	}
	return k;
}

int32_t SPL06_007::get_praw( bool &ok )
{
	_praw = 0;
	uint8_t data[3];
	if( !i2c_read_bytes( 0X00, 3, data ) )
		ok = false;
	_praw = (data[0] << 8) | data[1];
	_praw = (_praw << 8) | data[2];
	if(_praw & (1 << 23))
		_praw = _praw | 0XFF000000; // Set left bits to one for 2's complement conversion of negitive number
	ok = true;
	return _praw;
}

int16_t SPL06_007::get_c0()
{
	uint8_t tmp_MSB,tmp_LSB;
	tmp_MSB = i2c_read_uint8( 0X10);
	tmp_LSB = i2c_read_uint8( 0X11);
	tmp_LSB = tmp_LSB >> 4;
	c0 = (tmp_MSB << 4) | tmp_LSB;

	if(c0 & (1 << 11)) // Check for 2's complement negative number
		c0 = c0 | 0XF000; // Set left bits to one for 2's complement conversion of negitive number
	return c0;
}


int16_t SPL06_007::get_c1()
{
	uint8_t tmp_MSB,tmp_LSB;
	tmp_MSB = i2c_read_uint8( 0X11);
	tmp_LSB = i2c_read_uint8( 0X12);
	tmp_MSB = tmp_MSB & 0XF;

	c1 = (tmp_MSB << 8) | tmp_LSB;

	if(c1 & (1 << 11)) // Check for 2's complement negative number
		c1 = c1 | 0XF000; // Set left bits to one for 2's complement conversion of negitive number
	return c1;
}

int32_t SPL06_007::get_c00()
{
	int32_t tmp;
	uint8_t tmp_MSB,tmp_LSB,tmp_XLSB;

	tmp_MSB = i2c_read_uint8( 0X13);
	tmp_LSB = i2c_read_uint8( 0X14);
	tmp_XLSB = i2c_read_uint8( 0X15);

	tmp_XLSB = tmp_XLSB >> 4;
	tmp = (tmp_MSB << 8) | tmp_LSB;
	tmp = (tmp << 4) | tmp_XLSB;
	tmp = (uint32_t)tmp_MSB << 12 | (uint32_t)tmp_LSB << 4 | (uint32_t)tmp_XLSB >> 4;
	if(tmp & (1 << 19))
		tmp = tmp | 0XFFF00000; // Set left bits to one for 2's complement conversion of negitive number
	return tmp;
}

int32_t SPL06_007::get_c10()
{
	int32_t tmp;
	uint8_t tmp_MSB,tmp_LSB,tmp_XLSB;

	tmp_MSB = i2c_read_uint8( 0X15); // 4 bits
	tmp_LSB = i2c_read_uint8( 0X16); // 8 bits
	tmp_XLSB = i2c_read_uint8( 0X17); // 8 bits
	tmp_MSB = tmp_MSB & 0b00001111;

	tmp = (tmp_MSB << 4) | tmp_LSB;
	tmp = (tmp << 8) | tmp_XLSB;
	tmp = (uint32_t)tmp_MSB << 16 | (uint32_t)tmp_LSB << 8 | (uint32_t)tmp_XLSB;

	if(tmp & (1 << 19))
		tmp = tmp | 0XFFF00000; // Set left bits to one for 2's complement conversion of negitive number
	return tmp;
}

int16_t SPL06_007::get_16bit( uint8_t addr )
{
	uint8_t tmp_MSB,tmp_LSB;
	tmp_MSB = i2c_read_uint8( addr );
	tmp_LSB = i2c_read_uint8( addr+1 );
	return (int16_t)(tmp_LSB | (tmp_MSB<<8));
}

void SPL06_007::i2c_write_uint8( uint8_t eeaddress, uint8_t data )
{
	esp_err_t err = bus->writeByte(address, eeaddress, data );
	if( err != ESP_OK ){
		ESP_LOGE(FNAME,"Error I2C write, status :%d", err );
		errors++;
	}

}

bool SPL06_007::i2c_read_bytes( uint8_t eeaddress, int num, uint8_t *data )
{
	esp_err_t err = bus->readBytes(address, eeaddress, num, data );
	if( err != ESP_OK ){
		ESP_LOGE(FNAME,"Error I2C bytes read, status :%d", err );
		errors++;
		return false;
	}
	return true;
}

uint8_t SPL06_007::i2c_read_uint8( uint8_t eeaddress )
{
	uint8_t rdata = 0xFF;
	esp_err_t err = bus->readByte(address, eeaddress, &rdata );
	if( err != ESP_OK ){
		ESP_LOGE(FNAME,"Error I2C read, status :%d", err );
		errors++;
	}
	return rdata;
}
