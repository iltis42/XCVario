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

#include <cmath>
#include <logdef.h>
#include "QMC5883L.h"

/* Register numbers */
#define REG_X_LSB 0         // Output Data Registers for magnetic sensor.
#define REG_X_MSB 1
#define REG_Y_LSB 2
#define REG_Y_MSB 3
#define REG_Z_LSB 4
#define REG_Z_MSB 5
#define REG_STATUS 6        // Status Register.
#define REG_TEMP_LSB 7      // Output Data Registers for temperature.
#define REG_TEMP_MSB 8
#define REG_CONTROL1 9      // Control Register #1.
#define REG_CONTROL2 0xA    // Control Register #2.
#define REG_RST_PERIOD 0xB  // ET/RESET Period Register.
#define REG_RESERVED 0xC    // Reserved.
#define REG_CHIP_ID 0xD     // Chip ID register.

/* Flags for Status Register #1. */
#define STATUS_DRDY 1 // Data Ready.
#define STATUS_OVL 2  // Overflow flag.
#define STATUS_DOR 4  // Data skipped for reading.

/* Flags for Status Register #2. */
#define INT_ENB 0b00000001   // Interrupt Pin Enabling.
#define POL_PNT 0b01000000   // Pointer Roll-over.
#define SOFT_RST 0b10000000  // Soft Reset.

/* Flags for Control Register 1. */
#define MODE_STANDBY    0b00000000  // Standby mode.
#define MODE_CONTINUOUS 0b00000001  // Continuous read mode.

/*
  Creates instance for I2C connection with passing the desired parameters.
  No action is done at the bus. Note if i2cBus is not set in the constructor,
  you have to set it by calling method setBus(). The default address of the
  chip is 0x0D.
*/
QMC5883L::QMC5883L( const uint8_t addrIn,
                    const uint8_t odrIn,
                    const uint8_t rangeIn,
                    const uint8_t osrIn,
                    I2C_t *i2cBus ) :
  bus( i2cBus ),
  addr( addrIn ),
  odr( odrIn ),
  range( rangeIn ),
  osr( osrIn ),
  overflowWarning( false )
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
      ESP_LOGE( FNAME,
                "QMC5883L writeRegister( 0x%02X, 0x%02X, 0x%02X ) FAILED",
                addr, reg, value );
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
  esp_err_t err = bus->readBytes( addr, reg, count, data );

  if( err != ESP_OK )
    {
      ESP_LOGE( FNAME,
                "QMC5883L readRegister( 0x%02X, 0x%02X, %d ) FAILED",
                addr, reg, count );
      return 0;
    }

  return count;
}

/** Check, if the bus pointer is valid. */
bool QMC5883L::checkBus()
{
  if( bus == nullptr )
    {
      ESP_LOGE( FNAME, "QMC5883L bus pointer is zero" );
      return false;
    }

  return true;
}

// scan bus for I2C address
esp_err_t QMC5883L::selfTest()
{
  if( checkBus() == false )
    {
      return ESP_FAIL;
    }

  uint8_t chipId;

  // Try to read Register 0xD, it delivers the chip id 0xff for a QMC5883L
  esp_err_t err = bus->readByte( QMC5883L_ADDR, REG_CHIP_ID, &chipId );

  if( err != ESP_OK )
    {
      ESP_LOGE( FNAME,
                "QMC5883L self-test, scan for I2C address 0x%02X FAILED",
                QMC5883L_ADDR );
      return ESP_FAIL;
    }

  if( chipId != 0xff )
    {
      ESP_LOGE( FNAME,
                "QMC5883L self-test, chip ID 0x%02X is unsupported, expected 0xFF",
                chipId );
      return ESP_FAIL;

    }

  ESP_LOGI( FNAME,
            "QMC5883L selftest, scan for I2C address 0x%02X and chip ID 0x%02X PASSED",
            QMC5883L_ADDR, chipId );

  return ESP_OK;
}

/**
 * Configure the device with the set parameters and set the mode to continuous.
 * That means, the device starts working.
 */
esp_err_t QMC5883L::modeContinuous()
{
  esp_err_t e1, e2, e3, e4;
  e1 = e2 = e3 = e4 = 0;

  // Soft Reset
  e1 = writeRegister( addr, REG_CONTROL2, SOFT_RST );

  // Enable ROL_PTN, Pointer roll over function.
  e2 = writeRegister( addr, REG_CONTROL2, POL_PNT );

  // Define SET/RESET period. Should be set to 1
  e3 = writeRegister( addr, REG_RST_PERIOD, 1 );

  // Set mesaurement data and start it in dependency of mode bit.
  e4 = writeRegister( addr,
                      REG_CONTROL1,
                      osr | range | odr | MODE_CONTINUOUS );

  if( (e1 + e2 + e3 + e4) == 0 )
    {
      return ESP_OK;
    }

  return ESP_FAIL;
}

/**
 * Set the device in standby mode.
 */
esp_err_t QMC5883L::modeStandby()
{
  // Soft reset, device goes after that in the standby mode.
  return writeRegister( addr, REG_CONTROL2, SOFT_RST );
}

/**
 * Define SET/RESET period. Should be set to 1 after a reset.
 */
esp_err_t QMC5883L::setPeriodRegister()
{
  return writeRegister( addr, REG_RST_PERIOD, 0x01 );
}

/** Set oversampling rate OSR_64 ... OSR_512. */
void QMC5883L::setOverSampleRatio( const uint16_t osrIn )
{
  switch( osrIn )
  {
    case 512:
      osr = OSR_512;
      break;
    case 256:
      osr = OSR_256;
      break;
    case 128:
      osr = OSR_128;
      break;
    case 64:
      osr = OSR_64;
      break;
    default:
      ESP_LOGE( FNAME, "QMC5883L: Wrong Over Sample Ration value %d passed",
                osrIn );
      break;
  }
}

/** Set magnetic range for measurement RNG_2G, RNG_8G. */
void QMC5883L::setRange( const uint8_t rangeIn )
{
  switch( rangeIn )
  {
    case 2:
      range = RNG_2G;
      break;
    case 8:
      range = RNG_8G;
      break;
    default:
      ESP_LOGE( FNAME, "QMC5883L: Wrong Gauss Range value %d passed",
                rangeIn );
      break;
  }
}

/** Set ODR output data rate. */
void QMC5883L::setOutputDataRate( const uint8_t odrIn )
{
  switch( odrIn )
  {
    case 10:
      odr = ODR_10HZ;
      break;
    case 50:
      odr = ODR_50HZ;
      break;
    case 100:
      odr = ODR_100HZ;
      break;
    case 200:
      odr = ODR_200HZ;
      break;
    default:
      ESP_LOGE( FNAME, "QMC5883L: Wrong Output Data Rate value %d passed", odrIn );
      break;
  }
}

/**
 * Read status Register 1 (0x6) and return its content. If read has failed,
 * -1 is returned.
 */
int QMC5883L::readStatusFlags()
{
  uint8_t status;

  if( readRegister( addr, REG_STATUS, 1, &status ) != ESP_OK )
    {
      return -1;
    }

  return status;
}

/**
 * Read out the registers X, Y, Z (0...5) in raw format.
 *
 * The status register bits are checked, if the heading data available, if no
 * overflow has occurred and if no data locking is shown.
 *
 * Returns true in case of success otherwise false.
 */
bool QMC5883L::readRawHeading( int16_t *x, int16_t *y, int16_t *z )
{
// Check, if data are available
uint8_t data[6];
uint8_t status = 0;

// Read status register
uint8_t err = readRegister( addr, REG_STATUS, 1, &status );

if( err != ESP_OK )
  {
    return false;
  }

if( ( status & STATUS_OVL ) == true &&
    range == RNG_2G && overflowWarning == false )
  {
    // Overflow has occurred, give out a warning only once
    overflowWarning = true;
    ESP_LOGE( FNAME, "QMC5883L: readRawHeading detected a gauss overflow." );
    return false;;
  }

if( ( status & STATUS_DOR ) == true )
  {
    // Previous measure was read partially, sensor in Data Lock.
    // Read all data again to overcome lock.
    readRegister( addr, REG_X_LSB, 6, data );
    return false;;
  }

if( ( status & STATUS_DRDY ) == true )
  {
    // Data ready for reading
    if( readRegister( addr, REG_X_LSB, 6, data ) > 0 )
      {
        *x = ( data[1] << 8 ) | data[0];
        *y = ( data[3] << 8 ) | data[2];
        *z = ( data[5] << 8 ) | data[4];
        return true;
      }
   }

  return false;
}

/**
 * Read temperature in degree Celsius. If ok is passed, it is set to true,
 * if temperature data is valid, otherwise it is set to false.
 */
int16_t QMC5883L::readTemperature( bool *ok )
{
  uint8_t data[2];

  if( readRegister( addr, REG_TEMP_LSB, 2, data ) == 0 )
    {
      if( ok != nullptr )
        {
          *ok = false;
        }

      // Nothing has been read
      return 0.0;
    }

  int16_t t = ( data[1] << 8 ) | data[0];

  if( ok != nullptr )
    {
      *ok = true;
    }

  return t;
}

void QMC5883L::resetCalibration()
{
  xhigh = yhigh = 0;
  xlow = ylow = 0;
}

/**
 * Reads the heading in degrees of 1...360. If ok is passed, it is set to true,
 * if heading data is valid, otherwise it is set to false.
 */
float QMC5883L::readHeading( bool *ok )
{
  int16_t x, y, z;

  if( ok != nullptr )
    {
      *ok = false;
    }

  if( readRawHeading( &x, &y, &z ) == false )
    {
      return 0.0;
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
      return 0.0;
    }

  /* Recenter the measurement by subtracting the average */
  x -= (xhigh + xlow) / 2;
  y -= (yhigh + ylow) / 2;

  /* Rescale the measurement to the range observed. */
  float fx = static_cast<float>( x / (xhigh - xlow) );
  float fy = static_cast<float>( y / (yhigh - ylow) );

  float heading = 180.0 * ( atan2( fy, fx ) / M_PI );

  if( heading <= 0.0 )
    {
      heading += 360.0;
    }

  if( ok != nullptr )
    {
      *ok = true;
    }
  
  return heading;
}
