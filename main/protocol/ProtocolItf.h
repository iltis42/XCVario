/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "comm/Devices.h"
#include <string>

// Generic protocol state machine with special bit mask
const int CRC_BIT   = 0x0100;
const int GOTIT_BIT = 0x0200;
typedef enum
{
    START_TOKEN = 0,
    HEADER = CRC_BIT | 1,
    PAYLOAD = GOTIT_BIT | CRC_BIT | 2,
    STOP_TOKEN = GOTIT_BIT | 3,
    CHECK_CRC = GOTIT_BIT | 4,
    CHECK_OK = GOTIT_BIT | 5,
    COMPLETE = GOTIT_BIT | 6,
    GO_BINARY = 7
} gen_state_t;


class ProtocolState;

// Protocol parser interface
class ProtocolItf
{
public:
    ProtocolItf(DeviceId id, int sp, ProtocolState &sm) : _did(id), _send_port(sp), _sm(sm) {};
    virtual ~ProtocolItf() {}

    static constexpr int MAX_LEN = 128;

public:
    // API
    DeviceId getDeviceId() { return _did; } // The connected (!) device through protocol
    virtual ProtocolType getProtocolId() { return NO_ONE; }
    virtual gen_state_t nextByte(const char c) = 0; // return true if message is recognized and able to parse payload
    virtual gen_state_t nextStreamChunk(const char *cptr, int count) { return START_TOKEN; } // for binary protocols
    inline Message* newMessage() { return DEV::acqMessage(_did, _send_port); }
    // gen_state_t getState() const { return _state; }
    virtual bool isBinary() const { return false; }
    int getSendPort() const { return _send_port; }

protected:
    // routing
    const DeviceId _did;
    const int _send_port;

    // state machine
    ProtocolState &_sm;

    // small crc character buffer
    char _crc_buf[3];
};

// Protocol parser state
class ProtocolState
{
public:
    ProtocolState() = default;
    void reset()
    {
        _frame.clear();
        _state = START_TOKEN;
        _crc = 0;
    }
    inline bool push(char c)
    {
        if ( _state )
        {
            if ( _frame.size() < ProtocolItf::MAX_LEN ) {
                _frame.push_back(c);
                return true;
            }
            else {
                return false;
            }
        }
        _frame.assign(1, c);
        return true;
    }

    // frame buffer and state machine vars
    std::string _frame;
    gen_state_t _state = START_TOKEN;
    int _crc = 0; // checksum (binary)
};
