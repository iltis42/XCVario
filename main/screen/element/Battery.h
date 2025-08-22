/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ScreenElement.h"

// battery screen element
class Battery : public ScreenElement
{
public:
    Battery(int16_t cx, int16_t cy);
    // API
    using BatteryDisplay = enum { BAT_PERCENTAGE, BAT_VOLTAGE, BAT_VOLTAGE_BIG };
    void setThresholds();
    void blank();
    void draw(float mc);

    // attributes
private:
    int _charge = -1;
    int _yellow;
    int _red;
};