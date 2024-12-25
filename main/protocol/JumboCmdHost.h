/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

#include <string>

class JumboCmdHost final : public ProtocolItf
{
public:
    static constexpr int PROTOCOL_VERSION = 1;

public:
    JumboCmdHost(int mp, ProtocolState &sm) : ProtocolItf(mp, sm) {}
    virtual ~JumboCmdHost() = default;

    DeviceId getDeviceId() override { return JUMBO_DEV; } // The connected (!) device through protocol
    ProtocolType getProtocolId() override { return JUMBO_CMD; }

public:
    gen_state_t nextByte(const char c) override;

    // Some transmitter
    bool sendConnect();
    bool sendGetConfig(const int index);
    bool sendGetInfo();
    bool sendSelectConfig(const int wingconfig); // 0=full, 1=reduced, 2=config
    bool sendStartWipe(const int side); // 0=Right, 1=Left
    bool sendAbortWipe(const int side);
    bool sendShortPress(const int side);
    bool sendHoldPressed(const int side);
    bool sendReleasePressed(const int side);

private:
    // Received jumbo messages
    void connected();
    void config();
    void info();
    void alive();
    void event();
};
