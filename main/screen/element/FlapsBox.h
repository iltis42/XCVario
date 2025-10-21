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

union SwitchEvent {
    struct {
        uint8_t from;
        uint8_t to; // flap setting
    };
    uint16_t raw = 0;

    constexpr SwitchEvent() = delete;
    constexpr SwitchEvent(int f, int t)
        : from((f & 0xFF)), to((t & 0xFF)) {}
    constexpr bool operator!=(const SwitchEvent &other) const noexcept {
        return raw != other.raw; }
};

// a visual flaps assistant
class FlapsBox : public ScreenElement
{
public:
    FlapsBox(int16_t cx, int16_t cy, bool vertical=true);
    // API

    void setRef(int16_t x, int16_t y) { _ref_x=x; _ref_y=y; }
    void draw(float ias);

private:
    void drawLabels(float lwk_speed, float uwk_speed);

private: // attributes
    float _flaps_position = 0.;
    int   _last_flap_idx = 0;
    int   _snd_latency_cnt = 0;
    SwitchEvent _last_event;
    int   _same_event_to = -1;
    bool  _vertical;
    int16_t _LFH;
};