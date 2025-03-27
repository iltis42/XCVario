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
    const ParserEntry* getPT() const override { return _pt; }

    static int getXcvProtocolVersion() { return _protocol_version; };

    // Declare send routines in NmeaPrtcl class !

private:
    // Received messages
    static dl_action_t parseExcl_xsX(NmeaPrtcl *nmea);
    static dl_action_t parseExcl_xcs(NmeaPrtcl *nmea);

    static const ParserEntry _pt[];
    static uint8_t _protocol_version;
};
