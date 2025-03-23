/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class GpsMsg final : public NmeaPlugin
{
public:
    GpsMsg(NmeaPrtcl &nr, ProtocolType p) : NmeaPlugin(nr, p) {};
    virtual ~GpsMsg() = default;
    ConstParserMap* getPM() const { return &_pm; }

private:
    // Received messages
    static datalink_action_t parseGPRMC(NmeaPrtcl *nmea);
    static datalink_action_t parseGPGGA(NmeaPrtcl *nmea);
    static ConstParserMap _pm;
};
