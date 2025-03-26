/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

// The Flarm "BP" binary protocol

class FlarmBinary final : public ProtocolItf
{
public:
    FlarmBinary(DeviceId did, int mp, ProtocolState &sm, DataLink &dl) : ProtocolItf(did, mp, sm, dl) {}
    virtual ~FlarmBinary() = default;
    ProtocolType getProtocolId() const override { return FLARMBIN_P; }
    bool isBinary() const override { return true; }
    dl_control_t nextBytes(const char *cptr, int count) override;
    void setPeer(FlarmBinary *p);
    int getFrameCnt() { return _frame_counter++; }
    
    // Transmitted mesages
    bool setBaudrate(int, int);
    bool ping();

private:
    void send_chunk();
    // binary mode peer
    FlarmBinary *_binpeer;
    // frame counter monitor
    int _frame_counter = 0;
};
