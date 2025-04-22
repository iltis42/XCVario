/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/ClockIntf.h"

// draw a startup XCV logo
// precondition:
// - ucg adapter for the connected display
// - Clock timer

class BootUpScreen final : public Clock_I
{
public:
    BootUpScreen();
    ~BootUpScreen();

    // this will fill the logo completely
    void finish();

    // Clock tick callback
    bool tick() override;

private:
    unsigned char *logo_bitmap = nullptr;
    int width;
    int height;
    int radius;
    int x_offset;
    int y_offset;
};