#pragma once

#include "ProtocolItf.h"

class Anemoi final : public ProtocolItf
{
public:
    explicit Anemoi(int p, ProtocolState &sm) : ProtocolItf(ANEMOI_DEV, p, sm) {};
    virtual ~Anemoi() {}

public:
    ProtocolType getProtocolId() override { return ANEMOI_P; }
    bool isBinary() const override { return true; }
    datalink_action_t nextByte(const char c) override;

private:
    void parseWind();
    void parseStatus();
    int expected_len = 0;
};
