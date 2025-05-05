/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "FlarmHostMsg.h"

#include <logdef.h>

// The FLARM host protocol checker/forwarder.
//
// Supported messages:
// PFL*

FlarmHostMsg::FlarmHostMsg(NmeaPrtcl &nr) :
    NmeaPlugin(nr, FLARMHOST_P, false)
{
    _nmeaRef.setDefaultAction(DO_ROUTING);
}

dl_action_t FlarmHostMsg::parsePFLAX(NmeaPlugin *plg)
{
    ProtocolState *sm = plg->getNMEA().getSM();
    ESP_LOGI(FNAME, "FLAX called -----------------+");
    if ( sm->_frame.at(6) != ',' ) {
        ESP_LOGI(FNAME, "Start binary request");
        return NXT_PROTO;
    }
    return DO_ROUTING;
}

const ParserEntry FlarmHostMsg::_pt[] = {
    { Key("FLAX"), FlarmHostMsg::parsePFLAX },
    {}
};
