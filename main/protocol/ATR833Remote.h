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

class ATR833Remote final : public ProtocolItf
{
public:
    explicit ATR833Remote(int mp, ProtocolState &sm, DataLink &dl);
    virtual ~ATR833Remote() = default;
    ProtocolType getProtocolId() const override { return ATR833_REMOTE_P; }
    bool isBinary() const override { return true; }

public:
    dl_control_t nextBytes(const char* c, int len) override;


private:
    // Actions on commands
    uint8_t cs;
};
