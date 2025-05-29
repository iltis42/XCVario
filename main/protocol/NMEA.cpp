/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "NMEA.h"

#include  "comm/DataLink.h"
#include "AliveMonitor.h"
#include "nmea_util.h"
#include "logdefnone.h"

// global variables
NmeaPrtcl *ToyNmeaPrtcl = nullptr; // set to trigger a navi data stream

// A generic and extendable NMEA message parser.

NmeaPrtcl::~NmeaPrtcl()
{
    if ( this == ToyNmeaPrtcl ) {
        // reset the global toy protocol pointer
        ToyNmeaPrtcl = nullptr;
    }
    for (auto parser : _plugs) {
        // all parser tables
        delete parser;
    }
    _plugs.clear();
    _parsmap.clear();

    if ( _alive ) {
        delete _alive;
        _alive = nullptr;
    }
}

bool NmeaPrtcl::hasProtocol(ProtocolType p) const
{
    for ( auto &pl : _plugs ) {
        if ( pl->getPtyp() == p ) {
            return true;
        }
    }
    return false;
}

void NmeaPrtcl::addPlugin(NmeaPlugin *pm)
{
    ESP_LOGI(FNAME, "Add plugin %d", pm->getPtyp());
    // check if plugin already there
    if ( hasProtocol(pm->getPtyp()) ) {
        ESP_LOGW(FNAME, "Plugin already loaded");
        return; // do not add it
    }
    _plugs.push_back(pm);
    // copy the parser table
    for (const ParserEntry* entry = pm->getPT(); entry->second != nullptr; ++entry) {
        Key key = entry->first;
        NmeaMessageParser value = entry->second;
        if ( _parsmap.find(key) == _parsmap.end() ) { // do not overwrite entries
            _parsmap[key] = MapValue(value, pm);
            ESP_LOGI(FNAME, "copy parser for %s", key.toString().c_str());
        }
    }
}

void NmeaPrtcl::removeProtocol(ProtocolType p)
{
    NmeaPlugin *pm = nullptr;
    for ( auto it = _plugs.begin(); it != _plugs.end(); it++ ) {
        if ( (*it)->getPtyp() == p ) {
            pm = *it;
            _plugs.erase(it);
            break;
        }
    }
    if ( pm ) {
        // remove the parser table entries
        for (auto it = _parsmap.begin(); it != _parsmap.end(); ) {
            if (it->second.second == pm) {
                it = _parsmap.erase(it);  // erase returns the next valid iterator
            } else {
                it++;
            }
        }
        // delete the plugin
        ESP_LOGI(FNAME, "Delete plugin %d", pm->getPtyp());
        delete pm;
    }
    if ( _plugs.empty() ) {
        // no more plugins, delete the alive monitor
        if ( _alive ) {
            delete _alive;
            _alive = nullptr;
        }
    }
}

dl_control_t NmeaPrtcl::nextBytes(const char* c, int len)
{
    int pos = _sm._frame.size();
    _sm.push(*c);
    ESP_LOGD(FNAME, "state %d, pos %d next char %c", _sm._state, pos, *c);
    switch(_sm._state) {
    case START_TOKEN:
        if (*c == '$' || *c == '!') {
            _sm._state = HEADER;
            _sm._word_start.clear();
            _mkey.value = 0;
            _parser = {};
            ESP_LOGD(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        nmeaIncrCRC(_sm._crc, *c);
        if ( _mkey.shiftIn(*c) ) {
            auto it = _parsmap.find(_mkey);
            if ( it != _parsmap.end() ) {
                _parser = it->second;
            }
            _sm._header_len = pos+1;
            _sm._word_start.push_back(pos+1);
            _sm._state = PAYLOAD;
            ESP_LOGD(FNAME, "Msg HEADER, %s", _mkey.toString().c_str());
            break;
        }
        if (pos > 6) {
            _sm._state = START_TOKEN;
        }
        break;
    case PAYLOAD:
        if ( *c == ',' ) {
            _sm._word_start.push_back(pos+1); // another word start
        }
        else {
            if ( *c == '*' ) {
                _sm._word_start.push_back(pos+1);
                _sm._state = CHECK_CRC1; // Expecting a CRC to check
                break;
            }
            if ( *c == '\r' || *c == '\n' ) {
                _sm._state = COMPLETE;
                break;
            }
        }
        ESP_LOGD(FNAME, "Msg PAYLOAD");
        nmeaIncrCRC(_sm._crc, *c);
        break;
    case CHECK_CRC1:
        _crc_buf[0] = *c;
        _sm._state = CHECK_CRC2;
        break;
    case CHECK_CRC2:
    {
        _crc_buf[1] = *c;
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

    dl_control_t ret(NOACTION, _did);
    if ( _sm._state == COMPLETE )
    {
        NMEA::ensureTermination(_sm._frame);
        _sm._state = START_TOKEN; // restart parsing
        ESP_LOGI(FNAME, "Msg complete %s", _mkey.toString().c_str());
        ret.act = _default_action;
        if ( _parser.first ) {
            ret.act = (_parser.first)(_parser.second);
            ret.did = _parser.second->getRouteId();
        }
        if ( _alive) {
            _alive->keepAlive();
        }
    }
    return ret;
}

// plugin base with a potential different device id for routing
NmeaPlugin::NmeaPlugin(NmeaPrtcl &nr, ProtocolType ptyp, bool as) :
    _nmeaRef(nr),
    _pid(ptyp),
    _auto(as),
    _routeId(nr.getDL()->getDeviceId())
{
}
