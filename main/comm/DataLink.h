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

#include <vector>

// Data link layer to multiplex data stream to proper protocol parser.
class DataLink
{
public:
    DataLink(int listen_port) : _port(listen_port) {}
    ~DataLink();
    ProtocolItf* addProtocol(ProtocolType ptyp, DeviceId did, int sendport=0);
    ProtocolItf* getProtocol(ProtocolType ptyp=NO_ONE) const ;
    bool hasProtocol(ProtocolType ptyp) const;
    void process(const char *packet, int len);
    int getPort() const { return _port; }
    // dbg
    void dumpProto();

private:
    // helpers
    void forwardMsg(std::string &str, DeviceId odev);

private:
    std::vector<ProtocolItf*> _all_p;
    // std::deque<ProtocolItf*> _current_p;
    ProtocolItf *_gotit = nullptr; // The protocoll that recognized the current message
    ProtocolState _sm; // The message buffer for all protocol parser
    bool _binary_mode = false;
    // Listen on
    const int _port;
};
