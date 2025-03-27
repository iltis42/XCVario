#pragma once

#include "ProtocolItf.h"

class Anemoi final : public ProtocolItf
{
public:
    explicit Anemoi(int p, ProtocolState &sm, DataLink &dl) : ProtocolItf(ANEMOI_DEV, p, sm, dl) {};
    virtual ~Anemoi() {}

public:
    ProtocolType getProtocolId() const override { return ANEMOI_P; }
    bool isBinary() const override { return true; }
    dl_control_t nextBytes(const char* c, int len) override;

private:
    void parseWind();
    void parseStatus();
    int expected_len = 0;
};
