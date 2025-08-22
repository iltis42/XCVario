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
    static constexpr int DIVIDER = 4;

    static BootUpScreen *create();
    static void terminate();
    ~BootUpScreen();

    // this will fill the logo completely
    void finish(int part);
    static void draw();

    // Clock tick callback
    bool tick() override;

private:
    BootUpScreen();
    static BootUpScreen *inst;
    void animate();

    unsigned char *logo_bitmap = nullptr;
    int width;
    int height;
    int radius;
    int x_offset;
    int y_offset;
    int fini_part;
    int yline;
    int yline_to;
};
