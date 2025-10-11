#pragma once

#include "Poti.h"

constexpr uint8_t MPC4018_I2C_ADDR = 0x2f; // 0101111

// MCP4018 7 bit digital potentiometer.

class MCP4018 : public Poti
{
public:
    // Connect module using I2C port
    MCP4018() = delete;
    explicit MCP4018(i2cbus::I2C *i2cbus);
    virtual ~MCP4018() {}

    bool reset() override { return true; }; // no reset command
    e_poti_type getType() const override { return POTI_MCP4018; }
    bool writeWiper(int val, bool validate = false) override;

private:
    bool readWiper(int &val) override;
    static constexpr int MCP4018RANGE = 40; // do not use the full range of 127
};

