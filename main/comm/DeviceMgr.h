/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "Devices.h"
#include "DataLink.h"
#include "InterfaceCtrl.h"

#include <set>
#include <map>
#include <vector>

class DataLink;
class InterfaceCtrl;
class DeviceManager;

// global variable
extern DeviceManager* DEVMAN;


// A device describes the set of static and runtime relevant information
// to configure the comm links and access the device.
//
// Static parts of a supported device are id, protocol type(s), ..
// Runtime relevant info is the link it is connected to, the port(s) it is using for communication.
//
// Additionally a routing table that contains all the connected devices relation.

struct Device
{
    Device(DeviceId id) : _id(id) {}
    ~Device();
    int getSendPort(ProtocolType p) const;
    ProtocolItf* getProtocol(ProtocolType p) const;
    const DeviceId      _id;
    std::set<DataLink*> _dlink;
    InterfaceCtrl      *_itf;
};
    // InterfaceId                 _default_config; // RS232, Baudrate, etc.

// Information to first hand preconfigure an interface at the moment a device gets added to the system
struct InterfaceConfig
{
    InterfaceId     _id;
    // BaudRate        _xxx;
    // etc
};


// Device configuration options
// This describes all selectable options in terms of device connectivity for this system.
struct DevConfigItem
{
    DeviceId        _id;
    ProtocolType    _prtcl;
    int             _port; // can be 0 for devices that register each time
    InterfaceId     _intfId;
    InterfaceConfig _intfConfig;
};

// Routing table on device level, details on protocol requirements are not defined here
typedef std::vector<RoutingTarget> RoutingList;
typedef std::map<RoutingTarget, RoutingList> RoutingMap;


// A centeral for holding the device info at run time.
// The information might be from nvs configuration, or initially gathered from an 
// auto/manual configuration.
// There is no performance requirement as a router e.g. would rise. The
// collection of info is for the couple of moments you need to know where 
// to find e.g. the FLARM device as from an initiative communication (rare).
// Or for the boot-up configuration of interfaces, data link layer, and protocol association.

// As a result it establishes the device list mapping device id's to an interface. Accessing the
// hash map will act as routing table for each message sent to a connected device.

class DeviceManager
{
    typedef std::map<DeviceId, Device*> DevMap;

private:
    DeviceManager();

public:
    ~DeviceManager();
    static DeviceManager* Instance();
    // API
    ProtocolItf* addDevice(DeviceId dev, ProtocolType proto, int listen_port, int send_port, InterfaceId iid);
    Device* getDevice(DeviceId did);
    ProtocolItf* getProtocol(DeviceId dev, ProtocolType proto);
    // Remove device of this type
    void removeDevice(DeviceId did);
    InterfaceCtrl* getIntf(DeviceId did);
    RoutingList getRouting(RoutingTarget t);
    static int nrDevs() { return (DEVMAN) ? DEVMAN->getNrDevs() : 0; }
    int getNrDevs() const { return _device_map.size(); }
    // Search for the next free CAN id, organized in chunks of four in 5 prio categories.
    static int getFreeCANId(int prio);
    // debugging
    void dumpMap() const;

private:
    // Complete list of all devices known at runtime
    // Restriction: It can only contain one element of one device type
    // Hash table for routing purpose
    DevMap _device_map;
};

