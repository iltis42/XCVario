/*
 * DS18B20.cpp
 *
 *  Created on: Dec 21, 2017
 *      Author: iltis
 */

#include "driver/gpio.h"
#include "DS18B20.h"
#include "DallasRmt.h"
#include <esp_log.h>
#include "sdkconfig.h"
#include <stdio.h>

OnewireRmt owInst( GPIO_NUM_23, 0, 1);
DallasRmt dallasInst;

DS18B20::DS18B20(gpio_num_t pin, uint8_t res, int max_dev ) {
	_pin = pin;
	_res = res;
	_max_dev = max_dev;
	numDevices = 0;
}

bool DS18B20::begin(){
	gpio_set_pull_mode(_pin, GPIO_PULLUP_ONLY);
	dallas = &dallasInst;
	ow = &owInst;
	dallas->setOneWire(ow);
	dallas->begin();
	numDevices = dallas->getDeviceCount();
	printf("Found %d Dallas temperature devices\n", numDevices);
	if( numDevices )
		return true;
	else
		return false;
}

float DS18B20::getTemp(){
	 float temp = 0;
	 dallas->requestTemperatures();
	 temp = dallas->getTempCByIndex(0);
     if( temp == DEVICE_DISCONNECTED_C ) {
    	 // printf("T sensor disconnected\n");
    	 numDevices = dallas->getDeviceCount();
     }
	// printf("Temperatur: %f\n", temp);
	return temp;
}

DS18B20::~DS18B20() {

}
