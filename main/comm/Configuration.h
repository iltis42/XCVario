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


// Target device, or routing targets
union RoutingTarget {
    struct {
        DeviceId    did  : 7;
        int         intf_port : 25;
    };
    uint32_t raw = 0; // Access the packed 32-bit value

    // Convenience
    constexpr RoutingTarget(DeviceId did, ItfTarget itfp=0)
        : raw((did & 0x7f) | (itfp.raw & 0x1ffffff) << 7) {}
    constexpr RoutingTarget(DeviceId did, InterfaceId itf, int port)
        : raw((did & 0x7f) | ItfTarget(itf, port).raw << 7) {}
    constexpr RoutingTarget(uint32_t r)
        : raw(r) {}

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
    // Comparison operator for SetupNG
    constexpr bool operator==(const RoutingTarget& other) const {
        return raw == other.raw;
    }
    // For the route search
    bool match(const RoutingTarget& target) const {
        return ( did == target.did && getItfTarget().matchNoPhy(target.getItfTarget()) );
    }
};

using RoutingList = std::vector<RoutingTarget>;


// This stores:
//   up to 12 interface id's that can the device connect to
//   .. or up to 12 protocol id's that match to the devices
//   other attributes in a 4 bit bitfield

// A packed array of 5bit integral values
union PackedInt5Array {
    struct{
        uint64_t value  : 60;
        int      flags  : 4;
    };
    uint64_t data = 0; // 64-bit storage

    static constexpr int maxSize = 12;
    using ItfArray = std::array<InterfaceId, maxSize>;
    using PrtclArray = std::array<ProtocolType, maxSize>;

    PackedInt5Array() = default;
    constexpr PackedInt5Array(uint64_t d) : data(d) {}
    constexpr PackedInt5Array(ItfArray itf, int flgs=0) {
        data = pack(itf);
        data |= uint64_t(flgs)<<60;
    }
    constexpr PackedInt5Array(PrtclArray proto, int flgs=0) {
        data = pack(proto);
        data |= uint64_t(flgs)<<60;
    }


    // constexpr function to pack values at compile time
    static constexpr uint64_t pack(ItfArray vals) {
        uint64_t packed = 0;
        for (int i = 0; i < 12; ++i) {
            packed |= (static_cast<uint64_t>(vals[i]) & 0x1f) << (i * 5);
        }
        return packed;
    }
    static constexpr uint64_t pack(PrtclArray vals) {
        uint64_t packed = 0;
        for (int i = 0; i < 12; ++i) {
            packed |= (static_cast<uint64_t>(vals[i]) & 0x1f) << (i * 5);
        }
        return packed;
    }

    // Get value at runtime
    constexpr int get(int index) const {
        return (data >> (index * 5)) & 0x1f;
    }
    constexpr InterfaceId itf(int index) const {
        return static_cast<InterfaceId>(get(index));
    }
    constexpr ProtocolType proto(int index) const {
        return static_cast<ProtocolType>(get(index));
    }
    constexpr int getExtra() const { return flags; }

    // Set value at runtime
    void set(int index, int datum) {
        data &= ~(0x1f << (index * 5));
        data |= (static_cast<uint64_t>(datum) & 0x1f) << (index * 5);
    }

    // Compare
    constexpr bool operator==(const PackedInt5Array& other) const {
        return data == other.data;
    }
};

// some common flag constants
constexpr int IS_REAL       = 0x01;
constexpr int MULTI_CONF    = 0x02;
constexpr int HAS_PROFILE   = 0x04;
constexpr int IS_VARIANT    = 0x08; // Same device id, just another option to connect

// Flash stored device invariant attributes
struct DeviceAttributes
{
    std::string_view    name; // a readable name
    PackedInt5Array     itfs; // interface options
    PackedInt5Array     prcols; // protocol options, with nr of mandatory protos (from left)
    uint16_t            port; // optional default port

    union {
        struct {
            int isReal      : 1; // a device with a physical realastate and meant to be selectable
            int multipleConf: 1; // can be configured in multiple steps (e.g. Navi)
            int profileConf : 1; // protocols are organized through a profile, instead of a list
            int aVariant    : 1; // just a variant of the connection details, not a new device entry
        };
        int flags;
    };

    constexpr DeviceAttributes(std::string_view n, PackedInt5Array i, PackedInt5Array p, int o=0, int f=0 ) 
        : name(n), itfs(i), prcols(p), port(o), flags(f) {}

    bool isSelectable() const { return (bool)isReal; }
    bool isMultiConf() const { return (bool)multipleConf; }
    bool hasProfile() const { return (bool)profileConf; }
    bool isVariant() const { return (bool)aVariant; }

};

// Device serialization
struct DeviceNVS
{
    RoutingTarget   target; // devid, interface and port
    PackedInt5Array proto;  // all protocols

    // Default constructor for initialization
    constexpr DeviceNVS() = default;
    constexpr DeviceNVS(RoutingTarget t, PackedInt5Array::PrtclArray p, int f) : target(t), proto(p, f) {}
    constexpr DeviceNVS(uint32_t t, uint64_t p) : target(t), proto(p) {}
    
    constexpr bool operator==(const DeviceNVS& other) const {
        return target == other.target && proto == other.proto;
    }
};

