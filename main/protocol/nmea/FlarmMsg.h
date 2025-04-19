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
    const ParserEntry* getPT() const override { return _pt; }

private:
    // Received messages
    static dl_action_t parsePFLAA(NmeaPlugin *plg);
    static dl_action_t parsePFLAE(NmeaPlugin *plg);
    static dl_action_t parsePFLAU(NmeaPlugin *plg);
    static dl_action_t parsePFLAX(NmeaPlugin *plg);
    static dl_action_t parseExcl_xc(NmeaPlugin *plg);
    
    static const ParserEntry _pt[];
};
