/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "DeviceMgr.h"

#include "InterfaceCtrl.h"

#include <logdef.h>

DeviceManager* DEVMAN = nullptr; // singleton like

DeviceManager* DeviceManager::Instance()
{
    if ( ! DEVMAN ) {
        DEVMAN = new DeviceManager();
    }
    return DEVMAN;
}

// Do all it needs to prepare comm with device and route data
// It returns the pointer to the protocol as handle to send messages
ProtocolItf* DeviceManager::addDevice(DeviceId did, ProtocolType proto, int listen_port, int send_port, InterfaceCtrl *itf)
{
    Device *dev = getDevice(did);
    if ( ! dev ) {
        dev = new Device(did);
    }
    DataLink *dl = itf->getDataLink(listen_port);
    dev->_dlink.insert(dl);
    ProtocolItf* ret = dl->setProtocol(proto, send_port);
    dev->_itf = itf;

    _device_map[did] = dev; // and add
    ESP_LOGI(FNAME, "Add device %d", did);
    return ret;
}

Device *DeviceManager::getDevice(DeviceId did)
{
    DevMap::iterator it = _device_map.find(did);
    if ( it != _device_map.end() ) {
        return it->second;
    }
    return nullptr;
}

Device* DeviceManager::removeDevice(DeviceId did)
{
    DevMap::iterator it = _device_map.find(did);
    Device* ret = nullptr;
    if ( it != _device_map.end() ) {
        ret = it->second;
        ESP_LOGI(FNAME, "Delete device %d", did);
        _device_map.erase(it);
    }
    return ret;
}

// routing lookup table
InterfaceCtrl* DeviceManager::getIntf(const DeviceId did)
{
    DevMap::iterator it = _device_map.find(did);
    if ( it != _device_map.end() ) {
        return it->second->_itf;
    }
    ESP_LOGW(FNAME, "No device %d found", did);
    return nullptr;
}

// prio - 0,..,4 0:low prio data stream, .. 5: important high prio commanding
// return an id chunk of four to use (0..0x3fc), use then +0, +1, +2, +3
// else -1 in case there is no chunk left
int DeviceManager::getFreeCANId(int prio)
{
    static int nxt_free_slot[5] = { 0x400, 0x200, 0x100, 0x80, 0x40 }; // 5 prio slots of 15 chucks of 4 ids
    if ( prio > 4 ) {
        prio = 4;
    }
    if ( prio < 0 ) {
        prio = 0;
    }
    if ( (nxt_free_slot[prio] & 0x3c) != 0x3c ) { // chunk mask b0011 1100
        int slot = nxt_free_slot[prio];
        nxt_free_slot[prio] += 4; // next chunk of four
        return slot;
    }
    return -1;
}

// Resolve the existance of a device
Device::~Device()
{
    // Detach data links from interface
    for (DataLink* dl : _dlink) {
        _itf->RemoveDataLink(dl->getPort());
        delete dl;
    }
    // Clear the set
    _dlink.clear();
}

int Device::getPort(ProtocolType p)
{
    // Find port for protocol
    for (DataLink* dl : _dlink) {
        if ( dl->hasProtocol(p) ) {
            return dl->getPort();
        }
    }

    return 0;
}
