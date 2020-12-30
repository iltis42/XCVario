#ifndef LEAK_TEST_H
#define LEAK_TEST_H


#include "sensor.h"
#include "BME280_ESP32_SPI.h"
#include "AirspeedSensor.h"

class LeakTest{

public:
	static void start( BME280_ESP32_SPI &bmpBA, BME280_ESP32_SPI &bmpTE, AirspeedSensor *as );

};



#endif
