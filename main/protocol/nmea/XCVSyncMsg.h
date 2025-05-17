/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "protocol/NMEA.h"

class XCVSyncMsg  final : public NmeaPlugin
{
public:

    explicit XCVSyncMsg(NmeaPrtcl &nr, bool master, bool as);
    virtual ~XCVSyncMsg();
    const ParserEntry* getPT() const override { return _pt; }

    bool isMaster() const { return _is_master; }

public:
    // only needed from SetupNG
    bool sendItem(const char *key, char type, void *value, int len);
    // bool sendAck(uint8_t sync, const char *key, char type, void *value, int len);
 
private:
    bool _is_master;

    // Received messages
    static dl_action_t parseExcl_xsX(NmeaPlugin *plg);
    static const ParserEntry _pt[];
};

