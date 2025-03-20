/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class FlarmMsg final : public NmeaPlugin
{
public:
    FlarmMsg(NmeaPrtcl &nr);
    virtual ~FlarmMsg() = default;
    ConstParserMap* getPM() const { return &_pm; }

private:
    // Received messages
    static datalink_action_t parsePFLAA(NmeaPrtcl *nmea);
    static datalink_action_t parsePFLAE(NmeaPrtcl *nmea);
    static datalink_action_t parsePFLAU(NmeaPrtcl *nmea);
    static datalink_action_t parsePFLAX(NmeaPrtcl *nmea);
    static ConstParserMap _pm;
};
