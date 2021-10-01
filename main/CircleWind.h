/***********************************************************************
 **
 ************************************************************************
 **
 **   Copyright (c):  2002       by André Somers,
 **	                  2002-2006  Eckhard Völlm
 **                   2006-      by Axel Pauli
 **                   2021-      Eckhard Völlm
 **
 **
 **
 **
 **   $Id$
 **
 ***********************************************************************/

/**
 * \class CircleWind
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

#ifndef CIRCLEWIND_H
#define CIRCLEWIND_H

typedef enum e_circling { undefined, straight, circlingL, circlingR } t_circling;


#include "vector.h"
#include <cmath>

// #include "calculator.h"
// #include "gpsnmea.h"

#define NUM_RESULTS 10

class CircleWind
{

private:

public:

  CircleWind();

  virtual ~CircleWind();
  static void tick();

  /**
   * Call for wind measurement result. The result is included in wind,
   * the jitter of the measurement (1-5; 1 is bad, 5 is excellent) in jitter.
   */
  static inline void getWind( Vector &wind, int &qual ) { wind = result; qual=jitter; };

  /**
   * Called if the flight mode changes
   */
  static void newFlightMode( t_circling newMode );
  static inline t_circling getFlightMode() { return flightMode; };

  /*
   * Calculate flightmode from heading diff if circling is left or right
   */
  static void calcFlightMode( float diff, float speed );

  /**
   * Called if a new sample is available in the sample list.
   */
  static void newSample( Vector vector );

  static void restartCycle( bool clean );

  /**
   * Called if a new satellite constellation has been detected.
   */
  static void newConstellation( int numSat );

  /**
   * Called, if the GPS status has changed.
   */
  static void gpsStatusChange( bool newStatus );

  static void newWind( double angle, double speed );

  static bool getWind( int *dir, float *speed, int * age );

  static float getNumCircles() 	 {  return circleCount+(circleDegrees/360.0); }
  static int getSatCnt()     	 {  return satCnt; }
  static bool getGpsStatus()     {  return gpsStatus; }
  static float  getAngle() 		 { return result.getAngleDeg(); }
  static float  getSpeed() 		 { return result.getSpeed(); }
  static int  getAge() 			 { return _age; }
  static void resetAge();
  static int  getQuality() 		 { return rint(jitter*20); } // 0..100 %
  static const char * getStatus()      { return status; }
  static const char *getFlightModeStr();

private:

  static void _calcWind();
  static int circleCount; // we are counting the number of circles, the first onces are probably not very round
  static bool circleLeft; // true=left, false=right
  static int circleDegrees; // Degrees of current flown circle
  static int lastHeading; // Last processed heading
  static int satCnt;
  static int minSatCnt;
  static t_circling circlingMode;
  static int  gpsStatus;
  static Vector minVector;
  static Vector maxVector;
  static Vector result;
  static float jitter;
  static t_circling flightMode;
  static float direction;
  static float windspeed;
  static int num_samples;
  static int _age;
  static const char *status;
  static float headingDiff;
  static float lastWindSpeed;
  static Vector windVectors[NUM_RESULTS];
  static int curVectorNum;
  static int turn_left;
  static int turn_right;
  static int fly_straight;

};

#endif
