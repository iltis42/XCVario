/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "Devices.h"

class ProtocolItf;

// Data link layer to multiplex data stream to proper protocol parser.
class DataLink
{
public:
    DataLink(int listen_port) : _port(listen_port) {}
    ~DataLink();
    ProtocolItf* setProtocol(ProtocolType ptyp, int mport=0);
    ProtocolItf* getProtocol(ProtocolType ptyp) const ;
    bool hasProtocol(ProtocolType ptyp) const;
    ProtocolItf* removeProtocol();
    void process(const char *packet, int len);
    int getPort() const { return _port; }

private:
    ProtocolItf *_protocol = nullptr;
    bool _binary_mode = false;
    // Router info
    int _port = 0;
};
