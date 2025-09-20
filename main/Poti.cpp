/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "Poti.h"
#include "I2Cbus.hpp"
#include "logdef.h"

#include <esp_system.h>
#include <cmath>

bool Poti::begin()
{
	int wiper;
	if( readWiper( wiper ) ) {
		ESP_LOGI(FNAME,"wiper=%d", wiper );
		return(true);
	}
	// else
	ESP_LOGE(FNAME,"Error reading wiper!");
	return( false );
}

bool Poti::haveDevice() {
	ESP_LOGI(FNAME,"haveDevice");
	esp_err_t err = bus->testConnection(I2C_ADDR);
	if( err == ESP_OK ) {
		ESP_LOGI(FNAME,"haveDevice: OK");
		return true;
	}
	// else
	ESP_LOGI(FNAME,"haveDevice: NONE");
	return false;
}

// bool Poti::readVolume( float &val ) {
// ...
// }

// 0 .. 100%
bool Poti::writeVolume( float val )
{
    float db = (powf(1. + 9., val/100.) - 1.) / 9.;
	int ival = (int)(db * getRange());
    ESP_LOGI(FNAME,"db %f range %d, ival %d", db, getRange(), ival);

    return writeWiper(ival);
}
