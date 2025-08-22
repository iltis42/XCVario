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

// cruise status screen element
class CruiseStatus : public ScreenElement
{
public:
    CruiseStatus(int16_t cx, int16_t cy) : ScreenElement(cx, cy) {}

    // API
    void draw();
    void useSymbol(bool us) { _use_symbol = us; }

    // attributes
private:
    static constexpr const int16_t SYMBOL_SIZE = 10;
    bool _use_symbol = true;
};
