/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class CambridgeMsg final : public NmeaPlugin
{
public:
    CambridgeMsg(NmeaPrtcl &nr, ProtocolType p) : NmeaPlugin(nr, p) {};
    virtual ~CambridgeMsg() = default;
    const ParserEntry* getPT() const override { return _pt; }

    // Declare send routines in NmeaPrtcl class !

private:
    // Received messages
    static dl_action_t parseExcl_g(NmeaPrtcl *nmea);
    static const ParserEntry _pt[];
};
