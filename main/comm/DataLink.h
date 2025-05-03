/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "comm/Configuration.h"
#include "protocol/ProtocolItf.h"
#include "InterfaceCtrl.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <set>

using PortList = std::set<int>;
class NmeaPrtcl;
class ProtocolItf;

// Data link layer to multiplex data stream to proper protocol parser.
class DataLink
{
public:
    DataLink(int listen_port, int itfid);
    ~DataLink();
    ProtocolItf* addProtocol(ProtocolType ptyp, DeviceId did, int sendport=0);
    ProtocolItf* getProtocol(ProtocolType ptyp=NO_ONE) const;
    bool hasProtocol(ProtocolType ptyp) const;
    void deleteProtocol(ProtocolItf *proto);
    void process(const char *packet, int len);
    ProtocolItf *goBIN();
    void goNMEA();
    void switchProtocol();
    ProtocolItf *getBinary() const { return _binary; };
    NmeaPrtcl *getNmea() const { return _nmea; };
    void updateRoutes();
    int getPort() const { return _itf_id.port; } // the listen port
    InterfaceId getItfId() const { return _itf_id.iid; }
    ItfTarget getTarget() const { return _itf_id; }
    PortList getAllSendPorts() const;
    // house keeping
    void incrDeviceCount() { _dev_count++; }
    int decrDeviceCount() { return --_dev_count; }
    // dbg
    void dumpProto();
    bool isBinActive() const { return _active->isBinary(); }
    void setMonitor(bool m) { _monitoring = m; }

private:
    // helpers
    void doForward(DeviceId src_dev);
    void enforceNmea(DeviceId did, int sendport, ProtocolType ptyp);

private:
    ProtocolItf *_active = nullptr; // the currently used one
    NmeaPrtcl   *_nmea   = nullptr; // the nmea protocoll shell
    ProtocolItf *_binary = nullptr; // if set it will be the priority parser
    ProtocolState _sm; // The message buffer for all protocol parser
    // Listen on
    const ItfTarget _itf_id;
    bool _monitoring = false;
    // All protocols attached to this data link should have the same device id, here just for opt. checks
    DeviceId    _did = NO_DEVICE;
    // Routing
    RoutingList _routes;
    mutable SemaphoreHandle_t _route_mutex;
    // number of devices refering to this data link
    int _dev_count = 0; // only the last removed devices can delete the data link

};
