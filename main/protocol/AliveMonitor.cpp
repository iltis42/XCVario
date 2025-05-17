/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

 #include "AliveMonitor.h"

 #include "setup/SetupNG.h"

 #include "logdefnone.h"
 

AliveMonitor::AliveMonitor(SetupNG<int> *flag, int to_ms) :
    WDBark_I(),
    _alive_wd(this),
    _alive_flag(flag)
{
    assert(_alive_flag);
    _alive_wd.start(to_ms);

    _alive_flag->set(ALIVE_TIMEOUT); // configured, but need the proove of live data
}

void AliveMonitor::keepAlive()
{
    _alive_flag->set(ALIVE_OK);
    if ( _alive_wd.isRunning() ) {
        _alive_wd.pet();
    }
    else {
        _alive_wd.restart();
    }
    ESP_LOGI(FNAME, "Alive monitor %s", _alive_flag->key());
}

int AliveMonitor::get() const
{
    return _alive_flag->get();
}

void AliveMonitor::barked()
{
    // alive timeout
    _alive_flag->set(ALIVE_TIMEOUT);
    ESP_LOGI(FNAME, "Alive monitor timeout %s", _alive_flag->key());
}