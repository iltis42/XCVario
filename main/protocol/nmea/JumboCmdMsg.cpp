/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "JumboCmdMsg.h"

#include "protocol/nmea_util.h"

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
//   $PJMGC, <config token>\r\n
//
// - Get info query: Request to tell about all the items that are visible in the webservers "info" box.
//   $PJMIQ\r\n
//
// - Select wing configuration: The selected wing span configuration will be saved permanently (stored in nvs).
//   $PJMCW, <choice>*<CRC>\r\n
//
// - Start wipe command: Starts or Restarts a wiper cycle. Commanding a start while busy wiping will be ignored. The
//                      controlling peer needs to implement a tiny state machine to keep track. (different to button control)
//   $PJMWP, <R/L>\r\n
//
// - Abort command: Stops the wiper immediately. Commanding stop while "ready", or "aborted" will be ignored.
//   $PJMAB, <R/L>\r\n
//
// - Button short press: Alternatively this API would act as a soft button and create same reaction as the physical buttons 
//                  at the given moment.
//   $PJMBS, <R/L>\r\n
//
// - Button hold down: ditto.
//   $PJMBH, <R/L>\r\n
//
// - Button release (after hold down): ditto.
//   $PJMBR, <R/L>\r\n
//
// The jmubo responses
// - Connected:
//   $PJPRP, <protocol version>, <status>\r\n
//
// - Configuration:
//   $PJPRC, <config token>, <value>*<CRC>\r\n
//
// - Info:
//   $PJPRI, <fw version>, <SN>, <build date>, <...>*<CRC>\r\n
//
// - Alive:
//   $PJPJA, <R/L><percentage>\r\n
//
// - Event: Something happend like: Abort (overcurrent), transition to pull, transition to park position (finished), go nap, ...
//   $PJPJE, <R/L><event>*<CRC>\r\n
//
//

datalink_action_t JumboCmdMsg::connected(NmeaPrtcl *nmea)
{
    ESP_LOGI(FNAME, "JP connected");
    // todo set a flag
    return NOACTION;
}

typedef std::vector<const char*> TokenTable;
static TokenTable CONF_ITEM = { "WSPAN", "WSP_1", "LCORR", "RCORR", "WDFLT", "KICIT", "SEQNC", "REMDR", "WCONF"};

datalink_action_t JumboCmdMsg::config(NmeaPrtcl *nmea)
{
    // grab token from e.g. message "$PJPRC WSPAN, "
    ProtocolState *sm = nmea->getSM();
    std::string itoken = sm->_frame.substr(7);
    int idx = 0;
    TokenTable::iterator it = CONF_ITEM.begin();
    for ( ; it != CONF_ITEM.end(); it++, idx++ ) {
        if ( itoken.compare(*it) == 0 ) {
            break;
        }
    }

    ESP_LOGI(FNAME, "JP config");
    if ( it == CONF_ITEM.end() ) {
        return NOACTION; // unknown token
    }
    int value = atoi(sm->_frame.substr(13).c_str());
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
    return NOACTION;
}

datalink_action_t JumboCmdMsg::info(NmeaPrtcl *nmea)
{
    ESP_LOGI(FNAME, "JP info");
    // todo
    return NOACTION;
}

datalink_action_t JumboCmdMsg::alive(NmeaPrtcl *nmea)
{
    ESP_LOGI(FNAME, "JP alive");
    // todo
    return NOACTION;
}

datalink_action_t JumboCmdMsg::event(NmeaPrtcl *nmea)
{
    ESP_LOGI(FNAME, "JP event");
    // todo
    return NOACTION;
}

// NMEA plugin table
ConstParserMap JumboCmdMsg::_pm = {
    { Key("JPRP"), JumboCmdMsg::connected },
    { Key("JPRC"), JumboCmdMsg::config },
    { Key("JPRI"), JumboCmdMsg::info },
    { Key("JPJA"), JumboCmdMsg::alive },
    { Key("JPJE"), JumboCmdMsg::event }
};

//
// Transmitter routines
//
bool NmeaPrtcl::sendJPConnect()
{
    Message* msg = newMessage(); // set target
    if ( ! msg ) return false;

    msg->buffer = "$PJMPC\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::sendJPGetConfig(const int item_nr)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    msg->buffer = "$PJMGC, ";
    msg->buffer += CONF_ITEM[item_nr];
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::sendJPGetInfo()
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    msg->buffer = "$PJMIQ\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::sendJPSelectConfig(const int wingconfig)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    msg->buffer = "$PJMCW, " + std::to_string(wingconfig);
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::sendJPStartWipe(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMWP, ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::sendJPAbortWipe(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMAB, ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::sendJPShortPress(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMBS, ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::sendJPHoldPressed(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMBH, ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::sendJPReleasePressed(const int side)
{
    Message* msg = newMessage();
    if ( ! msg ) return false;

    char side_chr = 'R';
    if ( side == 1 ) {
        side_chr = 'L';
    }
    msg->buffer = "$PJMBR, ";
    msg->buffer.push_back(side_chr);
    msg->buffer += "\r\n";
    return DEV::Send(msg);
}

