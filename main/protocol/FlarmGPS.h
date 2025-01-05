/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

#include <string>
#include <vector>

class FlarmGPS final : public ProtocolItf
{
public:
    FlarmGPS(int mp, ProtocolState &sm) : ProtocolItf(FLARM_DEV, mp, sm) {}
    virtual ~FlarmGPS() = default;
    ProtocolType getProtocolId() override { return FLARM_P; }

public:
    datalink_action_t nextByte(const char c) override;
    datalink_action_t nextStreamChunk(const char *cptr, int count) override;

    // Transmitted mesages

private:
    // Received messages
    void parseGPRMC();
    void parseGPGGA();
    void parsePGRMZ();
    void parsePFLAA();
    void parsePFLAE();
    void parsePFLAU();
    bool parsePFLAX();
    // parse helpers
    int _header_len;
    std::vector<int> _word_start;
    // binary mode peer
    ProtocolItf *_binpeer;
};
