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
#include "comm/DeviceMgr.h"
#include "comm/SerialLine.h"
#include "nmea_util.h"

#include <logdefnone.h>

static uint16_t xmodem_crc(const uint8_t *data, int length, uint16_t crc0 = 0);

// The FLARM binary protocol synchronizer.
//
constexpr int SEND_THRESH = 80; // need to be smaller than the message buffer
constexpr int ESCAPE = 0x78;

void FlarmBinary::send_chunk()
{
    Message* msg = DEV::acqMessage(_binpeer->getDeviceId(), _binpeer->getSendPort());
    msg->buffer = _sm._frame;
    DEV::Send(msg);
    if ( _sm._opt == 0 ) {
        std::string str = msg->hexDump(9);
        ESP_LOGI(FNAME, ">%c %s", msg->target_id==FLARM_DEV?'D':'H', str.c_str());
    }
    // Partial reset to cope with large messages
    _sm._opt += _sm._frame.size();
    _sm._frame.clear();
}


datalink_action_t FlarmBinary::nextStreamChunk(const char *cptr, int count)
{
    datalink_action_t last_action = NOACTION;

    // The state machine variable useage here:
    // _esc is used as ESCAPE flag
    // _opt is used to cope with telegrams larger than the serial line buffers.
    // _crc holds the last received frame type (!)

    for (int i = 0; i < count; i++)
    {
        char c = *(cptr+i);
        _sm.push(c);
        if ( c == ESCAPE ) { // escape token
            // one more charachter to expect, escape and start token do not count
            _sm._esc = ESCAPE;
            _sm._opt -= 1;
            continue; // skip parsing step entirely
        }
        else if ( _sm._esc == ESCAPE ) {
            // correct the escaped byte
            c = (c == 0x55)? 0x78 : (c == 0x31)? 0x73 : c;
            _sm._esc = 0;
        }

        switch (_sm._state)
        {
        case START_TOKEN:
            if (c == 0x73)
            {
                _sm._state = HEADER;
                _sm._frame_len = 0;
                _sm._opt = -1; // the frame start token should be removed, but we need it for forwarding purpose of the telegram
                _sm._esc = 0;
                ESP_LOGD(FNAME, "Starttoken");
            }
            break;
        case HEADER:
            if (_sm._opt + _sm._frame.size() == 1)
            {
                _sm._frame_len += (unsigned char)c;
            }
            else
            {
                _sm._frame_len += ((unsigned)(c) << 8);
                if (_sm._frame_len > 512)
                {
                    ESP_LOGW(FNAME, "Odd length from flarm BP: %d: restart", _sm._frame_len);
                    _sm._state = START_TOKEN;
                    last_action = GO_NMEA;
                    break;
                }
                // ESP_LOGI(FNAME, "Header payload: %db", _sm._frame_len);
                _sm._state = PAYLOAD;
            }
            break;
        case PAYLOAD:
        {
            int allfs = _sm._opt + _sm._frame.size();
            if ( allfs == 4 ) { // SeqNo ls byte
                _frame_counter = c;
            }
            else if ( allfs == 5 ) { // SeqNo ms byte
                _frame_counter += ((unsigned)(c) << 8);
                ESP_LOGD(FNAME, "frame counter %d: 0x%x", _did, _frame_counter);
            }
            else if ( allfs == 6 ) { // msg type byte
                _sm._crc = c;
                ESP_LOGD(FNAME, "frame type 0x%x len %d", c, _sm._frame_len);
            }
            else if ( allfs >= _sm._frame_len )
            {
                // msg is read in completely
                _sm._state = START_TOKEN;
                send_chunk(); // forward msg (remaining data)
                if ( _sm._crc == 0x12 ) { // exit BP msg
                    ESP_LOGI(FNAME, "0x12 BP end <---------------- switch to nmea");
                    _binpeer->getDL()->goNMEA();
                    InterfaceCtrl *itf = DEVMAN->getIntf(FLARM_DEV);
                    vTaskDelay(pdMS_TO_TICKS(10));
                    itf->ConfigureIntf(SM_FLARM);
                    last_action = GO_NMEA;
                }
            }
            else if (_sm._frame.size() >= SEND_THRESH) {
                send_chunk(); // forward partial msg
            }
            break;
        }
        default:
            break;
        }
    }

    return last_action;
}

bool FlarmBinary::setBaudrate(int fnr, int br)
{
    Message* msg = newMessage();
    // magic, frame size lsb+msb, version, seq counter lsb+msb, msg type, crc lsb+msb, payload
    msg->buffer = { 0x73, 9, 0, 1};
    ESP_LOGI(FNAME, "Set baudrate %d (frm=%d)", br, fnr);
    msg->buffer.push_back((uint8_t)(fnr));
    msg->buffer.push_back((uint8_t)(fnr>>8));
    msg->buffer.push_back(0x02); // set baudrate msg type
    uint16_t crc = xmodem_crc((uint8_t*)(msg->buffer.data()+1), 6);
    crc = xmodem_crc((uint8_t*)&br, 1, crc);
    msg->buffer.push_back((uint8_t)(crc));
    msg->buffer.push_back((uint8_t)(crc>>8));
    msg->buffer.push_back((uint8_t)br);
    ESP_LOGI(FNAME, "0x: %s", msg->hexDump().c_str());
    ESP_LOGI(FNAME, "To t:%d p:%d", msg->target_id, msg->port);
    return DEV::Send(msg);
}

bool FlarmBinary::ping()
{
    Message* msg = newMessage();
    msg->buffer = { 0x73, 8, 0, 0 };
    int frc = getFrameCnt();
    ESP_LOGI(FNAME, "Send ping (frm=%d)", frc);
    msg->buffer.push_back((uint8_t)(frc));
    msg->buffer.push_back((uint8_t)(frc>>8));
    msg->buffer.push_back(0x01); // ping msg type
    uint16_t crc = xmodem_crc((uint8_t*)(msg->buffer.data()+1), 6);
    msg->buffer.push_back((uint8_t)(crc));
    msg->buffer.push_back((uint8_t)(crc>>8));
    return DEV::Send(msg);
}

void FlarmBinary::setPeer(FlarmBinary *p)
{
    _binpeer = p;
    ESP_LOGD(FNAME, "BP%d peer is dl%d/g%d", _dl.getItf(), _binpeer->getDL()->getItf(), _binpeer->getDeviceId());
}

uint16_t xmodem_crc(const uint8_t *data, int length, uint16_t crc0)
{
    uint16_t crc = crc0;

    while (length--) {
        crc ^= (*data++) << 8;  // XOR byte into high byte of CRC
        
        for (int i = 0; i < 8; i++) {  // Process each bit
            if (crc & 0x8000) {
                crc = (crc << 1) ^ 0x1021;  // XOR with polynomial if MSB is 1
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

