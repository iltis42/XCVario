/***********************************************************************
 **
 **   windanalyser.cpp
 **
 **   This file is part of Cumulus
 **
 ************************************************************************
 **
 **   Copyright (c):  2002      by Andre Somers
 **                   2009-2015 by Axel Pauli
 **
 **   This file is distributed under the terms of the General Public
 **   License. See the file COPYING for more information.
 **
 ***********************************************************************/

#include <cmath>
#include <math.h>
#include "windanalyser.h"
#include "logdef.h"

/*
  About Wind analysis

  Currently, the wind is analyzed by finding the minimum and the maximum
  ground speeds measured while flying a circle. The direction of the wind is taken
  to be the direction in which the speed reaches it's maximum value, the speed
  is half the difference between the maximum and the minimum speeds measured.
  A quality parameter, based on the number of circles already flown (the first
  circles are taken to be less accurate) and the angle between the headings at
  minimum and maximum speeds, is calculated in order to be able to weigh the
  resulting measurement. This method do not more work for wind speeds < 5Km/h.

  There are other options for determining the wind speed. You could for instance
  add all the vectors in a circle, and take the resulting vector as the wind speed.
  This is a more complex method, but because it is based on more heading/speed
  measurements by the GPS, it is probably more accurate. If equipped with
  instruments that pass along airspeed, the calculations can be compensated for
  changes in airspeed, resulting in better measurements. We are now assuming
  the pilot flies in perfect circles with constant airspeed, which is of course
  not a safe assumption.
  The quality indication we are calculation can also be approached differently,
  by calculating how constant the speed in the circle would be if corrected for
  the wind speed we just derived. The more constant, the better. This is again
  more CPU intensive, but may produce better results.

  Some of the errors made here will be averaged-out by the WindStore, which keeps
  a number of wind measurements and calculates a weighted average based on quality.

 */



#define MINTURNANGDIFF 4

WindAnalyser::WindAnalyser()
{
	// Initialization
	minVector.setSpeedKmh( 370.0 );
	maxVector.setSpeedKmh( 0.0 );
}

bool WindAnalyser::active = false;
int WindAnalyser::circleCount = 0; 			// we are counting the number of circles, the first onces are probably not very round
bool WindAnalyser::circleLeft = false; 		// true=left, false=right
int WindAnalyser::circleDegrees = 0; 		// Degrees of current flown circle
int WindAnalyser::lastHeading = -1; 		// Last processed heading
int WindAnalyser::satCnt = 0;
int WindAnalyser::minSatCnt = 5;
bool WindAnalyser::ciclingMode = false;
int  WindAnalyser::gpsStatus = false;
Vector WindAnalyser::minVector;
Vector WindAnalyser::maxVector;
Vector WindAnalyser::result;
int WindAnalyser::quality = 0;
t_circling WindAnalyser::flightMode = none;



WindAnalyser::~WindAnalyser()
{}

/** Called if a new sample is available in the sample list. */
void WindAnalyser::newSample( Vector curVec )
{
	ESP_LOGI(FNAME,"newSample dir:%3.2f° speed:%3.2f", curVec.getAngleDeg(), curVec.getSpeed() );
	// circle detection
	if( lastHeading != -1 )
	{
		double diff = abs( curVec.getAngleDeg() - lastHeading );

		if( diff > 180 )
		{
			// Correct difference, if current angle has 360 degree passed.
			// In such a case it starts with smaller values again.
			diff = abs(diff - 360 );
		}
		calcFlightMode( diff, curVec.getSpeed() );

		if( flightMode == circlingL || flightMode == circlingR ){
			circleDegrees += diff;
		}
	}
	lastHeading = curVec.getAngleDeg();

	if( flightMode != circlingL && flightMode != circlingR ){
		ESP_LOGI(FNAME,"FlightMode not circling %d", flightMode );
		return;
	}

	if( curVec.getSpeed() < minVector.getSpeed() )
	{
		// New minimum speed detected
		minVector = curVec;
		// ESP_LOGI(FNAME,"new minimum detected:  %3.2f°/%3.2f kmh", minVector.getAngleDeg(), minVector.getSpeed() );
	}

	if( curVec.getSpeed() > maxVector.getSpeed() )
	{
		// New maximum speed detected
		maxVector = curVec;
		// ESP_LOGI(FNAME,"new maximum detected: %3.2f°/%3.2f kmh",  maxVector.getAngleDeg(), maxVector.getSpeed() );
	}

	if( circleDegrees > 360 )
	{
		ESP_LOGI(FNAME,"full circle made, circles %d", circleCount);
		// increase the number of circles flown (used to determine the quality)
		circleCount++;

		// calculate the wind for this circle
		_calcWind();

		circleDegrees = 0;

		minVector.setSpeedKmh( 370.0 );
		maxVector.setSpeedKmh( 0.0 );
	}
}

void WindAnalyser::calcFlightMode( double headingDiff, double speed ){
	// ESP_LOGI(FNAME,"calcFlightMode head diff:%3.2f gs:%3.2f", headingDiff, speed  );
	if( speed < 25 ){
		if( flightMode != none ) {
			newFlightMode( none );
			flightMode = none;
		}
	}
	if( headingDiff > MINTURNANGDIFF ){
		if( flightMode != circlingR ) {
			newFlightMode( circlingR );
			flightMode = circlingR;
		}
	}
	else if( headingDiff < MINTURNANGDIFF ) {
		if( flightMode != circlingL ) {
			newFlightMode( circlingL );
			flightMode = circlingL;
		}
	}
	else{
		if( flightMode != none ) {
			newFlightMode( none );
			flightMode = none;
		}
	}
}


/** Called if the flight mode changes */
void WindAnalyser::newFlightMode( t_circling newFlightMode )
{
	// Reset the circle counter for each flight mode change. The important thing
	// to measure is the number of turns in a thermal per turn direction.
	ESP_LOGI(FNAME,"newFlightMode %d", newFlightMode );

	circleCount   = 0;
	circleDegrees = 0;
	lastHeading   = -1;
	minVector.setSpeedKmh( 370.0 );
	maxVector.setSpeedKmh( 0.0 );

	// We are inactive as default.
	active = false;

	if( newFlightMode == circlingL )
	{
		circleLeft = true;
	}
	else if( newFlightMode == circlingR )
	{
		circleLeft = false;
	}
	else
	{
		ciclingMode = false;

		// Ok, so we are not circling.
		return;
	}

	// Set circle mode to true.
	ciclingMode = true;

	// Do we have enough satellites in view? The minimum should be four. Otherwise
	// the calculated wind results are very bad.
	if( satCnt < minSatCnt )
	{
		return;
	}

	if( gpsStatus == 0 )
	{
		// We have not a valid fix.
		return;
	}

	// We are active now.
	active = true;
}

void WindAnalyser::_calcWind()
{
	ESP_LOGI(FNAME,"calcWind");
	float aDiff = Vector::angleDiff( minVector.getAngleDeg(), maxVector.getAngleDeg() );
	ESP_LOGI(FNAME,"calcWind, min/max diff %3.2f", aDiff );

	/*
    Determine quality.

    Currently, we are using the question how well the min and the max vectors
    are on opposing sides of the circle to determine the quality. 140 degrees is
    the minimum separation, 180 is ideal.
    Furthermore, the first two circles are considered to be of lesser quality.
	 */

	quality = 5 - (abs( aDiff ) / 8);

	if( circleCount < 2 )
	{
		quality--;
	}

	if( circleCount < 1 )
	{
		quality--;
	}

	//  << "WindQuality=" << quality;

	if( quality < 1 )
	{
		ESP_LOGI(FNAME,"quality %d < 1: Abort ", quality );
		return; // Measurement quality too low
	}

	// 5 is maximum quality, make sure we honor that.
	quality = std::min( quality, 5 );

	// Invert maxVector angle
	maxVector.setAngle( maxVector.getAngleDeg() + 180 );

	// take both directions for min and max vector into account

      ESP_LOGI(FNAME, "maxAngle=%3.1f°/%.0f   minAngle=%3.1f°/%.0f",
           maxVector.getAngleDeg(), maxVector.getSpeed(),
           minVector.getAngleDeg(), minVector.getSpeed() );


	// the direction of the wind is the direction where the greatest speed occurred
	result.setAngle( ( maxVector.getAngleDeg() + minVector.getAngleDeg() ) / 2);

	// The speed of the wind is half the difference between the minimum and the maximum speeds.
	result.setSpeedKmh( (maxVector.getSpeed() - minVector.getSpeed()) / 2.0 );

	// Let the world know about our measurement!
	ESP_LOGI(FNAME,"### CircleWind: %3.1f°/%.1fKm/h  Q:%d", result.getAngleDeg(), result.getSpeed(), quality );
}

void WindAnalyser::newConstellation( int numSat )
{
	ESP_LOGI(FNAME,"newConstellation num sat:%d", numSat );
	satCnt = numSat;

	if( active && (satCnt < minSatCnt) )
	{
		ESP_LOGW(FNAME,"Sat Constellation below minimum");
		// we are active, but the satellite count drops below minimum
		active = false;
		return;
	}

	if( !active && ciclingMode && satCnt >= minSatCnt )
	{
		ESP_LOGW(FNAME,"Sat Constellation okay, (re)start WindCalculation");
		// we are not active because we had low satellite count but that has been
		// changed now. So we become active.
		// Initialize analyzer-parameters
		circleCount   = 0;
		circleDegrees = 0;
		lastHeading   = -1;
	}
}

void WindAnalyser::gpsStatusChange( bool newStatus )
{
	ESP_LOGI(FNAME,"gpsStatusChange status:%d", newStatus );
	gpsStatus = newStatus;

	if( active && newStatus != false )
	{
		// we are active, but the GPS fix is lost
		active = false;
		return;
	}

	if( !active && ciclingMode )
	{
		// we are not active because we had no GPS fix but that has been
		// changed now. So we become active.
		// Initialize analyzer-parameters
		circleCount   = 0;
		circleDegrees = 0;
		lastHeading   = -1;
	}
}
