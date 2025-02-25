/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "FlarmSim.h"

#include "nmea_util.h"
#include "comm/DeviceMgr.h"
#include "protocol/ClockIntf.h"
#include "protocol/Clock.h"

#include <logdefnone.h>

#include <string>



// A FLARM "AU" message simulator

// Messages to simulate FLARM AU sentences sequence
const char *pflau_msg[] = {
    "1,-60,2,-100,355",
    "1,-20,2,-100,255",
    "1,-10,2,-80,175",
    "2,10,2,-40,150",
    "2,20,2,-20,130",
    "3,30,2,0,120",
    "3,60,2,20,125",
    "2,80,2,40,160",
    "1,90,2,80,210",
    "1,90,2,80,280",
    ""
    };

FlarmSim *FlarmSim::_sim = nullptr;

// iterate and inject through messages every other second
bool FlarmSim::tick()
{
    ESP_LOGI(FNAME,"flarmSim tick");

    if ( *pflau_msg[_next_msg] != '\0' )
    {
        std::string msg("$PFLAU,3,1,2,1,");
        msg += pflau_msg[_next_msg];
        msg += ",1234*";
        msg += NMEA::CheckSum(msg.c_str()) + "\r\n";
        DataLink *dl = *_d->_dlink.begin();
        dl->process(msg.c_str(), msg.size());
        _next_msg++;
    }
    else {
        delete this;
        return true;
    }
    return false;
}

FlarmSim::FlarmSim(Device *d) :
    Clock_I(100), // generates a time-out callback ca. every second
    _d(d)
{
    ESP_LOGI(FNAME,"Kick ticker");
    Clock::start(this);
}

FlarmSim::~FlarmSim()
{
    InterfaceCtrl *ic = _d->_itf;
    // Reconnect datalink
    ic->addDataLink(*(_d->_dlink.begin()));
    _sim = nullptr;
}

// precondition: Flarm device is configured
void FlarmSim::StartSim()
{
    if ( ! _sim ) {
        ESP_LOGI(FNAME,"Start Simulator");
        // only one at a time
        Device *d = DEVMAN->getDevice(FLARM_DEV);
        if ( d ) {
            ESP_LOGI(FNAME,"Found Flarm %d", d->_id);
            // a Flarm is connected
            InterfaceCtrl *ic = d->_itf;
            ic->MoveDataLink(0); // the device has a backup of the data link pointer
            _sim = new FlarmSim(d);
        }

    }
}

