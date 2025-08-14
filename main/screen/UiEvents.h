/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <cstdint>

// A max. 32bit event structure, that is pushed to the ui event queue.

// UI events have the following structure:
// - LSB 8 bits are the sub type, detail, or special info of what is going on
// - higher significant bits are defined as bit mask. Each bit is assigned to a event source or class.

// The event type is the event mask
enum UiEventType {
    EVT_BUTTON = 0x0100, // button event
    EVT_ROTARY = 0x0200, // rotary event
    EVT_SCREEN = 0x0400, // screen change event
    EVT_MODE   = 0x0800, // any mode change event
};

// The button press events are
struct ButtonEvent
{
    enum { SHORT_PRESS = 1,
           LONG_PRESS = 2,
           BUTTON_RELEASED = 3,
           ESCAPE = 4 };

    uint32_t raw;
    ButtonEvent() = delete;
    constexpr ButtonEvent(const uint8_t v) : raw((0xff & v) | EVT_BUTTON) {}
};

// The rotary events are
struct RotaryEvent
{
    // const rotary values are -3, -2, -1, 1, 2, 3
    uint32_t raw;
    RotaryEvent() = delete;
    constexpr RotaryEvent(const uint8_t v) : raw((0xff & v) | EVT_ROTARY) {}
};

// The screen events are
struct ScreenEvent
{
    enum { MSG_BOX = 1,
           BOOT_SCREEN,
           FLARM_ALARM,
           VOLT_ADJUST,
           QNH_ADJUST };

    uint32_t raw;
    ScreenEvent() = delete;
    constexpr ScreenEvent(const uint8_t v) : raw((0xff & v) | EVT_SCREEN) {}
};

// The mode events are
struct ModeEvent
{
    enum { MODE_S2F = 1,
           MODE_VARIO, 
           MODE_TOGGLE };

    uint32_t raw;
    ModeEvent() = delete;
    constexpr ModeEvent(const uint8_t v) : raw((0xff & v) | EVT_MODE) {}
};

// Button and Rotary are often handled the same way


// Convenience to read the events as int from the queue
union UiEvent {
    uint32_t code;
    ButtonEvent button; // button event
    RotaryEvent rotary; // rotary event
    int8_t getDetail() const { return code & 0xff; } // get the event code
    bool isButtonEvent() const { return (code & EVT_BUTTON) != 0; } // true if button event
    bool isRotaryEvent() const { return (code & EVT_ROTARY) != 0; } // true if rotary event
    bool isKnobEvent() const { return (code & (EVT_BUTTON&EVT_ROTARY)) != 0; } // true if button  or rotary event
    bool isScreenEvent() const { return (code & EVT_SCREEN) != 0; } // true if screen event
    bool isModeEvent() const { return (code & EVT_MODE) != 0; } // true if mode event
    // bool isFlarmEvent() const { return (code & EVT_FLARM) != 0; } // true if flarm event
    // bool isMboxEvent() const { return (code & EVT_MBOX) != 0; } // true if message box event
    UiEvent() : code(0) {};
    constexpr UiEvent(const int k) : code(k) {}
};
