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

#include <vector>
#include <set>

using PortList = std::set<int>;
class NmeaPrtcl;

// Data link layer to multiplex data stream to proper protocol parser.
class DataLink
{
public:
    DataLink(int listen_port, int itfid) : _itf_id(ItfTarget((InterfaceId)itfid,listen_port)) {}
    ~DataLink();
    ProtocolItf* addProtocol(ProtocolType ptyp, DeviceId did, int sendport=0);
    ProtocolItf* getProtocol(ProtocolType ptyp=NO_ONE) const;
    bool hasProtocol(ProtocolType ptyp) const;
    void deleteProtocol(ProtocolItf *proto);
    void process(const char *packet, int len);
    ProtocolItf *goBIN();
    void goNMEA();
    void switchProtocol();
    ProtocolItf *getBinary() const;
    void updateRoutes();
    int getPort() const { return _itf_id.port; } // the listen port
    int getItfId() const { return _itf_id.iid; }
    PortList getAllSendPorts() const;
    // dbg
    void dumpProto();

private:
    // helpers
    void forwardMsg(DeviceId src_dev);
    NmeaPrtcl *enforceNmea(DeviceId did, int sendport, ProtocolType ptyp);

private:
    ProtocolItf *_active = nullptr; // the currently used one
    ProtocolItf *_nmea   = nullptr; // the nmea protocoll shell
    ProtocolItf *_binary = nullptr; // if set it will be the priority parser
    ProtocolState _sm; // The message buffer for all protocol parser
    // Listen on
    const ItfTarget _itf_id;
    // All protocols attached to this data linnk should have the same device id, here just for opt. checks
    DeviceId    _did = NO_DEVICE;
    // Routing
    RoutingList _routes;
};
