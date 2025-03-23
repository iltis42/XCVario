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
    ConstParserMap* getPM() const { return &_pm; }

private:
    // Received messages
    static datalink_action_t parsePGRMZ(NmeaPrtcl *nmea);
    static ConstParserMap _pm;
};
