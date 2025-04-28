/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

 #include "WatchDog.h"

#include <esp_attr.h>
#include <cassert>

// Timer isr's
class WdTimeOut {
public:
    static void IRAM_ATTR timeout(WatchDog_C* wd_obj)
    {
        wd_obj->_cb->barked();
    }
};

// Ctor
WatchDog_C::WatchDog_C(WDBark_I *cb) :
    _cb(cb)
{
    assert(cb);
    esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)WdTimeOut::timeout,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "watchdog",
        .skip_unhandled_events = true,
    };
    esp_timer_create(&timer_args, &_timer);
}

// Dtor
WatchDog_C::~WatchDog_C()
{
    esp_timer_stop(_timer);
    esp_timer_delete(_timer);
    _timer = nullptr;
}

// Start
void WatchDog_C::start(const unsigned int t_ms)
{
    portENTER_CRITICAL(&_mux);
    esp_timer_stop(_timer);
    _timeout = t_ms * 1000;
    esp_timer_start_once(_timer, _timeout);
    portEXIT_CRITICAL(&_mux);
}

// Re-Start, returns true for a real start of the timer
// might be called from ISR !
bool IRAM_ATTR WatchDog_C::restart()
{
    portENTER_CRITICAL(&_mux);
    bool ret = ! esp_timer_is_active(_timer);
    esp_timer_stop(_timer);
    esp_timer_start_once(_timer, _timeout);
    portEXIT_CRITICAL(&_mux);

    return ret;
}

// Start in case of not yet startet, returns true when started
bool WatchDog_C::startCond(const unsigned int t_ms)
{
    bool ret = false;
    portENTER_CRITICAL(&_mux);
    if ( ! esp_timer_is_active(_timer) ) {
        _timeout = t_ms * 1000;
        esp_timer_start_once(_timer, _timeout);
    }
    portEXIT_CRITICAL(&_mux);

    return ret;
}

// Pet
void WatchDog_C::pet()
{
    portENTER_CRITICAL(&_mux);
    if ( esp_timer_is_active(_timer) ) {
        esp_timer_stop(_timer);
        esp_timer_start_once(_timer, _timeout);
    }
    portEXIT_CRITICAL(&_mux);
}

// Stop
void WatchDog_C::stop()
{
    portENTER_CRITICAL(&_mux);
    esp_timer_stop(_timer);
    portEXIT_CRITICAL(&_mux);
}

// Check
bool WatchDog_C::isRunning() const
{
    portENTER_CRITICAL(&_mux);
    bool ret = esp_timer_is_active(_timer);
    portEXIT_CRITICAL(&_mux);
    return ret;
}

