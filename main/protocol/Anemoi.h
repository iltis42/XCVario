#pragma once

#include "ProtocolItf.h"

class Anemoi final : public ProtocolItf
{
public:
    explicit Anemoi(int p) : ProtocolItf(p) { _binary = true; }
    virtual ~Anemoi() {}

    virtual DeviceId getDeviceId() { return ANEMOI_DEV; } // The connected (!) device through protocol
    virtual ProtocolType getProtocolId() { return ANEMOI; }

public:
    virtual gen_state_t nextByte(const char c) override;

private:
    void incrCRC(const char c) override;
    void parseWind();
    void parseStatus();
};
