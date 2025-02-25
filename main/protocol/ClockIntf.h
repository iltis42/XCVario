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
    Clock_I(int m=1) : MULTIPLIER(m) {}
    virtual ~Clock_I() {}
public:
    inline bool myTurn() { return (++_counter % MULTIPLIER) == 0; }
    virtual bool tick() = 0;
    unsigned int MULTIPLIER;
private:
    unsigned int _counter = 0;
};

