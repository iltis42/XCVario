/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "DataLink.h"

#include "protocol/CANMasterReg.h"
#include "protocol/JumboCmdHost.h"
#include "Messages.h"

#include "DeviceMgr.h"

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
ProtocolItf* DataLink::addProtocol(ProtocolType ptyp, int send_port)
{

    // Check if already there
    bool foundit = false;
    for (auto *it : _all_p) {
        if ( (*it).getProtocolId() == ptyp 
            && (*it).getSendPort() == send_port ) {
            foundit = true;
        }
    }
    if ( ! foundit ) {
        // Create a new one
        ProtocolItf *tmp = nullptr;
        switch (ptyp)
        {
        case REGISTRATION:
            ESP_LOGI(FNAME, "New MasterReg");
            tmp = new CANMasterReg(send_port, _sm);
            break;
        case JUMBO_CMD:
            ESP_LOGI(FNAME, "New JumboCmdHost");
            tmp = new JumboCmdHost(send_port, _sm);
            break;
        default:
            break;
        }
        ESP_LOGI(FNAME, "On send port %d", send_port);

        if ( tmp ) _all_p.push_back(tmp);
        return tmp;
    }
    return nullptr;
}

ProtocolItf* DataLink::getProtocol(ProtocolType ptyp) const
{
    if ( ptyp == NO_ONE ) {
        if ( ! _all_p.empty() ) {
            return _all_p.front();
        }
    }
    else {
        for (ProtocolItf *it : _all_p) {
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

void DataLink::process(const char *packet, int len)
{
    if ( _all_p.empty() ) {
        return;
    }

    gen_state_t state;

    if (packet == nullptr)
    {
        // Special use, "no data" timeout, might be expected and normal
        // We just reset the protocol state machine then
        _sm.reset();
        _binary_mode = false;
        return;
    }
    else if ( _all_p.size() == 1 )
    {
        ESP_LOGI(FNAME, "process %dchar: %s", len, packet);
        // most likely case, only one protocol to parse
        // process every frame byte through state machine
        for (int i = 0; i < len; i++) {
            if ( _sm.push(packet[i]) ) {
                state = _all_p.front()->nextByte(packet[i]);
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
        //         case GO_BINARY:
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

void DataLink::dumpProto()
{
    for (ProtocolItf *it : _all_p)
    {
        ESP_LOGI(FNAME, "  lp%d: did%d\tpid%d\tsp%d", getPort(), (*it).getDeviceId(), (*it).getProtocolId(), (*it).getSendPort());
    }
}

