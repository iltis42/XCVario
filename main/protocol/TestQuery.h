/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"
#include "ClockIntf.h"

#include <string>

class Clock;

class TestQuery final : public ProtocolItf, public Clock_I
{
public:
    explicit TestQuery(int mp, ProtocolState &sm);
    virtual ~TestQuery();

    DeviceId getDeviceId() override { return TEST_DEV; } // The connected (!) device through protocol
    ProtocolType getProtocolId() override { return TEST_P; }

public:
    gen_state_t nextByte(const char c) override;

    // The only transmitter
    bool sendTestQuery();

    // Clock tick callback
    void tick() override;

private:
    // Actions on commands
    gen_state_t parseTest();
    static Clock *MY_CLOCK;

};
