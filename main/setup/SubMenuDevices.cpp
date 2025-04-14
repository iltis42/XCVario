/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "SetupMenu.h"
#include "SetupMenuSelect.h"
#include "SetupMenuChar.h"
#include "SetupMenuValFloat.h"
#include "SetupAction.h"
#include "comm/DeviceMgr.h"
#include "comm/SerialLine.h"
#include "DataMonitor.h"
#include "WifiClient.h"
#include "sensor.h"
#include "logdef.h"

#include <esp_wifi.h>

#include <cinttypes>
#include <string_view>

static DeviceId new_device;
static InterfaceId new_interface;

//
// Interfaces
//
static void options_menu_create_wireless_custom_id(SetupMenu *top);
static void options_menu_create_wifi(SetupMenu *top);
static void options_menu_create_bluetooth(SetupMenu *top);
static void system_menu_create_interfaceS1(SetupMenu *top);
static void system_menu_create_interfaceS2(SetupMenu *top);
static void system_menu_create_interfaceCAN(SetupMenu *top);

static SetupMenuCreator_t get_itf_menu_creator(InterfaceId iid)
{
    if ( iid == WIFI_AP ) {
        return options_menu_create_wifi;
    }
    else if ( iid == S2_RS232 ) {
        return system_menu_create_interfaceS1;
    }
    else if ( iid == S2_RS232 ) {
        return system_menu_create_interfaceS2;
    }
    else if ( iid == CAN_BUS ) {
        return system_menu_create_interfaceCAN;
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
	strcpy(id, custom_wireless_id.get().id);
	id[p->getCharIndex()] = *c;
	ESP_LOGI(FNAME,"New ID %s", id );
	custom_wireless_id.set(id);
	return 0;
}

void options_menu_create_wireless_custom_id(SetupMenu *top)
{
    SetupMenuChar *c1 = new SetupMenuChar("Letter 1", RST_NONE, update_id, false, custom_wireless_id.get().id, 0);
    SetupMenuChar *c2 = new SetupMenuChar("Letter 2", RST_NONE, update_id, false, custom_wireless_id.get().id, 1);
    SetupMenuChar *c3 = new SetupMenuChar("Letter 3", RST_NONE, update_id, false, custom_wireless_id.get().id, 2);
    SetupMenuChar *c4 = new SetupMenuChar("Letter 4", RST_NONE, update_id, false, custom_wireless_id.get().id, 3);
    SetupMenuChar *c5 = new SetupMenuChar("Letter 5", RST_NONE, update_id, false, custom_wireless_id.get().id, 4);
    SetupMenuChar *c6 = new SetupMenuChar("Letter 6", RST_NONE, update_id, false, custom_wireless_id.get().id, 5);
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

static int master_xcv_lock(SetupMenuSelect *p) {
	ESP_LOGI(FNAME,"master_xcv_lock");
	MYUCG->setPrintPos(1, 130);
	int mxcv = WifiClient::getScannedMasterXCV();
	MYUCG->printf("Scanned: XCVario-%d", mxcv);
	if (master_xcvario_lock.get() == 1)
		master_xcvario.set(mxcv);
	return 0;
}

void options_menu_create_wifi(SetupMenu *top)
{
    SetupMenuValFloat *wifip = new SetupMenuValFloat("WIFI Power", "%", 10.0,
                                                     100.0, 5.0, update_wifi_power, false, &wifi_max_power);
    wifip->setPrecision(0);
    top->addEntry(wifip);
    wifip->setHelp("Maximum Wifi Power to be used 10..100% or 2..20dBm");

    SetupMenuSelect *wifimal = new SetupMenuSelect("Lock Master", RST_NONE,
                                                   master_xcv_lock, true, &master_xcvario_lock);
    wifimal->setHelp(
        "In wireless client role, lock this client to the scanned master XCVario ID above");
    wifimal->addEntry("Unlock");
    wifimal->addEntry("Lock");
    top->addEntry(wifimal);

    SetupMenu *cusid = new SetupMenu("Custom-ID", options_menu_create_wireless_custom_id);
    top->addEntry(cusid);
    cusid->setHelp("Select custom ID (SSID) for wireless BT (or WIFI) interface, e.g. D-1234. Restart device to activate", 215);
}

void options_menu_create_bluetooth(SetupMenu *top)
{
    // SetupMenuSelect *btm = new SetupMenuSelect("Wireless", RST_ON_EXIT, 0, true,
    //                                            &wireless_type);
    // btm->setHelp(
    //     "Activate wireless interface type to connect navigation devices, or to another XCVario as client (reboots)",
    //     220);
    // btm->addEntry("Disable");
    // btm->addEntry("Bluetooth");
    // btm->addEntry("Wireless Master");
    // btm->addEntry("Wireless Client");
    // btm->addEntry("Wireless Standalone");
    // btm->addEntry("Bluetooth LE");
    // top->addEntry(btm);

    // SetupMenuValFloat *wifip = new SetupMenuValFloat("WIFI Power", "%", 10.0,
    //                                                  100.0, 5.0, update_wifi_power, false, &wifi_max_power);
    // wifip->setPrecision(0);
    // top->addEntry(wifip);
    // wifip->setHelp("Maximum Wifi Power to be used 10..100% or 2..20dBm");

    // SetupMenuSelect *wifimal = new SetupMenuSelect("Lock Master", RST_NONE,
    //                                                master_xcv_lock, true, &master_xcvario_lock);
    // wifimal->setHelp(
    //     "In wireless client role, lock this client to the scanned master XCVario ID above");
    // wifimal->addEntry("Unlock");
    // wifimal->addEntry("Lock");
    // top->addEntry(wifimal);

    // SetupMenuSelect *datamon = new SetupMenuSelect("Monitor", RST_NONE,
    //                                                data_mon, true, nullptr);
    // datamon->setHelp(
    //     "Short press button to start/pause, long press to terminate data monitor",
    //     260);
    // datamon->addEntry("Disable");
    // datamon->addEntry("Bluetooth");
    // datamon->addEntry("Wifi 8880");
    // datamon->addEntry("Wifi 8881");
    // datamon->addEntry("Wifi 8882");
    // datamon->addEntry("RS232 S1");
    // datamon->addEntry("RS232 S2");
    // datamon->addEntry("CAN Bus");
    // top->addEntry(datamon);

    // SetupMenuSelect *datamonmod = new SetupMenuSelect("Monitor Mode", RST_NONE,
    //                                                   0, true, &data_monitor_mode);
    // datamonmod->setHelp(
    //     "Select data display as ASCII text or as binary hexdump");
    // datamonmod->addEntry("ASCII");
    // datamonmod->addEntry("Binary");
    // top->addEntry(datamonmod);

    // SetupMenu *cusid = new SetupMenu("Custom-ID", options_menu_create_wireless_custom_id);
    // top->addEntry(cusid);
    // cusid->setHelp("Select custom ID (SSID) for wireless BT (or WIFI) interface, e.g. D-1234. Restart device to activate", 215);

    // // SetupMenuSelect *clearcore = new SetupMenuSelect("Clear coredump", RST_NONE, reinterpret_cast<int (*)(SetupMenuSelect*)>(clear_coredump), false, nullptr);
    // // clearcore->addEntry("doit");
    // // top->addEntry(clearcore);
}

static int update_s1_baud(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select baudrate: %d", p->getSelect());
    S1->setBaud((e_baud)(p->getSelect())); // 0 off, 1: 4800, 2:9600, etc
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
    S1->setTxOn(p->getSelect()); // 0 normal Client (RO, Listener), 1 Master (Sender)
    return 0;
}

static int update_s1_protocol(SetupMenuSelect *p)
{
    if (p->getSelect() > 0)
    {
        ESP_LOGI(FNAME, "Select profile: %d", p->getSelect() - 1);
        S1->ConfigureIntf(p->getSelect() - 1); // SM_FLARM = 1, SM_RADIO = 2, ...
    }
    return 0;
}

void system_menu_create_interfaceS1(SetupMenu *top)
{
    SetupMenuSelect *s1sp2 = new SetupMenuSelect("Baudraute", RST_ON_EXIT, update_s1_baud, true, &serial1_speed);
    s1sp2->addEntry("4800 baud");
    s1sp2->addEntry("9600 baud");
    s1sp2->addEntry("19200 baud");
    s1sp2->addEntry("38400 baud");
    s1sp2->addEntry("57600 baud");
    s1sp2->addEntry("115200 baud");
    top->addEntry(s1sp2);

    SetupMenuSelect *stxi2 = new SetupMenuSelect("Signaling", RST_NONE, update_s1_pol, true, &serial1_tx_inverted);
    top->addEntry(stxi2);
    stxi2->setHelp("A logical '1' is represented by a negative voltage in RS232 Standard, whereas in RS232 TTL "
                    "uses a positive voltage");
    stxi2->addEntry("RS232 Standard");
    stxi2->addEntry("RS232 TTL");

    SetupMenuSelect *srxtw2 = new SetupMenuSelect("Swap RX/TX Pins", RST_NONE,
                                                  update_s1_pin, true, &serial1_pins_twisted);
    top->addEntry(srxtw2);
    srxtw2->setHelp("Option to swap RX and TX line for S1, a Flarm needs Normal, a Navi usually swapped. "
                    "After change also a true power-cycle is needed");
    srxtw2->addEntry("Normal");
    srxtw2->addEntry("Swapped");

    // SetupMenuSelect *stxdis1 = new SetupMenuSelect("Role", RST_NONE,
    //                                                update_s1_txena, true, &serial1_tx_enable);
    // top->addEntry(stxdis1);
    // stxdis1->setHelp(
    //     "Option for 'Client' mode to listen only on the RX line, disables TX line to receive only data");
    // stxdis1->addEntry("Client (RX)");
    // stxdis1->addEntry("Master (RX&TX)");

    SetupMenuSelect *sprots1 = new SetupMenuSelect("Protocol", RST_NONE,
                                                   update_s1_protocol, true, &serial1_protocol);
    top->addEntry(sprots1);
    sprots1->setHelp("Specify the protocol driver for the external device connected to S1", 240);
    sprots1->addEntry("Disable");
    sprots1->addEntry("Flarm");
    sprots1->addEntry("Radio");
    sprots1->addEntry("XCTNAV S3");
    sprots1->addEntry("OPENVARIO");
    sprots1->addEntry("XCFLARMVIEW");
}

static int update_s2_baud(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select baudrate: %d", p->getSelect()); // coldstart
    S2->setBaud((e_baud)(p->getSelect()));                  // 0: 4800, 1:9600, ..
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
    S2->setTxOn(p->getSelect()); // 0 normal Client (RO, Listener), 1 Master (Sender)
    return 0;
}

static int update_s2_protocol(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "Select profile: %d", p->getSelect() - 1);
    if (p->getSelect() > 0)
    {
        S2->ConfigureIntf(p->getSelect() - 1); // SM_FLARM = 0, SM_RADIO = 1, ...
    }
    return 0;
}

void system_menu_create_interfaceS2(SetupMenu *top)
{
    SetupMenuSelect *s2sp2 = new SetupMenuSelect("Baudraute", RST_ON_EXIT,
                                                 update_s2_baud, true, &serial2_speed);
    top->addEntry(s2sp2);
    s2sp2->addEntry("4800 baud");
    s2sp2->addEntry("9600 baud");
    s2sp2->addEntry("19200 baud");
    s2sp2->addEntry("38400 baud");
    s2sp2->addEntry("57600 baud");
    s2sp2->addEntry("115200 baud");

    SetupMenuSelect *stxi2 = new SetupMenuSelect("Signaling", RST_NONE, update_s2_pol, true, &serial2_tx_inverted);
    top->addEntry(stxi2);
    stxi2->setHelp("A logical '1' is represented by a negative voltage in RS232 Standard, whereas in RS232 TTL "
                    "uses a positive voltage");
    stxi2->addEntry("RS232 Standard");
    stxi2->addEntry("RS232 TTL");

    SetupMenuSelect *srxtw2 = new SetupMenuSelect("Swap RX/TX Pins", RST_NONE,
                                                  update_s2_pin, true, &serial2_pins_twisted);
    top->addEntry(srxtw2);
    srxtw2->setHelp("Option to swap RX and TX line for S1, a Flarm needs Normal, a Navi usually swapped. "
                    "After change also a true power-cycle is needed");
    srxtw2->addEntry("Normal");
    srxtw2->addEntry("Swapped");

    SetupMenuSelect *stxdis2 = new SetupMenuSelect("Role", RST_NONE, update_s2_txena, true, &serial2_tx_enable);
    top->addEntry(stxdis2);
    stxdis2->setHelp(
        "Option for 'Client' mode to listen only on the RX line, disables TX line to receive only data");
    stxdis2->addEntry("Client (RX)");
    stxdis2->addEntry("Master (RX&TX)");

    // Fixme move to devices
    // SetupMenuSelect *sprots1 = new SetupMenuSelect( "Protocol", RST_NONE,
    // 		update_s2_protocol, true, &serial2_protocol);
    // top->addEntry(sprots1);
    // sprots1->setHelp(
    // 		"Specify the protocol driver for the external device connected to S2",
    // 		240);
    // sprots1->addEntry( "Disable");
    // sprots1->addEntry( "Flarm");
    // sprots1->addEntry( "Radio");
    // sprots1->addEntry( "XCTNAV S3");
    // sprots1->addEntry( "OPENVARIO");
    // sprots1->addEntry( "XCFLARMVIEW");

    // SetupMenuSelect *datamon = new SetupMenuSelect("Monitor", RST_NONE,
    //                                                data_monS2, true, nullptr);
    // datamon->setHelp(
    //     "Short press button to start/pause, long press to terminate data monitor",
    //     260);
    // datamon->addEntry("Start S2 RS232");

    // top->addEntry(datamon);
}

void system_menu_create_interfaceCAN(SetupMenu *top)
{
    SetupMenuSelect *canmode = new SetupMenuSelect("Datarate", RST_ON_EXIT, 0, true, &can_speed);
    top->addEntry(canmode);
    canmode->setHelp("Datarate on high speed serial CAN interace in kbit per second");
    canmode->addEntry("250 kbit");
    canmode->addEntry("500 kbit");
    canmode->addEntry("1000 kbit (default)");

    SetupMenuSelect *devmod = new SetupMenuSelect("Mode", RST_ON_EXIT, 0, false, &can_mode);
    top->addEntry(devmod);
    devmod->setHelp(
        "Select 'Standalone' for single seater, 'Master' in front, 'Client' for secondary device in rear (reboots)");
    devmod->addEntry("Master");
    devmod->addEntry("Client");
    devmod->addEntry("Standalone");
}
    // // _serial1_speed
    // SetupMenu *rs232 = new SetupMenu("RS232 Interface S1", system_menu_create_interfaceS1);
    // sye->addEntry(rs232);

    // SetupMenu *can = new SetupMenu("CAN Interface", system_menu_create_interfaceCAN);

    // if (hardwareRevision.get() >= XCVARIO_21) {
    // 	SetupMenu *rs232_2 = new SetupMenu("RS232 Interface S2", system_menu_create_interfaceS2);
    // 	sye->addEntry(rs232_2);
    // }
    // if (hardwareRevision.get() >= XCVARIO_22) {
    // 	// Can Interface C1
    // 	SetupMenu *can = new SetupMenu("CAN Interface", system_menu_create_interfaceCAN);
    // 	sye->addEntry(can);
    // }

    // // NMEA protocol of variometer
    // SetupMenuSelect *nmea = new SetupMenuSelect("NMEA Protocol", RST_NONE, 0,
    // 		true, &nmea_protocol);
    // sye->addEntry(nmea);
    // nmea->setHelp(
    // 		"Setup the protocol used for sending NMEA sentences. This needs to match the device driver chosen in XCSoar/LK8000");
    // nmea->addEntry("OpenVario");
    // nmea->addEntry("Borgelt");
    // nmea->addEntry("Cambridge");
    // nmea->addEntry("XCVario");
    // nmea->addEntry("Disable");



//
// Devices
//
static int remove_device(SetupMenuSelect *p)
{
    if ( p->getSelect() == 1 ) {
        DeviceId did = (DeviceId)p->getParent()->getContId(); // dev id to remove
        ESP_LOGI(FNAME, "remove %d", did);
        DeviceManager *dm = DeviceManager::Instance();
        dm->removeDevice(did);
        p->getParent()->getParent()->setDirty();
    }
    p->setTerminateMenu();
    return 0;
}


static int create_device(SetupMenuSelect *p)
{
    if ( p->getSelect() == 1 ) {
        DeviceManager *dm = DeviceManager::Instance();
        dm->addDevice(new_device, NO_ONE, 0, 0, new_interface);
    }
    p->setTerminateMenu();
    p->getParent()->setDirty();
    return 0;
}

static int select_interface_action(SetupMenuSelect *p)
{
    SetupMenu *top = p->getParent();
    // memorize interface
    new_interface = (InterfaceId)p->getValue();
    ESP_LOGI(FNAME, "nr childs %s: %d", top->getTitle(), top->getNrChilds());
    if ( new_device > 0 && new_interface > 0) {
        SetupMenuSelect *confirm = static_cast<SetupMenuSelect*>(top->getEntry(3));
        confirm->unlock();
        top->highlightLast();
    }
    return 0;
}

static int select_device_action(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME,"action did %d", p->getValue());
    SetupMenu *top = p->getParent();
    // memorize and lock the device selector
    new_device = (DeviceId)p->getValue();

    SetupMenuSelect *interface = static_cast<SetupMenuSelect*>(top->getEntry(2));
    const DeviceAttributes *dattr = DeviceManager::getDevAttr(new_device);
    interface->delAllEntries();
    InterfacePack tmp(dattr->attr.interfaces);
    ESP_LOGI(FNAME,"List Itfs raw %x", dattr->attr.interfaces);
    for (int i=0; i<5; ++i) {
        InterfaceId iid = tmp.get(i);
        ESP_LOGI(FNAME,"Itf id %d", iid);
        if ( DEVMAN->isAvail(iid) ) {
            interface->addEntry(DeviceManager::getItfName(iid).data(), iid);
        }
    }
    if ( new_device > 0 && new_interface > 0) {
        SetupMenuSelect *confirm = static_cast<SetupMenuSelect*>(top->getEntry(3));
        confirm->unlock();
        top->highlightLast();
    }
    else {
        top->highlightEntry(interface);
    }

    return 0;
}

static void system_menu_add_device(SetupMenu *top)
{
    ESP_LOGI(FNAME,"Create new device menu");
    SetupMenuSelect *ndev = static_cast<SetupMenuSelect*>(top->getEntry(0));
    if ( ! ndev ) {
        ndev = new SetupMenuSelect("Device", RST_NONE, select_device_action, false);
        top->addEntry(ndev);
        // text
        SetupMenuSelect *text = new SetupMenuSelect("connected to", RST_NONE);
        text->lock();
        top->addEntry(text);
    }
    else {
        ndev->delAllEntries();
    }
    for ( auto did : DeviceManager::allKnownDevs() ) {
        ESP_LOGI(FNAME,"Dev %d", did);
        const DeviceAttributes *dattr = DeviceManager::getDevAttr(did);
        ESP_LOGI(FNAME,"Attr %lx", (unsigned long)(dattr->attr.data));
        if ( ! DEVMAN->getDevice(did) || dattr->attr.multipleConf ) {
            ndev->addEntry(DeviceManager::getDevName(did).data(), did);
        }
    }

    // empty interfaces list
    SetupMenuSelect *interface = static_cast<SetupMenuSelect*>(top->getEntry(2));
    SetupMenuSelect *confirm = static_cast<SetupMenuSelect*>(top->getEntry(3));
    if ( ! interface ) {
        interface = new SetupMenuSelect("Interface", RST_NONE, select_interface_action);
        top->addEntry(interface);
        confirm = new SetupMenuSelect("Create it", RST_NONE, create_device);
        confirm->mkConfirm();
        top->addEntry(confirm);
    }
    else {
        interface->delAllEntries();
    }
    interface->addEntry("---", NO_PHY);
    // confirmation
    confirm->lock();
    new_device = NO_DEVICE;
    new_interface = NO_PHY;
}

static int start_dm_action(SetupAction* p)
{
    DataMonitor *dm = new DataMonitor();
    dm->start(p, (ItfTarget)p->getCode());
    return 0;
}

static void system_menu_device(SetupMenu *top)
{
    DeviceId did = (DeviceId)top->getContId();
    DeviceManager *dm = DeviceManager::Instance();
    Device *dev = dm->getDevice(did);
    // Interface
    // static char itf_title[30];
    // sprintf(itf_title, "%s port %d", dm->getItfName(dev->_itf->getId()), dev->getSendPort());
    SetupMenu *itf = new SetupMenu(dm->getItfName(dev->_itf->getId()).data(), get_itf_menu_creator(dev->_itf->getId()));
    top->addEntry(itf);
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
        top->delEntry(ent);
        ent = top->getEntry(1);
    }
    // List all configured devices
    for ( auto dev : dm->allDevs() ) {
        std::string_view dnam = DeviceManager::getDevName(dev->_id);
        if ( ! dnam.empty() ) {
            SetupMenu *devmenu = new SetupMenu(dnam.data(), system_menu_device, dev->_id);
            devmenu->setHelp("Device details.");
            top->addEntry(devmenu);
        }
    }
}
