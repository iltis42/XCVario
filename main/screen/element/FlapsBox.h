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

// a visual flaps assistant
class FlapsBox : public ScreenElement
{
public:
    FlapsBox(int16_t cx, int16_t cy, bool vertical=true);
    // API

    void setRef(int16_t x, int16_t y) { _ref_x=x; _ref_y=y; }
    void draw(float ias=0.f);

private:
    void drawLabels(float lwk_speed, float uwk_speed);

private: // attributes
    float _flaps_position = 0.;
    int   _last_flap_idx = 0;
    int   _snd_latency_cnt = 0;
    bool  _vertical;
    int16_t _LFH;
    int16_t _LFW;
};