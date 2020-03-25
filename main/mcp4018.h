#include "I2C.h"
#include "esp_system.h"

#ifndef MCP4018_H
#define MCP4018_H

#define  MPC4018_I2C_ADDR 0x2f     // 0101111

//Library for the MCP4018 7 bit digital potentiometer.


class MCP4018 : public I2C
{
public:
  /*
  Creates instance
  Connect module using I2C port pins sda and scl.

  */
  MCP4018();

  bool begin(gpio_num_t sda, gpio_num_t scl);

  /*
  Destroys instance.
  */
  ~MCP4018();


  bool readWiper( uint16_t& val );
  bool writeWiper( uint16_t val );
  bool incWiper();
  bool decWiper();
  bool haveDevice();

private:
  // I2C i2c;

  int  errorcount;
  bool  _noDevice;
  uint16_t wiper;  // only bit 0..7 supported
};

#endif

