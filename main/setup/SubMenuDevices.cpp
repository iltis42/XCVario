/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "SetupMenu.h"
#include "SetupAction.h"
#include "comm/DeviceMgr.h"

#include <logdef.h>
#include <inttypes.h>
#include <array>
#include <string_view>
#include <algorithm>


// Define device names for the menu, only if they have a real a state (skip the virtual ones)
constexpr std::pair<DeviceId, std::string_view> DeviceNames[] = {
    {DeviceId::JUMBO_DEV, "jumbo putzi"},
    {DeviceId::ANEMOI_DEV, "Anemoi"},
    {DeviceId::XCVARIO_DEV, "Master XCVario"},
    {DeviceId::XCVARIOCLIENT_DEV, "Second XCVario"},
    {DeviceId::FLARM_DEV, "Flarm"},
    {DeviceId::NAVI_DEV, "Navi"},
    {DeviceId::MAGSENS_DEV, "Magnetic Sensor"},
    {DeviceId::RADIO_KRT2_DEV, "KRT 2"}
};

// Lookup function
constexpr std::string_view getDevName(DeviceId did) {
    for (const auto& entry : DeviceNames) {
        if (entry.first == did) {
            return entry.second;
        }
    }
    return "";
}

static int remove_device(SetupAction *p)
{
    ESP_LOGI(FNAME, "remove %d", p->getCode());
    return 0;
}

// static int callback2(SetupMenuSelect *p)
// {
//     return 0;
// }

static void system_menu_create_new_devices(SetupMenu *top)

{

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
        std::string_view dnam = getDevName(dev->_id);
        if ( ! dnam.empty() ) {
            SetupMenu *devmenu = new SetupMenu(dnam.data(), system_menu_create_dev);
            devmenu->setHelp("Device details.");
            top->addEntry(devmenu);
        }
    }
}
