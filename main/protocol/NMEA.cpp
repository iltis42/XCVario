/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "NMEA.h"

#include "nmea_util.h"
#include "logdefnone.h"


// A generic and extendable NMEA message parser.

NmeaPrtcl::~NmeaPrtcl()
{
    for (auto parser : _plugs) {
        // all parser tables
        delete parser;
    }
    _plugs.clear();
    _parsmap.clear();
}

void NmeaPrtcl::addPlugin(NmeaPlugin *pm)
{
    _plugs.push_back(pm);
    // copy the parser table
    for (const auto& [key, value] : *pm->getPM()) {
        _parsmap[key] = value;
    }
}

datalink_action_t NmeaPrtcl::nextByte(const char c)
{
    int pos = _sm._frame.size() - 1; // c already in the buffer
    ESP_LOGD(FNAME, "state %d, pos %d next char %c", _sm._state, pos, c);
    switch(_sm._state) {
    case START_TOKEN:
        if (c == '$' || c == '!') {
            _sm._state = HEADER;
            _sm._word_start.clear();
            _mkey.value = 0;
            _parser = nullptr;
            ESP_LOGD(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        nmeaIncrCRC(_sm._crc,c);
        if ( _mkey.shiftIn(c) ) {
            auto it = _parsmap.find(_mkey);
            if ( it != _parsmap.end() ) {
                _parser = it->second;
            }
            _sm._header_len = pos+1;
            _sm._state = PAYLOAD;
            ESP_LOGI(FNAME, "Msg HEADER, %s", k.toString().c_str());
            break;
        }
        if (pos > 6) {
            _sm._state = START_TOKEN;
        }
        break;
    case PAYLOAD:
        if ( c == ',' ) {
            _sm._word_start.push_back(pos+1); // another word start
        }
        if ( c == '*' ) {
            _sm._word_start.push_back(pos+1);
            _sm._state = CHECK_CRC1; // Expecting a CRC to check
            break;
        }
        if ( c != '\r' && c != '\n' ) {
            ESP_LOGD(FNAME, "Msg PAYLOAD");
            nmeaIncrCRC(_sm._crc,c);
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

    datalink_action_t action = NOACTION;
    if ( _sm._state == COMPLETE )
    {
        NMEA::ensureTermination(_sm._frame);
        _sm._state = START_TOKEN; // restart parsing
        ESP_LOGI(FNAME, "Msg complete %s", _mkey.toString().c_str());
        action = _default_action;
        if ( _parser ) {
            action = (_parser)(this);
        }
    }
    return action;
}


