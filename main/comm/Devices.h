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
    JUMBO_DEV,
    ANEMOI_DEV,
    XCVARIO_DEV,
    MASTER_DEV,  // CAN id registry
    FLARM_DEV,
    TEST_DEV,
    TEST_DEV2
} DeviceId;


// Supported protocol id's
typedef enum
{
    NO_ONE = 0, // Disable
    REGISTRATION_P, // CAN id registration 
    JUMBOCMD_P,
    ANEMOI_P,
    FLARM_P,
    FLARMHOST_P,
    MAGSENS_P,
    NMEA_P,
    TEST_P
} ProtocolType;

constexpr int CAN_REG_PORT = 0x7f0;

class Message;

namespace DEV {

Message* acqMessage(DeviceId target, int port);
inline void relMessage(Message *msg);

bool Send(Message* msg);

}
