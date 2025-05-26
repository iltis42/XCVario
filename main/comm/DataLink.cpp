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
#include "setup/DataMonitor.h"
#include "protocol/AliveMonitor.h"
#include "setup/SetupNG.h"
#include "logdefnone.h"

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
// returns the list of protocols that were added
EnumList DataLink::addProtocol(ProtocolType ptyp, DeviceId did, int sendport)
{
    ProtocolItf *tmp = nullptr;
    EnumList ret;

    // Check if already there
    if ( _nmea && _nmea->hasProtocol(ptyp) ) {
        tmp = _nmea;
    } else if (_binary && _binary->hasProtocol(ptyp)) {
        tmp = _binary;
    }
    if ( tmp ) {
        ESP_LOGW(FNAME, "Double insertion of device/protocol %d/%d.", did, ptyp);
        return ret;
    }

    // Check device id is equal to all others
    if ( _did != NO_DEVICE ) {
        if ( (_nmea && _nmea->getDeviceId() != did)
            || (_binary && _binary->getDeviceId() != did) )
        {
            ESP_LOGW(FNAME, "dLink itf/port %d/%d %d shared with %d.", _itf_id.iid, _itf_id.port, _did, did);
        }
    }
    
    // Set first to mark the most recent activity,
    // this influences the nmea plugin creation
    _did = did; // set the device id for routing
    ESP_LOGI(FNAME, "DL set dev id %d", _did);

    // Create a new one
    ret.insert(ptyp);
    switch (ptyp)
    {
    case REGISTRATION_P:
        ESP_LOGI(FNAME, "New CANMasterReg");
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new CANMasterRegMsg(*_nmea));
        if ( xcv_role.get() == SECOND_ROLE ) {
            _nmea->addPlugin(new CANClientQueryMsg(*_nmea));
            ret.insert(XCVQUERY_P);
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
        _nmea->addAliveMonitor(new AliveMonitor(&flarm_alive));
        _nmea->addPlugin(new GpsMsg(*_nmea));
        ret.insert(NMEASTD_P);
        _nmea->addPlugin(new GarminMsg(*_nmea));
        ret.insert(GARMIN_P);
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
        tmp = new MagSensBin(sendport, _sm, *this);
        tmp->addAliveMonitor(new AliveMonitor(&mags_alive, 200));
        break;
    case XCVARIO_P:
    {
        ESP_LOGI(FNAME, "New XCVario");
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new XCVarioMsg(*_nmea));
        _nmea->addPlugin(new CambridgeMsg(*_nmea, true));
        ret.insert(CAMBRIDGE_P);
        tmp = _nmea;
        break;
    }
    case XCVSYNC_P:
        ESP_LOGI(FNAME, "New XCVsync");
        enforceNmea(did, sendport, ptyp);
        _nmea->addAliveMonitor(new AliveMonitor(&xcv_alive, 800));
        // The SyncMsg serves on both side, need to know it's role
        // connect to a client -> you are master
        if ( xcv_role.get() == NO_ROLE && did==XCVARIOSECOND_DEV ) { xcv_role.set(MASTER_ROLE); }
        //                                        \/true when on master (!!), only CAN does it automatically
        _nmea->addPlugin(new XCVSyncMsg(*_nmea, did==XCVARIOSECOND_DEV, _itf_id == CAN_BUS));
        tmp = _nmea;
        break;
    case OPENVARIO_P:
    {
        ESP_LOGI(FNAME, "New OpenVario");
        // bool auto_setup = _nmea == nullptr;
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new OpenVarioMsg(*_nmea, false));
        tmp = _nmea;
        break;
    }
    case BORGELT_P:
    {
        ESP_LOGI(FNAME, "New Borgelt");
        // bool auto_setup = _nmea == nullptr;
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new BorgeltMsg(*_nmea, false));
        tmp = _nmea;
        break;
    }
    case CAMBRIDGE_P:
    {
        ESP_LOGI(FNAME, "New Cambridge");
        // bool auto_setup = _nmea == nullptr;
        enforceNmea(did, sendport, ptyp);
        _nmea->addPlugin(new CambridgeMsg(*_nmea, false));
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
        ret.clear();
        break;
    }
    ESP_LOGI(FNAME, "On send port %d", sendport);

    if ( tmp ) {

        if ( tmp->isBinary() ) {
            _binary = tmp;
            if ( !_nmea ) { _active = tmp; }
        } else {
            _active = tmp;
        }
    }

    return ret;
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
        if ( _nmea && _nmea->hasProtocol(ptyp) ) {
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
    if ( _nmea && _nmea->hasProtocol(ptyp) ) {
        return true;
    }
    else if ( _binary && _binary->getProtocolId() == ptyp ) {
        return true;
    }
    return false;
}

void DataLink::removeProtocol(ProtocolType ptyp)
{
    if ( _nmea && _nmea->hasProtocol(ptyp) ) {
        ESP_LOGI(FNAME, "Remove protocol %d", ptyp);
        _nmea->removeProtocol(ptyp);
        if ( _nmea->getNrPlugs() == 0 ) {
            delete(_nmea);
            _nmea = nullptr;
            _active = _binary;
        }
    }
    else if ( _binary && _binary->getProtocolId() == ptyp ) {
        delete(_binary);
        _binary = nullptr;
        _active = _nmea;
    }
}

void DataLink::process(const char *packet, int len)
{
    // Feed the data monitor
    if (_monitoring) {
        DM->monitorString(DIR_RX, _active && _active->isBinary(), packet, len);
    }

    if (_active == nullptr) {
        return;
    }

    if (packet == nullptr)
    {
        // Special use, "no data" timeout, might be expected and normal
        // plenty of use case options, none right now
        return;
    }
    // if ( _active->isBinary() ) {
    //     ESP_LOGI(FNAME, "dev %d", _active->getDeviceId());
    //     ESP_LOG_BUFFER_HEXDUMP(FNAME, packet, len, ESP_LOG_INFO);
    // } else {
    //     std::string str(packet, len);
    //     ESP_LOGI(FNAME, "%d> %s", _active->getDeviceId(), str.c_str());
    // }
    
    ESP_LOGD(FNAME, "%d proc %dchar: %c", _itf_id.iid, len, *packet);
    // process every frame byte through state machine
    for (; len > 0; ) {
        dl_control_t control = dl_control_t(NOACTION);
        if ( _sm.checkSpaceOne() ) // only check for one byte buffer space, beyond that it is protocol resposibility
        {
            control = _active->nextBytes(packet, len);
            if ( control.act & FORWARD_BIT ) {
                // DeviceId did = (control.did) ? control.did : _active->getDeviceId();
                ESP_LOGI(FNAME, "forward %d", control.did);
                doForward(control.did);
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
    if ( _binary && _active != _binary ) {
        _active = _binary;
        ESP_LOGI(FNAME, "Switch to binary protocol %d(%d)", _active->getProtocolId(),_active->isBinary());
        _sm.reset();
    }
    return _binary;
}

void DataLink::goNMEA()
{
    if ( _nmea && _active != _nmea ) {
        _active = _nmea;
        ESP_LOGI(FNAME, "Switch to nmea protocol %d(%d)", _active->getProtocolId(),_active->isBinary());
        _sm.reset();
    }
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
void DataLink::updateRoutes(DeviceId did)
{
    ESP_LOGI(FNAME, "get routing for d%d/i%d/o%d", did, _itf_id.getItfId(), _itf_id.port);
    xSemaphoreTake(_route_mutex, portMAX_DELAY);
    _routes = DEVMAN->getRouting(RoutingTarget(did, ItfTarget(_itf_id.iid, _itf_id.port)));
    xSemaphoreGive(_route_mutex);
}

EnumList DataLink::getAllSendPorts() const
{
    EnumList pl;
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
        ESP_LOGI(FNAME, "       nm did%d\tpid%d\tsp%d%c", _nmea->getDeviceId(), _nmea->getProtocolId(), _nmea->getSendPort(), (_nmea==_active)?'<':' ');
        for (auto it : static_cast<NmeaPrtcl*>(_nmea)->getAllPlugs() ) {
            ESP_LOGI(FNAME, "       nm plugin %d%c", it->getPtyp(), it->getAuto()?'a':'x');
        }
    }
    if ( _binary ) {
            ESP_LOGI(FNAME, "       bi did%d\tpid%d\tsp%d%c", _binary->getDeviceId(), _binary->getProtocolId(), _binary->getSendPort(), (_binary==_active)?'<':' ');
    }
}

bool DataLink::isBinActive() const
{
    return _active->isBinary();
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
