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

static DeviceId new_device;
static InterfaceId new_interface;

static int remove_device(SetupMenuSelect *p)
{
    if ( p->getSelect() == 1 ) {
        DeviceId did = (DeviceId)p->getParent()->getContId(); // dev id to remove
        ESP_LOGI(FNAME, "remove %d", did);
        DeviceManager *dm = DeviceManager::Instance();
        dm->removeDevice(did);
        p->getParent()->getParent()->setDirty();
    }
    return 0;
}


static int create_device(SetupMenuSelect *p)
{
    if ( p->getSelect() == 1 ) {
        DeviceManager *dm = DeviceManager::Instance();
        dm->addDevice(new_device, NO_ONE, 0, 0, new_interface);
    }
    // // remove dyn. menus
    // SetupMenu *top = p->getParent();
    // MenuEntry *ent = top->getEntry(1);
    // top->delEntry(ent);
    // ent = top->getEntry(1);
    // top->delEntry(ent);
    return 0;
}

static int new_interface_action(SetupMenuSelect *p)
{
    SetupMenu *top = p->getParent();
    // memorize interface
    new_interface = (InterfaceId)p->getValue();
    ESP_LOGI(FNAME, "nr childs %s: %d", top->getTitle(), top->getNrChilds());
    if ( top->getNrChilds() < 4 ) {
        SetupMenuSelect *confirm = new SetupMenuSelect("Really", RST_NONE, create_device);
        confirm->mkConfirm();
        // confirm->setOnce();
        top->addEntry(confirm);
        top->highlightLast();
    }
    return 0;
}

static int new_device_action(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME,"action did %d", p->getValue());
    SetupMenu *top = p->getParent();
    // memorize and lock the device selector
    new_device = (DeviceId)p->getValue();
    p->lock();
    SetupMenuSelect *text = new SetupMenuSelect("connected to", RST_NONE);
    text->lock();
    top->addEntry(text);
    SetupMenuSelect *interface = new SetupMenuSelect("Interface", RST_NONE, new_interface_action);
    DeviceManager *dm = DeviceManager::Instance();
    for ( auto iid : DeviceManager::allKnownIntfs() ) {
        if ( dm->isAvail(iid) ) {
            interface->addEntry(DeviceManager::getItfName(iid).data(), iid);
        }
    }
    top->addEntry(interface);
    top->highlightLast();

    return 0;
}

static void system_menu_add_device(SetupMenu *top)
{
    ESP_LOGI(FNAME,"Create new device menu");
    SetupMenuSelect *ndev = static_cast<SetupMenuSelect*>(top->getEntry(0));
    if ( ! ndev ) {
        ndev = new SetupMenuSelect("Device", RST_NONE, new_device_action, false);
        top->addEntry(ndev);
    }
    DeviceManager *dm = DeviceManager::Instance();
    ndev->delAllEntries();
    for ( auto did : DeviceManager::allKnownDevs() ) {
        if ( ! dm->getDevice(did) ) {
            ndev->addEntry(DeviceManager::getDevName(did).data(), did);
        }
    }
}

static int start_dm_action(SetupAction* p)
{
    // DEVMAN->startDM((DeviceId)p->getCode());
    return 0;
}

static void system_menu_device(SetupMenu *top)
{
    DeviceId did = (DeviceId)top->getContId();
    DeviceManager *dm = DeviceManager::Instance();
    Device *dev = dm->getDevice(did);
    // dev->
    SetupAction *monitor = new SetupAction("Start data monitor", start_dm_action, did);
    top->addEntry(monitor);
    SetupMenuSelect *remove = new SetupMenuSelect("Remove device", RST_NONE,remove_device, false, nullptr, false, false);
    remove->mkConfirm();
    top->addEntry(remove);
}

void system_menu_connected_devices(SetupMenu *top)
{
    SetupMenu *adddev = static_cast<SetupMenu*>(top->getEntry(0));
    if ( ! adddev ) {
        adddev = new SetupMenu("Add Device", system_menu_add_device);
        adddev->setHelp("Get XCVario to know about connected devices, enable it to handling data routes.");
        adddev->setDynContent();
        top->addEntry(adddev);
    }

    DeviceManager *dm = DeviceManager::Instance();
    MenuEntry *ent = top->getEntry(1);
    while ( ent ) {
        ESP_LOGI(FNAME, "del1 #%d", top->getNrChilds());
        top->delEntry(ent);
        ent = top->getEntry(1);
    }
    for ( auto dev : dm->allDevs() ) {
        std::string_view dnam = DeviceManager::getDevName(dev->_id);
        if ( ! dnam.empty() ) {
            SetupMenu *devmenu = new SetupMenu(dnam.data(), system_menu_device, dev->_id);
            devmenu->setHelp("Device details.");
            top->addEntry(devmenu);
        }
    }
}
