/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "TestQuery.h"

#include "nmea_util.h"

#include "Clock.h"
#include "comm/DeviceMgr.h"
#include "comm/Messages.h"
#include "logdef.h"

#include <cstring>
#include <string>

Clock *TestQuery::MY_CLOCK = nullptr;

TestQuery::TestQuery(int mp, ProtocolState &sm)
  : ProtocolItf(mp, sm),
    Clock_I(500) // generates a time-out callback ca. every fife seconds
{
    ESP_LOGI(FNAME, "Setup test bursts to send on serial line.");
    MY_CLOCK = new Clock();
   	Clock::start(this); // register query kicker time-out
}

TestQuery::~TestQuery()
{
    Clock::stop(this); // deregister time-out
}

gen_state_t TestQuery::nextByte(const char c)
{
    int pos = _sm._frame.size() - 1; // c already in the buffer
    ESP_LOGD(FNAME, "state %d, pos %d next char %c", _sm._state, pos, c);
    switch(_sm._state) {
    case START_TOKEN:
        if ( c == '$' ) {
            _sm._state = HEADER;
            _crc_buf[0] = '\0';
            ESP_LOGD(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        NMEA::incrCRC(_sm._crc,c);
        if ( pos < 4 ) { break; }
        if ( _sm._frame.substr(1,3) != "PJM" ) {
            _sm._state = START_TOKEN;
            break;
        }
        _sm._state = PAYLOAD;
        ESP_LOGD(FNAME, "Msg HEADER");
        break;
    case PAYLOAD:
        if ( c == '*' ) {
            _sm._state = CHECK_CRC; // Expecting a CRC to check
            break;
        }
        if ( c != '\r' && c != '\n' ) {
            ESP_LOGD(FNAME, "Msg PAYLOAD");
            NMEA::incrCRC(_sm._crc,c);
            break;
        } else {
            _sm._state = STOP_TOKEN;
        }
        // Fall through 
    case CHECK_CRC:
        if( _sm._state == CHECK_CRC ) { // did we not fall through
            if ( _crc_buf[0] == '\0' ) {
                // this is the first crc character
                _crc_buf[0] = c;
                break;
            }
            else {
                _crc_buf[1] = c;
                _crc_buf[2] = '\0';
                char read_crc = (char)strtol(_crc_buf, NULL, 16);
                ESP_LOGD(FNAME, "Msg CRC %s/%x - %x", _crc_buf, read_crc, _sm._crc);
                if ( read_crc != _sm._crc ) {
                    _sm._state = START_TOKEN;
                    break;
                }
                _sm._state = CHECK_OK; // fall through
            }
        }
        // Fall through 
    case STOP_TOKEN:
    case CHECK_OK:
    case COMPLETE:
        {
            _sm._state = START_TOKEN; // restart parsing
        }
        break;
    default:
        break;
    }
    return _sm._state;
}

gen_state_t TestQuery::parseTest()
{
    return START_TOKEN;
}


//
// Transmitter routine
//
bool TestQuery::sendTestQuery()
{
    ESP_LOGI(FNAME, "Send burst on S%d", _send_port );
    Message* msg = newMessage();
    msg->buffer = "$A_1234567890223456789032345678904234567890523456789062345678907234567890\r\n";
    DEV::Send(msg);

    msg = newMessage();
    msg->buffer = "$B_123456789022345678903234567890423456789052345678906234567890723456789\r\n";
    DEV::Send(msg);

    msg = newMessage();
    msg->buffer = "$C_12345678902234567890323456789042345678905234567890623456789072345678\r\n";
    DEV::Send(msg);

    msg = newMessage();
    msg->buffer = "$D_123456789022345678903234567890423456789052345678906234567890723456789\r\n";
    DEV::Send(msg);

    msg = newMessage();
    msg->buffer = "$E_12345678902234567890323456789042345678905234567890623456789072345678\r\n";
    DEV::Send(msg);

    msg = newMessage();
    msg->buffer = "$F_12345678902234567890323456789042345678905234567890623456789072345678\r\n";
    DEV::Send(msg);

    msg = newMessage();
    msg->buffer = "$G_123456789022345678903234567890423456789052345678906234567890723456789\r\n";
    DEV::Send(msg);

    msg = newMessage();
    msg->buffer = "$H_12345678902234567890323456789042345678905234567890623456789072345678\r\n";
    DEV::Send(msg);

    msg = newMessage();
    msg->buffer = "$I_12345678902234567890323456789042345678905234567890623456789072345678\r\n";
    DEV::Send(msg);

    msg = newMessage();
    msg->buffer = "$J_1234567890223456789032345678904234567890523456789062345678907234567\r\n";
    return DEV::Send(msg);
}

void TestQuery::tick()
{
    sendTestQuery();
}

