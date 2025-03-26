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
    static constexpr int LEGACY_MAGSTREAM_ID = 0x31;

    MagSensBinary(int mp, ProtocolState &sm, DataLink &dl);
    virtual ~MagSensBinary() = default;
    
    ProtocolType getProtocolId() const override { return MAGSENSBIN_P; }
    bool isBinary() const override { return true; }
    dl_control_t nextBytes(const char *cptr, int count) override;

    bool connected() const { return _connected > 0; }
    bool isActive() const;

private:
    int _connected = -10;
    unsigned long _delta_time; //msec
    unsigned long _last_sample_time;
};