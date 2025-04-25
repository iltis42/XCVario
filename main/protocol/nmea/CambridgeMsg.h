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
    CambridgeMsg(NmeaPrtcl &nr, bool as) : NmeaPlugin(nr, CAMBRIDGE_P, as) {};
    virtual ~CambridgeMsg() = default;
    const ParserEntry* getPT() const override { return _pt; }

    // Declare send routines in NmeaPrtcl class !

private:
    // Received messages
    static dl_action_t parseExcl_g(NmeaPlugin *plg);
    static const ParserEntry _pt[];
};
