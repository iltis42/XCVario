/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

// Clock interface
class Clock_I
{
public:
    explicit Clock_I(int m=1) : MULTIPLIER(m) {}
    Clock_I() = delete;
    virtual ~Clock_I() = default;
public:
    inline bool myTurn() { return (++_counter % MULTIPLIER) == 0; }
    virtual bool tick() = 0; // return true to unregister
    void setMultiplier(unsigned m) { MULTIPLIER = m; }
    unsigned MULTIPLIER;
private:
    unsigned _counter = 0;
};

