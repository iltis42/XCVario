/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "InterfaceCtrl.h"

#include "DataLink.h"
#include "SetupNG.h"

// 1..n relation from interface to data link layer
// Ability to set interface details through a common cotrol interface
// Add and remove data links to the interface

InterfaceCtrl::InterfaceCtrl(bool oto) :
    _one_to_one(oto)
{
    _dlink_mutex = xSemaphoreCreateMutex();
}

InterfaceCtrl::~InterfaceCtrl()
{
    DeleteAllDataLinks();
}

// get/create data link for this port
DataLink* InterfaceCtrl::newDataLink(int port)
{
    if ( _one_to_one ) {
        // Always reuse
        if ( _dlink.empty() ) {
            xSemaphoreTake(_dlink_mutex, portMAX_DELAY);
            _dlink[port] = new DataLink(port, getId());
            xSemaphoreGive(_dlink_mutex);
        }
        return _dlink.begin()->second;
    }
    else {
        // Should be a different port to all in the list, or reuse
        xSemaphoreTake(_dlink_mutex, portMAX_DELAY);
        auto it = _dlink.find(port);
        if ( it != _dlink.end()) {
            xSemaphoreGive(_dlink_mutex);
            return it->second;
        }
        DataLink *newdl = new DataLink(port, getId());
        _dlink[port] = newdl;
        xSemaphoreGive(_dlink_mutex);
        return newdl;
    }
}

// precondition: dl not yet in the map
void InterfaceCtrl::addDataLink(DataLink *dl)
{
    xSemaphoreTake(_dlink_mutex, portMAX_DELAY);
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
    xSemaphoreGive(_dlink_mutex);
}

// returns possibly a nullptr, when port is not found in the map
DataLink *InterfaceCtrl::MoveDataLink(int port)
{
    DataLink *tmp = nullptr;
    xSemaphoreTake(_dlink_mutex, portMAX_DELAY);
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
    xSemaphoreGive(_dlink_mutex);
    return tmp;
}

void InterfaceCtrl::DeleteDataLink(int port)
{
    xSemaphoreTake(_dlink_mutex, portMAX_DELAY);
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
    xSemaphoreGive(_dlink_mutex);
}

void InterfaceCtrl::DeleteAllDataLinks()
{
    for (auto &it : _dlink ) {
        delete it.second;
    }
    _dlink.clear();
}
