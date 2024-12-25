#pragma once

#include "ProtocolItf.h"

class Anemoi final : public ProtocolItf
{
public:
    explicit Anemoi(int p, ProtocolState &sm) : ProtocolItf(p, sm) {};
    virtual ~Anemoi() {}

    DeviceId getDeviceId() override { return ANEMOI_DEV; } // The connected (!) device through protocol
    ProtocolType getProtocolId() override { return ANEMOI; }
    bool isBinary() const override { return true; }

public:
    virtual gen_state_t nextByte(const char c) override;

private:
    void parseWind();
    void parseStatus();
    int expected_len = 0;
};
