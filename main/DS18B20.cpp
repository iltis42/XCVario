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


static DallasRmt* dallas = NULL;
static OnewireRmt* ow = NULL;

// static uint32_t readings = 0;

DS18B20::DS18B20(gpio_num_t pin, uint8_t res, int max_dev ) {
	_pin = pin;
	_res = res;
	_max_dev = max_dev;
	numDevices = 0;
}

bool DS18B20::begin(){
	gpio_set_pull_mode(_pin, GPIO_PULLUP_ONLY);
	dallas = new DallasRmt();
	ow = new OnewireRmt( _pin /*pin*/, 0 /*rmt_rx*/, 1 /*rmt_tx*/);
	dallas->setOneWire(ow);
	dallas->begin();
	numDevices = dallas->getDeviceCount();
	printf("Found %d Dallas temperature devices\n", numDevices);
	if( numDevices )
		return true;
	else
		return false;
}

float DS18B20::getTemp( bool& success ){
	 float temp = 0;
	 success = false;
	 uint8_t a;
	 dallas->getAddress( &a, 0 );

//	 if( !dallas->validAddress( &a ) ){
//		   // printf("DS18B20 reports invalid crc for address\n");
//		   return 0;
//	 }

	 bool c = dallas->isConnected( &a );
	 printf("Address: %d  Connected: %d\n", a, c );
	 if ( !c ) {
	        printf("DS18B20 not connected\n");
	        dallas->setOneWire(ow);
	        dallas->begin();
	        return 0;
	 }
	dallas->requestTemperaturesByAddress(&a);
	dallas->getWaitForConversion();
	temp = dallas->getTempC( &a );
// 	bool validFamily = dallas->validFamily( &a );
//	if( !validFamily ) {
//	   printf("DS18B20 reports invalid family connected\n");
//	   return 0;
//	}
	success = true;
	// printf("Temperatur: %f valid=%d\n", temp, success);
	return temp;
}


DS18B20::~DS18B20() {

}

