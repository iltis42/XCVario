/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "comm/Devices.h"


// Generic protocol state machine
typedef enum
{
    START_TOKEN,
    HEADER,
    PAYLOAD,
    STOP_TOKEN,
    COMPLETE,
    CHECK_CRC,
    CHECK_OK,
    CHECK_FAILED,
    GO_BINARY,
    RESTART
} gen_state_t;

class Message;

// Protocol parser interface
class ProtocolItf
{
public:
    ProtocolItf(int sp) : _send_port(sp) {};
    virtual ~ProtocolItf() {}

    static constexpr int MAX_LEN = 128;

public:
    // API
    virtual DeviceId getDeviceId() { return NO_DEVICE; } // The connected (!) device through protocol
    virtual ProtocolType getProtocolId() { return NO_ONE; }
    virtual gen_state_t nextByte(const char c) = 0;
    virtual gen_state_t nextStreamChunk(const char *cptr, int count) { return START_TOKEN; }
    inline Message* newMessage() { return DEV::acqMessage(getDeviceId(), _send_port); }
    gen_state_t getState() const { return _state; }
    const char *getBuffer() { return _framebuffer; }
    int getLength() const { return _pos; }
    bool isPassThrough() const { return _pass_through; }
    bool isBinary() const { return _binary; }
    int getSendPort() const { return _send_port; }
    virtual void timeOut() // called when for some time no input message
    {
        _state = RESTART;
        reset();
    }

protected:
    void reset()
    {
        _pos = 0;
        _len = 0;
        _crc = 0;
        *_crc_buf = '\0';
    }
    bool push_and_crc(char c)
    {
        if (_pos < MAX_LEN)
        {
            _framebuffer[_pos++] = c;
            incrCRC(c);
            return true;
        }
        return false;
    }
    bool push(char c)
    {
        if (_pos < MAX_LEN)
        {
            _framebuffer[_pos++] = c;
            return true;
        }
        return false;
    }
    virtual void incrCRC(const char c) { _crc ^= c; }

    // Attributes of the protocol to handle through data link
    bool _pass_through = false;
    bool _binary = false;
    // frame buffer and state machine vars
    char _framebuffer[MAX_LEN];
    gen_state_t _state = START_TOKEN;
    int _pos = 0;
    int _len = 0;
    char _crc = 0;    // checksum (binary)
    char _crc_buf[3]; // small crc character buffer

    // routing
    const int _send_port;
};
