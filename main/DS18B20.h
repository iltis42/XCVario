/*
 * DS18B20.h
 *
 *  Created on: Dec 21, 2017
 *      Author: iltis
 */

#ifndef MAIN_DS18B20_H_
#define MAIN_DS18B20_H_

#include "DallasRmt.h"

#include "esp_system.h"

#define MAX_DEVICES 10

class DS18B20 {
public:
	DS18B20( gpio_num_t pin, uint8_t res=10, int max_dev=10 );
	bool begin();
	float getTemp();
	bool haveDevice() { return (bool)numDevices; }
	virtual ~DS18B20();
private:
	gpio_num_t _pin;
	uint8_t _res;
	int _max_dev;
	int numDevices;
	DallasRmt* dallas;
	OnewireRmt* ow;
};

#endif /* MAIN_DS18B20_H_ */
