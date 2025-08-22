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

// Wind indicator around gauge center

class WindIndicator
{
public:
    WindIndicator() = delete;
    WindIndicator(PolarGauge &g, bool live);

    // API
    // void setWind(int16_t wdir, int16_t wval) { _dir = wdir; _val = wval; }
    void setColor(ucg_color_t c) { _color = c; }
    bool changed(int16_t wdir, int16_t wval);
    bool draw(int16_t wdir, int16_t wval);
    void drawWind(bool erase=false);

  private: // attributes
    const PolarGauge &_gauge;  // ref to gauge the indicator belonges to
    int16_t _dir = 0; // 0.5 deg
    int16_t _val = -1;
    bool _live = false;
    ucg_color_t _color;
    static int16_t _cheight;
    static int16_t _cwidth;
};
