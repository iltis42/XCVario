/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "WatchDog.h"


template<typename T>
class SetupNG;

// Watchdog based on esp_timer
class AliveMonitor :  public WDBark_I
{
public:
    explicit AliveMonitor(SetupNG<int>* flag, int to_ms = 1000);
    ~AliveMonitor() = default;

    // API
    void setTimeout(int to_ms) { _alive_wd.setTimeout(to_ms); }
    void keepAlive();

protected:
    // alive timeout
    void barked() override;

private:
    WatchDog_C    _alive_wd;
    SetupNG<int>* _alive_flag = nullptr;
};
