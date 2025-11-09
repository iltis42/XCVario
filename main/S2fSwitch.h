/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/ClockIntf.h"

#include <driver/gpio.h>
#include <cinttypes>

class S2fSwitch final : public Clock_I
{
public:
    S2fSwitch() = delete;
    S2fSwitch(gpio_num_t sw);
    virtual ~S2fSwitch();
    void updateSwitchSetup();
    void checkCruiseMode();
    bool tick() override; // polling the switch

private:
    // used gpio for switch
    gpio_num_t _sw;

    // active auto switch filter
    bool (*auto_plug)();
    bool _auto_state = true;
    int16_t _auto_lag;
    int16_t _lag_counter = 0;

    // switch polling
    int16_t _active_level = 1;
    bool _state = false;
    bool _lastButtonRead = false;
    int16_t _debounce = 0;
    int16_t _dcount = 4;
};

extern S2fSwitch *S2FSWITCH;
