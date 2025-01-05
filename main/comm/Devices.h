/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <cinttypes>

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


// Routing targets
struct RoutingTarget {
    union {
        struct {
            DeviceId    _target_dev  : 8;
            int         _target_port : 24;
        };
        uint32_t raw; // Access the packed 32-bit value
    };

    // Convenience
    constexpr RoutingTarget(DeviceId did, int port)
        : raw((static_cast<DeviceId>(did) & 0xff) | ((static_cast<int>(port) & 0xffffff) << 8)) {}

    // Default constructor for initialization
    constexpr RoutingTarget(int r = 0) : raw(r) {}

    // Comparison operator for std::map (based on raw)
    constexpr bool operator<(const RoutingTarget& other) const {
        return raw < other.raw;
    }
};

constexpr int CAN_REG_PORT = 0x7f0;

class Message;

namespace DEV {

Message* acqMessage(DeviceId target, int port);
inline void relMessage(Message *msg);

bool Send(Message* msg);

}
