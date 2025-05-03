/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class OpenVarioMsg final : public NmeaPlugin
{
public:
    OpenVarioMsg(NmeaPrtcl &nr, bool as) : NmeaPlugin(nr, OPENVARIO_P, as) {};
    virtual ~OpenVarioMsg() = default;
    const ParserEntry* getPT() const override { return _pt; }

    // Declare send routines in NmeaPrtcl class !

private:
    // Received messages
    static const ParserEntry _pt[];
};
