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
#include <set>
#include <array>
#include <string_view>


// Target device, or routing targets
union RoutingTarget {
    struct {
        DeviceId did       : 7;
        unsigned intf_port : 25;
    };
    uint32_t raw = 0; // Access the packed 32-bit value

    // Default constructor for initialization
    constexpr RoutingTarget(int r = 0) : raw(r) {}
    // Convenience ctor's
    constexpr RoutingTarget(DeviceId did, ItfTarget itfp=0)
        : raw((did & 0x7f) | (itfp.raw & 0x1ffffff) << 7) {}
    constexpr RoutingTarget(DeviceId did, InterfaceId itf, int port)
        : raw((did & 0x7f) | ItfTarget(itf, port).raw << 7) {}
    constexpr RoutingTarget(uint32_t r)
        : raw(r) {}
    // getter
    constexpr DeviceId getDeviceId() const { return did; }
    constexpr ItfTarget getItfTarget() const { return static_cast<ItfTarget>(raw>>7); }
    constexpr InterfaceId getItfId() const { return static_cast<ItfTarget>(raw>>7).iid; }
    constexpr int getPort() const { return static_cast<ItfTarget>(raw>>7).port; }
    //setter
    constexpr void setDeviceId(DeviceId d) { did = d; }
    constexpr void setItfTarget(ItfTarget i) { intf_port = static_cast<int>(i.raw); }
    constexpr void setItfId(InterfaceId id) { intf_port &= ~0x3f; intf_port |= (id & 0x3f); } // knowing ItfTarget
    constexpr void setItfPort(int p) { intf_port &= ~(0x1fffff << 4); intf_port |= ((p & 0x1fffff) << 4); }

    // Comparison operator for std::map (based on raw)
    constexpr bool operator<(const RoutingTarget& other) const { return raw < other.raw; }
    // Comparison operator for SetupNG
    constexpr bool operator==(const RoutingTarget& other) const { return raw == other.raw; }
    // For the route search
    bool match(const RoutingTarget& target) const { return ( did == target.did && getItfTarget().matchNoPhy(target.getItfTarget()) ); }

};

using RoutingList = std::set<RoutingTarget>;


// This stores:
//   up to 12 interface id's that can the device connect to
//   .. or up to 12 protocol id's that match to the devices
//   other attributes in a 4 bit bitfield

// A packed array of 5bit integral values
union PackedInt5Array {
    struct{
        uint64_t value  : 60;
        unsigned flags  : 4;
    };
    uint64_t data = 0; // 64-bit storage

    static constexpr int maxSize = 12;
    using ItfArray = std::array<InterfaceId, maxSize>;
    using PrtclArray = std::array<ProtocolType, maxSize>;

    constexpr PackedInt5Array() = default;
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
        for (int i = 0; i < maxSize; ++i) {
            packed |= (static_cast<uint64_t>(vals[i]) & 0x1f) << (i * 5);
        }
        return packed;
    }
    static constexpr uint64_t pack(PrtclArray vals) {
        uint64_t packed = 0;
        for (int i = 0; i < maxSize; ++i) {
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
        if ( index < maxSize ) {
            data &= ~(0x1f << (index * 5));
            data |= (static_cast<uint64_t>(datum) & 0x1f) << (index * 5);
        }
    }

    // Compare
    constexpr bool operator==(const PackedInt5Array& other) const {
        return data == other.data;
    }
};


// a small protocol list (up to 6 x 5bit items)
union ProtocolList {
    struct{
        unsigned proto : 30;
        unsigned flags : 2;
    };
    uint32_t data = 0; // 32-bit storage

    static constexpr int maxProto = 6;
    using PsmallArray = std::array<ProtocolType, maxProto>;

    constexpr ProtocolList() = default;
    constexpr ProtocolList(uint32_t d) : data(d) {}
    constexpr ProtocolList(PsmallArray p, int flgs) {
        data = 0;
        data |= pack(p);
        data |= uint32_t(flgs)<<30;
    }

    // constexpr function to pack values at compile time
    static constexpr uint32_t pack(PsmallArray vals) {
        uint32_t packed = 0;
        for (int i = 0; i < maxProto; ++i) {
            packed |= (static_cast<uint32_t>(vals[i]) & 0x1f) << (i * 5);
        }
        return packed;
    }

    // Get protocol at runtime
    constexpr int get(int index) const {
        return (data >> (index*5)) & 0x1f;
    }
    constexpr ProtocolType getProto(int index) const {
        return (index >= 0 && index < maxProto) ? static_cast<ProtocolType>(get(index)) : ProtocolType::NO_ONE; 
    }
    constexpr int getFlags() const { return flags; }

    // Set value at runtime
    void set(int index, int datum) {
        if ( index < maxProto ) {
            data &= ~((uint32_t)0x1f << (index*5));
            data |= ((uint32_t)(datum) & 0x1f) << (index*5);
        }
    }

    // Compare
    constexpr bool operator==(const ProtocolList& other) const {
        return data == other.data;
    }
};

//
// Device serialization
//
// the structure belongs through the SetupNG list to a device, the device id is contained redundently and does not
// imply a "never change" (!). It might be used for checks.
// SetupNG variable <--> DeviceNVS
// DeviceNVS -> dev+interface+port and protocols all from ONE data link, potentially different ports for NMEA and binary protocols
//              protocol list: 0: the bin proto, 1: the principal nmea 2-5: plugins as far as they are not auto components of the pricipal proto
// setup.flag := 1 is used as valid hint
struct DeviceNVS
{
    // all just for one data link that belongs to the device
    RoutingTarget   target; // devid, interface and listen port
    ProtocolList    setup;  // send port and all protocols
    uint16_t        bin_sp; // binary send port
    uint16_t        nmea_sp; // nmea send port
    
    // default constructor for initialization
    constexpr DeviceNVS() = default;
    constexpr DeviceNVS(const DeviceNVS&) = default;
    constexpr DeviceNVS(RoutingTarget t, ProtocolList s, uint16_t bp, uint16_t np) : 
        target(t), setup(s), bin_sp(bp), nmea_sp(np) { setup.flags=1; }
    constexpr DeviceNVS(uint32_t t, uint32_t s, uint16_t bp, uint16_t np) : target(t), setup(s), bin_sp(bp), nmea_sp(np) { setup.flags=1; }
    
    // getter
    constexpr int getBinSPort() const { return bin_sp; }
    constexpr int getNmeaSPort() const { return nmea_sp; }
    constexpr bool isValid() const { return setup.flags&1; } // a default ctor will always set to zero
    
    // compare
    constexpr bool operator==(const DeviceNVS& other) const {
        return target == other.target && setup == other.setup
                && bin_sp == other.bin_sp && nmea_sp == other.nmea_sp;
    }
};



// some common flag constants
constexpr int IS_REAL       = 0x01;
constexpr int MULTI_CONF    = 0x02;
constexpr int HAS_PROFILE   = 0x04;
constexpr int IS_VARIANT    = 0x08; // Same device id, just another option to connect
constexpr int MASTER_ONLY   = 0x10;
constexpr int SECOND_ONLY   = 0x20;

template<typename T>
class SetupNG;

using NvsPtr = SetupNG<DeviceNVS>*;

// Flash stored device invariant attributes
struct DeviceAttributes
{
    std::string_view    name; // a readable name
    PackedInt5Array     itfs; // interface options
    PackedInt5Array     prcols; // protocol options, with nr of mandatory protos (from left)
    uint16_t            port; // optional default port

    union {
        struct {
            unsigned isReal      : 1; // a device with a physical realastate and meant to be selectable
            unsigned multipleConf: 1; // can be configured in multiple steps (e.g. Navi)
            unsigned profileConf : 1; // protocols are organized through a profile, instead of a list
            unsigned aVariant    : 1; // just a variant of the connection details, not a new device entry
            unsigned roleDepndent: 2; // 0=independant, 1=master only, 2=second only
        };
        uint8_t flags;
    };
    NvsPtr nvsetup;             // the persistent version of the current device setup

    constexpr DeviceAttributes(std::string_view n, PackedInt5Array i, PackedInt5Array p, int o, int f, NvsPtr setup ) 
        : name(n), itfs(i), prcols(p), port(o), flags(f), nvsetup(setup) {}

    bool isSelectable() const { return nvsetup!=nullptr && !name.empty(); }
    bool isMultiConf() const { return (bool)multipleConf; }
    bool hasProfile() const { return (bool)profileConf; }
    bool isVariant() const { return (bool)aVariant; }
    unsigned getRoleDep() const { return roleDepndent; }
    bool roleFit(int role) const { return ( !roleDepndent || !role || (roleDepndent == role) ); }
};

