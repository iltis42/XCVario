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

class PolarGauge;

// Wind indicator around gauge center

class WindIndicator
{
public:
    WindIndicator() = delete;
    WindIndicator(PolarGauge &g, bool live);

    // API
    void forceRedraw() { dirty = true; }
    bool draw(int16_t wdir, int16_t wval);

private:
    enum DRAWTYP { ERASE = 0x10, SYNAPT = 1, INST = 2 };
    void drawWind(bool erase);
    // void drawPolarWind(int16_t a, int16_t w);

    // attributes
  private:
    PolarGauge &_gauge;  // ref to gauge the indicator belonges to
    int16_t _dir = 0; // 0.5 deg
    int16_t _val = 0;
    bool _live = false;
    static int16_t _cheight;
    static int16_t _cwidth;
    bool dirty = true;
};
