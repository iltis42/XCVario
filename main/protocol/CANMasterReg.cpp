/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "CANMasterReg.h"

#include "nmea_util.h"

#include "comm/Routing.h"
#include "comm/DeviceMgr.h"
#include "comm/Messages.h"

#include <logdef.h>

#include <cstring>

extern InterfaceCtrl* CAN;

// The CAN master registry, the host side protocol parser. One query is supported:
//
// - Registration query: The drive (client) requests a CAN id to listen on commands from the master, and a response
//      id to address the master properly.
//      The master listens for registration queries on the CAN id 0x7f0, and also replies on this id. The token
//      is used to match query and response on client side.
//   !jreg <token>, <protcol type>\r\n
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
//   !jacc <token>, <drive id>, <master id>*<CRC>\r\n
//
//  - Not accepting the registration: Not knowing what the purpose of this would be, despite to stop the client from continue to
//      trying. A typical expected communication pattern would be that the master is not connected or not there and the client 
//      would just never get a response and continue trying, not doing anything harmfull or wrong with it.
//   !jnac <token>\r\n
//


gen_state_t CANMasterReg::nextByte(const char c)
{
    ESP_LOGD(FNAME, "state %d, pos %d next char %c", _state, _pos, c);
    switch(_state) {
    case START_TOKEN:
    case CHECK_FAILED:
    case RESTART:
        if ( c == '!' ) {
            _state = HEADER;
            reset();
            push(c);
            ESP_LOGD(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        if ( c != 'j' ) {
            _state = RESTART;
            break;
        }
        _state = PAYLOAD;
        ESP_LOGD(FNAME, "Msg HEADER");
        push_and_crc(c);
        break;
    case PAYLOAD:
        if ( c == '*' ) {
            push(c);
            _state = CHECK_CRC; // Expecting a CRC to check
            break;
        }
        if ( c != '\r' && c != '\n' ) {
            ESP_LOGD(FNAME, "Msg PAYLOAD");
            if ( push_and_crc(c) ) {
                break;
            }
            // Buffer is full
            _state = STOP_TOKEN;
        }
        // Fall through 
    case CHECK_CRC:
        if( _state == CHECK_CRC ) { // did we not fall through
            if ( _crc_buf[0] == '\0' ) {
                // this is the first crc character
                _crc_buf[0] = c;
                break;
            }
            else {
                _crc_buf[1] = c;
                _crc_buf[2] = '\0';
                char read_crc = (char)strtol(_crc_buf, NULL, 16);
                ESP_LOGD(FNAME, "Msg CRC %s/%x - %x", _crc_buf, read_crc, _crc);
                if ( read_crc != _crc ) {
                    _state = CHECK_FAILED;
                    break;
                }
                _state = CHECK_OK; // fall through
            }
        }
        // Fall through 
    case STOP_TOKEN:
    case CHECK_OK:
    case COMPLETE:
        {
            push('\0'); // terminate the string buffer
            _state = START_TOKEN; // restart parsing
            ESP_LOGD(FNAME, "Msg complete %s", _framebuffer);
            std::string frame(_framebuffer);
            if ( frame.compare(2, 3, "reg") == 0 ) {
                registration_query();
            }
        }
        break;
    default:
        break;
    }
    return _state;
}

void CANMasterReg::registration_query()
{
    ESP_LOGI(FNAME, "JP registration query");
    // e.g. read message "!jacc XYZ, proto"
    if ( strlen(_framebuffer) < 9 ) {
        return;
    }
    std::string frame(&_framebuffer[6]);

    // grab the token
    for ( int i=0; i<3; i++ ) {
        if ( !isdigit(frame[i]) ) {
            return;
        }
    }
    _token = frame.substr(0, 3);
    ESP_LOGD(FNAME, "JP reg token %s", _token.c_str());

    // read the protocol type
    size_t pos = frame.find(',');
    ESP_LOGD(FNAME, "JP comma %d", pos);
    _protocol = NMEA::extractWord(frame, pos+1);
    ESP_LOGI(FNAME, "JP proto %s", _protocol.c_str());

    if ( _protocol.compare("JUMBOCMD") == 0 ) {
        int client_id;
        Device *dev = DEVMAN->getDevice(JUMBO_DEV);
        if ( dev ) {
            client_id = dev->getPort(JUMBO_CMD); // re-use
        } else {
            client_id = DeviceManager::getFreeCANId(1);
        }
        if ( client_id == -1 ) return; // no luck

        Message* msg = newMessage(); // set target
        if ( ! msg ) return;

        int master_id = client_id + 1;
        DEVMAN->addDevice(JUMBO_DEV, JUMBO_CMD, master_id, client_id, CAN);

        msg->buffer = "!jacc " + _token + ", " + std::to_string(client_id) + 
                                        ", " + std::to_string(master_id);
        msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
        DEV::Send(msg);

    }
    // else // other devices
    return;
}

