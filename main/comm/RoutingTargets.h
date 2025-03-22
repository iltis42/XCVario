/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "Devices.h"
#include "InterfaceCtrl.h"

#include <cinttypes>
#include <vector>


// Routing targets
union RoutingTarget {
    struct {
        DeviceId    did  : 7;
        int         intf_port : 25;
    };
    uint32_t raw; // Access the packed 32-bit value

    // Convenience
    constexpr RoutingTarget(DeviceId did, ItfTarget itfp=0)
        : raw((did & 0x7f) | (itfp.raw & 0x1ffffff) << 7) {}
    constexpr RoutingTarget(DeviceId did, InterfaceId itf, int port)
        : raw((did & 0x7f) | ItfTarget(itf, port).raw << 7) {}

    constexpr DeviceId getDeviceId() const {
        return did;
    }
    constexpr ItfTarget getItfTarget() const {
        return static_cast<ItfTarget>(raw>>7);
    }

    // Default constructor for initialization
    constexpr RoutingTarget(int r = 0) : raw(r) {}

    // Comparison operator for std::map (based on raw)
    constexpr bool operator<(const RoutingTarget& other) const {
        return raw < other.raw;
    }
    bool match(const RoutingTarget& target) const {
        return ( did == target.did && getItfTarget().matchNoPhy(target.getItfTarget()) );
    }
};

using RoutingList = std::vector<RoutingTarget>;


