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
    // static constexpr int MAGCTRL_ID = 0x30;
    // static constexpr int MAGSTREAM_ID = 0x31;

public:
    MagSensMsg(NmeaPrtcl &nr) : NmeaPlugin(nr) {};
    virtual ~MagSensMsg() = default;
    ConstParserMap* getPM() const { return &_pm; }


private:
    // Received messages
    static ConstParserMap _pm;
    
    // The receiver
    static datalink_action_t magsensVersion(NmeaPrtcl *nmea);
    static datalink_action_t magsensConfirmation(NmeaPrtcl *nmea);
};
