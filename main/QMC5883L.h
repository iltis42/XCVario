/**
MIT License

Copyright (c) 2020 Douglas Thain

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef QMC5883L_H
#define QMC5883L_H

#include "esp_system.h"
#include "I2Cbus.hpp"

class QMC5883L
{
public:
  /*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. Note if i2cBus is not set in the constructor,
    you have to set it by calling method setBus().
  */
  QMC5883L( const uint8_t addr,
            const uint8_t mode,
            const uint8_t rate,
            const uint8_t range,
            const uint8_t oversampling,
            I2C_t *i2cBus=nullptr );

  ~QMC5883L();

  /** Set the I2C bus used for the connection. */
  void setBus( I2C_t *theBus ) { bus = theBus; }

  /** Check for reply with I2C bus address */
  esp_err_t selfTest();

  /**
   * After excecuting the soft reset command, the chip goes into the standby mode.
   */
  uint8_t softReset();

  /**
   * This call resets the period register.
   */
  uint8_t resetPeriodRegister();

  /**
   * Read the Data Ready Register bit from status register 06H and return it.
   */
  int ready();

  /**
   * Configure device. In dependency of the mode bit, the device can start or
   * stop running.
   */
  uint8_t configureDevice();
  
  /**
   * Reads the heading in degrees of 1...360. If heading is not valid a value of
   * -1 is returned.
   */
  int readHeading();

  /**
   * Read temperature in degree Celsius. If the measurement is invalid,
   * a value of -999 is returned.
   */
  int readTemperature();

  /**
   * Read out the registers X, Y, Z (0...5) in raw format.
   * Returns true in case of success otherwise false.
   */
  bool readRawHeading( int16_t *x, int16_t *y, int16_t *z, int16_t *t );

  /**
   * Read out the registers TOUT (7...8) in raw format.
   * Returns true in case of success otherwise false.
   */
  bool readRawTemperature( int16_t *t );

  void resetCalibration();
  void setSamplingRate( const uint8_t rate );
  void setRange( const uint8_t range );
  void setOversampling( const uint8_t ovl );
  
  void setMode( const uint8_t mode );

  /** Write with data part. */
  esp_err_t writeRegister( const uint8_t addr,
                           const uint8_t reg,
                           const uint8_t value );

  /**
   * Read bytes from the chip.
   * Return the number of read bytes or 0 in error case.
   */
  uint8_t readRegister( const uint8_t addr,
                        const uint8_t reg,
                        const uint8_t count,
                        uint8_t *data );

private:

  /** Check, if the bus pointer is valid. */
  bool checkBus();

  I2C_t *bus;

  // Variables from original source
  int16_t xhigh, xlow;
  int16_t yhigh, ylow;
  uint8_t addr;
  uint8_t mode;
  uint8_t rate;
  uint8_t range;
  uint8_t oversampling;
};

#endif
