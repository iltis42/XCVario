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
    ConstParserMap* getPM() const { return &_pm; }

public:
    // Received messages
    static datalink_action_t parsePFLAX(NmeaPrtcl *nmea);
    static ConstParserMap _pm;
};
