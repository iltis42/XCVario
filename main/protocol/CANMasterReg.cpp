/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "CANMasterReg.h"

#include "nmea_util.h"
#include "Clock.h"
#include "comm/DeviceMgr.h"
#include "comm/Messages.h"

#include "logdef.h"

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
//      permanently and has to be able to double check after a power-cycle event if clients are still preset. This might happen through 
//      connect message of the JumboCmd protocol. A client that has not yet registered will not respond on the connect message.
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

CANMasterReg::CANMasterReg(int sendport, ProtocolState &sm, DataLink &dl) :
    ProtocolItf(MASTER_DEV, sendport, sm, dl),
    Clock_I(200)
{
    Clock::start(this);
}

datalink_action_t CANMasterReg::nextByte(const char c)
{
    int pos = _sm._frame.size() - 1;
    ESP_LOGI(FNAME, "state %d, pos %d next char %c", _sm._state, pos, c);
     switch (_sm._state)
     {
    case START_TOKEN:
        if (c == '$')
        {
            _sm._state = HEADER;
            ESP_LOGD(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        NMEA::incrCRC(_sm._crc, c);
        if (pos < 4)
        {
            break;
        }
        if (_sm._frame.substr(1, 3) != "PJP")
        {
            _sm._state = START_TOKEN;
            break;
        }
        _sm._state = PAYLOAD;
         ESP_LOGD(FNAME, "Msg HEADER");
        break;
    case PAYLOAD:
        if (c == '*')
        {
            _sm._state = CHECK_CRC1; // Expecting a CRC to check
            break;
        }
        if (c != '\r' && c != '\n')
        {
            ESP_LOGD(FNAME, "Msg PAYLOAD");
            NMEA::incrCRC(_sm._crc, c);
            break;
        }
        _sm._state = COMPLETE;
        break;
    case CHECK_CRC1:
        // this is the first crc character
        _crc_buf[0] = c;
        _sm._state = CHECK_CRC2;
        break;
    case CHECK_CRC2:
    {
        _crc_buf[1] = c;
        _crc_buf[2] = '\0';
        char read_crc = (char)strtol(_crc_buf, NULL, 16);
        ESP_LOGD(FNAME, "Msg CRC %s/%x - %x", _crc_buf, read_crc, _sm._crc);
        if ( read_crc != _sm._crc ) {
            _sm._state = START_TOKEN;
            break;
        }
        _sm._state = COMPLETE;
        break;
    }
    default:
        break;
    }
    if ( _sm._state == COMPLETE )
    {
        _sm._state = START_TOKEN;   // restart parsing
        ESP_LOGD(FNAME, "Msg complete %s", _sm._frame.c_str());
        if ( _sm._frame.compare(4, 3, "REG") == 0 )
        {
            registration_query();
        }
    }

    return NOACTION;
}

bool CANMasterReg::tick()
{
    sendLossOfResgitrations();
    return true;
}

void CANMasterReg::sendLossOfResgitrations()
{
    Message* msg = newMessage();
    ESP_LOGI(FNAME, "Pls re-register");
    msg->buffer = "$PJMLOR\r\n";
    DEV::Send(msg);
}

void CANMasterReg::registration_query()
{
    ESP_LOGI(FNAME, "JP registration query");
    // e.g. read message "$PJPREG, 123, proto"
    if ( _sm._frame.size() < 12 ) {
        return;
    }

    int pos = 8;
    std::string token = NMEA::extractWord(_sm._frame, pos);
    if ( token.size() != 3 ) {
        return;
    }
    ESP_LOGI(FNAME, "JP reg token %s", token.c_str());

    // read the protocol type
    ESP_LOGI(FNAME, "JP comma %d", pos);
    std::string protocol = NMEA::extractWord(_sm._frame, pos);
    ESP_LOGI(FNAME, "JP proto %s", protocol.c_str());

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
        if ( client_ch == -1 ) return; // no luck

        ESP_LOGD(FNAME, "use port %d", client_ch);

        Message* msg = newMessage(); // set target

        int master_ch = client_ch + 1;
        DEVMAN->addDevice(ndev, nproto, master_ch, client_ch, CAN_BUS);

        msg->buffer = "$PJMACC, " + token + ", " + std::to_string(client_ch) + 
                                        ", " + std::to_string(master_ch);
        msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
        DEV::Send(msg);
    }

    return;
}

