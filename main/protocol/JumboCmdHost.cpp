/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "JumboCmdHost.h"

#include "nmea_util.h"

#include "comm/DeviceMgr.h"
#include "comm/Messages.h"

#include <logdef.h>

#include <cstring>

// The jumbo command host side protocol parser. Those commands/queries are supported:
//
// - Connect query: Wakes the esp32 from light sleep when sleeping and a first trial cannot be repsonded.
//                  A second query should find the jumbo wake and responding.
//                  The response contains protocol revision and current status (mostly just "ready")
//   $PJMPC\r\n
//
// - Get config query: Request to tell about config items that are configurable through the web interface.
//   $PJMGC <config token>\r\n
//
// - Get info query: Request to tell about all the items that are visible in the webservers "info" box.
//   $PJMIQ\r\n
//
// - Select wing configuration: The selected wing span configuration will be saved permanently (stored in nvs).
//   $PJMCW <choice>*<CRC>\r\n
//
// - Start wipe command: Starts or Restarts a wiper cycle. Commanding a start while busy wiping will be ignored. The
//                      controlling peer needs to implement a tiny state machine to keep track. (different to button control)
//   $PJMWP <R/L>\r\n
//
// - Abort command: Stops the wiper immediately. Commanding stop while "ready", or "aborted" will be ignored.
//   $PJMAB <R/L>\r\n
//
// - Button short press: Alternatively this API would act as a soft button and create same reaction as the physical buttons 
//                  at the given moment.
//   $PJMBS <R/L>\r\n
//
// - Button hold down: ditto.
//   $PJMBH <R/L>\r\n
//
// - Button release (after hold down): ditto.
//   $PJMBR <R/L>\r\n
//
// The jmubo responses
// - Connected:
//   $PJPRP <protocol version>, <status>\r\n
//
// - Configuration:
//   $PJPRC <config token>, <value>*<CRC>\r\n
//
// - Info:
//   $PJPRI <fw version>, <SN>, <build date>, <...>*<CRC>\r\n
//
// - Alive:
//   $PJPJA <R/L><percentage>\r\n
//
// - Event: Something happend like: Abort (overcurrent), transition to pull, transition to park position (finished), go nap, ...
//   $PJPJE <R/L><event>*<CRC>\r\n
//
//


gen_state_t JumboCmdHost::nextByte(const char c)
{
    int pos = _sm._frame.size() - 1; // c already in the buffer
    ESP_LOGD(FNAME, "state %d, pos %d next char %c", _sm._state, pos, c);
    switch(_sm._state) {
    case START_TOKEN:
        if ( c == '$' ) {
            _sm._state = HEADER;
            _crc_buf[0] = '\0';
            ESP_LOGD(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        NMEA::incrCRC(_sm._crc,c);
        if ( pos < 4 ) { break; }
        if ( _sm._frame.substr(1,3) != "PJP" ) {
            _sm._state = START_TOKEN;
            break;
        }
        _sm._state = PAYLOAD;
        ESP_LOGD(FNAME, "Msg HEADER");
        break;
    case PAYLOAD:
        if ( c == '*' ) {
            _sm._state = CHECK_CRC; // Expecting a CRC to check
            break;
        }
        if ( c != '\r' && c != '\n' ) {
            ESP_LOGD(FNAME, "Msg PAYLOAD");
            NMEA::incrCRC(_sm._crc,c);
            break;
        }
        else {
            _sm._state = STOP_TOKEN;
        }
        // Fall through 
    case CHECK_CRC:
        if( _sm._state == CHECK_CRC ) { // did we not fall through
            if ( _crc_buf[0] == '\0' ) {
                // this is the first crc character
                _crc_buf[0] = c;
                break;
            }
            else {
                _crc_buf[1] = c;
                _crc_buf[2] = '\0';
                char read_crc = (char)strtol(_crc_buf, NULL, 16);
                ESP_LOGD(FNAME, "Msg CRC %s/%x - %x", _crc_buf, read_crc, _sm._crc);
                if ( read_crc != _sm._crc ) {
                    _sm._state = START_TOKEN;
                    break;
                }
                _sm._state = CHECK_OK; // fall through
            }
        }
        // Fall through 
    case STOP_TOKEN:
    case CHECK_OK:
    case COMPLETE:
        {
            _sm._frame.push_back('\0'); // terminate the string buffer
            gen_state_t next_state = START_TOKEN; // restart parsing
            ESP_LOGI(FNAME, "Msg complete %c%c", _sm._frame.at(4), _sm._frame.at(5));
            uint16_t mid = (_sm._frame.at(4)<<8) + _sm._frame.at(5);
            switch (mid) {
                case (('R' << 8) | 'P'):
                    connected();
                    break;
                case (('R' << 8) | 'C'):
                    if ( _sm._state == CHECK_OK ) {
                        config();
                    }
                    break;
                case (('R' << 8) | 'I'):
                    if ( _sm._state == CHECK_OK ) {
                        info();
                    }
                    break;
                case (('A' << 8) | ' '):
                    alive();
                    break;
                case (('E' << 8) | ' '):
                    event();
                    break;
                default:
                    break;
            }
            _sm._state = next_state;
        }
        break;
    default:
        break;
    }
    return _sm._state;
}

void JumboCmdHost::connected()
{
    ESP_LOGI(FNAME, "JP connected");
    // todo set a flag
}

typedef std::vector<const char*> TokenTable;
static TokenTable CONF_ITEM = { "WSPAN", "WSP_1", "LCORR", "RCORR", "WDFLT", "KICIT", "SEQNC", "REMDR", "WCONF"};

void JumboCmdHost::config()
{
    // grab token from e.g. message "$PJPRC WSPAN, "
    std::string itoken = _sm._frame.substr(7);
    int idx = 0;
    TokenTable::iterator it = CONF_ITEM.begin();
    for ( ; it != CONF_ITEM.end(); it++, idx++ ) {
        if ( itoken.compare(*it) == 0 ) {
            break;
        }
    }

    ESP_LOGI(FNAME, "JP config");
    if ( it == CONF_ITEM.end() ) {
        return; // unknown token
    }
    int value = atoi(_sm._frame.substr(13).c_str());
    switch (idx) {
    case 0:
        // todo // 0.1m
        break;
    case 1:
        // todo // 0.1m
        break;
    case 2:
        // todo // 0.1m
        break;
    case 3:
        // todo // 0.1m
        break;
    case 4:
        // todo // 0/1/2 (full/reduced/last)
        break;
    case 5:
        // todo  // 0-5x
        break;
    case 6:
        // todo // 0/1 (off/on)
        break;
    case 7:
        // todo // 0-200 min.
        break;
    case 8:
        // todo // 0/1 (full/reduced)
        break;
    default:
        break;
    }
}

void JumboCmdHost::info()
{
    ESP_LOGI(FNAME, "JP info");
    // todo
}

void JumboCmdHost::alive()
{
    ESP_LOGI(FNAME, "JP alive");
    // todo
}

void JumboCmdHost::event()
{
    ESP_LOGI(FNAME, "JP event");
    // todo
}


//
// Transmitter routines
//
bool JumboCmdHost::sendConnect()
{
    Message* msg = newMessage(); // set target
    if ( ! msg ) return false;

    msg->buffer = "$PJMPC\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendGetConfig(const int item_nr)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    msg->buffer = "$PJMGC ";
    msg->buffer += CONF_ITEM[item_nr];
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendGetInfo()
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    msg->buffer = "$PJMIQ\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendSelectConfig(const int wingconfig)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    msg->buffer = "$PJMCW " + std::to_string(wingconfig);
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendStartWipe(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMWP ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendAbortWipe(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMAB ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendShortPress(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMBS ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendHoldPressed(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMBH ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendReleasePressed(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMBR ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

