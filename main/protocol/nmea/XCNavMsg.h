/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class XCNavMsg final : public NmeaPlugin
{
public:
XCNavMsg(NmeaPrtcl &nr) : NmeaPlugin(nr, XCNAV_P) {};
    virtual ~XCNavMsg() = default;
    const ParserEntry* getPT() const override { return _pt; }

    // Declare send routines in NmeaPrtcl class !

private:
    // Received messages
    static dl_action_t parseDollar_g(NmeaPlugin *plg);

    static const ParserEntry _pt[];
};
