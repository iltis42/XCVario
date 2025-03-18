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
    ParserMap* getPM() const { return &_pm; }
    const char* getSenderId() const { return "PXC"; };

    // Put send routines into NmeaPrtcl !
    
private:
    // Received messages
    static datalink_action_t parsePXCV(NmeaPrtcl *nmea);
    static ParserMap _pm;
};
