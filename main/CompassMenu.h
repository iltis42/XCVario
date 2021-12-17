/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************

File: CompassMenu.h

Class to handle compass data and actions.

Author: Axel Pauli, February 2021

Last update: 2021-02-24

**************************************************************************/

#pragma once

#include "Compass.h"
#include "SetupMenuValFloat.h"
#include "SetupMenuSelect.h"
#include "SetupNG.h"

class CompassMenu
{
  public:

  /**
   * Creates a compass menu instance with an active compass object.
   */
  CompassMenu();

  ~CompassMenu();

  /** Compass Menu Action method to set deviations. */
  static int deviationAction( SetupMenuSelect *p );

  /** Compass Menu Action method to reset all deviations to 0. */
  static int resetDeviationAction( SetupMenuSelect *p );

  /** Compass Menu Action method to set declination valid. */
  static int declinationAction( SetupMenuValFloat *p );

  /** Method for receiving intermediate calibration results. */
  static bool calibrationReport( t_magn_axes raw, t_float_axes scale, t_float_axes bias, bitfield_compass b, bool print );

  /** Compass Menu Action method to calibrate sensor. */
  static int sensorCalibrationAction( SetupMenuSelect *p );

  static SetupNG<float> *deviations[8];

  // Backup for calibration data
  private:
  static t_magn_axes  raw_back;
  static t_float_axes scale_back;
  static t_float_axes bias_back;

  // Pointer to setup menu of sensorCalibrationAction
  static SetupMenuSelect* menuPtr;
};
