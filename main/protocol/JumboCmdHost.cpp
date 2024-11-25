/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "JumboCmdHost.h"

#include "nmea_util.h"

#include "comm/Routing.h"
#include "comm/Messages.h"

#include <logdef.h>

#include <cstring>

// The jumbo command host side protocol parser. Those commands/queries are supported:
//
// - Connect query: Wakes the esp32 from light sleep when sleeping and a first trial cannot be repsonded.
//                  A second query should find the jumbo wake and responding.
//                  The response contains protocol revision and current status (mostly just "ready")
//   !jpc\r\n
//
// - Get config query: Request to tell about config items that are configurable through the web interface.
//   !jgc <config token>\r\n
//
// - Get info query: Request to tell about all the items that are visible in the webservers "info" box.
//   !jiq\r\n
//
// - Select wing configuration: The selected wing span configuration will be saved permanently (stored in nvs).
//   !jsw <choice>*<CRC>\r\n
//
// - Start wipe command: Starts or Restarts a wiper cycle. Commanding a start while busy wiping will be ignored. The
//                      controlling peer needs to implement a tiny state machine to keep track. (different to button control)
//   !jwp <R/L>\r\n
//
// - Abort command: Stops the wiper immediately. Commanding stop while "ready", or "aborted" will be ignored.
//   !jab <R/L>\r\n
//
// - Button short press: Alternatively this API would act as a soft button and create same reaction as the physical buttons 
//                  at the given moment.
//   !jbs <R/L>\r\n
//
// - Button hold down: ditto.
//   !jbh <R/L>\r\n
//
// - Button release (after hold down): ditto.
//   !jbr <R/L>\r\n
//
// The jmubo responses
// - Connected:
//   !jrp <protocol version>, <status>\r\n
//
// - Configuration:
//   !jrc <config token> <value>*<CRC>\r\n
//
// - Info:
//   !jri <fw version>, <SN>, <build date>, <...>*<CRC>\r\n
//
// - Alive:
//   !ja <R/L><percentage>\r\n
//
// - Event: Something happend like: Abort (overcurrent), transition to pull, transition to park position (finished), go nap, ...
//   !je <R/L><event>*<CRC>\r\n
//
//


gen_state_t JumboCmdHost::nextByte(const char c)
{
    ESP_LOGI(FNAME, "state %d, pos %d next char %c", _state, _pos, c);
    switch(_state) {
    case START_TOKEN:
    case CHECK_FAILED:
    case RESTART:
        if ( c == '!' ) {
            _state = HEADER;
            reset();
            push(c);
            ESP_LOGI(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        if ( c != 'j' ) {
            _state = RESTART;
            break;
        }
        _state = PAYLOAD;
        ESP_LOGI(FNAME, "Msg HEADER");
        push_and_crc(c);
        break;
    case PAYLOAD:
        if ( c == '*' ) {
            push(c);
            _state = CHECK_CRC; // Expecting a CRC to check
            break;
        }
        if ( c != '\r' && c != '\n' ) {
            ESP_LOGI(FNAME, "Msg PAYLOAD");
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
                push(c); // not really needed
                break;
            }
            else {
                push(c); // not really needed
                _crc_buf[1] = c;
                _crc_buf[2] = '\0';
                char read_crc = (char)strtol(_crc_buf, NULL, 16);
                ESP_LOGI(FNAME, "Msg CRC %s/%x - %x", _crc_buf, read_crc, _crc);
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
            gen_state_t next_state = START_TOKEN; // restart parsing
            ESP_LOGI(FNAME, "Msg complete %c%c", _framebuffer[2], _framebuffer[3]);
            uint16_t mid = (_framebuffer[2]<<8) + _framebuffer[3];
            switch (mid) {
                case (('r' << 8) | 'p'):
                    connected();
                    break;
                case (('r' << 8) | 'c'):
                    if ( _state == CHECK_OK ) {
                        config();
                    }
                    break;
                case (('r' << 8) | 'i'):
                    if ( _state == CHECK_OK ) {
                        info();
                    }
                    break;
                case (('a' << 8) | ' '):
                    alive();
                    break;
                case (('e' << 8) | ' '):
                    event();
                    break;
                default:
                    break;
            }
            _state = next_state;
        }
        break;
    default:
        break;
    }
    return _state;
}

void JumboCmdHost::connected()
{
    ESP_LOGI(FNAME, "JP connected");
    // todo set a flag
}

typedef std::vector<const char*> TokenTable;
static TokenTable TOKENS = { "WSPAN", "WSP_1", "LCORR", "RCORR", "WDFLT", "KICIT", "SEQNC", "REMDR", "WCONF"};

void JumboCmdHost::config()
{
    // grab token from e.g. message "!jrc WSPAN"
    std::string frame(&_framebuffer[5]);
    int idx = 0;
    TokenTable::iterator it = TOKENS.begin();
    for ( ; it != TOKENS.end(); it++, idx++ ) {
        if ( frame.compare(*it) == 0 ) {
            break;
        }
    }

    ESP_LOGI(FNAME, "JP config");
    if ( it == TOKENS.end() ) {
        return; // unknown token
    }
    int value = atoi(&_framebuffer[10]);
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

    msg->buffer = "!jpc\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendGetConfig(const int item_nr)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    msg->buffer = "!jgc ";
    msg->buffer += TOKENS[item_nr];
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendGetInfo()
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    msg->buffer = "!jiq\r\n";
    return DEV::Send(msg);
}

bool JumboCmdHost::sendSelectConfig(const int wingconfig)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    msg->buffer = "!sw " + std::to_string(wingconfig);
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
    msg->buffer = "!jwp ";
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
    msg->buffer = "!jab ";
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
    msg->buffer = "!jbs ";
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
    msg->buffer = "!jbh ";
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
    msg->buffer = "!jbr ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

