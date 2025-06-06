/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "setup/SetupMenu.h"
#include "setup/SetupMenuSelect.h"
#include "setup/SetupMenuChar.h"
#include "setup/SetupMenuValFloat.h"
#include "SetupAction.h"
#include "comm/DeviceMgr.h"
#include "comm/CanBus.h"
#include "comm/SerialLine.h"
#include "comm/WifiApSta.h"
#include "setup/DataMonitor.h"
#include "sensor.h"
#include "logdef.h"

#include <esp_wifi.h>

#include <cinttypes>
#include <string_view>
#include "SubMenuDevices.h"

static DeviceId new_device;
static InterfaceId new_interface;
static std::string device_details;

//
// Navi flavors
//
static constexpr std::array<ProtocolType, 4> all_flavors = { XCVARIO_P, OPENVARIO_P, BORGELT_P, CAMBRIDGE_P };
static int getFlvEnumFromProto(ProtocolType ptyp)
{
    for (int i=0; i<all_flavors.size(); i++) {
        if ( all_flavors[i] == ptyp ) {
            return i;
        }
    }
    return 0; // default to XCVARIO_P
}

//
// Interfaces
//
static void connected_devices_menu_create_wifi(SetupMenu *top);
static void connected_devices_menu_create_bluetooth(SetupMenu *top);
static void connected_devices_menu_create_interfaceS1(SetupMenu *top);
static void connected_devices_menu_create_interfaceS2(SetupMenu *top);
static void connected_devices_menu_create_interfaceCAN(SetupMenu *top);
static void connected_devices_menu_create_interfaceI2C(SetupMenu *top);

static SetupMenuCreator_t get_itf_menu_creator(InterfaceId iid)
{
    if ( iid == WIFI_APSTA ) {
        return connected_devices_menu_create_wifi;
    }
    else if ( iid == BT_SPP ) {
        return connected_devices_menu_create_bluetooth;
    }
    else if ( iid == S1_RS232 ) {
        return connected_devices_menu_create_interfaceS1;
    }
    else if ( iid == S2_RS232 ) {
        return connected_devices_menu_create_interfaceS2;
    }
    else if ( iid == CAN_BUS ) {
        return connected_devices_menu_create_interfaceCAN;
    }
    else if ( iid == I2C ) {
        return connected_devices_menu_create_interfaceI2C;
    }
    return nullptr;
}

int update_wifi_power(SetupMenuValFloat *p)
{
    ESP_ERROR_CHECK(esp_wifi_set_max_tx_power(int(wifi_max_power.get() * 80.0 / 100.0)));
    return 0;
}

static int update_id(SetupMenuChar *p) {
	const char *c = p->value();
	ESP_LOGI(FNAME,"New Letter %c Index: %d", *c, (int)p->getCharIndex() );
	char id[10] = { 0 };
	strncpy(id, custom_wireless_id.get().id, 10);
	id[p->getCharIndex()] = *c;
	ESP_LOGI(FNAME,"New ID %s", id );
	custom_wireless_id.set(id);
	return 0;
}

static void wifi_menu_create_wireless_custom_id(SetupMenu *top)
{
    SetupMenuChar *c1 = new SetupMenuChar("Letter 1", RST_NONE, update_id, custom_wireless_id.get().id, 0);
    SetupMenuChar *c2 = new SetupMenuChar("Letter 2", RST_NONE, update_id, custom_wireless_id.get().id, 1);
    SetupMenuChar *c3 = new SetupMenuChar("Letter 3", RST_NONE, update_id, custom_wireless_id.get().id, 2);
    SetupMenuChar *c4 = new SetupMenuChar("Letter 4", RST_NONE, update_id, custom_wireless_id.get().id, 3);
    SetupMenuChar *c5 = new SetupMenuChar("Letter 5", RST_NONE, update_id, custom_wireless_id.get().id, 4);
    SetupMenuChar *c6 = new SetupMenuChar("Letter 6", RST_NONE, update_id, custom_wireless_id.get().id, 5);
    top->addEntry(c1);
    top->addEntry(c2);
    top->addEntry(c3);
    top->addEntry(c4);
    top->addEntry(c5);
    top->addEntry(c6);
    static const char keys[][4]{"", "0", "1", "2", "3", "4", "5", "6", "7",
                                "8", "9", "-", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J",
                                "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W",
                                "X", "Y", "Z"};
    c1->addEntryList(keys, sizeof(keys) / 4);
    c2->addEntryList(keys, sizeof(keys) / 4);
    c3->addEntryList(keys, sizeof(keys) / 4);
    c4->addEntryList(keys, sizeof(keys) / 4);
    c5->addEntryList(keys, sizeof(keys) / 4);
    c6->addEntryList(keys, sizeof(keys) / 4);
}

static int scan_for_master(SetupMenuSelect *p) {
    if ( p->getSelect() == 1 ) {
        ESP_LOGI(FNAME, "wifi scan for master");
        if ( WIFI->scanMaster(0) ) {
            p->setHelp("Successfully found a master XCVario, reboot to activate");
            p->scheduleReboot();
        }
        else {
            p->setHelp("Failed to scan for master XCVario, try again later");
        }
    }
    p->showhelp(true);
    vTaskDelay(pdMS_TO_TICKS(2000));
    p->setSelect(0);
    return 0;
}

static void options_menu_custom_id(SetupMenu *top)
{
    SetupMenu *cusid = new SetupMenu("Custom-ID", wifi_menu_create_wireless_custom_id);
    cusid->setBuzzword(static_cast<t_tenchar_id*>(custom_wireless_id.getPtr())->id);
    cusid->setHelp("Select custom ID (SSID) for wireless BT (or WIFI) interface, e.g. D-1234. Restart device to activate");
    top->addEntry(cusid);
}

static void connected_devices_menu_create_wifi(SetupMenu *top)
{
    SetupMenuValFloat *wifip = new SetupMenuValFloat("WIFI Power", "%", update_wifi_power, false, &wifi_max_power);
    wifip->setPrecision(0);
    wifip->setHelp("Maximum Wifi Power to be used 10..100% or 2..20dBm");
    top->addEntry(wifip);

    if ( xcv_role.get() == SECOND_ROLE ) {
        SetupMenuValFloat *masterid = new SetupMenuValFloat("Master Id XCVario-", "", nullptr, false, &master_xcvario);
        masterid->setHelp("Connect only to this master XCVario ID");
        masterid->lock();
        top->addEntry(masterid);
    
        SetupMenuSelect *wifimal = new SetupMenuSelect("Scan for Master", RST_NONE, scan_for_master);
        wifimal->setHelp("Scan for a master XCVario, and lock to it (cancel with button)");
        wifimal->addEntry("Cancel");
        wifimal->addEntry("Scan&Set");
        wifimal->setSelect(0); // default to cancel
        top->addEntry(wifimal);
    }
    options_menu_custom_id(top);
}

static void connected_devices_menu_create_bluetooth(SetupMenu *top)
{
    options_menu_custom_id(top);
}

static int update_s1_baud(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select baudrate: %d", p->getSelect());
    S1->setBaud((e_baud)(p->getSelect())); // 0: 4800, 1:9600, etc
    return 0;
}

static int update_s1_pol(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select RX/TX polarity: %d", p->getSelect());
    S1->setLineInverse(p->getSelect()); // 0 off, 1 invers or TTL (always both, RX/TX)
    return 0;
}

static int update_s1_pin(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select Pin Swap: %d", p->getSelect());
    S1->setPinSwap(p->getSelect()); // 0 normal (3:TX 4:RX), 1 swapped (3:RX 4:TX)
    return 0;
}

static int update_s1_txena(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select TX Enable: %d", p->getSelect());
    S1->setTxOn(p->getSelect()); // 0 RO Listener, 1 TX enabled
    return 0;
}

static void connected_devices_menu_create_interfaceS1(SetupMenu *top)
{
    SetupMenuSelect *s1sp2 = new SetupMenuSelect("Baudraute", RST_NONE, update_s1_baud, &serial1_speed);
    s1sp2->addEntry("4800 baud");
    s1sp2->addEntry("9600 baud");
    s1sp2->addEntry("19200 baud");
    s1sp2->addEntry("38400 baud");
    s1sp2->addEntry("57600 baud");
    s1sp2->addEntry("115200 baud");
    top->addEntry(s1sp2);

    SetupMenuSelect *stxi2 = new SetupMenuSelect("Signaling", RST_NONE, update_s1_pol, &serial1_ttl_signals);
    stxi2->setHelp("A logical '1' is represented by a negative voltage in RS232 Standard, whereas in RS232 TTL "
                    "uses a positive voltage");
    stxi2->addEntry("RS232 Standard");
    stxi2->addEntry("RS232 TTL");
    top->addEntry(stxi2);

    SetupMenuSelect *srxtw2 = new SetupMenuSelect("Swap RX/TX", RST_NONE, update_s1_pin, &serial1_pin_swap);
    srxtw2->setHelp("Option to swap RX and TX line, a Flarm needs Normal, a Navi usually swapped.");
    srxtw2->addEntry("Normal");
    srxtw2->addEntry("Swapped");
    top->addEntry(srxtw2);

    SetupMenuSelect *stxdis1 = new SetupMenuSelect("TX line", RST_NONE, update_s1_txena, &serial1_tx_enable);
    stxdis1->setHelp("Option to listen only on the RX line, disables TX line to receive only data");
    stxdis1->mkEnable();
    top->addEntry(stxdis1);
}

static int update_s2_baud(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select baudrate: %d", p->getSelect()); // coldstart
    S2->setBaud((e_baud)(p->getSelect()));
    return 0;
}

static int update_s2_pol(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select RX/TX polarity: %d", p->getSelect());
    S2->setLineInverse(p->getSelect()); // 0 off, 1 invers or TTL (always both, RX/TX)
    return 0;
}

static int update_s2_pin(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select Pin Swap: %d", p->getSelect());
    S2->setPinSwap(p->getSelect()); // 0 normal (3:TX 4:RX), 1 swapped (3:RX 4:TX)
    return 0;
}

static int update_s2_txena(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select TX Enable: %d", p->getSelect());
    S2->setTxOn(p->getSelect());
    return 0;
}

void connected_devices_menu_create_interfaceS2(SetupMenu *top)
{
    SetupMenuSelect *s2sp2 = new SetupMenuSelect("Baudraute", RST_NONE, update_s2_baud, &serial2_speed);
    s2sp2->addEntry("4800 baud");
    s2sp2->addEntry("9600 baud");
    s2sp2->addEntry("19200 baud");
    s2sp2->addEntry("38400 baud");
    s2sp2->addEntry("57600 baud");
    s2sp2->addEntry("115200 baud");
    top->addEntry(s2sp2);

    SetupMenuSelect *stxi2 = new SetupMenuSelect("Signaling", RST_NONE, update_s2_pol, &serial2_ttl_signals);
    stxi2->setHelp("A logical '1' is represented by a negative voltage in RS232 Standard, whereas in RS232 TTL "
                    "uses a positive voltage");
    stxi2->addEntry("RS232 Standard");
    stxi2->addEntry("RS232 TTL");
    top->addEntry(stxi2);

    SetupMenuSelect *srxtw2 = new SetupMenuSelect("Swap RX/TX", RST_NONE, update_s2_pin, &serial2_pin_swap);
    srxtw2->setHelp("Option to swap RX and TX line, a Flarm needs Normal, a Navi usually swapped.");
    srxtw2->addEntry("Normal");
    srxtw2->addEntry("Swapped");
    top->addEntry(srxtw2);

    ESP_LOGI(FNAME,"menu creation tx_ena %d", serial2_tx_enable.get() );
    SetupMenuSelect *stxdis2 = new SetupMenuSelect("TX line", RST_NONE, update_s2_txena, &serial2_tx_enable);
    stxdis2->setHelp("Option to listen only on the RX line, disables TX line to receive only data");
    stxdis2->mkEnable();
    top->addEntry(stxdis2);
}

static int update_can_datarate(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "New CAN data rate: %d", p->getSelect());
    CAN->ConfigureIntf(1);
    return 0;
}

void connected_devices_menu_create_interfaceCAN(SetupMenu *top)
{
    SetupMenuSelect *canmode = new SetupMenuSelect("Datarate", RST_NONE, update_can_datarate, &can_speed);
    top->addEntry(canmode);
    canmode->setHelp("Datarate on high speed serial CAN interace in kbit per second");
    canmode->addEntry("250 kbit");
    canmode->addEntry("500 kbit");
    canmode->addEntry("1000 kbit (default)");
}

void connected_devices_menu_create_interfaceI2C(SetupMenu *top)
{
	SetupMenuValFloat *compi2c = new SetupMenuValFloat("I2C Clock", "KHz", nullptr, false, &compass_i2c_cl, RST_ON_EXIT);
	top->addEntry(compi2c);
	compi2c->setHelp("Setup compass I2C Bus clock in KHz (reboots)");
}


//
// Devices
//
static int remove_device(SetupMenuSelect *p)
{
    if ( p->getSelect() == 1 ) {
        DeviceId did = (DeviceId)p->getParent()->getContId(); // dev id to remove
        ESP_LOGI(FNAME, "remove %d", did);
        if ( DEVMAN->removeDevice(did) ) {
            // restart needed
            p->scheduleReboot();
        }
        // clear nvs
        const DeviceAttributes &da = DeviceManager::getDevAttr(did);
        if ( da.nvsetup ) {
            // save it to nvs
            da.nvsetup->set(DeviceNVS(), false, false);
            da.nvsetup->commit();
        }
        p->getParent()->getParent()->setDirty();
    }

    p->setTerminateMenu();
    p->setSelect(0);
    return 0;
}

///////////////////////////
// Add Devices
static int select_device_action(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME,"action did %d", p->getValue());
    SetupMenu *top = p->getParent();
    // memorize and lock the device selector
    new_device = (DeviceId)p->getValue();
    p->lock();

    if ( new_device == NAVI_DEV ) {
        // need the navi flavor input, default to current nvs setup
        SetupMenuSelect *flavor = static_cast<SetupMenuSelect*>(top->getEntry(1));
        flavor->delAllEntries();
        for ( ProtocolType p : all_flavors ) {
            flavor->addEntry(DeviceManager::getPrtclName(p).data(), p);
        }
        flavor->unlock();
        flavor->setSelect(getFlvEnumFromProto((ProtocolType)nmea_protocol.get()));
        top->setHighlight(1);
    }
    else {
        top->setHighlight(3);
    }

    SetupMenuSelect *interface = static_cast<SetupMenuSelect*>(top->getEntry(3));
    const DeviceAttributes &dattr = DeviceManager::getDevAttr(new_device);
    interface->delAllEntries();
    const PackedInt5Array &tmp = dattr.itfs;
    ESP_LOGI(FNAME,"List Itfs raw %x", (unsigned)tmp.data);
    for (int i=0; i<tmp.maxSize; ++i) {
        InterfaceId iid = tmp.itf(i);
        ESP_LOGI(FNAME,"Itf id %d", iid);
        if ( iid != NO_PHY && DEVMAN->isAvail(iid) ) {
            interface->addEntry(DeviceManager::getItfName(iid).data(), iid);
        }
    }
    interface->unlock();
    return 0;
}
static int select_flavor_action(SetupMenuSelect *p)
{
    SetupMenu *top = p->getParent();
    nmea_protocol.set(p->getValue());
    top->setHighlight(3);
    return 0;
}
static int select_interface_action(SetupMenuSelect *p)
{
    SetupMenu *top = p->getParent();
    // memorize interface
    new_interface = (InterfaceId)p->getValue();
    ESP_LOGI(FNAME, "nr childs %s: %d", top->getTitle(), top->getNrChilds());
    if ( new_device > 0 && new_interface > 0) {
        SetupMenuSelect *confirm = static_cast<SetupMenuSelect*>(top->getEntry(4));
        p->lock();
        confirm->unlock();
        top->highlightLast();
    }
    return 0;
}
static int create_device_action(SetupMenuSelect *p)
{
    if ( p->getSelect() == 1 ) {
        // Confirmed; default protocols and port
        const DeviceAttributes &da = DeviceManager::getDevAttr(new_device, new_interface);
        Device *dev = nullptr;
        for (int i=0; i<da.prcols.getExtra(); ++i) {
            ProtocolType pid = da.prcols.proto(i);
            if ( new_device == NAVI_DEV ) {
                pid = (ProtocolType)nmea_protocol.get(); // navi flavor, override protocol table
            }
            if ( pid != NO_ONE ) {
                ESP_LOGI(FNAME,"add protocol %d for device id %d", pid, new_device);
                dev = DEVMAN->addDevice(new_device, pid, da.port, da.port, new_interface);
            }
        }
        if ( da.nvsetup && dev ) {
            // save it to nvs
            da.nvsetup->set(dev->getNvsData());
            da.nvsetup->commit();
        }
        // make sure there is a flarm host for any navi 
        if ( new_device == NAVI_DEV ) {
            new_device = FLARM_HOST_DEV;
            create_device_action(p); // recursive call to create the flarm host
        }
    
    }
    p->setTerminateMenu();
    p->setSelect(0); // reset to cancel
    p->getParent()->getParent()->setDirty();
    return 0;
}
static void connected_devices_menu_add_device(SetupMenu *top) // dynamic!
{
    ESP_LOGI(FNAME,"Create new device menu");
    SetupMenuSelect *ndev = static_cast<SetupMenuSelect*>(top->getEntry(0));
    SetupMenuSelect *flavor = static_cast<SetupMenuSelect*>(top->getEntry(1));
    if ( ! ndev ) {
        ndev = new SetupMenuSelect("Device", RST_NONE, select_device_action);
        top->addEntry(ndev);
        // flavor
        flavor = new SetupMenuSelect("Flavor", RST_NONE, select_flavor_action);
        top->addEntry(flavor);
        // text
        SetupMenuSelect *text = new SetupMenuSelect("connected to", RST_NONE);
        text->lock();
        top->addEntry(text);
    }
    else {
        ndev->delAllEntries();
        flavor->delAllEntries();
    }
    // navi flavor always a locked and empty select menu
    flavor->addEntry("---", 0);
    flavor->lock();

    // list all available devices for configuration
    for ( auto did : DeviceManager::allKnownDevs() ) {
        ESP_LOGI(FNAME,"Dev %d", did);
        const DeviceAttributes &da = DeviceManager::getDevAttr(did);
        ESP_LOGI(FNAME,"Itf %lx Prot %lx Flags %x", (unsigned long)(da.itfs.data), (unsigned long)(da.prcols.data), (unsigned int)(da.flags));
        // ESP_LOGI(FNAME,"Criteria: Sel %d Role %d rDep %d DEV %d multi %d", da.isSelectable(), xcv_role.get(), da.getRoleDep(), DEVMAN->getDevice(did)!=nullptr, da.multipleConf);
        if ( da.isSelectable() && da.roleFit(xcv_role.get()) && (!DEVMAN->getDevice(did) || da.multipleConf) ) {
            // ESP_LOGI(FNAME,"Add device %d", did);
            ndev->addEntry(da.name.data(), did);
        }
    }
    if ( ndev->numEntries() == 0 ) {
        ndev->addEntry("---", NO_DEVICE);
    }
    ndev->unlock();

    // create/get&empty interfaces list
    SetupMenuSelect *interface = static_cast<SetupMenuSelect*>(top->getEntry(3));
    SetupMenuSelect *confirm = static_cast<SetupMenuSelect*>(top->getEntry(4));
    if ( ! interface ) {
        interface = new SetupMenuSelect("Interface", RST_NONE, select_interface_action);
        top->addEntry(interface);
        confirm = new SetupMenuSelect("Create it", RST_NONE, create_device_action);
        confirm->mkConfirm();
        top->addEntry(confirm);
    }
    else {
        interface->delAllEntries();
    }
    interface->addEntry("---", NO_PHY);
    interface->lock();
    // confirmation
    confirm->lock();
    new_device = NO_DEVICE;
    new_interface = NO_PHY;
    top->highlightFirst();
}

/////////////////////////////////
// Device Details
static int start_dm_action(SetupAction* p)
{
    DataMonitor *dm = new DataMonitor();
    dm->start(p, (ItfTarget)p->getCode());
    return 0;
}
static void connected_devices_menu_device(SetupMenu *top) // dynamic!
{
    DeviceId did = (DeviceId)top->getContId();
    Device *dev = DEVMAN->getDevice(did);
    if ( top->getNrChilds() == 0 ) {

        // the interface
        SetupMenu *itf = new SetupMenu(DEVMAN->getItfName(dev->_itf->getId()).data(), get_itf_menu_creator(dev->_itf->getId()));
        top->addEntry(itf);

        // all data links to monitor
        std::string tmp;
        int lport = dev->_link->getPort();
        tmp = "Data Monitor";
        if ( ! dev->_itf->isOneToOne() ) {
            tmp += " port: " + std::to_string(lport);
        }
        SetupAction *monitor = new SetupAction(tmp.c_str(), start_dm_action, (int)dev->_link->getTarget().raw);
        top->addEntry(monitor);
        for ( int sp : dev->_link->getAllSendPorts() ) {
            if ( sp != lport ) {
                tmp = "Data Monitor port: " + std::to_string(sp);
                SetupAction *monitor = new SetupAction(tmp.c_str(), start_dm_action, (int)ItfTarget(dev->_itf->getId(), sp).raw);
                top->addEntry(monitor);
            }
        }

        // remove device
        SetupMenuSelect *remove = new SetupMenuSelect("Remove device", RST_NONE, remove_device);
        remove->mkConfirm();
        top->addEntry(remove);
    }

    // list protocols
    NmeaPrtcl *nmea = dev->_link->getNmea();
    device_details.assign("Protocols on ");
    device_details += dev->_itf->getStringId();
    device_details += ": ";
    if ( nmea ) {
        const std::vector<NmeaPlugin *> plglist = nmea->getAllPlugs();
        for (auto it : plglist) {
            if ( it != *plglist.begin() ) {
                device_details += ", ";
            }
            std::string_view tmp = DeviceManager::getPrtclName(it->getPtyp());
            device_details += tmp.data();
        }
    }
    ProtocolItf *binary = dev->_link->getBinary();
    if ( binary ) {
        device_details += "; ";
        std::string_view tmp = DeviceManager::getPrtclName(binary->getProtocolId());
        device_details += tmp.data();
    }
    top->setHelp(device_details.c_str());
}

///////////////////////////////////
// Connected Devices
void system_menu_connected_devices(SetupMenu *top) // dynamic!
{
    SetupMenu *adddev = static_cast<SetupMenu*>(top->getEntry(0));
    if ( ! adddev ) {
        adddev = new SetupMenu("Add Device", connected_devices_menu_add_device);
        adddev->setHelp("Get XCVario to know about your devices, it will handle data routing automatically");
        adddev->setDynContent();
        top->addEntry(adddev);
    }

    MenuEntry *ent = top->getEntry(1);
    while ( ent ) {
        top->delEntry(ent);
        ent = top->getEntry(1);
    }
    // List all configured devices
    for ( auto dev : DEVMAN->allDevs() ) {
        std::string_view dnam = DeviceManager::getDevName(dev->_id);
        if ( ! dnam.empty() ) {
            SetupMenu *devmenu = new SetupMenu(dnam.data(), connected_devices_menu_device, dev->_id);
            devmenu->setBuzzword(dev->_itf->getStringId());
            top->addEntry(devmenu);
        }
    }
}

void free_connected_devices_menu()
{
    device_details.clear();
    device_details.shrink_to_fit();
}
