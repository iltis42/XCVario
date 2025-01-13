/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "FlarmBin.h"

#include "comm/Messages.h"
#include "comm/DataLink.h"
#include "nmea_util.h"

#include <logdef.h>

// The FLARM binary protocol synchronizer.
//
constexpr int SEND_THRESH = 80; // need to be smaller than the message buffer

void FlarmBinary::send_chunk()
{
    Message* msg = DEV::acqMessage(_binpeer->getDeviceId(), _binpeer->getSendPort());
    msg->buffer = _sm._frame;
    DEV::Send(msg);
    if ( _sm._opt == 0 ) {
        std::string str = msg->hexDump(9);
        ESP_LOGI(FNAME, ">%c %s", msg->target_id==FLARM_DEV?'D':'H', str.c_str());
    }
    // else {
    //     if ( _sm._crc == 0xa0 ) 
    //         ESP_LOGI(FNAME, ">%c chunk %d: %s", msg->target_id==FLARM_DEV?'D':'H', _sm._opt + _sm._frame.size(), _sm._frame.c_str());
    // }
    // Partial reset to cope with large messages
    _sm._opt += _sm._frame.size();
    _sm._frame.clear();
}


datalink_action_t FlarmBinary::nextStreamChunk(const char *cptr, int count)
{
    datalink_action_t last_action = NOACTION;

    for (int i = 0; i < count; i++)
    {
        char c = *(cptr+i);

        switch (_sm._state)
        {
        case START_TOKEN:
            if (c == 0x73)
            {
                _sm._state = HEADER;
                _sm.push(c);
                _sm._frame_len = 0;
                _sm._opt = 0;
                ESP_LOGD(FNAME, "Starttoken");

            }
            break;
        case HEADER:
            if (_sm._frame_len == 0)
            {
                _sm._frame_len = (unsigned char)c;
            }
            else
            {
                _sm._frame_len += ((unsigned)(c) << 8) + 1; // the latter is to account  for the start token
                if (_sm._frame_len > 512)
                {
                    ESP_LOGW(FNAME, "Odd length from flarm bincom: %d: restart", _sm._frame_len);
                    _sm._state = START_TOKEN;
                    last_action = GO_NMEA;
                    break;
                }
                // ESP_LOGI(FNAME, "Payload %d", _sm._frame_len);
                _sm._state = PAYLOAD;
            }
            _sm.push(c);
            break;
        case PAYLOAD:
            if ( (_sm._opt + _sm._frame.size()) == 6 ) {
                _sm._crc = c;
                ESP_LOGD(FNAME, "frame type 0x%x len %d", c, _sm._frame_len);
            }
            _sm.push(c);
            if ( (_sm._opt + _sm._frame.size()) >= _sm._frame_len )
            {
                send_chunk();
                _sm._state = START_TOKEN;
                if ( _sm._crc == 0x12 ) { // terminate BP
                    ESP_LOGI(FNAME, "0x12 BP end <---------------- switch to nmea");
                    _binpeer->getDL()->goNMEA();
                    last_action = GO_NMEA;
                }
            }
            else if (_sm._frame.size() >= SEND_THRESH) {
                send_chunk();
            }
            break;
        default:
            break;
        }
    }

    return last_action;
}

void FlarmBinary::setPeer(ProtocolItf *p)
{
    _binpeer = p;
    ESP_LOGD(FNAME, "BP%d peer is dl%d/g%d", _dl.getItf(), _binpeer->getDL()->getItf(), _binpeer->getDeviceId());
}
