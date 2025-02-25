/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

#include <string>
#include <vector>

class FlarmHost final : public ProtocolItf
{
public:
    FlarmHost(int mp, ProtocolState &sm, DataLink &dl) : ProtocolItf(NAVI_DEV, mp, sm, dl) {}
    virtual ~FlarmHost() = default;
    ProtocolType getProtocolId() override { return FLARMHOST_P; }

public:
    datalink_action_t nextByte(const char c) override;
};
