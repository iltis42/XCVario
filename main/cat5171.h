#ifndef CAT5171_H
#define CAT5171_H

#include "I2C.h"
#include "esp_system.h"
#include "I2Cbus.hpp"
#include "Poti.h"

#define  CAT5171_I2C_ADDR  0b0101100     // AD0 = 0  0x4C

//Library for the CAT5171 7 bit digital potentiometer.

class CAT5171: public Poti
{
public:
  /*
  Creates instance
  Connect module using I2C port pins sda and scl.

  */
  CAT5171();

  bool begin();
  void setBus( I2C_t *theBus ) {  bus = theBus; };

  /*
  Destroys instance.
  */
  ~CAT5171();

  bool readWiper( uint16_t& val );
  bool writeWiper( uint16_t val );
  bool reset();
  bool incWiper();
  bool decWiper();
  bool haveDevice();
  inline int  getRange() { return 255; };
  inline int  getStep() { return 3; };

private:
  I2C_t *bus;
  int  errorcount;
  bool  _noDevice;
  uint16_t wiper;  // only bit 0..7 supported
};

#endif
