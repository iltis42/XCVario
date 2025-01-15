/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <map>

class DataLink;

// All managed interfaces
typedef enum {
    NO_PHY = 0,
    CAN_BUS,
    I2C,
    S0_RS232,
    S1_RS232,
    S2_RS232,
    BT_SERIAL,
    WIFI,
    BLT
} InterfaceId;

// Interface target
struct ItfTarget {
    union {
        struct {
            InterfaceId iid  : 8;
            int         port : 24;
        };
        uint32_t raw;
    };

    // Convenience
    constexpr ItfTarget(InterfaceId iid, int port=0)
        : raw((static_cast<InterfaceId>(iid) & 0xff) | ((static_cast<int>(port) & 0xffffff) << 8)) {}
    constexpr ItfTarget(int r = 0) : raw(r) {}
    constexpr bool operator<(const ItfTarget& other) const {
        return raw < other.raw;
    }
    constexpr bool operator==(const ItfTarget& other) const {
        return raw == other.raw;
    }
    constexpr bool operator!=(const ItfTarget& other) const {
        return raw != other.raw;
    }
};

// ISO/OSI 1..n relation from interface to data link layer.
// Because in the given embedded context a 1..1 relation is in most of the 
// use cases enough, it can be locked down to just one data link.

class InterfaceCtrl
{
public:
    InterfaceCtrl(bool oto=false) : _one_to_one(oto) {}
    virtual ~InterfaceCtrl();

    virtual InterfaceId getId() const { return NO_PHY; }
    virtual const char* getStringId() const { return ""; }
    // Choose from an enumerated set of configurations, -1: OFF; 0: as is; 1,2,3..: load a preset profile 
    virtual void ConfigureIntf(int cfg) = 0;
    // if blocked returns number of ms for next possible invocation, len might be reduced by # bytes that fit into send buffers
    virtual int Send(const char *msg, int &len, int port=0) = 0;
    DataLink* newDataLink(int port);
    void addDataLink(DataLink *dl);
    DataLink* MoveDataLink(int port);
    void DeleteDataLink(int port);
    void DeleteAllDataLinks();
    int getNrDLinks() const { return _dlink.size(); }

protected:
    std::map<int, DataLink*> _dlink;
private:
    bool _one_to_one;
};