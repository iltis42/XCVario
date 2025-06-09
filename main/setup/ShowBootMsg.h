/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "setup/SetupMenuDisplay.h"

class ShowBootMsg: public SetupMenuDisplay
{
public:
    ShowBootMsg(const char *title);
    ~ShowBootMsg() = default;

    void display(int mode = 0) override;
};
