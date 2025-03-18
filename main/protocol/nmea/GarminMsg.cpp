/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "GarminMsg.h"

#include "Flarm.h"
#include "sensor.h"

#include <logdef.h>


// The Garmin protocol parser.
//
// Supported messages:
// PGRMZ,<Value>,F,2


// $PGRMZ,880,F,2*3A
//
datalink_action_t GarminMsg::parsePGRMZ(NmeaPrtcl *nmea)
{
    if (alt_select.get() != AS_EXTERNAL) {
        return NOACTION;
    }
    ProtocolState *sm = nmea->getSM();
    const std::vector<int> *word = &sm->_word_start;
    ESP_LOGD(FNAME, "parsePGRMZ");

    if ( word->size() == 4 && sm->_frame.at(word->at(1)) == 'F' ) {
        int alt1013_ft = atoi(sm->_frame.c_str()+word->at(0));
        alt_external = Units::feet2meters((float)(alt1013_ft + 0.5));
        ESP_LOGI(FNAME, "PGRMZ %d: ALT(1013):%5.0f m", alt1013_ft, alt_external);
        Flarm::timeout = 10;
        Flarm::ext_alt_timer = 10; // Fall back to internal Barometer after 10 seconds
    }
    return DO_ROUTING;
}



ParserMap GarminMsg::_pm = {
    { Key("MZ"), GarminMsg::parsePGRMZ }
};


