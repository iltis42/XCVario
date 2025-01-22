/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "Devices.h"
#include "protocol/ProtocolItf.h"
#include "InterfaceCtrl.h"

#include <vector>

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
    int getPort() const { return _itf_id.port; }
    int getItf() const { return _itf_id.iid; }
    bool goBIN(ProtocolItf *peer);
    void goNMEA();
    int getNumNMEA() const;
    // dbg
    void dumpProto();

private:
    // helpers
    void forwardMsg(DeviceId src_dev);

private:
    std::vector<ProtocolItf*> _all_p;
    // std::deque<ProtocolItf*> _current_p;
    ProtocolItf *_nmea   = nullptr; // The protocoll in case there is only one of the kind
    ProtocolItf *_binary = nullptr; // If set it will be the priority parser
    ProtocolState _sm; // The message buffer for all protocol parser
    // Listen on
    const ItfTarget _itf_id;
};
