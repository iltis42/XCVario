/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
****************************************************************************

I2C driver for the chip QMC5883L, 3-Axis Magnetic Sensor.

QMC5883L data sheet:

https://datasheetspdf.com/pdf-file/1309218/QST/QMC5883L/1

Author: Axel Pauli, January 2021

****************************************************************************/

#ifndef QMC5883L_H
#define QMC5883L_H

#include "esp_system.h"
#include "I2Cbus.hpp"

/* The default I2C address of this chip */
#define QMC5883L_ADDR 0x0D

/* Flags for Control Register 1. */
#define ODR_10HZ   0b00000000 // Output Data Rate in Hz.
#define ODR_50HZ   0b00000100
#define ODR_100HZ  0b00001000
#define ODR_200HZ  0b00001100

#define RNG_2G 0b00000000  // Range 2 Gauss: for magnetic-clean environments.
#define RNG_8G 0b00010000  // Range 8 Gauss: for strong magnetic fields.

#define OSR_512 0b00000000  // Over Sample Rate 512: less noise, more power.
#define OSR_256 0b01000000
#define OSR_128 0b10000000
#define OSR_64  0b11000000  // Over Sample Rate 64: more noise, less power.

class QMC5883L
{
public:
  /*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. Note if i2cBus is not set in the constructor,
    you have to set it by calling method setBus(). The default address of the
    chip is 0x0D.
  */
  QMC5883L( const uint8_t addr,
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
   * Define SET/RESET period. Should be set to 1 after a reset.
   */
  esp_err_t setPeriodRegister();

  /**
   * Read status Register 1 (0x6) and return its content. If read has failed,
   * -1 is returned.
   */
  int readStatusFlags();

  /**
   *  Set the device in standby mode.
   */
  esp_err_t modeStandby();

  /**
   * Configure the device with the set parameters and set the mode to continuous.
   * That means, the device starts working.
   */
  esp_err_t modeContinuous();
  
  /**
   * Reads the heading in degrees of 1...360. If heading is not valid a value of
   * -1 is returned.
   */
  int readHeading();

  /**
   * Read temperature in degree Celsius. If the measurement is invalid,
   * a value of -999 is returned.
   */
  int16_t readTemperature();

  /**
   * Read out the registers X, Y, Z (0...5) in raw format.
   * Returns true in case of success otherwise false.
   */
  bool readRawHeading( int16_t *x, int16_t *y, int16_t *z );

  void resetCalibration();
  void setSamplingRate( const uint8_t rate );
  void setRange( const uint8_t range );
  void setOversampling( const uint16_t ovl );
  
  /** Sets the declination. Declination must be >= -180 and <= 180. */
  esp_err_t setDeclination( const int16_t declinationIn );

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
  int16_t xhigh, xlow;
  int16_t yhigh, ylow;
  uint8_t addr;
  uint8_t rate;
  uint8_t range;
  uint8_t oversampling;
  int16_t declination;
  bool overflowWarning;
};

#endif
