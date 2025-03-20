/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class XCVarioMsg final : public NmeaPlugin
{
public:
XCVarioMsg(NmeaPrtcl &nr) : NmeaPlugin(nr) {};
    virtual ~XCVarioMsg() = default;
    ConstParserMap* getPM() const { return &_pm; }

    // Declare send routines in NmeaPrtcl class !

private:
    // Received messages
    static datalink_action_t parsePXCV(NmeaPrtcl *nmea);
    static ConstParserMap _pm;
};
