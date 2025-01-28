/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "MagSensBin.h"

#include "QMCMagCAN.h"
#include "Clock.h"
#include "Compass.h"

#include <logdef.h>

// The legacy MagSens binary protocol.

MagSensBinary::MagSensBinary(int mp, ProtocolState &sm, DataLink &dl)
    : ProtocolItf(DeviceId::MAGSENS_DEV, mp, sm, dl)
{
    _last_sample_time = Clock::getMillis();
    _delta_time = 100; // expect 10Hz
}

datalink_action_t MagSensBinary::nextStreamChunk(const char *cptr, int count)
{
    // ESP_LOGI(FNAME, "Got data");
    QMCMagCAN* ms = static_cast<QMCMagCAN*>(Compass::getSensInst());

    // Just check on 6 bytes telegram length
    if ( count == 6 && ms ) {
        ms->fromCAN((t_magn_axes*)(cptr));

        if ( _connected > 0 ) {
            _last_sample_time = Clock::getMillis();
         }
         else {
            _connected++;
            unsigned long tmp = Clock::getMillis();
            _delta_time = (_delta_time + (tmp - _last_sample_time)) / 2;
            _last_sample_time = tmp;
         }
    }

    return NOACTION;
}

bool MagSensBinary::isActive() const
{
    ESP_LOGI(FNAME, "Active %ld %ld %ld", Clock::getMillis(), _last_sample_time, _delta_time);
    return (Clock::getMillis() - _last_sample_time) < (2 * _delta_time);
}
