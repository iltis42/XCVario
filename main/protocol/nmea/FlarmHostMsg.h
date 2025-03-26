/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class FlarmHostMsg final : public NmeaPlugin
{
public:
    FlarmHostMsg(NmeaPrtcl &nr);
    virtual ~FlarmHostMsg() = default;
    const ParserEntry* getPT() const override { return _pt; }

public:
    // Received messages
    static dl_action_t parsePFLAX(NmeaPrtcl *nmea);
    static const ParserEntry _pt[];
};
