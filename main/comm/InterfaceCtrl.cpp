/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "InterfaceCtrl.h"

#include "DataLink.h"

// 1..n relation from interface to data link layer
// Ability to set interface details through a common cotrol interface
// Add and remove data links to the interface

InterfaceCtrl::~InterfaceCtrl()
{
    DeleteAllDataLinks();
}

// get/create data link for this port
DataLink* InterfaceCtrl::newDataLink(int port)
{
    if ( _one_to_one ) {
        // Always replace
        if ( ! _dlink.empty() && _dlink.begin()->first != port ) {
            DeleteAllDataLinks();
        }
        if ( _dlink.empty() ) {
            _dlink[port] = new DataLink(port);
        }
        return _dlink.begin()->second;
    }
    else {
        // Should be a different port to all in the list, or replace
        auto it = _dlink.find(port);
        if ( it != _dlink.end()) {
            return it->second;
        }
        DataLink *newdl = new DataLink(port);
        _dlink[port] = newdl;
        return newdl;
    }
}

// precondition: dl not yet in the map
void InterfaceCtrl::addDataLink(DataLink *dl)
{
    if ( _one_to_one ) {
        // Always replace
        if ( ! _dlink.empty() ) {
            DeleteAllDataLinks();
        }
        _dlink[dl->getPort()] = dl;
    }
    else {
        auto it = _dlink.find(dl->getPort());
        if ( it != _dlink.end() ) {
            DataLink *tmp = it->second;
            it->second = dl;
            delete tmp;
        }
        else {
            _dlink[dl->getPort()] = dl;
        }
    }
}

// returns possibly a nullptr, when port is not found in the map
DataLink *InterfaceCtrl::MoveDataLink(int port)
{
    DataLink *tmp = nullptr;
    if ( _one_to_one ) {
        tmp = _dlink.begin()->second;
        _dlink.clear();
    }
    else {
        auto it = _dlink.find(port);
        if ( it != _dlink.end() ) {
            tmp = it->second;
            _dlink.erase(it);
        }
    }
    return tmp;
}

void InterfaceCtrl::DeleteDataLink(int port)
{
    if ( _one_to_one ) {
        DeleteAllDataLinks();
    }
    else {
        auto it = _dlink.find(port);
        if ( it != _dlink.end() ) {
            DataLink *tmp = it->second;
            _dlink.erase(it);
            delete tmp;
        }
    }
}

void InterfaceCtrl::DeleteAllDataLinks()
{
    for (auto &it : _dlink ) {
        delete it.second;
    }
    _dlink.clear();
}
