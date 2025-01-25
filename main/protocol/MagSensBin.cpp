/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "MagSensBin.h"

#include "QMCMagCAN.h"
#include "Compass.h"

#include <logdef.h>

// The legacy MagSens binary protocol.

datalink_action_t MagSensBinary::nextStreamChunk(const char *cptr, int count)
{
    // ESP_LOGI(FNAME, "Got data");
    QMCMagCAN* ms = static_cast<QMCMagCAN*>(Compass::getSensInst());

    // Just check on 6 bytes telegram length
    if ( count == 6 && ms ) {
        ms->fromCAN((t_magn_axes*)(cptr));
        _connected = true;
    }

    return NOACTION;
}

