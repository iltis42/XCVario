/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ScreenElement.h"

#include <cinttypes>

// altimeter screen element
class Altimeter : public ScreenElement
{
public:
    Altimeter(int16_t cx, int16_t cy);
    // API
    using AltitudeDisplay = enum { MODE_QNH, MODE_QFE };
    using AltQuantisation = enum { ALT_QUANT_DISABLE, ALT_QUANT_2, ALT_QUANT_5, ALT_QUANT_10, ALT_QUANT_20 };

    void setRef(int16_t x, int16_t y) { _ref_x=x; _ref_y=y; }
    void drawUnit();
    void draw(float mc);

private: // attributes
    float _altflt = 0.;
    int   _alt_prev = 0;
    int16_t _unit;
    int16_t _unit_drawn;
    // for roling altimeter
	int16_t _char_width;
	int16_t _char_height;
    int16_t _quant = 0;
    uint8_t _last_quant = 0;
    float fraction_prev = 1.;
    char altpart_prev_s[32] = "";
};