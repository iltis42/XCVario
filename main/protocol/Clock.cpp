/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "Clock.h"
#include "ClockIntf.h"

#include <freertos/FreeRTOS.h>
#include <esp_attr.h>

#include <set>
esp_timer_handle_t Clock::_clock_timer = nullptr;

static volatile unsigned long msec_counter = 0;

// Simple fix number of slots registry
static std::set<Clock_I*> clock_registry;

// Timer SR (called in a timer task context)
static void IRAM_ATTR clock_timer_sr(std::set<Clock_I*> *registry)
{
    // be in sync with millis, but sparse
    msec_counter = esp_timer_get_time() / 1000;
    // tick callbacks
    for (auto it = registry->begin(); it != registry->end(); ) {
        if ((*it)->myTurn()) {
            if ((*it)->tick()) {
                it = registry->erase(it);
                continue;
            }
        }
        it++;
    }
}


// The clock
Clock::Clock()
{
    // setup clock timer only once
    if ( _clock_timer == 0 ) {
        esp_timer_create_args_t t_args = {
            .callback = (esp_timer_cb_t)clock_timer_sr,
            .arg = (void*)(&clock_registry),
            .dispatch_method = ESP_TIMER_TASK,
            .name = "clock",
            .skip_unhandled_events = true,

        };
        esp_timer_create(&t_args, &_clock_timer);
        esp_timer_start_periodic(_clock_timer, TICK_ATOM * 1000); // the timers API is on usec
        clock_registry.clear();
    }
}


void Clock::start(Clock_I *cb)
{
    esp_timer_stop(_clock_timer);
    clock_registry.insert(cb);
    esp_timer_start_periodic(_clock_timer, TICK_ATOM * 1000);
}
void Clock::stop(Clock_I *cb)
{
    esp_timer_stop(_clock_timer);
    auto it = clock_registry.find(cb);
    if ( it != clock_registry.end() ) {
        clock_registry.erase(it);
    }
    esp_timer_start_periodic(_clock_timer, TICK_ATOM * 1000);
}

unsigned long Clock::getMillis()
{
    return msec_counter;
}
int Clock::getSeconds()
{
    return static_cast<int>(msec_counter / 1000);
}
