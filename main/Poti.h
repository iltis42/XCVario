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
  virtual bool readWiper( uint16_t& val ) = 0;
  virtual bool writeWiper( uint16_t val ) = 0;
  virtual bool incWiper() = 0;
  virtual bool decWiper() = 0;
  virtual bool haveDevice() = 0;
  virtual int  getRange() = 0;
  virtual int  getStep() = 0;
};

#endif
