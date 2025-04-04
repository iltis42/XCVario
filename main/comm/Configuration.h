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
#include <array>
#include <string_view>


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


// This stores:
//   up to 5 interface id's that can the device connect to
//   up to 6 protocol id's that match to the devices
//   other attributes in a bitfield
union PackedAttributeList {
    struct{
        int  interfaces  : 20;
        int  protocols   : 30;
        bool isReal      : 1; // A device with a physical realastate vs. a virtual
        bool hasProfile  : 1; // Protocols are organized through a profile, instead of a list
        // bool reserved    : 5; // fill to 7 bits
    };
    uint64_t data = 0; // 64-bit storage

    // constexpr function to pack values at compile time
    static constexpr uint64_t stuff(std::array<InterfaceId, 5> vals) {
        uint64_t stuffed = 0;
        for (int i = 0; i < 5; ++i) {
            stuffed |= (static_cast<uint64_t>(vals[i]) & 0x1f) << (i * 5);
        }
        return stuffed;
    }

    static constexpr uint64_t pack(std::array<ProtocolType, 6> vals) {
        uint64_t packed = 0;
        for (int i = 0; i < 6; ++i) {
            packed |= (static_cast<uint64_t>(vals[i]) & 0x1f) << (i * 5 + 20);
        }
        return packed;
    }

    constexpr PackedAttributeList(std::array<InterfaceId, 5> itf, std::array<ProtocolType, 6> val, bool ir) {
        data = stuff(itf);
        data |= pack(val);
        data |= ir ? (uint64_t)1<<51 : 0;
    }

    // Get value at runtime
    constexpr ProtocolType get(int index) const {
        return static_cast<ProtocolType>((data >> (index * 5)) & 0x1f);
    }
};


// Flash stored device invariant attributes
struct DeviceAttributes
{
    std::string_view    name; // a readable name
    PackedAttributeList attr; // protocol dependencies, and other info

    constexpr DeviceAttributes(std::string_view n, PackedAttributeList a) : name(n), attr(a) {}
};