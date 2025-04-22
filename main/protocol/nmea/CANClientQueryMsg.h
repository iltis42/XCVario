/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"
#include "protocol/ClockIntf.h"

class CANClientQueryMsg  final : public NmeaPlugin, public Clock_I
{
public:
    static constexpr int MAX_NR_TRIALS = 20;

    explicit CANClientQueryMsg(NmeaPrtcl &nr);
    virtual ~CANClientQueryMsg();

    const ParserEntry* getPT() const override { return _pt; }

public:
    // Clock tick callback
    bool tick() override;

    // var needed from static parser
    char Q_TOKEN[4];
 
private:
    // only needed from this plugin
    bool sendRegistrationQuery();

    // Received messages
    static dl_action_t registration(NmeaPlugin *plg);
    static dl_action_t restart_query(NmeaPlugin *plg);
    static dl_action_t rejected(NmeaPlugin *plg);
    static const ParserEntry _pt[];
};

