#pragma once

#include "Poti.h"

constexpr uint8_t CAT5171_I2C_ADDR = 0x2C; // 0b0101100

// CAT5171 7 bit digital potentiometer.

class CAT5171 : public Poti
{
public:
    // Connect module using I2C port
    CAT5171() = delete;
    explicit CAT5171(i2cbus::I2C *i2cbus);
    virtual ~CAT5171() {}

    bool reset() override;
    e_poti_type getType() const override { return POTI_CAT5171; }
    bool writeWiper(int val, bool validate = false) override;

private:
    bool readWiper(int &val) override;
    static constexpr int CAT5171RANGE = 80; // do not use the full range of 255
};
