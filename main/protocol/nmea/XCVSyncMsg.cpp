/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "XCVSyncMsg.h"

#include "protocol/nmea_util.h"
#include "comm/Messages.h"
#include "setup/SetupNG.h"

#include "logdefnone.h"

#include <cstring>
#include "XCVarioMsg.h"

// The XCV sync messages to synchronize a client vario.

XCVSyncMsg::XCVSyncMsg(NmeaPrtcl &nr, bool master, bool as) :
    NmeaPlugin(nr, XCVSYNC_P, as),
    _is_master(master)
{
    // tell the only user of this
    SetupCommon::setSyncProto(this);
    // route diverse protocols further (e.g. Flarm to master-second-BT...)
    _nmeaRef.setDefaultAction(DO_ROUTING);
}

XCVSyncMsg::~XCVSyncMsg()
{
    SetupCommon::setSyncProto(nullptr);
}

//
// The sync transmitter routine
//
bool XCVSyncMsg::sendItem(const char *key, char type, void *value, int len)
{
    Message* msg = _nmeaRef.newMessage();

    ESP_LOGI(FNAME,"sendItem: %s", key );
    char sender = _is_master ? 'M' : 'C';

    ESP_LOGI(FNAME,"sender: %c", sender );
    msg->buffer =  "!xs";
    msg->buffer.push_back(sender);
    msg->buffer.push_back(',');
    msg->buffer += key;
    msg->buffer.push_back(',');
    msg->buffer.push_back(type);
    msg->buffer.push_back(',');
    char buf[40];
    if (type == 'F') {
        sprintf(buf, "%.3f", *(float *)(value));
    } else if (type == 'I') {
        sprintf(buf, "%d", *(int *)(value));
    }
    msg->buffer += buf;
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    return DEV::Send(msg);
}

dl_action_t XCVSyncMsg::parseExcl_xsX(NmeaPlugin *plg)
{
    ProtocolState *sm = plg->getNMEA().getSM();
    const std::vector<int> *word = &sm->_word_start;

    ESP_LOGI(FNAME,"parseXS %s", sm->_frame.c_str() );
    // char sender_role = sm->_frame[3]; // Master | Client
    int pos = word->at(0);
    std::string key = NMEA::extractWord(sm->_frame, pos);
    char type = sm->_frame[word->at(1)];
    float val = atof(sm->_frame.c_str() + word->at(2));
    ESP_LOGI(FNAME,"parsed NMEA: role=%c type=%c key=%s val=%f vali=%d", sender_role, type , key.c_str(), val, (int)val );
    SetupCommon *item = SetupCommon::getMember(key.c_str());
    if ( item ) {
        if( type == 'F' ) {
            SetupNG<float> *mi = static_cast<SetupNG<float> *>(item);
            mi->set( val, false );
        }
        else if( type == 'I' ) {
            SetupNG<int> *mi = static_cast<SetupNG<int> *>(item);
            mi->set( (int)val, false );
        }
    }
    else {
        ESP_LOGW(FNAME,"Setup item with key %s not found", key.c_str() );
    }

    return NOACTION; // never forward the XCV internal blabla
}

const ParserEntry XCVSyncMsg::_pt[] = {
    {Key("xsA"), XCVSyncMsg::parseExcl_xsX},
    {Key("xsC"), XCVSyncMsg::parseExcl_xsX},
    {Key("xsM"), XCVSyncMsg::parseExcl_xsX},
    {}
};
