/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "FlarmHost.h"
#include "FlarmBin.h"

#include "nmea_util.h"
#include "comm/Messages.h"
#include "comm/DeviceMgr.h"

#include <logdef.h>

#include <cstring>

// The FLARM host protocol checker/forwarder.
//
// Supported messages:
// PFL*

datalink_action_t FlarmHost::nextByte(const char c)
{
    int pos = _sm._frame.size() - 1; // c already in the buffer
    ESP_LOGD(FNAME, "state %d, pos %d next char %c", _sm._state, pos, c);
    switch(_sm._state) {
    case START_TOKEN:
        if ( c == '$' ) {
            _sm._state = HEADER;
            ESP_LOGD(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        NMEA::incrCRC(_sm._crc,c);
        if ( pos < 4 ) { break; }
        if ( _sm._frame.substr(1,3) != "PFL" ) {
            // Flarm sender id
            break;
        }
        _sm._state = PAYLOAD;
        ESP_LOGD(FNAME, "Msg HEADER");
        break;
    case PAYLOAD:
        if ( c == '*' ) {
            _sm._state = CHECK_CRC1; // Expecting a CRC to check
            break;
        }
        if ( c != '\r' && c != '\n' ) {
            ESP_LOGD(FNAME, "Msg PAYLOAD");
            NMEA::incrCRC(_sm._crc,c);
            break;
        }
        _sm._state = COMPLETE;
        break;
    case CHECK_CRC1:
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
        ESP_LOGI(FNAME, "Msg complete %s", _sm._frame.c_str());
        // trigger forwarding
        _sm._state = START_TOKEN;
        NMEA::ensureTermination(_sm._frame);
        if ( _sm._frame.substr(4,2) == "AX" && _sm._frame.at(6) != ',' ) {
            ESP_LOGI(FNAME, "Start binary request");
            _binpeer = DEVMAN->getProtocol(FLARM_DEV, FLARM_P);
            return GO_BINARY;
        }
        return DO_ROUTING;
    }
    return NOACTION;
}

datalink_action_t FlarmHost::nextStreamChunk(const char *cptr, int count)
{
    datalink_action_t last_action = NOACTION;
    for (int i = 0; i < count; i++) {
        datalink_action_t res = flarmBinSM(_sm, *(cptr+i));
        if ( res & COMPLETE_BIT ) {
            Message* msg = DEV::acqMessage(_binpeer->getDeviceId(), _binpeer->getSendPort());
            msg->buffer = _sm._frame;
            DEV::Send(msg);
            _sm.reset();
            last_action = res;
        }
    }
    return last_action;
}


