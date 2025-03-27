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
    static dl_action_t parsePFLAA(NmeaPrtcl *nmea);
    static dl_action_t parsePFLAE(NmeaPrtcl *nmea);
    static dl_action_t parsePFLAU(NmeaPrtcl *nmea);
    static dl_action_t parsePFLAX(NmeaPrtcl *nmea);
    static dl_action_t parseExcl_xc(NmeaPrtcl *nmea);
    
    static const ParserEntry _pt[];
};
