#ifndef CAT5171_H
#define CAT5171_H

// #include "I2C.h"
#include "esp_system.h"
#include "I2Cbus.hpp"
#include "Poti.h"

#define  CAT5171_I2C_ADDR  0b0101100     // AD0 = 0  0x2C

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

  bool readVolume( float& val );
  bool writeVolume( float val );
  bool reset();
  bool haveDevice();
  void setHalfScale() { _scale = 200; };
  bool writeWiper( int val, bool validate=false );

private:
  bool readWiper( int& val );
#define CAT5171RANGE 255
  inline int  getRange() { return CAT5171RANGE; };
  inline float getInvRange() { return (1.0/CAT5171RANGE); };
  inline int  getStep() { return 3; };
  I2C_t *bus;
  int  errorcount;
  bool  _noDevice;
  int wiper;  // only bit 0..7 supported
  int _scale;
};

#endif
