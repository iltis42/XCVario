/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once


// List of supported devices
typedef enum
{
    NO_DEVICE,
    ANEMOI_DEV,
    ATR833_HOST_DEV,
    FLARM_DEV,
    FLARM_HOST_DEV,
    JUMBO_DEV,
    KRT2_HOST_DEV,
    MAGSENS_DEV,
    MASTER_DEV,  // CAN id registry
    NAVI_DEV,
    RADIO_ATR833_DEV, // <- 10
    RADIO_KRT2_DEV,
    RADIO_REMOTE_DEV,
    XCVARIO_DEV,
    XCVARIOCLIENT_DEV,
    FLARM_PROXY,
    RADIO_PROXY,
    TEST_DEV,
    TEST_DEV2
} DeviceId;


// Supported protocol id's
typedef enum
{
    NO_ONE = 0, // not a protocol
    ANEMOI_P,
    ATR833_REMOTE_P,
    BORGELT_P,
    CAMBRIDGE_P,
    FLARM_P,
    FLARMBIN_P,
    FLARMHOST_P,
    GARMIN_P,
    JUMBOCMD_P,
    KRT2_REMOTE_P, // <- 10
    MAGSENS_P,
    MAGSENSBIN_P,
    NMEASTD_P,
    OPENVARIO_P,
    REGISTRATION_P, // CAN id registration
    XCVARIO_P,
    XCVQUERY_P,
    XCVSYNC_P,
    TEST_P
} ProtocolType;


constexpr int CAN_REG_PORT = 0x7f0;

class Message;

namespace DEV
{

Message* acqMessage(DeviceId target, int port);
inline void relMessage(Message *msg);

bool Send(Message* msg);

}
