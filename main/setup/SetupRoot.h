/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "SetupMenu.h"

class IpsDisplay;

class SetupRoot final : public SetupMenu
{
public:
    SetupRoot(IpsDisplay *display); // defines root
    virtual ~SetupRoot();
    void display(int mode=0) override {};
    const char* value() const override { return nullptr; }

    // API
    void begin(MenuEntry *setup=nullptr);
    void exit(int levels=0) override;
    int getActiveScreen() const  { return active_screen; }
    // interaction
    void up(int count) override;
	void down(int count) override;
	void press() override;
	void release() override {};
	void longPress() override;
	void escape() override;

private:
    IpsDisplay* _display;
    int active_screen = 0;  // 0 = Vario
};
