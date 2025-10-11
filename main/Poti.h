#pragma once

#include <protocol/ClockIntf.h>

#include <cinttypes>

namespace i2cbus {
    class I2C;
}

enum e_poti_type
{
    POTI_NONE = 0,
    POTI_CAT5171,
    POTI_MCP4018
};

// digital poti interface
class Poti : public Clock_I
{
public:
    Poti() = delete;
    explicit Poti(i2cbus::I2C *i2cbus, uint8_t addr);
    virtual ~Poti() {};
    bool begin();
    virtual bool reset() = 0;
    bool haveDevice();
    virtual e_poti_type getType() const = 0;
    bool tick() override;

    void softSetVolume(float val);
    // virtual bool readVolume( float& val );
    bool writeVolume(float val); // 0..100 %
    virtual bool writeWiper(int val, bool validatet = false) = 0;

private:
    virtual bool readWiper(int &val) = 0;
    
protected:
    static constexpr int calcDbFromVolume(float val);
	i2cbus::I2C *bus = 0;
    const uint8_t I2C_ADDR;
    int errorcount = 0;
    int lastWiperValue = 0;
    int targetWiperValue = 0;
    static int RANGE;
};
