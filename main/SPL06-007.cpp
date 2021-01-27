#include "SPL06-007.h"
#include "logdef.h"

SPL06_007::SPL06_007(){
	bus = 0;
	address = 0;
}

bool SPL06_007::begin(char slave_adr) {
	address = slave_adr;
	// ---- Oversampling of >8x for temperature or pressuse requires FIFO operational mode which is not implemented ---
	// ---- Use rates of 8x or less until feature is implemented ---
	i2c_write_uint8( 0X06, 0x03);	// Pressure 8x oversampling
	i2c_write_uint8( 0X07, 0X83);	// Temperature 8x oversampling
	i2c_write_uint8( 0X08, 0B0111);	// continuous temp and pressure measurement
	i2c_write_uint8( 0X09, 0X00);	// FIFO Pressure measurement
	return true;
}

bool SPL06_007::selfTest( int& adval ){
	uint8_t rdata = 0xFF;
	esp_err_t err = bus->readByte(address, 0x0D, &rdata );  // ID
	if( err != ESP_OK )
		ESP_LOGE(FNAME,"Error I2C read, status :%d", err );

	ESP_LOGI(FNAME,"SPL06_007 selftest, scan for I2C address %02x PASSED, Product ID: %d, Revision ID:%d", address, rdata>>4 , rdata&0x0F );
	/*
	measure();
	adval = P_dat;
	 */
	if( err != ESP_OK )
		return false;
	else
		return true;
}

double SPL06_007::get_altitude(double pressure, double seaLevelhPa) {
	double altitude;
	altitude = 44330 * (1.0 - pow(pressure / seaLevelhPa, 0.1903));
	ESP_LOGI(FNAME,"SPL06_007::get_altitude: %f", altitude );
	return altitude;
}

double SPL06_007::get_traw_sc()
{
	int32_t traw = get_traw();
	return (double(traw)/get_scale_factor( 0x07 ));
}

double SPL06_007::get_temp_c()
{
	int16_t c0,c1;
	c0 = get_c0();
	c1 = get_c1();
	double traw_sc = get_traw_sc();
	return (double(c0) * 0.5f) + (double(c1) * traw_sc);
}

double SPL06_007::get_temp_f()
{
	int16_t c0,c1;
	c0 = get_c0();
	c1 = get_c1();
	double traw_sc = get_traw_sc();
	return (((double(c0) * 0.5f) + (double(c1) * traw_sc)) * 9/5) + 32;
}

int32_t SPL06_007::get_traw()
{
	int32_t tmp;
	uint8_t tmp_MSB,tmp_LSB,tmp_XLSB;
	tmp_MSB = i2c_read_uint8( 0X03); // MSB
	tmp_LSB = i2c_read_uint8( 0X04); // LSB
	tmp_XLSB = i2c_read_uint8( 0X05); // XLSB

	tmp = (tmp_MSB << 8) | tmp_LSB;
	tmp = (tmp << 8) | tmp_XLSB;

	if(tmp & (1 << 23))
		tmp = tmp | 0XFF000000; // Set left bits to one for 2's complement conversion of negitive number
	return tmp;
}

double SPL06_007::get_praw_sc()
{
	int32_t praw = get_praw();
	return (double(praw)/get_scale_factor( 0x06 ));
}

double SPL06_007::get_pcomp()
{
	int32_t c00,c10;
	int16_t c01,c11,c20,c21,c30;
	c00 = get_c00();
	c10 = get_c10();
	c01 = get_c01();
	c11 = get_c11();
	c20 = get_c20();
	c21 = get_c21();
	c30 = get_c30();
	double traw_sc = get_traw_sc();
	double praw_sc = get_praw_sc();
	return double(c00) + praw_sc * (double(c10) + praw_sc * (double(c20) + praw_sc * double(c30))) + traw_sc * double(c01) + traw_sc * praw_sc * ( double(c11) + praw_sc * double(c21));
}

double SPL06_007::get_pressure()
{
	double pcomp = get_pcomp();
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

int32_t SPL06_007::get_praw()
{
	int32_t tmp;
	uint8_t tmp_MSB,tmp_LSB,tmp_XLSB;
	tmp_MSB = i2c_read_uint8( 0X00); // MSB
	tmp_LSB = i2c_read_uint8( 0X01); // LSB
	tmp_XLSB = i2c_read_uint8( 0X02); // XLSB

	tmp = (tmp_MSB << 8) | tmp_LSB;
	tmp = (tmp << 8) | tmp_XLSB;
	if(tmp & (1 << 23))
		tmp = tmp | 0XFF000000; // Set left bits to one for 2's complement conversion of negitive number
	return tmp;
}

int16_t SPL06_007::get_c0()
{
	int16_t tmp;
	uint8_t tmp_MSB,tmp_LSB;

	tmp_MSB = i2c_read_uint8( 0X10);
	tmp_LSB = i2c_read_uint8( 0X11);
	tmp_LSB = tmp_LSB >> 4;
	tmp = (tmp_MSB << 4) | tmp_LSB;

	if(tmp & (1 << 11)) // Check for 2's complement negative number
		tmp = tmp | 0XF000; // Set left bits to one for 2's complement conversion of negitive number
	return tmp;
}


int16_t SPL06_007::get_c1()
{
	int16_t tmp;
	uint8_t tmp_MSB,tmp_LSB;

	tmp_MSB = i2c_read_uint8( 0X11);
	tmp_LSB = i2c_read_uint8( 0X12);
	tmp_MSB = tmp_MSB & 0XF;

	tmp = (tmp_MSB << 8) | tmp_LSB;

	if(tmp & (1 << 11)) // Check for 2's complement negative number
		tmp = tmp | 0XF000; // Set left bits to one for 2's complement conversion of negitive number
	return tmp;
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

int16_t SPL06_007::get_c01()
{
	int16_t tmp;
	uint8_t tmp_MSB,tmp_LSB;

	tmp_MSB = i2c_read_uint8( 0X18);
	tmp_LSB = i2c_read_uint8( 0X19);

	tmp = (tmp_MSB << 8) | tmp_LSB;
	return tmp;
}

int16_t SPL06_007::get_c11()
{
	int16_t tmp;
	uint8_t tmp_MSB,tmp_LSB;

	tmp_MSB = i2c_read_uint8( 0X1A);
	tmp_LSB = i2c_read_uint8( 0X1B);

	tmp = (tmp_MSB << 8) | tmp_LSB;
	return tmp;
}

int16_t SPL06_007::get_c20()
{
	int16_t tmp;
	uint8_t tmp_MSB,tmp_LSB;

	tmp_MSB = i2c_read_uint8( 0X1C);
	tmp_LSB = i2c_read_uint8( 0X1D);

	tmp = (tmp_MSB << 8) | tmp_LSB;
	return tmp;
}

int16_t SPL06_007::get_c21()
{
	int16_t tmp;
	uint8_t tmp_MSB,tmp_LSB;

	tmp_MSB = i2c_read_uint8( 0X1E);
	tmp_LSB = i2c_read_uint8( 0X1F);

	tmp = (tmp_MSB << 8) | tmp_LSB;
	return tmp;
}

int16_t SPL06_007::get_c30()
{
	int16_t tmp;
	uint8_t tmp_MSB,tmp_LSB;

	tmp_MSB = i2c_read_uint8( 0X20);
	tmp_LSB = i2c_read_uint8( 0X21);

	tmp = (tmp_MSB << 8) | tmp_LSB;
	return tmp;
}

void SPL06_007::i2c_write_uint8( uint8_t eeaddress, uint8_t data )
{
	esp_err_t err = bus->writeByte(address, eeaddress, data );
	if( err != ESP_OK )
		ESP_LOGE(FNAME,"Error I2C write, status :%d", err );

}

uint8_t SPL06_007::i2c_read_uint8( uint8_t eeaddress )
{
	uint8_t rdata = 0xFF;
	esp_err_t err = bus->readByte(address, eeaddress, &rdata );
	if( err != ESP_OK )
		ESP_LOGE(FNAME,"Error I2C read, status :%d", err );
	return rdata;
}
