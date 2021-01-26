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

#include <cmath>
#include "QMC5883L.h"
#include <logdef.h>

/*
 * QMC5883L data sheet
 * http://wiki.epalsite.com/images/7/72/QMC5883L-Datasheet-1.0.pdf
 */

/* The default I2C address of this chip */
#define QMC5883L_ADDR 0x0D

/* Register numbers */
#define QMC5883L_X_LSB 0
#define QMC5883L_X_MSB 1
#define QMC5883L_Y_LSB 2
#define QMC5883L_Y_MSB 3
#define QMC5883L_Z_LSB 4
#define QMC5883L_Z_MSB 5
#define QMC5883L_STATUS 6
#define QMC5883L_TEMP_LSB 7
#define QMC5883L_TEMP_MSB 8
#define QMC5883L_CONTROL1 9
#define QMC5883L_CONTROL2 0xA
#define QMC5883L_RESET_PERIOD 0xB
#define QMC5883L_RESERVED 0xC
#define QMC5883L_CHIP_ID 0xD

/* Bit values for the STATUS register */
#define QMC5883L_STATUS_DRDY 1
#define QMC5883L_STATUS_OVL 2
#define QMC5883L_STATUS_DOR 4

/* Oversampling values for the CONFIG register */
#define QMC5883L_CONFIG_OS512 0b00000000
#define QMC5883L_CONFIG_OS256 0b01000000
#define QMC5883L_CONFIG_OS128 0b10000000
#define QMC5883L_CONFIG_OS64  0b11000000

/* Range values for the CONFIG register */
#define QMC5883L_CONFIG_2GAUSS 0b00000000
#define QMC5883L_CONFIG_8GAUSS 0b00010000

/* Rate values for the CONFIG register */
#define QMC5883L_CONFIG_10HZ   0b00000000
#define QMC5883L_CONFIG_50HZ   0b00000100
#define QMC5883L_CONFIG_100HZ  0b00001000
#define QMC5883L_CONFIG_200HZ  0b00001100

/* Mode values for the CONFIG register */
#define QMC5883L_CONFIG_STANDBY    0b00000000
#define QMC5883L_CONFIG_CONTINUOUS 0b00000001

/* Apparently M_PI isn't available in all environments. */
#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif


QMC5883L::QMC5883L( const uint8_t addrIn,
                    const uint8_t modeIn,
                    const uint8_t rateIn,
                    const uint8_t rangeIn,
                    const uint8_t oversamplingIn,
                    2C_t *i2cBus ) :
  bus( i2cBus ),
  addr( addrIn ),
  mode( modeIn ),
  rate( rateIn ),
  range( rangeIn ),
  oversampling( oversamplingIn )
{
  if( addrIn == 0 )
    {
      // set address to default value of chip, if it is zero.
      addr = QMC5883L_ADDR;
    }
}

QMC5883L::~QMC5883L()
{
}

/** Check, if the bus pointer is valid. */
bool QMC5883L::checkBus()
{
  if( bus == nullptr )
    {
      ESP_LOGI( FNAME, "QMC5883L bus pointer is zero" );
      return false;
    }

  return true;
}

// scan bus for I2C address
esp_err_t QMC5883L::selfTest()
{
  if( checkBus() == false )
    {
      eturn ESP_FAIL;
    }

  uint8_t data[1];

  // Try to read Register 06H, it can deliver one byte.
  esp_err_t err = bus->readByte( QMC5883L_ADDR, QMC5883L_STATUS, data );

  if( err != ESP_OK )
    {
      ESP_LOGI( FNAME, "QMC5883L self-test, scan for I2C address %02x FAILED",
      QMC5883L_ADDR );

      return ESP_FAIL;
    }

  ESP_LOGI( FNAME, "QMC5883L selftest, scan for I2C address %02x PASSED",
            QMC5883L_ADDR );

  return ESP_OK;
}

/** Write with data part. */
esp_err_t QMC5883L::writeRegister( const uint8_t addr,
                                   const uint8_t reg,
                                   const uint8_t value )
{
  if( checkBus() == false )
    {
      return ESP_FAIL;
    }

  esp_err_t err = bus->writeByte( addr, reg, value );

  if( err != ESP_OK )
    {
      ESP_LOGI( FNAME, "QMC5883L writeRegister( %02X, %02X, %02X ) FAILED",
                adr, reg, value );
      return ESP_FAIL;
    }

  return err;
}

/**
 * Read bytes from the chip.
 * Return the number of read bytes or 0 in error case.
 */
uint8_t QMC5883L::readRegister( const uint8_t addr,
                                const uint8_t reg,
                                const uint8_t count,
                                uint8_t *data  )
{
  if( checkBus() == false )
    {
      return 0;
    }

  // read bytes from chip
  esp_err_t err = readBytes( addr, reg, count, data );

  if( err != ESP_OK )
    {
      ESP_LOGI( FNAME, "QMC5883L readRegister( %02X, %02x, %d ) FAILED",
                adr, reg, count );
      return 0;
    }

  return count;
}

/**
 * Configure device. In dependency of the mode bit, the device can start or
 * stop running.
 */
esp_err_t QMC5883L::configureDevice()
{
  esp_err_t e1, e2, e3;
  e1 = e2 = e3 = 0;

  // Enable ROL_PTN, Pointer roll over function.
  e1 = writeRegister( addr, QMC5883L_CONTROL2, 0x40 );

  // Set mesaurement data and start it in dependency of mode bit.
  e2 = writeRegister( addr, QMC5883L_CONTROL1, oversampling | range | rate | mode );
  e3 = resetPeriodRegister();

  if( (e1 + e2 + e3) == 0 )
    {
      return ESP_OK;
    }

  return ESP_FAIL;
}

/**
 * After excecuting the soft reset command, the chip goes into the standby mode.
 */
esp_err_t QMC5883L::softReset()
{
  return writeRegister( addr, QMC5883L_CONTROL2, 0x80 );
}

/**
 * After excecuting the reset command, the chip goes into the standby mode.
 */
esp_err_t QMC5883L::resetPeriodRegister()
{
  return writeRegister( addr, QMC5883L_RESET_PERIOD, 0x01 );
}

void QMC5883L::setOversampling( const uint8_t x )
{
  switch(x)
  {
    case 512:
      oversampling = QMC5883L_CONFIG_OS512;
      break;
    case 256:
      oversampling = QMC5883L_CONFIG_OS256;
      break;
    case 128:
      oversampling = QMC5883L_CONFIG_OS128;
      break;
    case 64:
      oversampling = QMC5883L_CONFIG_OS64;
      break;
  }
}

void QMC5883L::setRange( const uint8_t x )
{
  switch(x)
  {
    case 2:
      range = QMC5883L_CONFIG_2GAUSS;
      break;
    case 8:
      range = QMC5883L_CONFIG_8GAUSS;
      break;
  }
}

void QMC5883L::setSamplingRate( const uint8_t x )
{
  switch(x)
  {
    case 10:
      rate = QMC5883L_CONFIG_10HZ;
      break;
    case 50:
      rate = QMC5883L_CONFIG_50HZ;
      break;
    case 100:
      rate = QMC5883L_CONFIG_100HZ;
      break;
    case 200:
      rate = QMC5883L_CONFIG_200HZ;
      break;
  }
}

void setMode( const uint8_t mode )
{
  switch(x)
  {
    case QMC5883L_CONFIG_STANDBY:
      mode = QMC5883L_CONFIG_STANDBY;
      break;
    case QMC5883L_CONFIG_CONTINUOUS:
      rate = QMC5883L_CONFIG_CONTINUOUS;
      break;
  }
}

/**
 * Read the Data Ready Register bit from status register 06H and return it.
 */
int QMC5883L::ready()
{
  uint8_t status;

  if( readRegister( addr, QMC5883L_STATUS, 1, &status ) != ESP_OK )
    {
      return 0;
    }

  return status & QMC5883L_STATUS_DRDY; 
}

/**
 * Read out the registers X, Y, Z (0...5) in raw format.
 * Returns true in case of success otherwise false.
 */
bool QMC5883L::readRawHeading( int16_t *x, int16_t *y, int16_t *z )
{
  if( ready() == 0 )
    {
      return false;
    }

  uint8_t data[6];

  if( readRegister( addr, QMC5883L_X_LSB, 6, data ) == 0 )
    {
      // Nothing has been read
      return 0;
    }

  *x = ( data[1] << 8 ) | data[0];
  *y = ( data[3] << 8 ) | data[2];
  *z = ( data[5] << 8 ) | data[4];

  return true;
}

/**
 * Read out the registers TOUT (7...8) in raw format.
 * Returns true in case of success otherwise false.
 */
bool QMC5883L::readRawTemperature( int16_t *t )
{
  if( ready() == 0 )
    {
      return false;
    }

  uint8_t data[2];

  if( readRegister( addr, QMC5883L_TEMP_LSB, 2, data ) == 0 )
    {
      // Nothing has been read
      return 0;
    }

  *t = ( data[1] << 8 ) | data[0];

  return true;
}

void QMC5883L::resetCalibration()
{
  xhigh = yhigh = 0;
  xlow = ylow = 0;
}

/**
 * Reads the heading in degrees of 1...360. If heading is not valid a value of
 * -1 is returned.
 */
int QMC5883L::readHeading()
{
  int16_t x, y, z;

  if( readRaw( &x, &y, &z ) == false )
    {
      return -1;
    }

  /* Update the observed boundaries of the measurements */

  if( x < xlow )
    xlow = x;
  if( x > xhigh )
    xhigh = x;
  if( y < ylow )
    ylow = y;
  if( y > yhigh )
    yhigh = y;

  /* Bail out if not enough data is available. */
  if( xlow == xhigh || ylow == yhigh )
    {
      return -1;
    }

  /* Recenter the measurement by subtracting the average */

  x -= (xhigh + xlow) / 2;
  y -= (yhigh + ylow) / 2;

  /* Rescale the measurement to the range observed. */
  
  float fx = (float) x / (xhigh - xlow);
  float fy = (float) y / (yhigh - ylow);

  int heading = static_cast<int> ( rint( 180.0 * atan2( fy, fx ) / M_PI ) );

  if( heading <= 0 )
    {
      heading += 360;
    }
  
  return heading;
}

/**
 * Read temperature in degree Celsius. If the measurement is invalid,
 * a value of -999 is returned.
 */
int QMC5883L::readTemperature()
{
  int16_t t;

  if( readRaw( &t ) == false )
    {
      return -999;
    }

    return t;
}
