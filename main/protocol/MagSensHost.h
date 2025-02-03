/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

class MagSensHost final : public ProtocolItf
{
public:
    // static constexpr int MAGCTRL_ID = 0x30;
    // static constexpr int MAGSTREAM_ID = 0x31;

public:
    MagSensHost(int mp, ProtocolState &sm, DataLink &dl) : ProtocolItf(DeviceId::MAGSENS_DEV, mp, sm, dl) {}
    virtual ~MagSensHost() {}

public:
    ProtocolType getProtocolId() override { return MAGSENS_P; }
    datalink_action_t nextByte(const char c) override;
    bool isBinary() const override { return false; }

    // Some transmitter
    bool sendHello();
    bool sendCalibration(); // Todo add calib param
    bool startStream(int choice);
    bool killStream();
    bool prepareUpdate(int len, int pack);
    bool firmwarePacket(const char *buf, int len);
    int waitConfirmation();

private:
    // The receiver
    void Version();
    void Confirmation();
    // update packet counter
    int _conf_pack_nr = 0;
};
