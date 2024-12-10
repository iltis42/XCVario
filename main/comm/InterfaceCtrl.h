/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <vector>

class DataLink;

// All managed interfaces
typedef enum {
    NO_PHY = 0,
    CAN_BUS,
    S1_RS232,
    S2_RS232,
    WIFI,
    BLT
} InterfaceId;


// ISO/OSI 1..n relation from interface to data link layer.
// Because in the given embedded context a 1..1 relation is in most of the 
// use cases enough, the API can be locked down to just one data link.

class InterfaceCtrl
{
public:
    InterfaceCtrl(bool oto=false) : one_to_one(oto) {}
    virtual ~InterfaceCtrl();

    virtual InterfaceId getId() const { return NO_PHY; }
    virtual const char* getStringId() const { return ""; }
    virtual void ConfigureIntf(int cfg) = 0;
    virtual bool Send(const char *msg, int len, int port=0) = 0;
    DataLink* getDataLink(int port);
    int getNrDLinks() const { return _dlink.size(); }
    void DeleteDataLink(int port);
    void DeleteAllDataLinks();

protected:
    std::vector<DataLink*> _dlink;
    DataLink* _onedlink = nullptr;
private:
    bool one_to_one;
};