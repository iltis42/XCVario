/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "Poti.h"
#include "I2Cbus.hpp"
#include "protocol/Clock.h"
#include "logdefnone.h"

#include <esp_system.h>
#include <cmath>


int Poti::RANGE;

Poti::Poti(i2cbus::I2C *i2cbus, uint8_t addr) : Clock_I(1), bus(i2cbus), I2C_ADDR(addr)
{
}

bool Poti::begin()
{
    int wiper;
    if (readWiper(wiper))
    {
        ESP_LOGI(FNAME, "wiper=%d", wiper);
        return (true);
    }
    // else
    ESP_LOGE(FNAME, "Error reading wiper!");
    return (false);
}

bool Poti::haveDevice()
{
    esp_err_t err = bus->testConnection(I2C_ADDR);
    if (err == ESP_OK)
    {
        ESP_LOGI(FNAME, "haveDevice: OK");
        return true;
    }
    // else
    ESP_LOGI(FNAME, "haveDevice: NONE");
    return false;
}

bool Poti::tick()
{
	if (lastWiperValue != targetWiperValue)
	{
		int step = (targetWiperValue > lastWiperValue) ? 1 : -1;
		lastWiperValue += std::abs(targetWiperValue - lastWiperValue) > 5 ? step*5 : step;
        writeWiper( lastWiperValue );
        ESP_LOGI(FNAME, "soft %d, ival %d", targetWiperValue, lastWiperValue);
        return false;
	}

    return true;
}

void Poti::softSetVolume(float val)
{
    targetWiperValue = calcDbFromVolume(val);
    if (lastWiperValue != targetWiperValue)
    {
        Clock::start(this);
    }
}

// bool Poti::readVolume( float &val ) {
// ...
// }

// 0 .. 100%
bool Poti::writeVolume(float val)
{
    int ival = calcDbFromVolume(val);
    ESP_LOGI(FNAME, "range %d, ival %d", RANGE, ival);

    return writeWiper(ival);
}

// db mapping of volume 0..100% range
constexpr int Poti::calcDbFromVolume(float val)
{
    float db = (powf(1. + 9., val / 100.) - 1.) / 9.;
    return (int)(db * RANGE);
}