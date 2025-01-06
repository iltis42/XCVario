/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "FlarmBin.h"

#include <logdef.h>

// The FLARM binary protocol synchronizer.
//

datalink_action_t flarmBinSM(ProtocolState &s, const char c)
{

    switch (s._state)
    {
    case START_TOKEN:
        if (c == 0x73)
        {
            s._state = HEADER;
            s._frame.assign(&c, 1);
            s._frame_len = 0;
        }
        break;
    case HEADER:
        if (s._frame_len == 0)
        {
            s._frame_len = (unsigned char)c;
        }
        else
        {
            s._frame_len += ((unsigned)(c) << 8);
            if (s._frame_len > 512)
            {
                ESP_LOGW(FNAME, "Odd length from flarm bincom: %d: restart", s._frame_len);
                s._state = START_TOKEN;
                break;
            }
            s._state = PAYLOAD;
        }
        s._frame.push_back(c);
        break;
    case PAYLOAD:
        if ( s._frame.size() == 5 && c == 0x12 ) {
            s._crc = GO_NMEA; // terminate BP
        }
        s._frame.push_back(c);
        if (s._frame.size() >= s._frame_len)
        {
            s._state = START_TOKEN;
            return datalink_action_t((int)DO_ROUTING | s._crc);
        }
        break;
    default:
        break;
    }
    return NOACTION;
}

