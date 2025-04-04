/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "SetupMenu.h"
#include "SetupMenuSelect.h"
#include "SetupAction.h"
#include "comm/DeviceMgr.h"
#include "logdef.h"

#include <cinttypes>
#include <string_view>



static int remove_device(SetupAction *p)
{
    ESP_LOGI(FNAME, "remove %d", p->getCode());
    return 0;
}

static int new_device_action(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME,"action did %d", p->getValue());
    return 0;
}

static void system_menu_create_new_devices(SetupMenu *top)
{
    SetupMenuSelect *ndev = new SetupMenuSelect("Device", RST_NONE, new_device_action, true);
    DeviceManager *dm = DeviceManager::Instance();
    for ( auto did : DeviceManager::allKnownDevs() ) {
        if ( ! dm->getDevice(did) ) {
        ndev->addEntry(DeviceManager::getDevName(did).data(), did);
        // std::string_view dnam = getDevName(dev->_id);
        // if ( ! dnam.empty() ) {
        //     SetupMenu *devmenu = new SetupMenu(dnam.data(), system_menu_create_dev);
        //     devmenu->setHelp("Device details.");
        //     top->addEntry(devmenu);
        }
    }
    top->addEntry(ndev);
}

static void system_menu_create_dev(SetupMenu *top)
{
    SetupAction *remove = new SetupAction("Remove device", remove_device, 0);
    top->addEntry(remove);
}

void system_menu_create_devices(SetupMenu *top)
{
    SetupMenu *newdev = new SetupMenu("Add Device", system_menu_create_new_devices);
    newdev->setHelp("Get XCVario to know about connected devices, enable it to handling data routes.");
    top->addEntry(newdev);

    DeviceManager *dm = DeviceManager::Instance();
    for ( auto dev : dm->allDevs() ) {
        std::string_view dnam = DeviceManager::getDevName(dev->_id);
        if ( ! dnam.empty() ) {
            SetupMenu *devmenu = new SetupMenu(dnam.data(), system_menu_create_dev);
            devmenu->setHelp("Device details.");
            top->addEntry(devmenu);
        }
    }
}
