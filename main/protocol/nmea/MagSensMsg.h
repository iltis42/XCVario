/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class MagSensMsg final : public NmeaPlugin
{
public:
    MagSensMsg(NmeaPrtcl &nr) : NmeaPlugin(nr, MAGSENS_P) {};
    virtual ~MagSensMsg() = default;
    const ParserEntry* getPT() const override { return _pt; }


private:
    // Received messages
    static const ParserEntry _pt[];
    
    // The receiver
    static dl_action_t magsensVersion(NmeaPlugin *plg);
    static dl_action_t magsensConfirmation(NmeaPlugin *plg);
    static dl_action_t streamData(NmeaPlugin *plg);
};
