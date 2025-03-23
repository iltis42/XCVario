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
    XCVarioMsg(NmeaPrtcl &nr, ProtocolType p) : NmeaPlugin(nr, p) {};
    virtual ~XCVarioMsg() = default;
    ConstParserMap* getPM() const { return &_pm; }

    static int getXcvProtocolVersion() { return _protocol_version; };

    // Declare send routines in NmeaPrtcl class !

private:
    // Received messages
    static datalink_action_t parseExcl_xsX(NmeaPrtcl *nmea);
    static datalink_action_t parseExcl_xcs(NmeaPrtcl *nmea);

    static ConstParserMap _pm;
    static uint8_t _protocol_version;
};
