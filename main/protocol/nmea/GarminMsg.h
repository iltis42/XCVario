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
    GarminMsg(NmeaPrtcl &nr) : NmeaPlugin(nr, GARMIN_P) {};
    virtual ~GarminMsg() = default;
    const ParserEntry* getPT() const override { return _pt; }

private:
    // Received messages
    static dl_action_t parsePGRMZ(NmeaPlugin *plg);
    static const ParserEntry _pt[];
};
