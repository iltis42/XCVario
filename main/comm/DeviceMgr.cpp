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
#include "SerialLine.h"
#include "protocol/ProtocolItf.h"
#include "DataMonitor.h"

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

// static routing table on device level
//
// entries with zero termination, entirely as ro flash data, no RAM usage
static constexpr RoutingTarget flarm_routes[] = { {NAVI_DEV, S2_RS232, 0}, {NAVI_DEV, WIFI_AP, 8881}, {NAVI_DEV, BT_SPP, 0}, {XCVARIOCLIENT_DEV, CAN_BUS, 20}, {XCVARIO_DEV, CAN_BUS, 20}, {} };
static constexpr RoutingTarget krt2_routes[] = { {NAVI_DEV, S2_RS232, 0}, {NAVI_DEV, WIFI_AP, 8882}, {NAVI_DEV, BT_SPP, 0}, {XCVARIOCLIENT_DEV, CAN_BUS, 20}, {XCVARIO_DEV, CAN_BUS, 20}, {} };
static constexpr RoutingTarget navi_routes[] = { {FLARM_DEV, S1_RS232, 0}, {FLARM_DEV, S2_RS232, 0}, {RADIO_KRT2_DEV, S2_RS232, 0}, {FLARM_DEV, CAN_BUS, 20}, {} };
static constexpr std::pair<RoutingTarget, const RoutingTarget*> Routes[] = {
    { RoutingTarget(FLARM_DEV, NO_PHY, 0), flarm_routes },
    { RoutingTarget(RADIO_KRT2_DEV, NO_PHY, 0), krt2_routes },
    { RoutingTarget(NAVI_DEV, NO_PHY, 0), navi_routes }
};
// Search the flash data table
static constexpr const RoutingTarget* findRoute(const RoutingTarget& target) {
    // Search through Routes and find a match to the source device
    for (const auto& entry : Routes) {
        if (entry.first.match(target)) {
            return entry.second; // Return the matched routing list
        }
    }
    return nullptr;
}


// known device attributes
//
// constexpr PackedAttributeList flarm_attr{ PackedAttributeList::pack({ ProtocolType::FLARM_P, ProtocolType::FLARMBIN_P}) };


// Define device names for the configuration menu
constexpr std::pair<DeviceId, DeviceAttributes> DAVATTR[] = {
    {DeviceId::JUMBO_DEV,  {"jumbo putzi", {{CAN_BUS}, {JUMBOCMD_P}, true }}},
    {DeviceId::ANEMOI_DEV, {"Anemoi", {{S1_RS232, S2_RS232, CAN_BUS}, {ANEMOI_P}, true}}},
    {DeviceId::XCVARIO_DEV, {"Master XCV", {{CAN_BUS, WIFI_CLIENT, BT_SPP, S2_RS232}, {XCVSYNC_P}, true}}},
    {DeviceId::XCVARIOCLIENT_DEV, {"Second XCV", {{CAN_BUS, WIFI_AP, BT_SPP, S2_RS232}, {XCVSYNC_P}, true}}},
    {DeviceId::FLARM_DEV,  {"Flarm", {{S1_RS232, CAN_BUS}, {FLARM_P, FLARMBIN_P}, true}}},
    {DeviceId::NAVI_DEV,   {"Navi", {{S2_RS232, WIFI_AP, BT_SPP, BT_LE}, {XCVARIO_P, OPENVARIO_P, BORGELT_P, CAMBRIDGE_P}, true}}},
    {DeviceId::MAGSENS_DEV, {"Magnetic Sensor", {{I2C, CAN_BUS}, {MAGSENS_P, MAGSENSBIN_P}, true}}},
    {DeviceId::RADIO_KRT2_DEV, {"KRT 2", {{S2_RS232, CAN_BUS}, {KRT2_REMOTE_P}, true}}},
    {DeviceId::RADIO_ATR833_DEV, {"ATR833", {{S2_RS232, CAN_BUS}, {ATR833_REMOTE_P}, true}}},
    {DeviceId::MASTER_DEV, {"CAN auto", {{CAN_BUS}, {REGISTRATION_P}, true}}}
    
};

// Lookup functions
DeviceAttributes DeviceManager::getDevAttr(DeviceId did) {
    for (const auto& entry : DAVATTR) {
        if (entry.first == did) {
            return entry.second;
        }
    }
    return DeviceAttributes("no devices", {{}, {}, true});
}

std::string_view DeviceManager::getDevName(DeviceId did) {
    for (const auto& entry : DAVATTR) {
        if (entry.first == did) {
            return entry.second.name;
        }
    }
    return "";
}

std::vector<DeviceId> DeviceManager::allKnownDevs()
{
    std::vector<DeviceId> ret;
    for (const auto& entry : DAVATTR) {
        ret.push_back(entry.first);
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

// a dummy interface
class DmyItf final : public InterfaceCtrl
{
public:
    const char* getStringId() const override { return "NUL"; }
    void ConfigureIntf(int cfg) override {}
    int Send(const char *msg, int &len, int port=0) { return 0; }
};
static DmyItf dummy_itf;

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
        else if ( plsrety == 0 ) {
            DM.monitorString(ItfTarget(itf->getId(),port), DIR_TX, msg->buffer.c_str(), len);
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
// It returns the pointer to the protocol as handle to send messages
ProtocolItf* DeviceManager::addDevice(DeviceId did, ProtocolType proto, int listen_port, int send_port, InterfaceId iid)
{
    // On first device a send task needs to be created
    if ( ! SendTask ) {
        xTaskCreate(TransmitTask, "genTx", 3000, ItfSendQueue, 21, &SendTask);
    }
    InterfaceCtrl *itf = &dummy_itf;
    if ( iid == CAN_BUS ) {
        if ( CAN ) {
            if(!CAN->isInitialized()) {
                CAN->begin();
            }
            itf = CAN;
        }
    }
    else if ( iid == WIFI_AP) {
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
        if ( BTspp && ! BTspp->isRunning() ) {
            BTspp->ConfigureIntf(0);
            BTspp->start();
        }
        itf = BTspp;
    }
    else if ( iid == NO_PHY ) {
        // Device might exist already
        itf = getIntf(did);
    }
    bool is_new = false;
    Device *dev = getDevice(did);
    if ( ! dev ) {
        dev = new Device(did);
        is_new = true;
    }
    // Retrieve, or create a new data link
    DataLink *dl = itf->newDataLink(listen_port);
    ProtocolItf* ret = dl->addProtocol(proto, did, send_port); // Add proto, if not yet there

    if ( is_new ) {
        // Add only if new
        _device_map[did] = dev; // and add, in case this dev is new
        dev->_itf = itf;
    }
    dev->_dlink.insert(dl);
    refreshRouteCache();

    ESP_LOGI(FNAME, "After add device %d.", did);
    dumpMap();

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
            if ( itf->getId() == CAN_BUS ) {
                CAN->stop();
            }
        }
    }
    refreshRouteCache();
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

