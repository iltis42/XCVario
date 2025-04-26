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
#include "BLESender.h"
#include "SerialLine.h"
#include "protocol/ProtocolItf.h"
#include "protocol/NMEA.h"
#include "DataMonitor.h"
#include "SetupNG.h"

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
static constexpr RoutingTarget flarm_routes[] = { 
    {NAVI_DEV, S2_RS232, 0}, {NAVI_DEV, WIFI_AP, 8881}, {NAVI_DEV, BT_SPP, 0}, {XCVARIOCLIENT_DEV, CAN_BUS, 20}, 
    {XCVARIO_DEV, CAN_BUS, 20}, {} };
static constexpr RoutingTarget krt2_routes[] = { 
    {NAVI_DEV, S2_RS232, 0}, {NAVI_DEV, WIFI_AP, 8882}, {NAVI_DEV, BT_SPP, 0}, {XCVARIOCLIENT_DEV, CAN_BUS, 20}, 
    {XCVARIO_DEV, CAN_BUS, 20}, {} };
static constexpr RoutingTarget navi_routes[] = { 
    {FLARM_DEV, S1_RS232, 0}, {FLARM_DEV, S2_RS232, 0}, {RADIO_KRT2_DEV, S2_RS232, 0}, {FLARM_DEV, CAN_BUS, 20}, {} };
static constexpr std::pair<RoutingTarget, const RoutingTarget*> Routes[] = {
    { RoutingTarget(FLARM_DEV, NO_PHY, 0), flarm_routes },
    { RoutingTarget(RADIO_KRT2_DEV, NO_PHY, 0), krt2_routes },
    { RoutingTarget(NAVI_DEV, NO_PHY, 0), navi_routes }
};
// Search the flash data table
static const RoutingTarget* findRoute(const RoutingTarget& target) {
    // Search through Routes and find a match to the source device
    for (const auto& entry : Routes) {
        if (entry.first.match(target)) {
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
    {DeviceId::MASTER_DEV, {"Auto-connect", {{CAN_BUS}}, {{REGISTRATION_P}, 1}, CAN_REG_PORT, IS_REAL, nullptr}}, // no nvs link, start allways
    {DeviceId::FLARM_DEV,  {"Flarm", {{S1_RS232, S2_RS232, CAN_BUS}}, {{FLARM_P, FLARMBIN_P}, 2}, 0, IS_REAL, &flarm_devsetup}},
    {DeviceId::JUMBO_DEV,  {"jumbo putzi", {{CAN_BUS}}, {{JUMBOCMD_P}, 1} , 0, 0, nullptr}}, // auto dev
    {DeviceId::XCVARIO_DEV, {"Master XCV", {{WIFI_CLIENT, BT_SPP, S1_RS232, S2_RS232}}, {{XCVSYNC_P}, 1}, 8884, IS_REAL, &master_devsetup}},
    {DeviceId::XCVARIO_DEV, {"Master finder", {{CAN_BUS}}, {{XCVQUERY_P}, 1}, CAN_REG_PORT, IS_VARIANT, nullptr}}, // auto through XCV role
    {DeviceId::XCVARIOCLIENT_DEV, {"Second XCV", {{WIFI_AP, BT_SPP, S1_RS232, S2_RS232}}, {{XCVSYNC_P}, 1}, 8884, IS_REAL, &second_devsetup}},
    {DeviceId::MAGSENS_DEV, {"Magnetic Sensor", {{I2C}}, {{MAGSENSBIN_P}, 1}, 31, IS_REAL, nullptr}},
    {DeviceId::NAVI_DEV,   {"Navi", {{WIFI_AP, S1_RS232, S2_RS232, BT_SPP, BT_LE, CAN_BUS}}, 
                                    {{XCVARIO_P, CAMBRIDGE_P, OPENVARIO_P, BORGELT_P, FLARMHOST_P, FLARMBIN_P, KRT2_REMOTE_P, ATR833_REMOTE_P}, 2}, 
                                    8880, IS_REAL|MULTI_CONF, &navi_devsetup}},
    {DeviceId::NAVI_DEV,   {"Flarm host", {{WIFI_AP, CAN_BUS}}, {{FLARMHOST_P, FLARMBIN_P}, 2}, 8881, IS_VARIANT, &navi_flarm_setup}},
    {DeviceId::NAVI_DEV,   {"Radio remote", {{WIFI_AP}}, {{KRT2_REMOTE_P}, 1}, 8882, IS_VARIANT, &navi_radio_etup}},
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
    {BT_LE, "BT low energy"},
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
    {REGISTRATION_P, "Auto registration."},
    {XCVSYNC_P, "XCV sync"},
    {JUMBOCMD_P, "jumbo command"},
    {ANEMOI_P, "Anemoi"},
    {FLARM_P, "Flarm"},
    {FLARMHOST_P, "Flarm host"},
    {FLARMBIN_P, "Flarm BP"},
    {MAGSENS_P, "Magsens"},
    {MAGSENSBIN_P, "Magsens BP"},
    {XCVARIO_P, "XCVario"},
    {OPENVARIO_P, "Open-Vario"},
    {BORGELT_P, "Borgelt"},
    {CAMBRIDGE_P, "Cambridge"},
    {KRT2_REMOTE_P, "KRT2"},
    {ATR833_REMOTE_P, "ATR833"}
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
        else if ( plsrety == 0 && monitor_target == ItfTarget(itf->getId(), port) ) {
            DM->monitorString(DIR_TX, msg->buffer.c_str(), len);
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

    // Fixme keep alive and ping code is a application layer concern and has to move
    // void CANbus::txtick(int tick){

    // SString msg;
    // // CAN bus send
    // if ( !can_tx_q.isEmpty() ){
    // 	// ESP_LOGI(FNAME,"There is CAN data");
    // 	if( _connected_xcv ){
    // 		// ESP_LOGI(FNAME,"CAN TX Q:%d", can_tx_q.numElements() );
    // 		while( Router::pullMsg( can_tx_q, msg ) ){
    // 			// ESP_LOGI(FNAME,"CAN TX len: %d bytes Q:%d", msg.length(), can_tx_q.numElements() );
    // 			// ESP_LOG_BUFFER_HEXDUMP(FNAME,msg.c_str(),msg.length(), ESP_LOG_INFO);
    // 			DM.monitorString( MON_CAN, DIR_TX, msg.c_str(), msg.length() );
    // 			if( !sendNMEA( msg ) ){
    // 				_connected_timeout_xcv +=150;  // if sending fails as indication for disconnection
    // 				// ESP_LOGW(FNAME,"CAN TX NMEA failed, timeout=%d", _connected_timeout_xcv );
    // 				if( _connected_timeout_xcv > 1000 )
    // 					recover();
    // 			}
    // 		}
    // 	}
    // }
    // // Router::routeCAN();
    // if( !(tick%100) ){
    // 	if( ((can_mode.get() == CAN_MODE_CLIENT)  && _connected_xcv) || can_mode.get() == CAN_MODE_MASTER ){ // sent from client only if keep alive is there
    // 		msg.set( "K" );
    // 		if( !sendData( _can_id_keepalive_tx, msg.c_str(), 1 ) )
    // 		{
    // 			_connected_timeout_xcv +=150;  // if sending fails as indication for disconnection
    // 			if( !_keep_alive_fails ){
    // 				ESP_LOGW(FNAME,"Permanent CAN TX Keep Alive failure");
    // 				_keep_alive_fails = true;
    // 			}
    // 			if( _connected_timeout_xcv > 1000 )
    // 				recover();
    // 		}else{
    // 			if( _keep_alive_fails ){
    // 				ESP_LOGI(FNAME,"Okay again CAN TX Keep Alive");
    // 				_keep_alive_fails = false;
    // 			}
    // 		}
    // 	}
    // }

    vTaskDelete(NULL);
}

DeviceManager::DeviceManager()
{
    ItfSendQueue = xQueueCreate( MSG_POOL_SIZE+1, sizeof(Message*) );
    
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
Device* DeviceManager::addDevice(DeviceId did, ProtocolType proto, int listen_port, int send_port, InterfaceId iid, bool ato)
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
                CAN->begin();
            }
            itf = CAN;
        }
    }
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
    else if ( iid == NO_PHY ) {
        // Device might exist already
        itf = getIntf(did);
    }
    bool is_new = false;
    Device *dev = getDevice(did);
    if ( ! dev ) {
        dev = new Device(did);
        dev->_auto = ato;
        is_new = true;
    }
    // Retrieve, or create a new data link
    DataLink *dl = itf->newDataLink(listen_port);
    dl->addProtocol(proto, did, send_port); // Add proto, if not yet there

    if ( is_new ) {
        // Add only if new
        _device_map[did] = dev; // and add, in case this dev is new
        dev->_itf = itf;
    }
    dev->_dlink.insert(dl);
    refreshRouteCache();

    ESP_LOGI(FNAME, "After add device %d.", did);
    dumpMap();

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
    DevMap::iterator it = _device_map.find(XCVARIOCLIENT_DEV);
    if ( it != _device_map.end() ) {
        return it->second;
    }
    it = _device_map.find(XCVARIO_DEV);
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
void DeviceManager::removeDevice(DeviceId did)
{
    DevMap::iterator it = _device_map.find(did);
    Device* dev = nullptr;
    if ( it != _device_map.end() ) {
        dev = it->second;
        ESP_LOGI(FNAME, "Delete device %d", did);
        _device_map.erase(it);
        InterfaceCtrl *itf = dev->_itf;
        delete dev;
        // is it the last device on this interface
        if ( itf->getNrDLinks() == 0 ) {
            if ( itf == CAN ) {
                ESP_LOGI(FNAME, "stopping CAN");
                CAN->stop();
            }
            else if ( itf == Wifi ) {
                ESP_LOGI(FNAME, "stopping Wifi");
                delete Wifi;
                Wifi = nullptr;
            }
            else if ( itf == BTspp ) {
                ESP_LOGI(FNAME, "stopping BTspp");
                BTspp->stop();
            }
            else if ( itf == S1 ) {
                ESP_LOGI(FNAME, "stopping S1");
                S1->stop();
            }
            else if ( itf == S2 ) {
                ESP_LOGI(FNAME, "stopping S2");
                S2->stop();
            }
        }
    }
    refreshRouteCache();

    ESP_LOGI(FNAME, "After remove device %d.", did);
    dumpMap();
}

// routing lookup table
InterfaceCtrl* DeviceManager::getIntf(DeviceId did)
{
    DevMap::iterator it = _device_map.find(did);
    if ( it != _device_map.end() ) {
        return it->second->_itf;
    }
    ESP_LOGW(FNAME, "No device %d found", did);
    return nullptr;
}

bool DeviceManager::isIntf(ItfTarget iid) const
{
    for ( auto dev : _device_map ) {
        // all devices
        if ( dev.second->_itf->getId() == iid.iid ) {
            return true;
        }
    }
    return false;
}

bool DeviceManager::isAvail(InterfaceId iid) const
{
    if ( iid == CAN_BUS && (isIntf(I2C) || hardwareRevision.get() < XCVARIO_22) ) {
        return false;
    }
    else if ( iid == I2C && isIntf(CAN_BUS) ) {
        return false;
    }
    else if ( (iid == BT_SPP || iid == BT_LE) && isIntf(WIFI_AP) ) {
        return false;
    }
    else if ( iid == WIFI_AP && (isIntf(BT_SPP) || isIntf(BT_LE)) ) {
        return false;
    }
    else if ( iid == S2_RS232 && hardwareRevision.get() < XCVARIO_21 ) {
        return false;
    }
    return true;
}


// Result should be cashed for performance purpose.
RoutingList DeviceManager::getRouting(RoutingTarget target)
{
    // find routing entry, respect NO_PHY wildcards
    const RoutingTarget *route_it = findRoute(target);
    ESP_LOGI(FNAME, "Search route: %x(%d:%d:%d)", (unsigned)target.raw, target.getDeviceId(), target.getItfTarget().iid, target.getItfTarget().port);
    if ( route_it )
    {
        RoutingList activeList;

        ESP_LOGD(FNAME, "Found route: %x", (unsigned)route_it->raw);
        // look for existing devices and push only those to the routing list
        for (const RoutingTarget* entry = route_it; entry->raw != 0; ++entry)
        {
            ESP_LOGD(FNAME, "Try match: %x", (unsigned)entry->raw);
            // is dev configured, are itf and port identical
            DevMap::iterator devit = _device_map.find(entry->did);
            if ( devit != _device_map.end() ) {
                // check itf and  port
                Device *dev = devit->second;
                ESP_LOGD(FNAME, "Found dev %d on itf: %d", devit->first, dev->_itf->getId());
                if ( dev->_itf->getId() == entry->getItfTarget().iid ) {
                    ESP_LOGD(FNAME, "Itfmatch: %d==%d", dev->_itf->getId(), dev->_itf->getId());
                    PortList pl = dev->getPortList();
                    if ( pl.find(entry->getItfTarget().port) != pl.end() ) {
                        ESP_LOGI(FNAME, "Take dev %d from routing list.", entry->did);
                        activeList.push_back(*entry); // Ok, take this route
                    }
                }
            }
        }
        activeList.shrink_to_fit();
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
        for ( auto dl : dev.second->_dlink ) {
            // all data  links
            dl->updateRoutes();
        }
    }
}

// Start a binary data route
DataLink *DeviceManager::getFlarmHost()
{
    for ( auto &d : _device_map ) {
        DataLink *dl = d.second->getDLforProtocol(FLARMHOST_P);
        if ( dl ) return dl;
    }
    return nullptr;
}

// DeviceNVS DeviceManager::makePersistent()
// {
//     std::string token_base(flarm_devsetup.key(), 3); // first entry used as template
//     int nvs_idx = 0;
//     for ( auto &d : _device_map )
//     {
//         for ( auto nvs_item : d.second->getNvsData() ) {
//             std::string token = token_base + std::to_string(nvs_idx++);
//             ESP_LOGI(FNAME, "NVS for did%d %s", d.second->_id, token.c_str());
//             ESP_LOGI(FNAME, "NvsData: t%x - s%x (%d/%d)", (unsigned)nvs_item.target.raw, (unsigned)nvs_item.setup.data, nvs_item.bin_sp, nvs_item.nmea_sp);
//             SetupNG<DeviceNVS> nvs(token.c_str(), {});
//             nvs.set(nvs_item, false, false);
//             nvs.commit();
//         }
//     }
//     ESP_LOGI(FNAME, "Wrote %d dev entries to NVS", nvs_idx);
//     // Remove trailing empty entries
//     while ( nvs_idx < 30 ) {
//         std::string token = token_base + std::to_string(nvs_idx++);
//         SetupNG<DeviceNVS> nvs(token.c_str(), {});
//         if ( ! nvs.exists() ) {
//             break;
//         };
//         nvs.erase();
//     }
// }

void DeviceManager::reserectFromNvs()
{
    // go through all possible setup entries and see if there is some thing valid
    int nr_set_up = 0;
    for (const auto& entry : DEVATTR) {
        if ( entry.second.nvsetup ) {
            nr_set_up++;
            // setup device
            DeviceNVS &nvs = entry.second.nvsetup->getRef();
            ESP_LOGI(FNAME, "Entry: t%x - s%x (%d/%d)", (unsigned)nvs.target.raw, (unsigned)nvs.setup.data, nvs.bin_sp, nvs.nmea_sp);
            DeviceId did = nvs.target.getDeviceId();
            int listen_port = nvs.target.getItfTarget().port;
            InterfaceId iid = nvs.target.getItfTarget().iid;
            ESP_LOGI(FNAME, "Reserect: did%d p%d iid%d", did, listen_port, iid);
            // bin proto option
            if ( nvs.setup.getProto(0) ) {
                ESP_LOGI(FNAME, "BinP: pid%d sp%d", nvs.setup.getProto(0), nvs.getBinSPort());
                addDevice(did, nvs.setup.getProto(0), listen_port, nvs.getBinSPort(), iid);
            }
            // principle nmea proto option
            if ( nvs.setup.getProto(1) ) {
                ESP_LOGI(FNAME, "NmP: pid%d sp%d", nvs.setup.getProto(1), nvs.getNmeaSPort());
                addDevice(did, nvs.setup.getProto(1), listen_port, nvs.getNmeaSPort(), iid);
            }
            // more nmea plug options
            for ( int i=2; i<ProtocolList::maxProto; i++ ) {
                if ( nvs.setup.getProto(i) ) {
                    ESP_LOGI(FNAME, "plugin: %d", nvs.setup.getProto(i));
                    addDevice(did, nvs.setup.getProto(i), listen_port, nvs.getNmeaSPort(), iid);
                }
            }
        }
    }
    ESP_LOGI(FNAME, "Reserected %d dev entries from NVS", nr_set_up);
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
        ESP_LOGI(FNAME, "%d: %p (did%d/iid%s/dl*%d)", it.first, d, d->_id, d->_itf->getStringId(), d->_dlink.size());
        for ( auto &l : d->_dlink ) {
            l->dumpProto();
        }
    }
}

bool DeviceManager::startMonitoring(ItfTarget iid)
{
    bool ret = false;
    for ( auto dev : _device_map ) {
        // all devices
        for (DataLink* dl : dev.second->_dlink) {
            ESP_LOGI(FNAME, "DM %x<>%x", (unsigned)dl->getTarget().raw, (unsigned)iid.raw );
            if ( dl->getTarget() == iid ) {
                ESP_LOGI(FNAME, "DM activate");
                ret = dl->isBinActive();
                dl->setMonitor(true);
            }
            else {
                dl->setMonitor(false);
            }
        }
    }
    // activate the send callback
    monitor_target = iid;
    return ret;
}

void DeviceManager::stopMonitoring()
{
    monitor_target = ItfTarget();
    for ( auto dev : _device_map ) {
        // all devices
        for (DataLink* dl : dev.second->_dlink) {
            dl->setMonitor(false);
        }
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
    for (DataLink* dl : _dlink) {
        _itf->DeleteDataLink(dl->getPort());
    }
    // Clear the set
    _dlink.clear();
}

ProtocolItf *Device::getProtocol(ProtocolType p) const
{
    // Find protocol
    for (DataLink* dl : _dlink) {
        ProtocolItf *tmp = dl->getProtocol(p);
        if ( tmp ) {
            return tmp;
        }
    }
    return nullptr;
}

DataLink *Device::getDLforProtocol(ProtocolType p) const
{
    // Find protocol
    for (DataLink* dl : _dlink) {
        if ( dl->getProtocol(p) ) {
            return dl;
        }
    }
    return nullptr;
}

PortList Device::getPortList() const
{
    PortList pl;
    for (DataLink* dl : _dlink) {
        for ( int p : dl->getAllSendPorts()) {
            pl.insert(p);
        }
    }
    return pl;
}

// get all setup lines to write into nvs for this device
DeviceNVS Device::getNvsData() const
{
    ESP_LOGI(FNAME, "NvsData #ld%d", _dlink.size());
    DeviceNVS entry;
    if ( !_dlink.empty() ) {
        DataLink* dl = *_dlink.begin();
        entry.target = RoutingTarget(_id, dl->getTarget());
        int idx = 0;
        ProtocolItf *bn = dl->getBinary(); // binary option
        if ( bn ) {
            entry.bin_sp = bn->getSendPort();
            entry.setup.set(idx++, bn->getProtocolId());
            ESP_LOGI(FNAME, "NvsData t%x bsp%d pid%d", (unsigned)entry.target.raw, entry.bin_sp, entry.setup.get(0));
        }
        NmeaPrtcl *nm = static_cast<NmeaPrtcl*>(dl->getNmea());
        if ( nm ) {
            entry.nmea_sp = nm->getSendPort();
            entry.setup.set(idx++, nm->getProtocolId()); // principal entry
            for (auto it : nm->getAllPlugs() ) {
                if ( ! it->getAuto() ) {
                    entry.setup.set(idx++, it->getPtyp()); // plugins
                }
            }
        }
        entry.setup.flags=1; // set valid
    }
    return entry;
}
// std::vector<DeviceNVS> Device::getNvsData() const
// {
//     std::vector<DeviceNVS> NL;
//     if ( ! _auto ) {
//         NL.reserve(_dlink.size());
//         ESP_LOGI(FNAME, "NvsData #ld%d", _dlink.size());
//         for (DataLink* dl : _dlink) {
//             DeviceNVS entry;
//             entry.target = RoutingTarget(_id, dl->getTarget());
//             int idx = 0;
//             ProtocolItf *bn = dl->getBinary(); // binary option
//             if ( bn ) {
//                 entry.bin_sp = bn->getSendPort();
//                 entry.setup.set(idx++, bn->getProtocolId());
//                 ESP_LOGI(FNAME, "NvsData t%x bsp%d pid%d", (unsigned)entry.target.raw, entry.bin_sp, entry.setup.get(0));
//             }
//             NmeaPrtcl *nm = static_cast<NmeaPrtcl*>(dl->getNmea());
//             if ( nm ) {
//                 entry.nmea_sp = nm->getSendPort();
//                 entry.setup.set(idx++, nm->getProtocolId()); // principal entry
//                 for (auto it : nm->getAllPlugs() ) {
//                     if ( ! it->getAuto() ) {
//                         entry.setup.set(idx++, it->getPtyp()); // plugins
//                     }
//                 }
//             }
//             NL.push_back(entry);
//         }
//     }
//     else {
//         ESP_LOGI(FNAME, "NvsData AUTO");
//     }
//     return NL;
// }


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

