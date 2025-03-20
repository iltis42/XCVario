/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "CANMasterRegMsg.h"

#include "protocol/nmea_util.h"
#include "protocol/Clock.h"
#include "comm/DeviceMgr.h"
#include "comm/Messages.h"

#include "logdefnone.h"

#include <cstring>

extern InterfaceCtrl* CAN;

// The CAN master registry, the host side protocol parser. One query is supported:
//
// - Registration query: The drive (client) requests a CAN id to listen on commands from the master, and a response
//      id to address the master properly.
//      The master listens for registration queries on the CAN id 0x7f0, and also replies on this id. The token
//      is used to match query and response on client side.
//   $PJPREG, <token>, <protcol type>\r\n
//
//
// The response to the registration query:
// - Accepting the registration: The token must be the same as in the query. Second parameter is ascii/decimal encoded the 
//      CAN bus id dedicated to the clint from now on. Third parameter is the masters id.
//      In case this response got not received well from the client it will query again. This repetition can be determined as 
//      of containing the same pair of token and protocol type. In case the token is different it might be another clients query.
//      __Important__: This query is supposed to happen once for the upptime of the client! In case the master gets restarted and
//      not so the client this query will not be happening another time. The master is expected to store the distributed id's
//      permanently and has to be able to double check after a power-cycle event if clients are still present. OR, just sent the 
//      broadcast to re-register.
//      At this moment it does not matter if the power-cycle is an out of the ordinary event, or just ordinary next event the systems 
//      are comming up.
//      A power-cycled client will restart the registration from scratch.
//   $PJMACC, <token>, <drive id>, <master id>*<CRC>\r\n
//
// - Not accepting the registration: Not knowing what the purpose of this would be, despite to stop the client from continue to
//      trying. A typical expected communication pattern would be that the master is not connected or not there and the client 
//      would just never get a response and continue trying, not doing anything harmfull or wrong with it.
//   $PJMNAC, <token>\r\n
//
// - Re-registration broadcast: Trigger a new registration cycle from devices. This comes handy when a master reboots unintendedly.
//   $PJMLOR\r\n

CANMasterRegMsg::CANMasterRegMsg(NmeaPrtcl &nr) :
    NmeaPlugin(nr),
    Clock_I(200)
{
    Clock::start(this);
}


bool CANMasterRegMsg::tick()
{
    sendLossOfResgitrations();
    return true;
}

void CANMasterRegMsg::sendLossOfResgitrations()
{
    Message* msg = DEV::acqMessage(_nmeaRef.getDeviceId(), _nmeaRef.getSendPort());
    ESP_LOGI(FNAME, "Pls re-register");
    msg->buffer = "$PJMLOR\r\n";
    DEV::Send(msg);
}

datalink_action_t CANMasterRegMsg::registration_query(NmeaPrtcl *nmea)
{
    ESP_LOGI(FNAME, "JP registration query");
    // e.g. read message "$PJPREG, 123, proto"
    ProtocolState *sm = nmea->getSM();
    if ( sm->_frame.size() < 12 ) {
        return NOACTION;
    }

    int pos = sm->_word_start[0];
    ESP_LOGD(FNAME, "wordstart0 %d", pos);
    std::string token = NMEA::extractWord(sm->_frame, pos);
    if ( token.size() != 3 ) {
        return NOACTION;
    }
    ESP_LOGD(FNAME, "JP reg token %s", token.c_str());

    // read the protocol type
    ESP_LOGD(FNAME, "JP comma %d", pos);
    std::string protocol = NMEA::extractWord(sm->_frame, pos);
    ESP_LOGD(FNAME, "JP proto %s", protocol.c_str());

    // Todo option to use the token, when more devices of one kind need to be connected

    DeviceId ndev = NO_DEVICE;
    ProtocolType nproto = NO_ONE;
    if ( protocol.compare("JUMBOCMD") == 0 ) {
        ndev = JUMBO_DEV;
        nproto = JUMBOCMD_P;
    } else if ( protocol.compare("MAGSENS") == 0 ) {
        ndev = MAGSENS_DEV;
        nproto = MAGSENS_P;
    }
    if ( ndev != NO_DEVICE ) {
        int client_ch = DEVMAN->getSendPort(ndev, nproto);
        if ( client_ch > 0 ) {
            ESP_LOGD(FNAME, "reuse port %d", client_ch);
        } else {
            client_ch = DeviceManager::getFreeCANId(1);
            ESP_LOGD(FNAME, "new port %d", client_ch);
        }
        if ( client_ch == -1 ) return NOACTION; // no luck

        ESP_LOGD(FNAME, "use port %d", client_ch);

        Message* msg = DEV::acqMessage(nmea->getDeviceId(), nmea->getSendPort());

        int master_ch = client_ch + 1;
        DEVMAN->addDevice(ndev, nproto, master_ch, client_ch, CAN_BUS);

        msg->buffer = "$PJMACC, " + token + ", " + std::to_string(client_ch) + 
                                        ", " + std::to_string(master_ch);
        msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
        DEV::Send(msg);
    }

    return DO_ROUTING;
}

ConstParserMap CANMasterRegMsg::_pm = {
    { Key("PREG"), CANMasterRegMsg::registration_query }
};

