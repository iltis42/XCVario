#pragma once

#include <cinttypes>

namespace i2cbus {
    class I2C;
}

// digital poti interface
class Poti
{
public:
    explicit Poti(i2cbus::I2C *i2cbus, uint8_t addr) : bus(i2cbus), I2C_ADDR(addr) {};
    Poti() = delete;
    virtual ~Poti() {};
    bool begin();
    virtual bool reset() = 0;
    bool haveDevice();
    // virtual bool readVolume( float& val );
    bool writeVolume(float val); // 0..100 %
    virtual bool writeWiper(int val, bool validatet = false) = 0;

private:
    virtual int getRange() = 0;
    virtual bool readWiper(int &val) = 0;
    
protected:
	i2cbus::I2C *bus = 0;
    const uint8_t I2C_ADDR;
    int errorcount = 0;
};
