#pragma once

namespace i2cbus {
    class I2C;
}

// digital poti interface
class Poti
{
public:
  virtual ~Poti() {};
  virtual bool begin() = 0;
  virtual void setBus( i2cbus::I2C *theBus ) = 0;
  virtual bool readVolume( float& val ) = 0;
  virtual bool writeVolume( float val ) = 0;
  virtual bool haveDevice() = 0;
  virtual void setHalfScale() = 0;
  virtual bool writeWiper( int val, bool validatet=false ) = 0;
private:
  virtual int  getRange() = 0;
  virtual float getInvRange() = 0;
  virtual bool readWiper( int& val ) = 0;
  virtual int  getStep() = 0;
};
