#include "I2C.h"
#include "esp_system.h"
#include "I2Cbus.hpp"

#ifndef MCP3221_H
#define MCP3221_H

#define  MCP3221_CONVERSE 0x4d //10011010 NOTE IF IT ENDS IN 1, this is the READ ADDRESS. This is all this device does.
                                 //It opens a conversation via this specific READ address

//Library for the MCP3221 12 BIT ADC.
/*
      MCP3221  Top View

      +  -o   o- SCL
      -  -o
      S  -o   o- SDA



 */

class MCP3221
{
public:
  /*
  Creates instance
  Connect module using I2C port pins sda and scl. The output is referenced to the supply voltage which can be
  2.7v to 5.0v. The read will return the correct voltage, if you supply the correct supplyVoltage when instantiating.
  */
  MCP3221();

  bool begin();
  void setBus( I2C_t *theBus ) {  bus = theBus; };

  /*
  Destroys instance.
  */
  ~MCP3221();

  // raw read value of airspeed sensor
  int readVal();

  // check for reply with I2C bus address
  esp_err_t selfTest();

  /*
  Reads the analog register of the MCP3221 and converts it to a useable value. (a voltage)
  */
  esp_err_t readRaw(uint16_t &val);

  // alpha = 1.0  means no filter
  //         0.1  means 10 samples until full value

  float readAVG( float alpha );
  inline bool haveDevice() {
	  uint16_t val;
	  if( readRaw( val ) == ESP_OK )
	     return true;
	  else
		 return false;
  }

private:
  // I2C i2c;
  I2C_t *bus;
  char _data[2];
  int  errorcount;
  float exponential_average;
  bool  _noDevice;
};

#endif

extern MCP3221 *MCP;
