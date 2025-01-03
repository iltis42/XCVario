/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ClockIntf.h"

class DataLink;
class Device;

class FlarmSim final : public Clock_I
{
public:
    static void StartSim();

    // Clock tick callback
    void tick() override;

private:
    FlarmSim() = delete;
    explicit FlarmSim(Device *d);
    ~FlarmSim();

    int     _next_msg = 0;
    Device  *_d; // hijacked device
    // Active flagging instance
    static FlarmSim *_sim;
};
