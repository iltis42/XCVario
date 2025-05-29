/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "DeviceMgr.h"

#include "comm/Configuration.h"
#include "comm/Messages.h"
#include "comm/CanBus.h"
#include "comm/WifiAP.h"
#include "comm/BTspp.h"
#include "comm/I2CWrapper.h"
#include "BLESender.h"
#include "SerialLine.h"
#include "protocol/ProtocolItf.h"
#include "protocol/NMEA.h"
#include "protocol/MagSensBin.h"
#include "setup/DataMonitor.h"
#include "setup/SetupNG.h"
#include "Compass.h"

#include "sensor.h"
#include "logdef.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#include <deque>

// global variables
DeviceManager* DEVMAN = nullptr; // singleton like
QueueHandle_t ItfSendQueue = 0;
MessagePool MP;

// static vars
static TaskHandle_t SendTask = nullptr;

//
// A routing table that contains the static connected devices relation.
//
// entries with zero termination, entirely as ro flash data
static constexpr RoutingTarget flarm_routes_synch[] = { 
    {FLARM_HOST_DEV, S2_RS232, 0}, {FLARM_HOST_DEV, WIFI_AP, 8881}, {FLARM_HOST_DEV, BT_SPP, 0}, {XCVARIOSECOND_DEV, CAN_BUS, 0}, 
    {XCVARIOFIRST_DEV, CAN_BUS, 0}, {} };
static constexpr RoutingTarget flarm_routes[] = { 
    {FLARM_HOST_DEV, S2_RS232, 0}, {FLARM_HOST_DEV, WIFI_AP, 8881}, {FLARM_HOST_DEV, BT_SPP, 0}, {} };
static constexpr RoutingTarget radio_routes[] = { 
    {RADIO_KRT2_DEV, S2_RS232, 0}, {RADIO_ATR833_DEV, S2_RS232, 0}, {XCVARIOFIRST_DEV, CAN_BUS, 0}, {} };
static constexpr RoutingTarget navi_routes[] = { 
    {FLARM_DEV, S1_RS232, 0}, {FLARM_DEV, CAN_BUS, 0}, {RADIO_KRT2_DEV, S2_RS232, 0}, {} };
static constexpr RoutingTarget fhost_routes[] = { 
    {FLARM_DEV, S1_RS232, 0}, {FLARM_DEV, CAN_BUS, 0}, {} };
static constexpr RoutingTarget proxy_routes[] = { 
    {NAVI_DEV, S2_RS232, 0}, {FLARM_HOST_DEV, WIFI_AP, 8881}, {FLARM_HOST_DEV, BT_SPP, 0}, {} };
static constexpr std::pair<RoutingTarget, const RoutingTarget*> Routes[] = {
    { RoutingTarget(FLARM_DEV, S1_RS232, 0), flarm_routes_synch },
    { RoutingTarget(FLARM_DEV, CAN_BUS, 0), flarm_routes },
    { RoutingTarget(RADIO_REMOTE_DEV, NO_PHY, 0), radio_routes },
    { RoutingTarget(NAVI_DEV, NO_PHY, 0), navi_routes },
    { RoutingTarget(FLARM_HOST_DEV, NO_PHY, 0), fhost_routes },
    { RoutingTarget(XCVARIOFIRST_DEV, NO_PHY, 0), proxy_routes },
    { RoutingTarget(XCVARIOSECOND_DEV, NO_PHY, 0), proxy_routes }
};
// Search the flash data table
static const RoutingTarget* findRoute(const RoutingTarget& source) {
    // Search through Routes and find a match to the source device
    for (const auto& entry : Routes) {
        if (entry.first.match(source)) {
            return entry.second; // Return the matched routing list
        }
    }
    return nullptr;
}

//
// Known device attributes
//
// Define device names for the configuration menu
// - The device id, must be a unique entry in this list with assiciated name
// - Device attributes
//   + a readable name
//   + a list of interfaces, first one is a default proposal
//   + later entries with same dev id speicialize and override the first default
//   + a list of protocol, first N are the by default configured ones
//   + an interface profile enum, if needed
// - 
constexpr std::pair<DeviceId, DeviceAttributes> DEVATTR[] = {
    {DeviceId::ANEMOI_DEV, {"Anemoi", {{S2_RS232, S1_RS232, CAN_BUS}}, {{ANEMOI_P}, 1}, 0, IS_REAL, &anemoi_devsetup}},
    {DeviceId::CANREGISTRAR_DEV, {"Auto-connect", {{CAN_BUS}}, {{REGISTRATION_P}, 1}, CAN_REG_PORT, IS_REAL, &auto_connect}},
    {DeviceId::FLARM_DEV,  {"Flarm", {{S1_RS232, S2_RS232}}, {{FLARM_P, FLARMBIN_P}, 2}, 0, IS_REAL, &flarm_devsetup}},
    // {DeviceId::FLARM_DEV,  {"", {{XCVPROXY}}, {{FLARM_P, FLARMBIN_P}, 2}, 0, 0, &flarm_devsetup}},
    {DeviceId::JUMBO_DEV,  {"jumbo putzi", {{CAN_BUS}}, {{JUMBOCMD_P}, 1} , 0, 0, nullptr}}, // auto dev
    {DeviceId::XCVARIOFIRST_DEV, {"Master XCV", {{WIFI_CLIENT, BT_SPP, S1_RS232, S2_RS232}}, {{XCVSYNC_P}, 1}, 8884, IS_REAL, &master_devsetup}},
    {DeviceId::XCVARIOFIRST_DEV, {"", {{CAN_BUS}}, {{XCVQUERY_P}, 1}, CAN_REG_PORT, IS_VARIANT, nullptr}}, // auto through XCV role
    {DeviceId::XCVARIOFIRST_DEV, {"", {{S2_RS232}}, {{XCVSYNC_P}, 1}, 0, IS_REAL|SECOND_ONLY, &master_devsetup}},
    {DeviceId::XCVARIOSECOND_DEV, {"Second XCV", {{WIFI_AP, BT_SPP, S1_RS232, S2_RS232}}, {{XCVSYNC_P}, 1}, 8884, IS_REAL|MASTER_ONLY, &second_devsetup}},
    {DeviceId::XCVARIOSECOND_DEV, {"", {{S2_RS232}}, {{XCVSYNC_P}, 1}, 0, IS_REAL|MASTER_ONLY, &second_devsetup}},
    {DeviceId::MAGLEG_DEV, {"MagSens rev0", {{I2C, CAN_BUS}}, {{MAGSENSBIN_P}, 1}, 0, IS_REAL, &magleg_devsetup}},
    {DeviceId::MAGLEG_DEV, {"", {{CAN_BUS}}, {{MAGSENSBIN_P}, 1}, MagSensBin::LEGACY_MAGSTREAM_ID, IS_REAL, &magleg_devsetup}},
    {DeviceId::MAGLEG_DEV, {"", {{I2C}}, {{MAGSENSBIN_P}, 0}, 0, IS_REAL, &magleg_devsetup}},
    {DeviceId::MAGSENS_DEV, {"MagSens rev1", {{CAN_BUS}}, {{MAGSENS_P}, 1}, 0, IS_REAL, nullptr}}, // auto start
    {DeviceId::NAVI_DEV,   {"Navi", {{WIFI_AP, S1_RS232, S2_RS232, BT_SPP, BT_LE, CAN_BUS}}, 
                                    {{XCVARIO_P, CAMBRIDGE_P, OPENVARIO_P, BORGELT_P, KRT2_REMOTE_P, ATR833_REMOTE_P}, 1}, 
                                    8880, IS_REAL, &navi_devsetup}},
    {DeviceId::NAVI_DEV,   {"", {{S2_RS232}}, {{XCVARIO_P, CAMBRIDGE_P, OPENVARIO_P, BORGELT_P, KRT2_REMOTE_P, ATR833_REMOTE_P}, 1}, 
                                    0, IPTOS_RELIABILITY, &navi_devsetup}},
    {DeviceId::NAVI_DEV,   {"", {{BT_SPP}}, {{XCVARIO_P, CAMBRIDGE_P, OPENVARIO_P, BORGELT_P, KRT2_REMOTE_P, ATR833_REMOTE_P}, 1}, 
                                    0, IS_REAL, &navi_devsetup}},
    {DeviceId::FLARM_HOST_DEV, {"Flarm host", {{WIFI_AP, S2_RS232, BT_SPP}}, {{FLARMHOST_P, FLARMBIN_P}, 2}, 8881, MULTI_CONF, &flarm_host_setup}},
    // {DeviceId::FLARM_HOST_DEV, {"", {{CAN_BUS}}, {{FLARMHOST_P, FLARMBIN_P}, 2}, 0, 0, &flarm_host_setup}},
    {DeviceId::FLARM_HOST_DEV, {"", {{S2_RS232}}, {{FLARMHOST_P, FLARMBIN_P}, 2}, 0, 0, &flarm_host_setup}},
    {DeviceId::FLARM_HOST_DEV, {"", {{BT_SPP}}, {{FLARMHOST_P, FLARMBIN_P}, 2}, 0, 0, &flarm_host_setup}},
    {DeviceId::RADIO_REMOTE_DEV, {"Radio remote", {{WIFI_AP}}, {{KRT2_REMOTE_P}, 1}, 8882, 0, &radio_host_setup}},
    {DeviceId::RADIO_KRT2_DEV, {"KRT 2", {{S2_RS232, CAN_BUS}}, {{KRT2_REMOTE_P}, 1}, 0, IS_REAL, &krt_devsetup}},
    {DeviceId::RADIO_ATR833_DEV, {"ATR833", {{S2_RS232, CAN_BUS}}, {{ATR833_REMOTE_P}, 1}, 0, IS_REAL, &atr_devsetup}}
};

// Lookup functions
constexpr const DeviceAttributes NullDev("Null", {{NO_PHY}}, {{NO_ONE}}, 0, 0, nullptr);
const DeviceAttributes& DeviceManager::getDevAttr(DeviceId did, InterfaceId via) {
    // retrieve first attribute entry
    for (const auto& entry : DEVATTR) {
        if (entry.first == did) {
            if (via == NO_PHY) {
                return entry.second;
            }
            if (via == entry.second.itfs.get(0)) {
                return entry.second;
            }
        }
    }
    return NullDev;
}
std::string_view DeviceManager::getDevName(DeviceId did) {
    auto da = getDevAttr(did);
    if ( &da != &NullDev) {
        return da.name; // first hit rules
    }
    return "";
}
std::vector<DeviceId> DeviceManager::allKnownDevs()
{
    std::vector<DeviceId> ret;
    for (const auto& entry : DEVATTR) {
        if (! entry.second.name.empty()  && ! entry.second.isVariant()) { // fixme empty()?
            ret.push_back(entry.first);
        }
    }
    return ret;
}

constexpr std::pair<InterfaceId, std::string_view> INTFCS[] = {
    {CAN_BUS, "CAN bus"},
    {I2C, "I2C bus"},
    {S1_RS232, "S1 serial"},
    {S2_RS232, "S2 serial"},
    {WIFI_AP, "Wifi AP"},
    {WIFI_CLIENT, "Wifi client"},
    {BT_SPP, "BT serial"},
    {BT_LE, "BT low energy"}
    // {XCVPROXY, "XCV proxy"}
};

std::string_view DeviceManager::getItfName(InterfaceId iid) {
    for (const auto& entry : INTFCS) {
        if (entry.first == iid) {
            return entry.second;
        }
    }
    return "";
}

std::vector<InterfaceId> DeviceManager::allKnownIntfs()
{
    std::vector<InterfaceId> ret;
    for (const auto& entry : INTFCS) {
        ret.push_back(entry.first);
    }
    return ret;
}

constexpr std::pair<ProtocolType, std::string_view> PRTCLS[] = {
    {REGISTRATION_P, "Auto registration"},
    {XCVSYNC_P, "XCV sync"},
    {JUMBOCMD_P, "jumbo cmd"},
    {ANEMOI_P, "Anemoi"},
    {FLARM_P, "Flarm"},
    {FLARMHOST_P, "Flarm host"},
    {FLARMBIN_P, "Flarm BP"},
    {GARMIN_P, "Garmin"},
    {MAGSENS_P, "Magsens"},
    {MAGSENSBIN_P, "Magsens BP"},
    {NMEASTD_P, "NMEA std"},
    {XCVARIO_P, "XCVario"},
    {OPENVARIO_P, "Open-Vario"},
    {BORGELT_P, "Borgelt"},
    {CAMBRIDGE_P, "Cambridge"},
    {KRT2_REMOTE_P, "KRT2"},
    {ATR833_REMOTE_P, "ATR833"},
    {XCVQUERY_P, "XCV query"},
};

std::string_view DeviceManager::getPrtclName(ProtocolType pid) {
    for (const auto& entry : PRTCLS) {
        if (entry.first == pid) {
            return entry.second;
        }
    }
    return "";
}

// a dummy interface
class DmyItf final : public InterfaceCtrl
{
public:
    const char* getStringId() const override { return "NUL"; }
    void ConfigureIntf(int cfg) override {}
    int Send(const char *msg, int &len, int port=0) { return 0; }
};
static DmyItf dummy_itf;
static ItfTarget monitor_target = {};

static int tt_snd(Message *msg)
{
    int len = msg->buffer.length();
    int port = msg->port;
    InterfaceCtrl *itf = DEVMAN->getIntf(msg->target_id);
    int plsrety = 0;
    if (itf)
    {
        ESP_LOGD(FNAME, "send %s/%d NMEA len %d, msg: %s", itf->getStringId(), port, len, msg->buffer.c_str());
        plsrety = itf->Send(msg->buffer.c_str(), len, port);
        if ( plsrety > 0 ) {
            ESP_LOGD(FNAME, "reshedule message %d/%d", len, msg->buffer.length());
            msg->buffer.erase(0, len); // chop sent bytes off
        }
        else if ( DM && plsrety == 0 && monitor_target == ItfTarget(itf->getId(), port) ) {
            DM->monitorString(DIR_TX, !std::isprint(msg->buffer[0]), msg->buffer.c_str(), len);
        }
    }
    return plsrety;
}

// generic transmitter grabbing messages from a queue
void TransmitTask(void *arg)
{
    QueueHandle_t queue = (QueueHandle_t)arg;
    Message *msg;
    std::deque<Message *> later;
    int pls_retry = 0;

    while (true)
    {
        // sleep until the queue gives us something to do, or we have to do a retry
        TickType_t timeout = (pls_retry==0) ? portMAX_DELAY : pdMS_TO_TICKS(pls_retry);
        bool new_msg = xQueueReceive(queue, &msg, timeout) == pdTRUE;

        if (msg == nullptr) {
            break;
        } // termination signal

        if ( new_msg ) {
            // always check against the later fifo first, not to garble msg sequence
            bool wait = false;
            if( !later.empty() ) {
                // might be just another msg to wait for the same itf
                for ( Message *i : later ) {
                    if ( msg->target_id == i->target_id ) {
                        wait = true;
                        break;
                    }
                }
            }
            if ( wait ) {
                // put this straight onto the later queue
                ESP_LOGD(FNAME, "straight wait");
                later.push_back(msg);
            }
            else {
                // Regular case
                ESP_LOGD(FNAME, "regular send");
                pls_retry = tt_snd(msg);
                if ( pls_retry < 1 ) {
                    ESP_LOGD(FNAME, "regular done");
                    DEV::relMessage(msg);
                } else {
                    ESP_LOGD(FNAME, "retry pushed");
                    later.push_back(msg);
                    continue; // just pushed this one, no need to check again
                }
            }
        }

        // Always have to go through the later list until the first msg gets rejected 
        // to know the proper tim-out to wait again
        while ( !later.empty() ) {
            msg = later.front();
            int cmplen = msg->buffer.size();
            ESP_LOGD(FNAME, "postponed send %d", cmplen);
            pls_retry = tt_snd(msg);
            if ( pls_retry > 0 ) {
                if ( (cmplen - msg->buffer.size()) > 0 ) {
                    break; // some bytes got sent
                }
                // Inaceptable for a retry, discard
                pls_retry = 0;
            }
            DEV::relMessage(msg);
            later.pop_front();
        }
    }
    vTaskDelete(NULL);
}

DeviceManager::DeviceManager()
{
    ItfSendQueue = xQueueCreate( MSG_POOL_SIZE+1, sizeof(Message*) );
    _devmap_mutex = xSemaphoreCreateMutex();

}

DeviceManager::~DeviceManager()
{
    xQueueSend( ItfSendQueue, nullptr, 0 ); // for transmitter
}

// The device manager
DeviceManager* DeviceManager::Instance()
{
    if ( ! DEVMAN ) {
        DEVMAN = new DeviceManager();
    }
    return DEVMAN;
}

// Do all it needs to prepare comm with device and route data
// It returns the pointer to the device
Device* DeviceManager::addDevice(DeviceId did, ProtocolType proto, int listen_port, int send_port, InterfaceId iid)
{
    // On first device a send task needs to be created
    if ( ! SendTask ) {
        xTaskCreate(TransmitTask, "genTx", 3000, ItfSendQueue, 21, &SendTask);
    }
    InterfaceCtrl *itf = &dummy_itf;
    if ( iid == CAN_BUS ) {
        // CAN is special and not on all HW versions. 
        // Do not try to create it
        if ( CAN && CAN->getTestOk() ) {
            if ( ! CAN->isInitialized() ) {
                CAN->ConfigureIntf(0); // default setup
            }
            itf = CAN;
        }
    }
    // else if ( iid == XCVPROXY) {
    //     itf = &dummy_itf;
    //     Device *dev = getXCVPeer();
    //     if ( dev ) {
    //         // XCV proxy is already there
    //         itf = dev->_itf;
    //         listen_port = dev->getListenPort();
    //         send_port = dev->getSendPort(XCVSYNC_P);
    //     }
    // }
    else if ( iid == WIFI_AP) {
        if ( ! Wifi ) {
            WifiAP::createWifiAP();
        }
        if ( Wifi ) {
            Wifi->ConfigureIntf(listen_port);
            itf = Wifi;
        }
    }
    else if ( iid == S1_RS232) {
        if ( S1 ) {
            S1->ConfigureIntf(-1); // load nvs setup
            itf = S1;
        }
    }
    else if ( iid == S2_RS232) {
        if ( S2 ) {
            S2->ConfigureIntf(-1);
            itf = S2;
        }
    }
    else if ( iid == BT_SPP) {
        if ( ! BTspp ) {
            BTspp = new BTSender();
        }
        if ( BTspp && ! BTspp->isRunning() ) {
            BTspp->ConfigureIntf(0);
            BTspp->start();
        }
        itf = BTspp;
    }
    else if ( iid == BT_LE) {
        // if ( ) .. fixme
        // blesender.begin();
        // itf = BT_..;
    }
    else if ( iid == I2C ) {
        if ( ! I2Cext && (! S2 || ! S2->getTestOk()) ) {
            // double check S2 not active - they share the gpio's
            itf = new I2CWrapper();
            itf->ConfigureIntf(0); // default setup
            ESP_LOGI(FNAME, "I2C conf");
        }
        itf = I2Cext;
    }
    // else // NO_PHY is just the hint to take the same interface
    
    bool is_new = false;
    Device *dev = getDevice(did);
    if (dev && itf == &dummy_itf) { // that means NO_PHY -> "take the same" was requeset
        // Device already exists
        itf = dev->_itf;
    }
    if ( dev && dev->_itf != itf ) {
        // Device already exists, but on another interface
        ESP_LOGW(FNAME, "Device %d already exists on another interface", did);
        return nullptr;
    }
    if ( ! dev ) {
        dev = new Device(did);
        is_new = true;
        // Retrieve, or create a new data link
        DataLink *dl = itf->newDataLink(listen_port);
        dev->_link = dl;
        dl->incrDeviceCount();
        dev->_itf = itf;

        // some devices we need to create some gears to process the data stream
        if ( did == MAGLEG_DEV ) {
            Compass::createCompass(itf->getId());
        }
    }

    EnumList pl = dev->_link->addProtocol(proto, did, send_port); // Add proto, if not yet there
    dev->_protos.insert(pl.begin(), pl.end());

    xSemaphoreTake(_devmap_mutex, portMAX_DELAY);
    if ( is_new ) {
        // Add only if new
        _device_map[did] = dev; // and add, in case this dev is new
        _interface_map[itf->getId()] = itf;
    }
    xSemaphoreGive(_devmap_mutex);
    refreshRouteCache();

    // ESP_LOGI(FNAME, "After add device %d.", did);
    // dumpMap();

    return dev;
}

Device *DeviceManager::getDevice(DeviceId did)
{
    DevMap::iterator it = _device_map.find(did);
    if ( it != _device_map.end() ) {
        return it->second;
    }
    return nullptr;
}

Device *DeviceManager::getXCVPeer()
{
    DevMap::iterator it = _device_map.find((xcv_role.get()==MASTER_ROLE)?XCVARIOSECOND_DEV:XCVARIOFIRST_DEV);
    if ( it != _device_map.end() ) {
        return it->second;
    }
    return nullptr;
}

ProtocolItf *DeviceManager::getProtocol(DeviceId did, ProtocolType proto)
{
    Device *d = getDevice(did);
    if ( d ) {
        return d->getProtocol(proto);
    }
    return nullptr;
}

NmeaPrtcl *DeviceManager::getNMEA(DeviceId did)
{
    Device *d = getDevice(did);
    if ( d ) {
        return d->_link->getNmea();
    }
    return nullptr;
}

// convenience
int DeviceManager::getSendPort(DeviceId did, ProtocolType proto)
{
    Device *dev = getDevice(did);
    if ( dev ) {
        return dev->getSendPort(proto);
    }
    return -1;
}

// Remove device from map, delete device and all resources
// returns true, when a reboot is needed
bool DeviceManager::removeDevice(DeviceId did)
{
    xSemaphoreTake(_devmap_mutex, portMAX_DELAY);
    DevMap::iterator it = _device_map.find(did);
    Device* dev = nullptr;
    if ( it != _device_map.end() ) {
        dev = it->second;
        ESP_LOGI(FNAME, "Delete device %d", did);
        _device_map.erase(it);
    }
    xSemaphoreGive(_devmap_mutex); // handling device map is done
    bool ret = false;

    if ( dev ) {
        InterfaceCtrl *itf = dev->_itf;
        delete dev;
        // is it the last device on this interface
        if ( itf->getNrDLinks() == 0 ) {
            _interface_map.erase(itf->getId());
            if ( itf == CAN ) {
                ESP_LOGI(FNAME, "stopping CAN");
                CAN->stop();
            }
            else if ( itf == Wifi ) { // fixme, restart not reliably working
            //     ESP_LOGI(FNAME, "stopping Wifi");
            //     WifiAP *tmp = Wifi;
            //     Wifi = nullptr;
            //     delete tmp;
                ret = true; // restart needed
            }
            else if ( itf == BTspp ) {
                ESP_LOGI(FNAME, "stopping BTspp");
                BTspp->stop();
                ret = true; // restart needed
            }
            else if ( itf == S1 ) {
                ESP_LOGI(FNAME, "stopping S1");
                S1->stop();
            }
            else if ( itf == S2 ) {
                ESP_LOGI(FNAME, "stopping S2");
                S2->stop();
            }
            else if ( itf == I2Cext ) {
                ESP_LOGI(FNAME, "stopping I2C");
                delete I2Cext;
            }
        }
    }
    refreshRouteCache();

    ESP_LOGI(FNAME, "After remove device %d.", did);
    dumpMap();
    
    return ret;
}

// routing lookup table
InterfaceCtrl* DeviceManager::getIntf(DeviceId did)
{
    xSemaphoreTake(_devmap_mutex, portMAX_DELAY);
    DevMap::iterator it = _device_map.find(did);
    InterfaceCtrl* tmp = nullptr;
    if ( it != _device_map.end() ) {
        tmp = it->second->_itf;
    }
    xSemaphoreGive(_devmap_mutex);
    return tmp;
}

bool DeviceManager::isIntf(ItfTarget tgt) const
{
    auto it = _interface_map.find(tgt.iid);
    if ( it != _interface_map.end() ) {
        return true;
    }
    return false;
}

bool DeviceManager::isAvail(InterfaceId iid) const
{
    // CAN depends on HW revision only
    if ( iid == CAN_BUS && hardwareRevision.get() < XCVARIO_22 ) {
        return false;
    }
    // S2 depends on WH revision and shares pins with I2C
    else if ( iid == S2_RS232 && (isIntf(I2C) || hardwareRevision.get() < XCVARIO_21) ) {
        return false;
    }
    else if ( iid == I2C && isIntf(S2_RS232) ) {
        return false;
    }
    // BT and WIFI are mutually exclusive
    else if ( (iid == BT_SPP || iid == BT_LE) && isIntf(WIFI_AP) ) {
        return false;
    }
    else if ( iid == WIFI_AP && (isIntf(BT_SPP) || isIntf(BT_LE)) ) {
        return false;
    }
    return true;
}


// Result is cashed for performance purpose.
RoutingList DeviceManager::getRouting(RoutingTarget source)
{
    // find routing entry, respect NO_PHY wildcards
    const RoutingTarget *route_list = findRoute(source);
    ESP_LOGD(FNAME, "Search route: %x(%d:%d:%d)", (unsigned)source.raw, source.getDeviceId(), source.getItfTarget().iid, source.getItfTarget().port);
    if ( route_list )
    {
        RoutingList activeList;

        // look for existing devices and push only those to the routing list
        for (const RoutingTarget* entry = route_list; entry->raw != 0; ++entry)
        {
            ESP_LOGD(FNAME, "Try match: d%d/i%d/o%d", entry->did, entry->getItfId(), entry->getPort());
            // is dev configured, are itf and port identical
            DevMap::iterator devit = _device_map.find(entry->did);
            if ( devit != _device_map.end() ) {
                // check itf and  port
                Device *dev = devit->second;
                ESP_LOGD(FNAME, "Found dev %d on itf: %d", devit->first, dev->_itf->getId());
                if ( dev->_itf->getId() == entry->getItfId() ) {
                    ESP_LOGD(FNAME, "Itfmatch: %d==%d", dev->_itf->getId(), dev->_itf->getId());
                    EnumList pl = dev->getSendPortList();
                    if ( entry->getPort()==0 && pl.size()==1) {
                        // Take this joker port one
                        RoutingTarget merget = *entry;
                        merget.setItfPort(*pl.begin());
                        ESP_LOGD(FNAME, "Take joker port %d, d%d/i%d/p%d", *pl.begin(), merget.did, merget.getItfId(), merget.getPort());
                        activeList.insert(merget);
                    }
                    else if ( pl.find(entry->getItfTarget().port) != pl.end() ) {
                        ESP_LOGD(FNAME, "Take route d%d/i%d/p%d", entry->did, entry->getItfId(), entry->getPort());
                        activeList.insert(*entry); // Ok, take this route
                    }
                }
            }
        }
        return activeList; // will be a move by RVO
    }
    else {
        return RoutingList();
    }
}

// Refresh routes after a change of the devices map
void DeviceManager::refreshRouteCache()
{
    for ( auto dev : _device_map ) {
        // all devices
        dev.second->_link->updateRoutes(dev.second->_id);
    }
}

void DeviceManager::setFlarmBPInitiator(DataLink *dl)
{
    _flarm_bp = dl;
}

// Start a binary data route
DataLink *DeviceManager::getFlarmBPInitiator()
{
    return _flarm_bp;
}

void DeviceManager::reserectFromNvs()
{
    // go through all possible setup entries and see if there is some thing valid
    int nr_set_up = 0;
    for (const auto& entry : DEVATTR) {
        // check nvsetup exists - otherwise a reserection is not desired (auto setup)
        // check if store data has the valid tag
        // check on role compatibility
        if ( entry.second.nvsetup && entry.second.nvsetup->get().isValid() 
            && ( !entry.second.getRoleDep() || entry.second.getRoleDep() == xcv_role.get()) ) {
            // setup device
            DeviceNVS *nvs = static_cast<DeviceNVS*>(entry.second.nvsetup->getPtr());
            ESP_LOGI(FNAME, "Entry: t%x - s%x (%d/%d)", (unsigned)nvs->target.raw, (unsigned)nvs->setup.data, nvs->bin_sp, nvs->nmea_sp);
            DeviceId did = nvs->target.getDeviceId();
            int listen_port = nvs->target.getItfTarget().port;
            InterfaceId iid = nvs->target.getItfTarget().iid;
            ESP_LOGI(FNAME, "Reserect: did%d p%d iid%d", did, listen_port, iid);
            // bin proto option
            if ( nvs->setup.getProto(0) ) {
                ESP_LOGI(FNAME, "BinP: pid%d sp%d", nvs->setup.getProto(0), nvs->getBinSPort());
                addDevice(did, nvs->setup.getProto(0), listen_port, nvs->getBinSPort(), iid);
                nr_set_up++;
            }
            // principle nmea proto option
            ESP_LOGI(FNAME, "Nmea: sendport%d", nvs->getNmeaSPort());
            // more nmea plug options
            for ( int i=1; i<ProtocolList::maxProto; i++ ) {
                if ( nvs->setup.getProto(i) ) {
                    ESP_LOGI(FNAME, "plugin: %d", nvs->setup.getProto(i));
                    addDevice(did, nvs->setup.getProto(i), listen_port, nvs->getNmeaSPort(), iid);
                    nr_set_up++;
                }
            }
        }
    }
    ESP_LOGI(FNAME, "Reserected %d dev entries from NVS", nr_set_up);
    if ( nr_set_up == 0 ) {
        // A very first start w/ devices. Add a Flarm automatically
        assert(S1);
        S1->ConfigureIntf(SM_FLARM); // load flarm serial default profile
        Device *dev = DEVMAN->addDevice(FLARM_DEV, FLARM_P, 0, 0, S1_RS232);
        DEVMAN->addDevice(FLARM_DEV, FLARMBIN_P, 0, 0, NO_PHY);
        if ( dev ) {
            // save it to nvs
            flarm_devsetup.set(dev->getNvsData());
        }
        if ( S2 ) {
            S2->ConfigureIntf(SM_XCTNAV_S3); // load XCTouchNav serial default profile
            dev = DEVMAN->addDevice(NAVI_DEV, XCVARIO_P, 0, 0, S2_RS232);
            if ( dev ) { navi_devsetup.set(dev->getNvsData()); }
            dev = DEVMAN->addDevice(FLARM_HOST_DEV, FLARMHOST_P, 0, 0, S2_RS232);
            if ( dev ) { flarm_host_setup.set(dev->getNvsData()); }
        }
    }
}

// prio - 0,..,4 0:low prio data stream, .. 5: important high prio commanding
// return an id chunk of four to use (0..0x3fc), use then +0, +1, +2, +3
// else -1 in case there is no chunk left
static int nxt_free_slot[5] = { 0x400, 0x200, 0x100, 0x80, 0x40 }; // 5 prio slots of 15 chucks of 4 ids
int DeviceManager::reserveCANId(int prio)
{
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

void DeviceManager::undoReserveCANId(int prio)
{
    if ( prio > 4 ) {
        prio = 4;
    }
    if ( prio < 0 ) {
        prio = 0;
    }
    if ( (nxt_free_slot[prio] & 0x3c) >= 4 ) {
        nxt_free_slot[prio] -= 4; // revert reservation
    }
}

void DeviceManager::dumpMap() const
{
    ESP_LOGI(FNAME, "Device map dump:");
    for ( auto &it : _device_map ) {
        Device* d = it.second;
        ESP_LOGI(FNAME, "%d: %p (did%d/%s(%d))", it.first, d, d->_id, d->_itf->getStringId(), d->_itf->getId());
        d->_link->dumpProto();
    }
}

void DeviceManager::startMonitoring(ItfTarget tgt)
{
    // all interfaces
    for ( auto itf : _interface_map ) {
        itf.second->startMonitoring(tgt);
    }
    // activate also the send callback
    monitor_target = tgt;
}

void DeviceManager::stopMonitoring()
{
    // stop send monitor
    monitor_target = ItfTarget();
    // all interfaces
    for ( auto itf : _interface_map ) {
        itf.second->stopMonitoring();
    }
}

std::vector<const Device*> DeviceManager::allDevs() const
{
    std::vector<const Device*> ret;
    for ( auto &it : _device_map ) {
        ret.push_back(it.second);
    }
    return ret;
}

//
// A Device
//

// Resolve the existance of a device
Device::~Device()
{
    ESP_LOGI(FNAME, "Dtor device %d.", _id);
    // Detach data links from interface
    if ( _link->decrDeviceCount() == 0 ) {
        // last device on this link
        ESP_LOGI(FNAME, "Last device on %s", _itf->getStringId());
        _itf->DeleteDataLink(_link->getPort());
    }
    else {
        ESP_LOGI(FNAME, "Still %d devices on %s, remove protos.", _link->getDeviceCount(), _itf->getStringId());
        for (auto it : _protos) {
            _link->removeProtocol(static_cast<ProtocolType>(it));
        }
        // _link->removeId(_id); not needed
    }
}

ProtocolItf *Device::getProtocol(ProtocolType p) const
{
    // Find protocol
    ProtocolItf *tmp = _link->getProtocol(p);
    if ( tmp ) {
        return tmp;
    }
    return nullptr;
}

DataLink *Device::getDLforProtocol(ProtocolType p) const
{
    // Find protocol
    if ( _link->getProtocol(p) ) {
        return _link;
    }
    return nullptr;
}

EnumList Device::getSendPortList() const
{
    EnumList pl;
    for ( int p : _link->getAllSendPorts()) {
        pl.insert(p);
    }
    return pl;
}

int Device::getListenPort() const
{
    // !!! Assuming we have just one data link per devices fixme
    return _link->getPort();
}

// get all setup lines to write into nvs for this device
DeviceNVS Device::getNvsData() const
{
    ESP_LOGI(FNAME, "NvsData did%d", _id);
    DeviceNVS entry;
    entry.target = RoutingTarget(_id, _link->getTarget());
    ProtocolItf *bn = _link->getBinary(); // binary option
    if ( bn ) {
        entry.bin_sp = bn->getSendPort();
        entry.setup.set(0, bn->getProtocolId());
        ESP_LOGI(FNAME, "NvsData t%x bsp%d pid%d", (unsigned)entry.target.raw, entry.bin_sp, entry.setup.get(0));
    }
    NmeaPrtcl *nm = static_cast<NmeaPrtcl*>(_link->getNmea());
    
    if ( nm ) {
        entry.nmea_sp = nm->getSendPort();
        // entry.setup.set(1, nm->getProtocolId());
        int idx = 1;
        ESP_LOGI(FNAME, "NvsData t%x nmsp%d", (unsigned)entry.target.raw, entry.nmea_sp);
        for (auto it : nm->getAllPlugs() ) {
            // list when not auto loaded, but does belong to this device
            if ( ! it->getAuto() && it->getRouteId() == _id ) {
                entry.setup.set(idx, it->getPtyp()); // plugins
                ESP_LOGI(FNAME, "NvsExtra %d: pid%d", idx, entry.setup.get(idx));
                idx++;
            }
            else {
                ESP_LOGI(FNAME, "NvsDropped  pid%d did%d", it->getPtyp(), _id);
            }
            if ( idx >= ProtocolList::maxProto ) {
                ESP_LOGW(FNAME, "Nvs no space left %d: pid%d", idx, entry.setup.get(idx));
                break; // no more space
            }
        }
    }
    entry.setup.flags=1; // set valid
    return entry;
}


int Device::getSendPort(ProtocolType p) const
{
    // get port for protocol
    ProtocolItf *tmp = getProtocol(p);
    if ( tmp ) {
        return tmp->getSendPort();
    }

    return -1; // invalid port
}



// A set of convenience routines to use the pool
namespace DEV
{

Message* acqMessage(DeviceId target_id, int port)
{
    Message* m = MP.getOne();
    m->target_id = target_id;
    m->port = port;
    return m;
}

void relMessage(Message *msg)
{
    MP.recycleMsg(msg);
}

bool Send(Message* msg)
{
    if ( pdTRUE != xQueueSend( ItfSendQueue, (void * ) &msg, portMAX_DELAY ) ) {
        // drop it
        ESP_LOGW(FNAME, "Dropped message to %d", msg->target_id);
        MP.recycleMsg(msg);
        return false;
    }
    return true;
}

} // namespace

