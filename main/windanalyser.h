/***********************************************************************
 **
 **   windanalyser.h
 **
 **   This file is part of Cumulus.
 **
 ************************************************************************
 **
 **   Copyright (c):  2002       by André Somers,
 **	                  2002-2006  Eckhard Völlm
 **                   2006-      by Axel Pauli
 **
 **
 **   This file is distributed under the terms of the General Public
 **   License. See the file COPYING for more information.
 **
 **   $Id$
 **
 ***********************************************************************/

/**
 * \class WindAnalyser
 *
 * \author André Somers, Eckhard Völlm, Axel Pauli
 *
 * \brief wind analyzer
 *
 * The wind analyzer processes the list of flight samples looking
 * for wind speed and direction.
 *
 * \date 2002-2021
 */

#ifndef WINDANALYSER_H
#define WINDANALYSER_H

typedef enum e_circling { none, straight, circlingL, circlingR } t_circling;


#include "vector.h"
// #include "calculator.h"
// #include "gpsnmea.h"

class WindAnalyser
{

private:

public:

  WindAnalyser();

  virtual ~WindAnalyser();


  /**
   * Call for wind measurement result. The result is included in wind,
   * the quality of the measurement (1-5; 1 is bad, 5 is excellent) in quality.
   */
  static inline void getWind( Vector &wind, int &qual ) { wind = result; qual=quality; };

  /**
   * Called if the flight mode changes
   */
  static void newFlightMode( t_circling newMode );
  /*
   * Calculate flightmode from heading diff if circling is left or right
   */
  static void calcFlightMode( double diff, double speed );

  /**
   * Called if a new sample is available in the sample list.
   */
  static void newSample( Vector vector );

  /**
   * Called if a new satellite constellation has been detected.
   */
  static void newConstellation( int numSat );

  /**
   * Called, if the GPS status has changed.
   */
  static void gpsStatusChange( bool newStatus );

private:

  static void _calcWind();

  /** active is set to true or false by the newFlightMode slot. */
  static bool active;
  static int circleCount; // we are counting the number of circles, the first onces are probably not very round
  static bool circleLeft; // true=left, false=right
  static int circleDegrees; // Degrees of current flown circle
  static int lastHeading; // Last processed heading
  static int satCnt;
  static int minSatCnt;
  static bool ciclingMode;
  static int  gpsStatus;
  static Vector minVector;
  static Vector maxVector;
  static Vector result;
  static int quality;
  static t_circling flightMode;

};

#endif
