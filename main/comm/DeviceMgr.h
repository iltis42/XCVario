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
#include "Messages.h"
#include "InterfaceCtrl.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <set>
#include <map>
#include <vector>

class DataLink;
class InterfaceCtrl;
class ProtocolItf;
class DeviceManager;

// global variable
extern DeviceManager* DEVMAN;


// A device describes the set of static and runtime relevant information
// to configure the comm links and access the device.
//
// Static parts of a supported device are id, protocol type(s), ..
// Runtime relevant info is the link it is connected to, the port(s) it is using for communication.
//

struct Device
{
    Device(DeviceId id) : _id(id) {}
    ~Device();
    int getSendPort(ProtocolType p) const;
    ProtocolItf* getProtocol(ProtocolType p) const;
    DataLink *getDLforProtocol(ProtocolType p) const;
    EnumList getSendPortList() const;
    int getListenPort() const;
    bool isAlive() const { return true; } // fixme
    DeviceNVS getNvsData() const;
    // Attributes
    const DeviceId      _id;
    DataLink           *_link = nullptr; // can have only one link layer, but it might be shared with other devs
    EnumList            _protos = {}; // list of protocols belonging to this device, needed to remove a devices properly
    InterfaceCtrl      *_itf = nullptr;
};


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
    using DevMap = std::map<DeviceId, Device*>; // dynamic RAM data
    using ItfMap = std::map<InterfaceId, InterfaceCtrl*>; // just a cached Itf summary for quick access

private:
    DeviceManager();

public:
    ~DeviceManager();
    static DeviceManager* Instance();
    // API
    Device* addDevice(DeviceId dev, ProtocolType proto, int listen_port, int send_port, InterfaceId iid, bool nvsave=false);
    Device* getDevice(DeviceId did);
    Device* getXCVPeer();
    ProtocolItf *getProtocol(DeviceId dev, ProtocolType proto);
    NmeaPrtcl *getNMEA(DeviceId did);
    int getSendPort(DeviceId did, ProtocolType proto);
    // Remove device of this type
    bool removeDevice(DeviceId did, bool nvsave=false);
    InterfaceCtrl* getIntf(DeviceId did);
    bool isIntf(ItfTarget iid) const;
    bool isAvail(InterfaceId iid) const;
    RoutingList getRouting(RoutingTarget t);
    void refreshRouteCache();
    void setFlarmBPInitiator(DataLink *dl);
    DataLink *getFlarmBPInitiator();
    static int nrDevs() { return (DEVMAN) ? DEVMAN->getNrDevs() : 0; }
    int getNrDevs() const { return _device_map.size(); }
    // void makePersistent();
    void reserectFromNvs();
    void introduceDevices();
    // Search for the next free CAN id, organized in chunks of four in 5 prio categories.
    static int reserveCANId(int prio);
    static void undoReserveCANId(int prio);
    // debugging
    void dumpMap() const;
    void startMonitoring(ItfTarget iid); // Data Monitor
    void stopMonitoring();

    // Setup access
    static const DeviceAttributes& getDevAttr(DeviceId did, InterfaceId via=NO_PHY);
    static std::string_view getDevName(DeviceId did);
    static std::vector<DeviceId> allKnownDevs();
    static std::string_view getItfName(InterfaceId iid);
    static std::vector<InterfaceId> allKnownIntfs();
    static std::string_view getPrtclName(ProtocolType pid);
    std::vector<const Device *> allDevs() const;
    // Enforce the proper interface configuration
    static void EnforceIntfConfig(InterfaceId iid, DeviceId did);

private:
    // Complete list of all devices known at runtime
    // Restriction: It can only contain one element of one device type
    // Hash table for routing purpose
    DevMap _device_map;
    ItfMap _interface_map; // as convenience cashed access to interfaces, dev map is primary source of info
    mutable SemaphoreHandle_t _devmap_mutex;
    // Flarm specific trace to handle the Flarm binary protocol
    DataLink *_flarm_bp = nullptr; // the flarm binary protocol initiator
};

