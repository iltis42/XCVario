/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "WatchDogIntf.h"

#include <freertos/FreeRTOS.h>

#include <esp_timer.h>
#include <cstdint>

class WdTimeOut; // time out handler

// Watchdog based on esp_timer
class WatchDog_C
{
public:
    explicit WatchDog_C(WDBark_I *cb);
    ~WatchDog_C();

    // API
    void start(const unsigned int t_ms); // ms
    bool restart();
    bool startCond(const unsigned int t_ms);
    void pet();
    void stop();
    bool isRunning() const;
    void setTimeout(const unsigned int t) { _timeout = static_cast<uint64_t>(t) * 1000; } // msec

private:
    friend class WdTimeOut;
    esp_timer_handle_t _timer;
    uint64_t _timeout = 1000000;
    WDBark_I* _cb;
    mutable portMUX_TYPE _mux = portMUX_INITIALIZER_UNLOCKED;
};
