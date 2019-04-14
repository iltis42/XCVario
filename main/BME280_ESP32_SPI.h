/*
  BME280_ESP32_SPI.h  Bosch BMP/BME280 for ESP32
  Version 1.1

Reference library:
https://github.com/BoschSensortec/BMP280_driver

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.   If not, see <http://www.gnu.org/licenses/>.

Written: Dec 7 2017.
Last Updated: Dec 07 2017.
This header must be included in any derived code or copies of the code.
Based on the data sheet provided by Bosch for the BMP280 environmental sensor,
calibration code based on algorithms provided by referenced Bosch library,
altitude calculation by open source community on github.

 */

#ifndef BME280_ESP32_SPI_h_
#define BME280_ESP32_SPI_h_
#include <esp_system.h>
#include "driver/gpio.h"
#include "SPI.h"

class BME280_ESP32_SPI
{
public:
	BME280_ESP32_SPI();
	BME280_ESP32_SPI(gpio_num_t sclk, gpio_num_t mosi, gpio_num_t miso, gpio_num_t cs, uint32_t freq);
	void begin(uint8_t Stanby_t, uint8_t filter, uint8_t overS_T, uint8_t overS_P, uint8_t overS_H, uint8_t mode);

	double readTemperature();
	double readPressure();
	double readPressureAVG( float alpha=0.1 );
	double readHumidity();
	double readAltitude(double SeaLevel_Pres=1013.25);
	double calcAVGAltitude(double SeaLevel_Pres, double p );
	uint8_t readID();

private:
	void WriteRegister(uint8_t reg_address, uint8_t data);
	void readCalibration(void);
	int32_t compensate_T(int32_t adc_T);
	uint32_t compensate_P(int32_t adc_P);
	uint32_t compensate_H(int32_t adc_H);
	uint16_t read16bit(uint8_t reg);
	uint8_t read8bit(uint8_t reg);
	double _avg_alt;

private:
	gpio_num_t _sclk, _mosi, _miso, _cs;
	uint32_t _freq;
	int32_t  _t_fine;

	uint16_t _dig_T1;
	int16_t  _dig_T2;
	int16_t  _dig_T3;

	uint16_t _dig_P1;
	int16_t  _dig_P2;
	int16_t  _dig_P3;
	int16_t  _dig_P4;
	int16_t  _dig_P5;
	int16_t  _dig_P6;
	int16_t  _dig_P7;
	int16_t  _dig_P8;
	int16_t  _dig_P9;

	uint8_t _dig_H1;
	int16_t _dig_H2;
	uint8_t _dig_H3;
	int16_t _dig_H4;
	int16_t _dig_H5;
	int8_t  _dig_H6;
	double exponential_average;
	bool init_err;
};

#endif
