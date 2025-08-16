/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "AdaptUGC.h"

#include <cinttypes>

class PolarGauge;


////////////////////////////
// ArrowIndicator

// A pointer linked to a polar gauge

typedef struct Triangle
{
    int16_t x_0 = 0, y_0 = 0, x_1 = 0, y_1 = 1, x_2 = 1, y_2 = 0;
} Triangle_t;

class ArrowIndicator
{
public:
    ArrowIndicator() = delete;
    ArrowIndicator(PolarGauge &g, int16_t tipref, int16_t length);
    // API
    void setColor(ucg_color_t c) { color = c; }
    bool draw(int16_t val);
    int16_t getBase() { return _base; }

private: // attributes
    PolarGauge &_gauge;             // ref to gauge the pointer belonges to
    const int16_t _tip;             // distance from base center to arrow tip
    const int16_t _base;            // distance from base center to shoulder of the arrow
    const int16_t _base_val_offset = 8; // .5deg steps from base point to shoulder point of arrow
    static constexpr const int16_t _half_width = 8;
    int16_t _needle_pos = 0;        // .5deg diced up
    ucg_color_t color;
    Triangle_t prev;
};
