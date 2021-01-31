/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************

File: Compass.h

Class to handle compass data and actions.

Author: Axel Pauli, January 2021

**************************************************************************/

#pragma once

#include "SetupMenuValFloat.h"
#include "SetupMenuSelect.h"
#include "SetupNG.h"
#include "QMC5883L.h"

class Compass : public QMC5883L
{
public:

  /*
    Creates instance for I2C connection with passing the desired parameters.
    No action is done at the bus. Note if i2cBus is not set in the constructor,
    you have to set it by calling method setBus(). The default address of the
    chip is 0x0D.
   */
  Compass( const uint8_t addr,
           const uint8_t odr,
           const uint8_t range,
           const uint8_t osr,
           I2C_t *i2cBus=nullptr );

  ~Compass();

  /** Returns the heading by considering deviation. If ok is passed, it is set
   * to true, if heading data is valid, otherwise it is set to false.*/
  float trueHeading( bool *ok=nullptr );

  // Compass Menu Action method
  int calibrateAction( SetupMenuSelect *p );

private:

  static SetupNG<float> *deviations[8];
};

