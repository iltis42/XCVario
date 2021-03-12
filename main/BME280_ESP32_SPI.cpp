/*
  BME280_ESP32_SPI.cpp Bosch BME280 for ESP32

 */

#include "freertos/FreeRTOS.h"
#include <freertos/task.h>
#include <string.h>
#include <cstdint>
#include <cstdio>
#include <sdkconfig.h>
#include <math.h>
#include "BME280_ESP32_SPI.h"
#include <Arduino.h>
#include <SPI.h>
#include <logdef.h>


extern xSemaphoreHandle spiMutex;

bool BME280_ESP32_SPI::setSPIBus(gpio_num_t sclk, gpio_num_t mosi, gpio_num_t miso, gpio_num_t cs, uint32_t freq)
{
	_sclk = sclk;
	_mosi = mosi;
	_miso = miso;
	_cs =   cs;
	_freq = freq;
	return true;
}

BME280_ESP32_SPI::BME280_ESP32_SPI()
{
	_t_fine = 0;
	_dig_T1 = 0;
	_dig_T2 = 0;
	_dig_T3 = 0;

	_dig_P1 = 0;
	_dig_P2 = 0;
	_dig_P3 = 0;
	_dig_P4 = 0;
	_dig_P5 = 0;
	_dig_P6 = 0;
	_dig_P7 = 0;
	_dig_P8 = 0;
	_dig_P9 = 0;

	_dig_H1 = 0;
	_dig_H2 = 0;
	_dig_H3 = 0;
	_dig_H4 = 0;
	_dig_H5 = 0;
	_dig_H6 = 0;
	exponential_average = 0;
	init_err = false;
	_avg_alt = 0;
	_avg_alt_std = 0;
	_freq = 0;
	_cs = 0;
	_miso = GPIO_NUM_MAX;
	_mosi = GPIO_NUM_MAX;
	_sclk = GPIO_NUM_MAX;
}

#define c_sb      0   //stanby 0: 0,5 mS 1: 62,5 mS 2: 125 mS
#define c_filter  0   //filter O = off
#define c_osrs_t  5   //OverSampling Temperature
#define c_osrs_p  5   //OverSampling Pressure (5:x16 4:x8, 3:x4 2:x2 )
#define c_osrs_h  0   //OverSampling Humidity x4
#define c_Mode    3   //Normal mode

//****************BME280_ESP32_SPI*************************************************
bool BME280_ESP32_SPI::begin(){
	pinMode(_cs, GPIO_MODE_OUTPUT);
	digitalWrite(_cs, HIGH);
	spis = SPISettings( _freq, SPI_MSBFIRST, SPI_MODE3 );
	// SPI.begin( _sclk, _miso, _mosi, _cs );
	ESP_LOGI(FNAME,"freq=%d", _freq );
	SPI.setFrequency(_freq);
	SPI.setHwCs( false );

	uint8_t spi3or4 = 0; //SPI 3wire or 4wire, 0=4wire, 1=3wire
	uint8_t ctrl_meas = (c_osrs_t << 5) | (c_osrs_p << 2) | c_Mode;
	uint8_t config    = (c_sb << 5) | (c_filter << 2) | spi3or4;
	uint8_t ctrl_hum  = c_osrs_h;

	WriteRegister(0xE0, 0xB6); //reset device
	int id = readID();
	int count = 0;
	while( (id != 0x58) && (count < 20) ) {
		id = readID();
		delay( 20 );
		count++;
	}
	if( count == 500 ) {
		ESP_LOGE(FNAME,"Error init BMP280 CS=%d", _cs );
		init_err = true;
	}
	else
		ESP_LOGI(FNAME,"BMP280 ID = %02x", id );

	WriteRegister(0xF2, ctrl_hum);
	WriteRegister(0xF4, ctrl_meas);
	WriteRegister(0xF5, config);

	delay( 20 );
	readCalibration();
	if( _dig_T2 == 0 && _dig_T2 == 0 ) {
		delay( 20 );
		readCalibration();
		ESP_LOGI(FNAME,"BMP280 Calibration Data read retry CS: %d ", _cs);
	}
	if( _dig_T2 == 0 && _dig_T2 == 0 ) {
		ESP_LOGE(FNAME,"BMP280 Calibration Data Error  CS: %d !", _cs);
		init_err = true;
	}
	return init_err;
}

//***************BME280 ****************************
void BME280_ESP32_SPI::WriteRegister(uint8_t reg_address, uint8_t data) {
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	SPI.beginTransaction( spis );
	digitalWrite(_cs, LOW);
	SPI.transfer( reg_address & 0x7F); // write, bit 7 low
	SPI.transfer( data);
	digitalWrite(_cs, HIGH);
	SPI.endTransaction();
	xSemaphoreGive(spiMutex);
}

//*******************************************************
void BME280_ESP32_SPI::readCalibration(void) {
	// ESP_LOGI(FNAME,"BME280_ESP32_SPI::readCalibration");
	_dig_T1 = read16bit(0x88);

	_dig_T2 = (int16_t)read16bit(0x8A);
	_dig_T3 = (int16_t)read16bit(0x8C);

	ESP_LOGI(FNAME,"BME280_ESP32_SPI::readCalibration  CS %d  T1 %d T2 %02x T3 %02x", _cs, _dig_T1, _dig_T2, _dig_T3);

	_dig_P1 = read16bit(0x8E);
	_dig_P2 = (int16_t)read16bit(0x90);
	_dig_P3 = (int16_t)read16bit(0x92);
	_dig_P4 = (int16_t)read16bit(0x94);
	_dig_P5 = (int16_t)read16bit(0x96);
	_dig_P6 = (int16_t)read16bit(0x98);
	_dig_P7 = (int16_t)read16bit(0x9A);
	_dig_P8 = (int16_t)read16bit(0x9C);
	_dig_P9 = (int16_t)read16bit(0x9E);

	/*
	_dig_H2 = (int16_t)read16bit(0xE1);
	_dig_H1 = read8bit(0xA1);
	_dig_H3 = read8bit(0xE3);
	_dig_H4 = (int16_t)((read8bit(0xE4) << 4) | (read8bit(0xE5) & 0x0F));
	_dig_H5 = (int16_t)((read8bit(0xE6) << 4) | (read8bit(0xE5) >> 4));
	_dig_H6 = (int8_t)read8bit(0xE7);
	*/
}

//***************BME280 ****************************
double BME280_ESP32_SPI::readTemperature( bool& success ){
	// ESP_LOGI(FNAME, "++BPM280 read Temp cs:%d ", _cs);
	uint8_t tx[4];
	uint8_t rx[4];
	memset(tx, 0, 4);
    tx[0] = 0xFA;
    xSemaphoreTake(spiMutex,portMAX_DELAY );
    SPI.beginTransaction( spis );
    digitalWrite(_cs, LOW);
    SPI.transfer( tx[0] );
	tx[0] = 0;
	SPI.transferBytes( tx, rx, 3 );
	// ESP_LOGI(FNAME, "read Temp rx length %d bytes", l/8);
	digitalWrite(_cs, HIGH);
	SPI.endTransaction();
	xSemaphoreGive(spiMutex);

	// for(uint8_t i=0; i<l/8; i++)
	//  	ESP_LOGI(FNAME, "%02x ", rx[i]);
	if ( rx[0] == 0 && rx[1] == 0 && rx[3] == 0 )
		success = false;
	else
		success = true;
	uint32_t adc_T = (rx[0] << 12) | (rx[1] << 4) | (rx[2] >> 4); //0xFA, msb+lsb+xlsb=19bit

	// ESP_LOGI(FNAME, "--BMP280 raw adc_T=%d  CS=%d", adc_T, _cs );
	double t=compensate_T((int32_t)adc_T) / 100.0;
	// ESP_LOGI(FNAME, "--BMP280 read Temp=%0.1f  CS=%d", t, _cs );
	// ESP_LOGI(FNAME,"   Calibration  CS %d  T1 %d T2 %02x T3 %02x", _cs, _dig_T1, _dig_T2, _dig_T3);

	return t;
}

//***************BME280 ****************************
double BME280_ESP32_SPI::readPressure(bool &ok){
	if( init_err ){
		ok=false;
		return 0.0;
	}
	// ESP_LOGI(FNAME,"++BMP280 readPressure cs:%d", _cs);
	bool success;
	readTemperature( success );
	int loop = 0;
	while( !success && loop < 100) {  // workaround as first read after others access SPI reads zero
		// delay(1);
		delayMicroseconds(100);
		readTemperature( success );
		loop++;
	}
	if( loop == 100 )
		ESP_LOGE(FNAME,"Error reading temp BMP280 CS: %d !", _cs );
	uint8_t tx[4];
	uint8_t rx[4];
	memset(tx, 0, 4);
    tx[0] = 0xF7;  // 0xF7 puressure msb read, bit 7 high

    xSemaphoreTake(spiMutex,portMAX_DELAY );
    SPI.beginTransaction( spis );
    digitalWrite(_cs, LOW);
	SPI.transfer( tx[0] );
	tx[0] = 0;
	SPI.transferBytes( tx, rx, 3 );
	// ESP_LOGI(FNAME, "read P rx length %d", l);
	// for(uint8_t i=0; i<3; i++)
	//		ESP_LOGI(FNAME, "%02x", rx[i]);
	digitalWrite(_cs, HIGH);
	SPI.endTransaction();
	xSemaphoreGive(spiMutex);
	uint32_t adc_P = (rx[0] << 12) | (rx[1] << 4) | (rx[2] >> 4); //0xF7, msb+lsb+xlsb=19bit
    // ESP_LOGI(FNAME,"--BMP280 readPressure");
	return compensate_P((int32_t)adc_P) / 100.0;
}

//***************BME280****************************
double BME280_ESP32_SPI::readHumidity(){
	// ESP_LOGI(FNAME,"++BMP280 readHumidity");
	uint32_t data[2];
	bool success;
	readTemperature( success );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	SPI.beginTransaction( spis );
	digitalWrite(_cs, LOW);
	SPI.transfer( 0xFD | 0x80); //0xFD Humidity msb read, bit 7 high
	data[0] = SPI.transfer( 0x00 );
	data[1] = SPI.transfer( 0x00 );
	digitalWrite(_cs, HIGH);
	SPI.endTransaction();
	xSemaphoreGive(spiMutex);

	uint32_t adc_H = (data[0] << 8) | data[1];  //0xFD, msb+lsb=19bit(16:0)
	return compensate_H((int32_t)adc_H) / 1024.0;
}

//*******************************************
int32_t BME280_ESP32_SPI::compensate_T(int32_t adc_T) {
	int32_t var1, var2, T;

	var1 = ((((adc_T >> 3) - ((int32_t)_dig_T1<<1))) * ((int32_t)_dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)_dig_T1)) * ((adc_T>>4) - ((int32_t)_dig_T1))) >> 12) * ((int32_t)_dig_T3)) >> 14;

	_t_fine = var1 + var2;
	T = (_t_fine * 5 + 128) >> 8;
	return T;
}

//*******************************************
uint32_t BME280_ESP32_SPI::compensate_P(int32_t adc_P) {

	int32_t var1, var2;
	uint32_t P;

	var1 = (((int32_t)_t_fine)>>1) - (int32_t)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11) * ((int32_t)_dig_P6);
	var2 = var2 + ((var1*((int32_t)_dig_P5))<<1);
	var2 = (var2>>2)+(((int32_t)_dig_P4)<<16);
	var1 = (((_dig_P3 * (((var1>>2)*(var1>>2)) >> 13)) >>3) + ((((int32_t)_dig_P2) * var1)>>1))>>18;
	var1 = ((((32768+var1))*((int32_t)_dig_P1))>>15);

	if (var1 == 0) {
		return 0;
	}

	P = (((uint32_t)(((int32_t)1048576)-adc_P)-(var2>>12)))*3125;

	if(P<0x80000000) {
		P = (P << 1) / ((uint32_t) var1);
	}else{
		P = (P / (uint32_t)var1) * 2;
	}
    // ESP_LOGI(FNAME,"compensate P T=%0.1f  adcP=%0.1f", (float)((_t_fine * 5 + 128) >> 8), (float)(P));

	var1 = (((int32_t)_dig_P9) * ((int32_t)(((P>>3) * (P>>3))>>13)))>>12;
	var2 = (((int32_t)(P>>2)) * ((int32_t)_dig_P8))>>13;
	P = (uint32_t)((int32_t)P + ((var1 + var2 + _dig_P7) >> 4));

	return P;
}
//*******************************************
uint32_t BME280_ESP32_SPI::compensate_H(int32_t adc_H) {
	int32_t v_x1;

	v_x1 = (_t_fine - ((int32_t)76800));
	v_x1 = (((((adc_H << 14) -(((int32_t)_dig_H4) << 20) - (((int32_t)_dig_H5) * v_x1)) +
			((int32_t)16384)) >> 15) * (((((((v_x1 * ((int32_t)_dig_H6)) >> 10) *
					(((v_x1 * ((int32_t)_dig_H3)) >> 11) + ((int32_t) 32768))) >> 10) + ((int32_t)2097152)) *
					((int32_t) _dig_H2) + 8192) >> 14));
	v_x1 = (v_x1 - (((((v_x1 >> 15) * (v_x1 >> 15)) >> 7) * ((int32_t)_dig_H1)) >> 4));
	v_x1 = (v_x1 < 0 ? 0 : v_x1);
	v_x1 = (v_x1 > 419430400 ? 419430400 : v_x1);

	return (uint32_t)(v_x1 >> 12);
}

//*******************************************************************
double BME280_ESP32_SPI::readAltitude(double SeaLevel_Pres, bool &ok ) {
	if( init_err ) {
		ok = false;
		return 0.0;
	}
	// ESP_LOGI(FNAME,"++BMP280 readAltitude QNH=%0.1f CS:%d", SeaLevel_Pres, _cs);
	double pressure = readPressure(ok);
	// ESP_LOGI(FNAME,"press=%0.1f", pressure);
	double altitude = calcAltitude( SeaLevel_Pres, pressure );
	// ESP_LOGI(FNAME,"--BME280 readAltitude  qnh: %0.1f p=%0.1f alt=%0.1f", SeaLevel_Pres, pressure, altitude);
	return altitude;
}

double BME280_ESP32_SPI::calcAVGAltitude(double SeaLevel_Pres, double pressure ) {
	if( init_err )
			return 0.0;
	// ESP_LOGI(FNAME,"++BMP280 readAVGAltitude QNH=%0.1f CS:%d", SeaLevel_Pres, _cs);
	double alt = calcAltitude( SeaLevel_Pres, pressure );
	_avg_alt = (alt - _avg_alt )*0.2 + _avg_alt;
	// ESP_LOGI(FNAME,"--BME280 readAVGAltitude qnh: %0.1f p=%0.1f avlt=%0.1f alt=%0.1f", SeaLevel_Pres, pressure, _avg_alt, alt );
	return _avg_alt;
}

double BME280_ESP32_SPI::calcAVGAltitudeSTD( double pressure ) {
	if( init_err )
			return 0.0;
	// ESP_LOGI(FNAME,"++BMP280 readAVGAltitude QNH=%0.1f CS:%d", SeaLevel_Pres, _cs);
	double alt = calcAltitude( 1013.25, pressure );
	_avg_alt_std = (alt - _avg_alt_std )*0.2 + _avg_alt_std;
	// ESP_LOGI(FNAME,"--BME280 readAVGAltitudeSTD qnh: %0.1f p=%0.1f avlt=%0.1f alt=%0.1f", 1013.25, pressure, _avg_alt_std, alt );
	return _avg_alt_std;
}

bool BME280_ESP32_SPI::selfTest( float& t, float &p ) {
	uint8_t id = readID();
	if( id != 0x58 ) {
		ESP_LOGE(FNAME,"BMP280 Error, Chip ID reading failed BMP280 chip select pin %d read 0x%.2X (instead 0x58) ", _cs, id  );
		return( false );
	}
	bool success = false;
	double temp=0;
	for( int i=0; i<10; i++ ){
		temp += readTemperature(success);
		delay(100);
	}
	t = (float)(temp/10);
	if( success == false ) {
		ESP_LOGE(FNAME,"BMP280 Error, Temperatur reading BMP280 failed, invalid readout");
		return( false );
	}
	if( (t < -32.0) || (t  > 70.0) ) {
		ESP_LOGW(FNAME,"HW Error, Temperatur value reading BMP280 at normal condition (20 °C +-10) out of bounds readout T=%f", (float)t );
		return( false );
	}
	if( init_err ) {
		ESP_LOGE(FNAME,"BMP280 Error, initialisation invalid response from device");
		return( false );
	}
	p=0;
	bool ok;
	for( int i=0; i<10; i++ ){
		p += (float)readPressure(ok);
		delay(100);
	}
	p=p/10;

    return( true );
}



uint8_t BME280_ESP32_SPI::readID()
{
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	SPI.beginTransaction( spis );
	digitalWrite(_cs, LOW);
	SPI.transfer( 0xD0 | 0x80); //0xDO Chip ID
	uint8_t id;
	id = SPI.transfer( 0 );
	digitalWrite(_cs, HIGH);
	SPI.endTransaction();
	xSemaphoreGive(spiMutex);
	ESP_LOGI(FNAME,"BMP280 Chip ID: %02x, cs=%d", id, _cs );
	return id;
}

//***************BME280****************************
uint16_t BME280_ESP32_SPI::read16bit(uint8_t reg) {
	uint16_t data;   //0xFD Humidity msb read =bit 7 high
	uint8_t d1,d2;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	SPI.beginTransaction( spis );
	digitalWrite(_cs, LOW);
	SPI.transfer( reg | 0x80);
	d1 = SPI.transfer( 0);
	d2 = SPI.transfer( 0);
	data = (d2 << 8) | d1;
	digitalWrite(_cs, HIGH);
	SPI.endTransaction();
	xSemaphoreGive(spiMutex);
	ESP_LOGV(FNAME,"read 16bit: %04x", data );
	return data;
}

//***************BME280****************************
uint8_t BME280_ESP32_SPI::read8bit(uint8_t reg) {
	uint8_t data;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	SPI.beginTransaction( spis );
	digitalWrite(_cs, LOW);
	SPI.transfer( reg | 0x80); // read = bit 7 high
	data = SPI.transfer( 0 );
	digitalWrite(_cs, HIGH);
	SPI.endTransaction();
	xSemaphoreGive(spiMutex);
	ESP_LOGI(FNAME,"read 8bit: %02x", data );
	return data;
}

double BME280_ESP32_SPI::readPressureAVG( float alpha ){
	if( init_err )
		return 0.0;
	bool ok;
    double newval = readPressure(ok);
	if ( exponential_average == 0 ){
		exponential_average = newval;
	}
	exponential_average = exponential_average + alpha*(newval - exponential_average);
	return exponential_average;
}
