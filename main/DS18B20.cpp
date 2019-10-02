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

float DS18B20::getTemp(){
	float temp = 0;
	 if (0 == numDevices) {
	        printf("DS18B20 No device found\n");
	        return 0;
	    }
	    dallas->getWaitForConversion();
	    dallas->requestTemperatures();
	    // ESP_LOGW("DS18B20", "wait=%d, conversionTime=%0.2f ms", wait, 0.0);

	    uint8_t deviceAddress[8];
	    for (int i = 0; i < numDevices; ++i) {
	        temp = dallas->getTempCByIndex(i);
	        dallas->getAddress(deviceAddress, i);
	        char buf[17];
	        char*p = buf;
	        for (int i = 0; i < 8; ++i) {
	            p += snprintf(p, sizeof (buf) - 2 * i, "%02x", deviceAddress[i]);
	        }
	        // ESP_LOGW("DS18B20", "index=%d, address=%s, temp=%.2f", i, buf, temp);
	    }
	return temp;
}


DS18B20::~DS18B20() {

}

