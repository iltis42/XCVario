/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "DataLink.h"

#include "protocol/nmea/CANMasterRegMsg.h"
#include "protocol/JumboCmdHost.h"
#include "protocol/nmea/FlarmMsg.h"
#include "protocol/nmea/FlarmHostMsg.h"
#include "protocol/nmea/GarminMsg.h"
#include "protocol/nmea/GpsMsg.h"
#include "protocol/nmea/XCVarioMsg.h"
#include "protocol/nmea/OpenVarioMsg.h"
#include "protocol/nmea/MagSensMsg.h"
#include "protocol/FlarmBin.h"
#include "protocol/MagSensBin.h"
#include "protocol/TestQuery.h"
#include "Messages.h"
#include "DeviceMgr.h"

#include "DataMonitor.h"

#include "logdef.h"

DataLink::~DataLink()
{
    for (auto *it : _all_p) {
        ESP_LOGI(FNAME, "delete  prtcl %p", it);
        delete it;
    }
    _all_p.clear();
}

// protocol factory
ProtocolItf* DataLink::addProtocol(ProtocolType ptyp, DeviceId did, int sendport)
{

    // Check if already there
    bool foundit = false;
    for (auto *it : _all_p) {
        if ( (*it).getProtocolId() == ptyp 
            && (*it).getSendPort() == sendport ) {
            foundit = true;
        }
    }
    if ( ! foundit ) {
        // Create a new one
        ProtocolItf *tmp = nullptr;
        switch (ptyp)
        {
        case REGISTRATION_P:
        {
            ESP_LOGI(FNAME, "New CANMasterReg");
            NmeaPrtcl *nmea = new NmeaPrtcl(did, sendport, ptyp, _sm, *this);
            nmea->addPlugin(new CANMasterRegMsg(*nmea));
            tmp = nmea;
            break;
        }
        case JUMBOCMD_P:
            ESP_LOGI(FNAME, "New JumboCmdHost");
            tmp = new JumboCmdHost(sendport, _sm, *this);
            break;
        case FLARM_P:
        {
            ESP_LOGI(FNAME, "New Flarm");
            NmeaPrtcl *nmea = new NmeaPrtcl(did, sendport, ptyp, _sm, *this);
            nmea->addPlugin(new GpsMsg(*nmea));
            nmea->addPlugin(new GarminMsg(*nmea));
            nmea->addPlugin(new FlarmMsg(*nmea));
            tmp = nmea;
            break;
        }
        case FLARMHOST_P:
        {
            ESP_LOGI(FNAME, "New FlarmHost");
            NmeaPrtcl *nmea = new NmeaPrtcl(did, sendport, ptyp, _sm, *this);
            nmea->addPlugin(new FlarmHostMsg(*nmea));
            tmp = nmea;
            break;
        }
        case FLARMBIN_P:
            ESP_LOGI(FNAME, "New FlarmBinary");
            tmp = new FlarmBinary(did, sendport, _sm, *this);
            break;
        case MAGSENS_P:
        {
            ESP_LOGI(FNAME, "New CAN MagSens");
            NmeaPrtcl *nmea = new NmeaPrtcl(did, sendport, ptyp, _sm, *this);
            nmea->addPlugin(new MagSensMsg(*nmea));
            tmp = nmea;
            break;
        }
        case MAGSENSBIN_P:
            ESP_LOGI(FNAME, "New MAGCANBinary");
            tmp = new MagSensBinary(sendport, _sm, *this);
            break;
        case XCVARIO_P:
        {
            ESP_LOGI(FNAME, "New XCVario");
            NmeaPrtcl *nmea = new NmeaPrtcl(did, sendport, ptyp, _sm, *this);
            nmea->addPlugin(new XCVarioMsg(*nmea));
            tmp = nmea;
            break;
        }
        case OPENVARIO_P:
        {
            ESP_LOGI(FNAME, "New OpenVario");
            NmeaPrtcl *nmea = new NmeaPrtcl(did, sendport, ptyp, _sm, *this);
            nmea->addPlugin(new OpenVarioMsg(*nmea));
            tmp = nmea;
            break;
        }
        case TEST_P:
            ESP_LOGI(FNAME, "New Test Proto");
            tmp = new TestQuery(did, sendport, _sm, *this);
            break;
        default:
            break;
        }
        ESP_LOGI(FNAME, "On send port %d", sendport);

        if ( tmp ) {
            // Check device id is equal to all others
            if ( _did == NO_DEVICE ) {
                _did = tmp->getDeviceId(); // why not = did?
            } else {
                for ( auto it : _all_p ) {
                    if ( (*it).getDeviceId() != _did ) {
                        ESP_LOGW(FNAME, "DevId missmatch in protocol list for Itf/port %d/%d %d: %d.", 
                            _itf_id.iid, _itf_id.port, _did, (*it).getDeviceId());
                    }
                }
            }
            _all_p.push_back(tmp);
            if ( _all_p.size() == 1 && tmp->isBinary() ) {
                _binary = tmp;
            }
            else if ( getNumNMEA() == 1 ) {
                _nmea = tmp;
            }
            return tmp;
        }
    }
    else {
        ESP_LOGW(FNAME, "Double insertion of device/protocol %d/%d.", did, ptyp);
    }
    return nullptr;
}

ProtocolItf* DataLink::getProtocol(ProtocolType ptyp) const
{
    // ESP_LOGI(FNAME, "DL itf%d port%d did%d looking for proto%d", _itf_id.iid, _itf_id.port, _did, ptyp);
    if ( ptyp == NO_ONE ) {
        if ( ! _all_p.empty() ) {
            return _all_p.front();
        }
    }
    else {
        for (ProtocolItf *it : _all_p) {
            // ESP_LOGI(FNAME, "pc %d", (*it).getProtocolId());
            if ( (*it).getProtocolId() == ptyp ) {
                return it;
            }
        }
    }
    return nullptr;
}

bool DataLink::hasProtocol(ProtocolType ptyp) const
{
    for (ProtocolItf *it : _all_p) {
        if ( (*it).getProtocolId() == ptyp ) {
            return true;
        }
    }
    return false;
}

void DataLink::deleteProtocol(ProtocolItf *proto)
{
    for (auto it = _all_p.begin(); it != _all_p.end(); it++) {
        if (*it == proto) {
            delete *it;
            _all_p.erase(it);
            return;
        }
    }
}

void DataLink::process(const char *packet, int len)
{
    // Feed the data monitor
    DM.monitorString(_itf_id, DIR_RX, packet, len);

    if ( _all_p.empty() ) {
        return;
    }

    datalink_action_t action = NOACTION;

    if (packet == nullptr)
    {
        // Special use, "no data" timeout, might be expected and normal
        // We just reset the protocol state machine then
        goNMEA();
        ESP_LOGW(FNAME, "timeout Itf/Port %d/%d", _itf_id.iid, _itf_id.port);
        return;
    }
    
    if ( _binary ) {
        ESP_LOGD(FNAME, "%d procB %dchar: %c", _itf_id.iid, len, *packet);
        if ( _binary->nextStreamChunk(packet, len) == GO_NMEA ) {
            goNMEA();
        }
    }
    else if ( _nmea )
    {
        ESP_LOGD(FNAME, "%d procN %dchar: %s", _itf_id.iid, len, packet);
        // most likely case, only one protocol to parse
        // process every frame byte through state machine
        ProtocolItf *prtcl = _all_p.front();
        for (int i = 0; i < len; i++) {
            if ( _sm.push(packet[i]) ) {
                action = prtcl->nextByte(packet[i]);
                if ( action )
                {
                    if ( action & FORWARD_BIT ) {
                        forwardMsg(prtcl->getDeviceId());
                    }
                    if ( action == NXT_PROTO ) {
                        _sm.reset();
                        break; // end loop imidiately
                    }
                    else if ( action == GO_NMEA ) {
                        goNMEA();
                    }

                }
                ESP_LOGD(FNAME, "crc := %d", _sm._crc);
            } else {
                _sm.reset();
            }
        }
    }
    else 
    {
        // multiple protocols registered
        // todo

        // process every frame byte through state machine
        // for (int i = 0; i < len; i++)
        // {
        //     // if ( buf.size() < ProtocolItf::MAX_LEN ) {
        //     //     buf += packet[i];
        //     // }
        //     _sm.push(c);
        //     for (ProtocolItf *it : _all_p)
        //     {
        //         ProtocolItf *prtcl = _gotit ? _gotit : it;
        //         state = prtcl->nextByte(packet[i]);
        //         switch(state) {
        //         case START_TOKEN:
        //         // case START_TOKEN:
        //             buf.clear();
        //             break;
        //         case PAYLOAD:
        //             _gotit = it;
        //             break;
        //         case CHECK_OK:
        //             ; // route further
        //             break;
        //         case NXT_PROTO:
        //             _binary_mode = true;
        //             break;
        //         default:
        //             break;
        //         }
                
        //         if ( _gotit ) {
        //             break;
        //         }
        //     }
        // }
    }
}

ProtocolItf* DataLink::goBIN()
{
    ProtocolItf *bin = getBinary();
    if ( bin ) {
        _binary = bin;
        return bin;
    }
    return nullptr;
}

void DataLink::goNMEA()
{
    _binary = nullptr;
    _sm.reset();
}

int DataLink::getNumNMEA() const
{
    int count = 0;
    for (ProtocolItf *it : _all_p) {
        if ( (*it).isBinary() ) {
            continue;
        }
        count++;
    }
    return count;
}

ProtocolItf* DataLink::getBinary() const
{
    for (ProtocolItf *it : _all_p) {
        if ( (*it).isBinary() ) {
            return it;
        }
    }
    return nullptr;
}

void DataLink::updateRoutes()
{
    ESP_LOGD(FNAME, "get routing for %d/%d", _did, _itf_id.port);
    _routes = DEVMAN->getRouting(RoutingTarget(_did, _itf_id.port));
}

PortList DataLink::getAllSendPorts() const
{
    PortList pl;
    for (ProtocolItf *it : _all_p) {
        pl.insert((*it).getSendPort());
    }
    return pl;
}

void DataLink::dumpProto()
{
    for (ProtocolItf *it : _all_p)
    {
        ESP_LOGI(FNAME, "    lp%d: did%d\tpid%d\tsp%d", getPort(), (*it).getDeviceId(), (*it).getProtocolId(), (*it).getSendPort());
    }
}

void DataLink::forwardMsg(DeviceId src_dev)
{
    // consider forwarding
    for ( auto &target : _routes ) {
        Message* msg = DEV::acqMessage(target.did, target.port);
        ESP_LOGD(FNAME, "route %d/%d to %d/%d", src_dev, _itf_id.port, msg->target_id, target.port);
        msg->buffer = _sm._frame;
        DEV::Send(msg);
    }
}
