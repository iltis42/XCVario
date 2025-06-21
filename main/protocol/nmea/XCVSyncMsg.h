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
    bool isSyncInitPending() const { return _kick_sync; }
    bool syncStarted() const { return !_kick_sync; }

public:
    // only needed from SetupNG
    bool sendItem(const char *key, char type, void *value, int len);
    bool sendInitSyncRequest();

private:
    bool _is_master;
    bool _kick_sync; // a once to flip variable

    // Received messages
    static dl_action_t parseExcl_xsX(NmeaPlugin *plg);
    static dl_action_t parseExcl_xsSyncInit(NmeaPlugin *plg);
    static const ParserEntry _pt[];
};
