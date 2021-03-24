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
  CompassMenu( Compass& compassIn);

  ~CompassMenu();

  /** Compass Menu Action method to set deviations. */
  int deviationAction( SetupMenuSelect *p );

  /** Compass Menu Action method to reset all deviations to 0. */
  int resetDeviationAction( SetupMenuSelect *p );

  /** Compass Menu Action method to set declination valid. */
  int declinationAction( SetupMenuValFloat *p );

  /** Method for receiving intermediate calibration results. */
  static bool calibrationReport( float xscale, float yscale, float zscale, float xbias, float ybias, float zbias );

  /** Compass Menu Action method to calibrate sensor. */
  int sensorCalibrationAction( SetupMenuSelect *p );

  static SetupNG<float> *deviations[8];

  private:

  // active compass instance
  Compass& compass;

  // Compass filter
  CompassFilter filter;

  // Pointer to setup menu of sensorCalibrationAction
  static SetupMenuSelect* menuPtr;
};
