/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "DeviceMgr.h"

#include "InterfaceCtrl.h"
#include "comm/Messages.h"
#include "comm/CanBus.h"
#include "protocol/ProtocolItf.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>
#include <logdef.h>

// global variables
DeviceManager* DEVMAN = nullptr; // singleton like
QueueHandle_t ItfSendQueue = 0;

// static vars
static TaskHandle_t SendTask = nullptr;

// generic transmitter grabbing messages from a queue
void TransmitTask(void *arg)
{
    QueueHandle_t queue = (QueueHandle_t)arg;
    Message *msg;

    while (true)
    {
        // sleep until the queue gives us something to do
        if (xQueueReceive(queue, &msg, portMAX_DELAY))
        {

            if (msg == nullptr)
            {
                break;
            } // termination signal

            int len = msg->buffer.length(); // Including the terminating \0 -> need to remove this one byte at RX from strlen
            int port = msg->port;
            InterfaceCtrl *itf = DEVMAN->getIntf(msg->target_id);
            if (itf)
            {
                ESP_LOGI(FNAME, "send %s/%d NMEA len %d, msg: %s", itf->getStringId(), port, len, msg->buffer.c_str());

                if ( !itf->Send(msg->buffer.c_str(), len - 1, port) ) {
                    ESP_LOGI(FNAME, "failed sending");
                }
            }
            DEV::relMessage(msg);
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
    ItfSendQueue = xQueueCreate( 10, sizeof(Message*) );
    
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
ProtocolItf* DeviceManager::addDevice(DeviceId did, ProtocolType proto, int listen_port, int send_port, InterfaceCtrl *itf)
{
    // On first device a send task needs to be created
    if ( ! SendTask ) {
        xTaskCreate(TransmitTask, "CanTx", 4096, ItfSendQueue, 10, &SendTask);
    }

    Device *dev = getDevice(did);
    if ( ! dev ) {
        dev = new Device(did);
    }
    DataLink *dl = itf->getDataLink(listen_port);
    dev->_dlink.insert(dl);
    ProtocolItf* ret = dl->setProtocol(proto, send_port);
    dev->_itf = itf;

    dumpMap();
    _device_map[did] = dev; // and add
    ESP_LOGI(FNAME, "Add device %d", did);
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

ProtocolItf *DeviceManager::getProtocol(DeviceId dev, ProtocolType proto)
{
    Device *d = getDevice(dev);
    if ( d ) {
        return d->getProtocol(proto);
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
InterfaceCtrl* DeviceManager::getIntf(DeviceId did)
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

void DeviceManager::dumpMap() const
{
    ESP_LOGI(FNAME, "Device map dump:");
    for ( auto &it : _device_map ) {
        Device* d = it.second;
        ESP_LOGI(FNAME, "%d: %p (did%d/iid%s/dl*%d)", it.first, d, d->_id, d->_itf->getId(), d->_dlink.size());
        for ( auto &l : d->_dlink ) {
            ProtocolItf* p = l->getProtocol(NO_ONE);
            ESP_LOGI(FNAME, "  l%d: did%d,pid%d,sp%d", l->getPort(), p->getDeviceId(), p->getProtocolId(), p->getSendPort());
        }
    }
}

// Resolve the existance of a device
Device::~Device()
{
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

int Device::getSendPort(ProtocolType p) const
{
    // get port for protocol
    ProtocolItf *tmp = getProtocol(p);
    if ( tmp ) {
        return tmp->getSendPort();
    }

    return 0;
}

