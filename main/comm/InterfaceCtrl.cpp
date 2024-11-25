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
    RemoveAllDataLinks();
}

// get/create data link for this port
DataLink* InterfaceCtrl::getDataLink(int port)
{
    if ( one_to_one ) {
        // Always replace
        if ( ! _onedlink ) {
            _onedlink = new DataLink(port);
        }
        return _onedlink;
    } else {
        // Should be a different port to all in the list
        for (auto it = _dlink.begin(); it != _dlink.end(); it++ ) {
            if ( (*it)->getPort() == port ) {
                return (*it);
            }
        }
        DataLink *newdl = new DataLink(port);
        _dlink.push_back(newdl);
        return newdl;
    }
}

void InterfaceCtrl::RemoveDataLink(int port)
{
    if ( one_to_one ) {
        if ( _onedlink ) {
            delete _onedlink;
            _onedlink = nullptr;
        }
    } else {
        for (auto it = _dlink.begin(); it != _dlink.end(); ) {
            if ( (*it)->getPort() == port ) {
                delete (*it);
                it = _dlink.erase(it);
                break;
            }
            ++it;
        }
    }
}

void InterfaceCtrl::RemoveAllDataLinks()
{
    for (auto &it : _dlink ) {
        delete it;
    }
    _dlink.clear();
}
