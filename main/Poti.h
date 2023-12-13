#ifndef POTI_H
#define POTI_H

#include "I2Cbus.hpp"

// Template for digital Poti Interface

class Poti
{
public:
  virtual ~Poti() {};
  virtual bool begin() = 0;
  virtual void setBus( I2C_t *theBus ) = 0;
  virtual bool readVolume( float& val ) = 0;
  virtual bool writeVolume( float val ) = 0;
  virtual bool haveDevice() = 0;
private:
  virtual int  getRange() = 0;
  virtual float getInvRange() = 0;
  virtual bool readWiper( int& val ) = 0;
  virtual bool writeWiper( int val ) = 0;
  virtual int  getStep() = 0;
};

#endif
