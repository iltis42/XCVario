/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

class CANMasterReg  final : public ProtocolItf
{
public:
    // The master registry takes it's own identity, not knowing what kind of device
    // will request. Registration happens on one single port and a unique query id.
    explicit CANMasterReg(int sendport, ProtocolState &sm, DataLink &dl) : ProtocolItf(MASTER_DEV, sendport, sm, dl) {}
    virtual ~CANMasterReg() = default;

    virtual ProtocolType getProtocolId() { return REGISTRATION_P; }

public:
    virtual datalink_action_t nextByte(const char c) override;

private:
    // Received jumbo messages
    void registration_query();
};
