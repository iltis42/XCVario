/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "DataLink.h"

#include "protocol/nmea/CANMasterRegMsg.h"
#include "protocol/nmea/CANClientQueryMsg.h"
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

DataLink::DataLink(int listen_port, int itfid) :
    _itf_id(ItfTarget((InterfaceId)itfid,listen_port))
{
    _route_mutex = xSemaphoreCreateMutex();
}

DataLink::~DataLink()
{
    for (ProtocolItf* it : std::array<ProtocolItf*, 2>{_nmea, _binary}) {
        if ( it ) {
            delete it;
        }
    }
}

void DataLink::enforceNmea(DeviceId did, int sendport, ProtocolType ptyp)
{
    if ( !_nmea ) {
        // Create an NMEA protocol first
        _nmea = new NmeaPrtcl(did, sendport, ptyp, _sm, *this);
    }
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
    switch (ptyp)
    {
    case REGISTRATION_P:
        ESP_LOGI(FNAME, "New CANMasterReg");
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new CANMasterRegMsg(*_nmea));
        if ( xcv_role.get() == SECOND_ROLE ) {
            _nmea->addPlugin(new CANClientQueryMsg(*_nmea));
        }
        tmp = _nmea;
        break;
    case JUMBOCMD_P:
        ESP_LOGI(FNAME, "New JumboCmd");
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new JumboCmdMsg(*_nmea));
        tmp = _nmea;
        break;
    case FLARM_P:
        ESP_LOGI(FNAME, "New Flarm");
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new GpsMsg(*_nmea));
        _nmea->addPlugin(new GarminMsg(*_nmea));
        _nmea->addPlugin(new FlarmMsg(*_nmea));
        tmp = _nmea;
        break;
    case FLARMHOST_P:
        ESP_LOGI(FNAME, "New FlarmHost");
         enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new FlarmHostMsg(*_nmea));
        tmp = _nmea;
        break;
    case FLARMBIN_P:
        ESP_LOGI(FNAME, "New FlarmBinary");
        tmp = new FlarmBinary(did, sendport, _sm, *this);
        break;
    case MAGSENS_P:
        ESP_LOGI(FNAME, "New CAN MagSens");
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new MagSensMsg(*_nmea));
        tmp = _nmea;
        break;
    case MAGSENSBIN_P:
        ESP_LOGI(FNAME, "New MAGCANBinary");
        tmp = new MagSensBinary(sendport, _sm, *this);
        break;
    case XCVARIO_P:
    {
        ESP_LOGI(FNAME, "New XCVario");
        bool auto_setup = _nmea == nullptr;
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new XCVarioMsg(*_nmea));
        _nmea->addPlugin(new CambridgeMsg(*_nmea, auto_setup));
        tmp = _nmea;
        break;
    }
    case XCVSYNC_P:
        ESP_LOGI(FNAME, "New XCVsync");
        enforceNmea(did, sendport, ptyp);
        // The SyncMsg serves on both side, need to know it's role
        // connect to a client -> you are master
        if ( xcv_role.get() == NO_ROLE && did==XCVARIOCLIENT_DEV ) { xcv_role.set(MASTER_ROLE); }
        _nmea->addPlugin(new XCVSyncMsg(*_nmea, did==XCVARIOCLIENT_DEV)); // true when on master (!!)
        tmp = _nmea;
        break;
    case OPENVARIO_P:
    {
        ESP_LOGI(FNAME, "New OpenVario");
        bool auto_setup = _nmea == nullptr;
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new OpenVarioMsg(*_nmea, auto_setup));
        tmp = _nmea;
        break;
    }
    case BORGELT_P:
    {
        ESP_LOGI(FNAME, "New Borgelt");
        bool auto_setup = _nmea == nullptr;
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new BorgeltMsg(*_nmea, auto_setup));
        tmp = _nmea;
        break;
    }
    case CAMBRIDGE_P:
    {
        ESP_LOGI(FNAME, "New Cambridge");
        bool auto_setup = _nmea == nullptr;
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new CambridgeMsg(*_nmea, auto_setup));
        tmp = _nmea;
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
    if (_monitoring) {
        DM->monitorString(DIR_RX, packet, len);
    }

    if (_active == nullptr) {
        return;
    }

    if (packet == nullptr)
    {
        // Special use, "no data" timeout, might be expected and normal
        // We just reset the protocol state machine then
        goNMEA();
        // ESP_LOGW(FNAME, "timeout Itf/Port %d/%d", _itf_id.iid, _itf_id.port);
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
                doForward(_active->getDeviceId());
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

// called from different contexts
void DataLink::updateRoutes()
{
    ESP_LOGD(FNAME, "get routing for %d/%d", _did, _itf_id.port);
    xSemaphoreTake(_route_mutex, portMAX_DELAY);
    _routes = DEVMAN->getRouting(RoutingTarget(_did, ItfTarget(_itf_id.iid, _itf_id.port)));
    xSemaphoreGive(_route_mutex);
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
    ESP_LOGI(FNAME, "    listen port %d", getPort());
    if ( _nmea ) {
        ESP_LOGI(FNAME, "       nm did%d\tpid%d\tsp%d", _nmea->getDeviceId(), _nmea->getProtocolId(), _nmea->getSendPort());
        for (auto it : static_cast<NmeaPrtcl*>(_nmea)->getAllPlugs() ) {
            ESP_LOGI(FNAME, "       nm plugin %d%c", it->getPtyp(), it->getAuto()?'a':'x');
        }
    }
    if ( _binary ) {
            ESP_LOGI(FNAME, "       bi did%d\tpid%d\tsp%d", _binary->getDeviceId(), _binary->getProtocolId(), _binary->getSendPort());
    }
}

// called only from one and always same itf receiver context
void DataLink::doForward(DeviceId src_dev)
{
    // consider forwarding
    // std::string log("route ");
    // log += std::to_string(src_dev) + "/" + std::to_string(_itf_id.port) + " to ";
    xSemaphoreTake(_route_mutex, portMAX_DELAY);
    for ( auto &target : _routes ) {
        Message* msg = DEV::acqMessage(target.did, target.getItfTarget().port);
        // log += std::to_string(target.did) + "/" + std::to_string(target.getItfTarget().port) + ", ";
        msg->buffer = _sm._frame;
        DEV::Send(msg);
    }
    xSemaphoreGive(_route_mutex);
    // if ( _routes.size() > 0 ) {
    //     ESP_LOGI(FNAME, "%s", log.c_str());
    // }
}
