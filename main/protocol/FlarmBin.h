/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

class FlarmBinary final : public ProtocolItf
{
public:
    FlarmBinary(DeviceId did, int mp, ProtocolState &sm, DataLink &dl) : ProtocolItf(did, mp, sm, dl) {}
    virtual ~FlarmBinary() = default;
    ProtocolType getProtocolId() override { return FLARMBIN_P; }
    datalink_action_t nextStreamChunk(const char *cptr, int count) override;
    void setPeer(ProtocolItf *p);
    
private:
    void send_chunk();
    // binary mode peer
    ProtocolItf *_binpeer;
};