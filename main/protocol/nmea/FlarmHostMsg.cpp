/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "FlarmHostMsg.h"
#include "comm/DeviceMgr.h"

#include "logdefnone.h"

// The FLARM host protocol checker/forwarder.
//
// Supported messages:
// PFL*

FlarmHostMsg::FlarmHostMsg(NmeaPrtcl &nr) :
    NmeaPlugin(nr, FLARMHOST_P, false)
{
    _nmeaRef.setDefaultAction(DO_ROUTING);
}

// Initiate a flarm BP session
// $PFLAX*2E
dl_action_t FlarmHostMsg::parsePFLAX(NmeaPlugin *plg)
{
    ProtocolState *sm = plg->getNMEA().getSM();
    ESP_LOGI(FNAME, "FLAX request received from host -----------------+");
    ESP_LOGI(FNAME, "Start binary request");
    DEVMAN->setFlarmBPInitiator(plg->getNMEA().getDL());
    return NXT_PROTO; // proactively to mute all other NMEA activities on the line
}

const ParserEntry FlarmHostMsg::_pt[] = {
    { Key("FLAX"), FlarmHostMsg::parsePFLAX },
    {}
};
