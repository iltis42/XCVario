/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "DataLink.h"

#include "protocol/nmea/CANMasterRegMsg.h"
#include "protocol/nmea/JumboCmdMsg.h"
#include "protocol/nmea/FlarmMsg.h"
#include "protocol/nmea/FlarmHostMsg.h"
#include "protocol/nmea/GarminMsg.h"
#include "protocol/nmea/GpsMsg.h"
#include "protocol/nmea/XCVarioMsg.h"
#include "protocol/nmea/OpenVarioMsg.h"
#include "protocol/nmea/BorgeltMsg.h"
#include "protocol/nmea/CambridgeMsg.h"
#include "protocol/nmea/MagSensMsg.h"
#include "protocol/FlarmBin.h"
#include "protocol/KRT2Remote.h"
#include "protocol/MagSensBin.h"
#include "protocol/TestQuery.h"
#include "Messages.h"
#include "DeviceMgr.h"
#include "DataMonitor.h"
#include "logdef.h"

#include <array>

DataLink::~DataLink()
{
    for (ProtocolItf* it : std::array<ProtocolItf*, 2>{_nmea, _binary}) {
        if ( it ) {
            delete it;
        }
    }
}

NmeaPrtcl *DataLink::enforceNmea(DeviceId did, int sendport, ProtocolType ptyp)
{
    if ( !_nmea ) {
        // Create an NMEA protocol first
        return static_cast<NmeaPrtcl*> ( new NmeaPrtcl(did, sendport, ptyp, _sm, *this) );
    }
    return static_cast<NmeaPrtcl*>( _nmea);
}

// protocol factory
ProtocolItf* DataLink::addProtocol(ProtocolType ptyp, DeviceId did, int sendport)
{
    ProtocolItf *tmp = nullptr;

    // Check if already there
    if ( _nmea && _nmea->hasProtocol(ptyp) ) {
        tmp = _nmea;
    } else if (_binary && _binary->hasProtocol(ptyp)) {
        tmp = _binary;
    }
    if ( tmp ) {
        ESP_LOGW(FNAME, "Double insertion of device/protocol %d/%d.", did, ptyp);
        return tmp;
    }


    // Create a new one
    // bool nmea_plugin = false;
    switch (ptyp)
    {
    case REGISTRATION_P:
    {
        ESP_LOGI(FNAME, "New CANMasterReg");
        NmeaPrtcl *nmea = enforceNmea(did, sendport, ptyp);
        nmea->addPlugin(new CANMasterRegMsg(*nmea));
        tmp = nmea;
        break;
    }
    case JUMBOCMD_P:
    {
        ESP_LOGI(FNAME, "New JumboCmdHost");
        NmeaPrtcl *nmea = enforceNmea(did, sendport, ptyp);
        nmea->addPlugin(new JumboCmdMsg(*nmea));
        tmp = nmea;
        break;
    }
    case FLARM_P:
    {
        ESP_LOGI(FNAME, "New Flarm");
        NmeaPrtcl *nmea = enforceNmea(did, sendport, ptyp);
        nmea->addPlugin(new GpsMsg(*nmea, ptyp));
        nmea->addPlugin(new GarminMsg(*nmea, ptyp));
        nmea->addPlugin(new FlarmMsg(*nmea));
        tmp = nmea;
        break;
    }
    case FLARMHOST_P:
    {
        ESP_LOGI(FNAME, "New FlarmHost");
        NmeaPrtcl *nmea = enforceNmea(did, sendport, ptyp);
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
        NmeaPrtcl *nmea = enforceNmea(did, sendport, ptyp);
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
        NmeaPrtcl *nmea = enforceNmea(did, sendport, ptyp);
        nmea->addPlugin(new XCVarioMsg(*nmea, ptyp));
        nmea->addPlugin(new CambridgeMsg(*nmea, ptyp));
        tmp = nmea;
        break;
    }
    case OPENVARIO_P:
    {
        ESP_LOGI(FNAME, "New OpenVario");
        NmeaPrtcl *nmea = enforceNmea(did, sendport, ptyp);
        nmea->addPlugin(new OpenVarioMsg(*nmea, ptyp));
        tmp = nmea;
        break;
    }
    case BORGELT_P:
    {
        ESP_LOGI(FNAME, "New Borgelt");
        NmeaPrtcl *nmea = enforceNmea(did, sendport, ptyp);
        nmea->addPlugin(new BorgeltMsg(*nmea, ptyp));
        tmp = nmea;
        break;
    }
    case CAMBRIDGE_P:
    {
        ESP_LOGI(FNAME, "New Cambridge");
        NmeaPrtcl *nmea = enforceNmea(did, sendport, ptyp);
        nmea->addPlugin(new CambridgeMsg(*nmea, ptyp));
        tmp = nmea;
        break;
    }
    case KRT2_REMOTE_P:
        ESP_LOGI(FNAME, "New KRT2 Remote");
        tmp = new KRT2Remote(sendport, _sm, *this);
        break;
    case TEST_P:
        ESP_LOGI(FNAME, "New Test Proto");
        // tmp = new TestQuery(did, sendport, _sm, *this); todo, test proto does not fit into scheme any more
        break;
    default:
        break;
    }
    ESP_LOGI(FNAME, "On send port %d", sendport);

    if ( tmp ) {
        // Check device id is equal to all others
        if ( _did == NO_DEVICE ) {
            _did = did;
        } 
        else if ( (_nmea && _nmea->getDeviceId() != did)
                || (_binary && _binary->getDeviceId() != did) )
        {
            ESP_LOGW(FNAME, "DevId missmatch in protocol list for Itf/port %d/%d %d: %d.", _itf_id.iid, _itf_id.port, _did, did);
        }

        if ( tmp->isBinary() ) {
            _binary = tmp;
            if ( !_nmea ) { _active = tmp; }
        } else {
            _nmea = tmp;
            _active = tmp;
        }
    }

    return tmp;
}

ProtocolItf* DataLink::getProtocol(ProtocolType ptyp) const
{
    // ESP_LOGI(FNAME, "DL itf%d port%d did%d looking for proto%d", _itf_id.iid, _itf_id.port, _did, ptyp);
    if ( ptyp == NO_ONE ) {
        if ( _nmea ) {
            return _nmea;
        }
        else {
            return _binary;
        }
    }
    else {
        if ( _nmea && _nmea->getProtocolId() == ptyp ) {
            return _nmea;
        }
        else if ( _binary && _binary->getProtocolId() == ptyp ) {
            return _binary;
        }
    }
    return nullptr;
}

bool DataLink::hasProtocol(ProtocolType ptyp) const
{
    if ( getProtocol(ptyp) ) {
        return true;
    }
    return false;
}

void DataLink::deleteProtocol(ProtocolItf *proto)
{
    if ( _nmea == proto ) {
        delete(_nmea);
        _nmea = nullptr;
        _active = _binary;
    }
    else if ( _binary == proto ) {
        delete(_binary);
        _binary = nullptr;
        _active = _nmea;
    }
}

void DataLink::process(const char *packet, int len)
{
    // Feed the data monitor
    DM.monitorString(_itf_id, DIR_RX, packet, len);

    if (_active == nullptr) {
        return;
    }

    if (packet == nullptr)
    {
        // Special use, "no data" timeout, might be expected and normal
        // We just reset the protocol state machine then
        goNMEA();
        ESP_LOGW(FNAME, "timeout Itf/Port %d/%d", _itf_id.iid, _itf_id.port);
        return;
    }
    
    ESP_LOGD(FNAME, "%d proc %dchar: %c", _itf_id.iid, len, *packet);
    // process every frame byte through state machine
    for (; len > 0; ) {
        dl_control_t control = dl_control_t(NOACTION);
        if ( _sm.push(*packet) ) // only push one byte, beyond that it is protocol resposibility
        {
            control = _active->nextBytes(packet, len);
            if ( control.act & FORWARD_BIT ) {
                forwardMsg(_active->getDeviceId());
            }
            if ( control.act == NXT_PROTO ) {
                switchProtocol();
                break; // end loop imidiately
            }
        } else {
            _sm.reset();
        }
        len -= control.pcount;
        packet += control.pcount;
    }
}

ProtocolItf* DataLink::goBIN()
{
    if ( _binary ) {
        _active = _binary;
    }
    _sm.reset();
    return _binary;
}

void DataLink::goNMEA()
{
    if ( _nmea ) {
        _active = _nmea;
    }
    _sm.reset();
}

void DataLink::switchProtocol()
{
    if ( _active == _nmea ) {
        goBIN();
    }
    else {
        goNMEA();
    }
}

ProtocolItf* DataLink::getBinary() const
{
    return _binary;
}

void DataLink::updateRoutes()
{
    ESP_LOGD(FNAME, "get routing for %d/%d", _did, _itf_id.port);
    _routes = DEVMAN->getRouting(RoutingTarget(_did, ItfTarget(_itf_id.iid, _itf_id.port)));
}

PortList DataLink::getAllSendPorts() const
{
    PortList pl;
    for (ProtocolItf* it : std::array<ProtocolItf*, 2>{_nmea, _binary}) {
        if ( it ) {
            pl.insert(it->getSendPort());
        }
    }
    return pl;
}

void DataLink::dumpProto()
{
    for (ProtocolItf* it : std::array<ProtocolItf*, 2>{_nmea, _binary}) {
        if (it) {
            ESP_LOGI(FNAME, "    lp%d: did%d\tpid%d\tsp%d", getPort(), it->getDeviceId(), it->getProtocolId(), it->getSendPort());
        }
    }
}

void DataLink::forwardMsg(DeviceId src_dev)
{
    // consider forwarding
    // std::string log("route ");
    // log += std::to_string(src_dev) + "/" + std::to_string(_itf_id.port) + " to ";
    for ( auto &target : _routes ) {
        Message* msg = DEV::acqMessage(target.did, target.getItfTarget().port);
        // log += std::to_string(target.did) + "/" + std::to_string(target.getItfTarget().port) + ", ";
        msg->buffer = _sm._frame;
        DEV::Send(msg);
    }
    // if ( _routes.size() > 0 ) {
    //     ESP_LOGI(FNAME, "%s", log.c_str());
    // }
}
