/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "setup/SetupMenu.h"
#include "protocol/WatchDog.h"

enum ScreenTypes // bit field
{
    SCREEN_VARIO = 1,
    SCREEN_GMETER = 2,
    SCREEN_HORIZON = 4,
    SCREEN_FLARM = 8,
    SCREEN_LIST_END = 16
}; // all regular screens

class IpsDisplay;

class SetupRoot final : public SetupMenu, public WDBark_I
{
public:
    SetupRoot(IpsDisplay *display); // defines root
    virtual ~SetupRoot();
    void display(int mode=0) override {};
    const char* value() const override { return nullptr; }
    void barked() override;

    // API
    static void initScreens();
    void begin(MenuEntry *setup=nullptr);
    void exit(int levels=0) override;
    int getActiveScreen() const  { return active_screen; }
    // interaction
    void rot(int count) override;
    void press() override;
    void longPress() override;

private:
    IpsDisplay* _display;
    int active_screen = SCREEN_VARIO;
    WatchDog_C  _ui_mon_wd;
};
