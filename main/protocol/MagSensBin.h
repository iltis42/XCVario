/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

class MagSensBinary final : public ProtocolItf
{
public:
    MagSensBinary(int mp, ProtocolState &sm, DataLink &dl) : ProtocolItf(DeviceId::MAGSENS_DEV, mp, sm, dl) {}
    virtual ~MagSensBinary() = default;
    
    ProtocolType getProtocolId() override { return MAGSENSBIN_P; }
    bool isBinary() const override { return true; }
    datalink_action_t nextStreamChunk(const char *cptr, int count) override;

    bool connected() const { return _connected; }

private:
    bool _connected = false;
};