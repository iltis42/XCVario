/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "XCNavMsg.h"
#include "protocol/nmea_util.h"
#include "comm/Messages.h"
#include "setup/SetupNG.h"
#include "screen/UiEvents.h"
#include "screen/DrawDisplay.h"
#include "sensor.h"

#include "logdefnone.h"

#include <string>
#include <cstdlib>


//
// The XCNav remote control key parser.
//

dl_action_t XCNavMsg::parseDollar_g(NmeaPlugin *plg)
{
    ProtocolState *sm = plg->getNMEA().getSM();
    const std::vector<int> *word = &sm->_word_start;

    ESP_LOGI(FNAME, "XCNav control: %s", sm->_frame.c_str());

    if (word->size() < 4)
    {
        // Telegramm incomplete
        return NOACTION;
    }

    const char *s = sm->_frame.c_str();
    switch (sm->_frame.at(3))
    {
    case 's': // nonstandard CAI 302 extension for S2F mode switch, e.g. for XCNav remote stick
    {
        ESP_LOGI(FNAME, "Detected S2F cmd");
        int mode = atoi(s + word->at(0) + 1);
        ESP_LOGI(FNAME, "New S2F mode: %d", mode);
        cruise_mode.set(mode);
        break;
    }
    case 'v': // nonstandard CAI 302 extension for volume Up/Down, e.g. for XCNav remote stick
    {
        ESP_LOGI(FNAME, "Detected volume cmd");
        int steps = atoi(s + word->at(0) + 1);
        float v = audio_volume.get() + steps;
        if (v <= 100.0 && v >= 0.0)
        {
            audio_volume.set(v);
            ESP_LOGI(FNAME, "Volume change: %d steps, new volume: %.0f", steps, v);
        }
        break;
    }
    case 'r': // nonstandard CAI 302 extension for Rotary Movement, e.g. for XCNav remote stick to navigate
    {
        char func = *(s + word->at(0) + 1);
        UiEvent event;
        ESP_LOGI(FNAME, "Detected rotary message");
        if (func == 'p')
        {
            ESP_LOGI(FNAME, "Short Press");
            event.button = ButtonEvent(ButtonEvent::SHORT_PRESS);
        }
        else if (func == 'l')
        {
            ESP_LOGI(FNAME, "Long Press");
            event.button = ButtonEvent(ButtonEvent::LONG_PRESS);
        }
        else if (func == 'u')
        {
            ESP_LOGI(FNAME, "Up");
            event.rotary = RotaryEvent(1);
        }
        else if (func == 'd')
        {
            ESP_LOGI(FNAME, "Down");
            event.rotary = RotaryEvent(-1);
        }
        else if (func == 'x')
        {
            ESP_LOGI(FNAME, "Escape");
            event.button = ButtonEvent(ButtonEvent::ESCAPE);
        }
        xQueueSend(uiEventQueue, &event, 0);
        break;
    }
    case 'w': // nonstandard CAI 302 extension for gear warning enable/disable
    {
        int gear = atoi(s + word->at(0) + 1);
        ESP_LOGI(FNAME, "Detected gear warning message");
        if (gear == 0)
        {
            ESP_LOGI(FNAME, "Gear warning disable");
            gflags.gear_warn_external = false;
        }
        else if (gear == 1)
        {
            ESP_LOGI(FNAME, "Gear warning enable");
            gflags.gear_warn_external = true;
        }
        break;
    }
    default:
        break;
    }

    return NOACTION;
}

const ParserEntry XCNavMsg::_pt[] = {
    {Key("g"), XCNavMsg::parseDollar_g},
    {}
};
