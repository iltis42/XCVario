/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class GarminMsg final : public NmeaPlugin
{
public:
    GarminMsg(NmeaPrtcl &nr, ProtocolType p) : NmeaPlugin(nr, p) {};
    virtual ~GarminMsg() = default;
    const ParserEntry* getPT() const override { return _pt; }

private:
    // Received messages
    static datalink_action_t parsePGRMZ(NmeaPrtcl *nmea);
    static const ParserEntry _pt[];
};
