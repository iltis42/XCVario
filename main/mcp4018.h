#ifndef MCP4018_H
#define MCP4018_H

// #include "I2C.h"
#include "esp_system.h"
#include "I2Cbus.hpp"
#include "Poti.h"

constexpr uint8_t MPC4018_I2C_ADDR = 0x2f;     // 0101111

//Library for the MCP4018 7 bit digital potentiometer.

class MCP4018: public Poti
{
public:
  /*
  Creates instance
  Connect module using I2C port pins sda and scl.

  */
  MCP4018();

  bool begin();
  void setBus( I2C_t *theBus ) {  bus = theBus; };

  /*
  Destroys instance.
  */
  ~MCP4018();

  bool readVolume( float& val );
  bool writeVolume( float val );
  bool haveDevice();
  void setHalfScale() { _scale = 200; };
  bool writeWiper( int val, bool valiadate=false );

private:
  bool readWiper( int& val );
#define MCP4018RANGE 127
  inline int  getRange() { return MCP4018RANGE; };
  inline float getInvRange() { return (1.0/MCP4018RANGE); };
  inline int  getStep() { return 2; };
  I2C_t *bus;
  int  errorcount;
  bool  _noDevice;
  int wiper;  // only bit 0..7 supported
  int _scale;
};

#endif
