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
    FlarmHost(int mp, ProtocolState &sm) : ProtocolItf(NAVI_DEV, mp, sm) {}
    virtual ~FlarmHost() = default;
    ProtocolType getProtocolId() override { return FLARMHOST_P; }

public:
    datalink_action_t nextByte(const char c) override;
    datalink_action_t nextStreamChunk(const char *cptr, int count) override;

private:
    // binary mode peer
    ProtocolItf *_binpeer;
};
