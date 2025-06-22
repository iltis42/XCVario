/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

class MagSensBin final : public ProtocolItf
{
public:
    static constexpr int LEGACY_MAGSTREAM_ID = 0x31;

    MagSensBin(int mp, ProtocolState &sm, DataLink &dl);
    virtual ~MagSensBin();

    ProtocolType getProtocolId() const override { return MAGSENSBIN_P; }
    void addAliveMonitor(AliveMonitor *am) override { _alive = am; }
    bool isBinary() const override { return true; }
    dl_control_t nextBytes(const char *cptr, int count) override;

    bool connected() const { return _connected > 0; }
    bool isActive() const;

private:
    int _connected = -10;
    unsigned long _delta_time; //msec
    unsigned long _last_sample_time;
    AliveMonitor *_alive = nullptr; // alive monitor for the mag stream
};
