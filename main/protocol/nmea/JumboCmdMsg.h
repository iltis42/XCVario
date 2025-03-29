/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class SetupAction;

class JumboCmdMsg final : public NmeaPlugin
{
public:
    static constexpr int PROTOCOL_VERSION = 1;

public:
    JumboCmdMsg(NmeaPrtcl &nr) : NmeaPlugin(nr, JUMBOCMD_P) {};
    virtual ~JumboCmdMsg() = default;
    const ParserEntry* getPT() const override { return _pt; }

    // API to setup menu
    static SetupAction* RightAction;
    static SetupAction* LeftAction;
    
private:
    // Received messages
    static const ParserEntry _pt[];

    // Received jumbo messages
    static dl_action_t connected(NmeaPrtcl *nmea);
    static dl_action_t config(NmeaPrtcl *nmea);
    static dl_action_t info(NmeaPrtcl *nmea);
    static dl_action_t alive(NmeaPrtcl *nmea);
    static dl_action_t event(NmeaPrtcl *nmea);
};
