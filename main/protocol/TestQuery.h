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

class TestQuery final : public ProtocolItf, public Clock_I
{
public:
    explicit TestQuery(DeviceId did, int mp, ProtocolState &sm, DataLink &dl);
    virtual ~TestQuery();

    ProtocolType getProtocolId() const override { return TEST_P; }

public:
    dl_control_t nextBytes(const char* c, int len) override;

    // The only transmitter
    bool sendTestQuery();

    // Clock tick callback
    bool tick() override;

private:
    // Actions on commands
    void parseTest();
};
