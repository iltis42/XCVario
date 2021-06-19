/***********************************************************************
 **
 **   CircleWind.cpp
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
#include "CircleWind.h"
#include "logdef.h"
#include "sensor.h"


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

CircleWind::CircleWind()
{
	// Initialization
	minVector.setSpeedKmh( 370.0 );
	maxVector.setSpeedKmh( 0.0 );
}

int CircleWind::circleCount = 0; 		// we are counting the number of circles, the first onces are probably not very round
bool CircleWind::circleLeft = false; 	// true=left, false=right
int CircleWind::circleDegrees = 0; 		// Degrees of current flown circle
int CircleWind::lastHeading = -1; 		// Last processed heading
int CircleWind::satCnt = 0;
int CircleWind::minSatCnt = 5;
t_circling CircleWind::circlingMode = undefined;
int  CircleWind::gpsStatus = false;
Vector CircleWind::minVector;
Vector CircleWind::maxVector;
Vector CircleWind::result;
float CircleWind::quality = 0;
int CircleWind::num_samples = 0;
float CircleWind::direction = 0;
float CircleWind::windspeed = 0;
int CircleWind::_age = 0;
const char * CircleWind::status = "idle";

t_circling CircleWind::flightMode = undefined;



CircleWind::~CircleWind()
{}

/** Called if a new sample is available in the sample list. */
void CircleWind::newSample( Vector curVec )
{
	if( wireless == WL_WLAN_CLIENT )
		return;
	// circle detection
	if( lastHeading != -1 )
	{
		float diff = Vector::angleDiffDeg( curVec.getAngleDeg(), lastHeading );
		calcFlightMode( diff, curVec.getSpeed() );
		if( flightMode == circlingL || flightMode == circlingR )
			circleDegrees += abs(diff);
	}
	lastHeading = curVec.getAngleDeg();

	if( flightMode != circlingL && flightMode != circlingR ){
		// ESP_LOGI(FNAME,"FlightMode not circling %d", flightMode );
		status = "Not Circling";
		circleDegrees = 0;
		return;
	}
	status = "Sampling";
	ESP_LOGI(FNAME,"GPS Sample, dir:%3.2f° speed:%3.2f, Circling:%d", curVec.getAngleDeg(), curVec.getSpeed(), (flightMode == circlingL || flightMode == circlingR) );

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

	if( circleDegrees > 361 )  // a bit more than one circle to ensure both ends are in
	{
		status = "Calculating";
		circleCount++;  // increase the number of circles flown (used to determine the quality)
		ESP_LOGI(FNAME,"full circle made, circles %d last circle had %d °", circleCount, circleDegrees );
		_calcWind(); 	// calculate the wind for this circle
		restartCycle( false );
	}
}

void CircleWind::calcFlightMode( float headingDiff, float speed ){
	// ESP_LOGI(FNAME,"calcFlightMode head diff:%3.2f gs:%3.2f", headingDiff, speed  );
	if( speed < 25 ){
		if( flightMode != undefined ) {
			newFlightMode( undefined );
			flightMode = undefined;
			ESP_LOGI(FNAME,"calcFlightMode() New flightmode: undefined, no GS");
		}
	}
	else{
		if( headingDiff > MINTURNANGDIFF ){
			if( flightMode != circlingR ) {
				newFlightMode( circlingR );
				flightMode = circlingR;
				ESP_LOGI(FNAME,"calcFlightMode() New flightmode: circle right");
			}
		}
		else if( headingDiff < -MINTURNANGDIFF ) {
			if( flightMode != circlingL ) {
				newFlightMode( circlingL );
				flightMode = circlingL;
				ESP_LOGI(FNAME,"calcFlightMode() New flightmode: circle left");
			}
		}
		else{
			if( flightMode != straight ) {
				newFlightMode( straight );
				flightMode = straight;
				ESP_LOGI(FNAME,"calcFlightMode() New flightmode: straight");
			}
		}
	}
}


/** Called if the flight mode changes */
void CircleWind::newFlightMode( t_circling newFlightMode )
{
	if( wireless == WL_WLAN_CLIENT )
		return;
	// Reset the circle counter for each flight mode change. The important thing
	// to measure is the number of turns in a thermal per turn direction.
	ESP_LOGI(FNAME,"newFlightMode %d", newFlightMode );
	if( newFlightMode == circlingL || newFlightMode == circlingR ){
		if( circlingMode != newFlightMode ){
			circlingMode = newFlightMode;
			restartCycle( true );
		}
	}
}

void CircleWind::_calcWind()
{
	if( wireless == WL_WLAN_CLIENT )
		return;

	// Invert maxVector angle
	maxVector.setAngle( maxVector.getAngleDeg() + 180 );

	float aDiff = Vector::angleDiffDeg( minVector.getAngleDeg(), maxVector.getAngleDeg() );
	ESP_LOGI(FNAME,"calcWind, min/max diff %3.2f", aDiff );

	/*
    Determine quality.

    Currently, we are using the question how well the min and the max vectors
    are on opposing sides of the circle to determine the quality. 140 degrees is
    the minimum separation, 180 is ideal.
    Furthermore, the first two circles are considered to be of lesser quality.

    Display e.g. -40% (Q*20) %, ->  Q(final) = -2;
    @ 1 Circle Q = -1.
    abs( aDiff ) / 8 = 6 -> aDiff = 48°


	 */

	quality = 5.0 - (abs( aDiff ) / max_circle_wind_diff.get() ) * 5.0;   // 90 degree diff is considered zero quality

	if( quality < 1 )
	{
		ESP_LOGI(FNAME,"quality bad %3.1f < 1: Abort ", quality );
		status = "Too Low Qual";
		return; // Measurement quality too low
	}

	// take both directions for min and max vector into account
	ESP_LOGI(FNAME, "maxAngle=%3.1f°/%.0f   minAngle=%3.1f°/%.0f Quality=%3.1f",
			maxVector.getAngleDeg(), maxVector.getSpeed(),
			minVector.getAngleDeg(), minVector.getSpeed(), quality );


	// the direction of the wind is the direction where the greatest speed occurred
	result.setAngle( (maxVector.getAngleDeg() + minVector.getAngleDeg()) / 2.0 );

	// The speed of the wind is half the difference between the minimum and the maximum speeds.
	result.setSpeedKmh( (maxVector.getSpeed() - minVector.getSpeed()) / 2.0 );

	// Let the world know about our measurement!
	ESP_LOGI(FNAME,"### RAW CircleWind: %3.1f°/%.1fKm/h  Q:%f", result.getAngleDeg(), result.getSpeed(), quality );
	newWind( result.getAngleDeg(), result.getSpeed(), quality );
	_age = 0;
}

void CircleWind::newWind( double angle, double speed, float q ){
	num_samples++;
	float kq = 0;
	if( num_samples == 1 ){
		direction = angle;
		windspeed = speed;
	}
	else{
		kq = q/10.0;
		direction += Vector::angleDiffDeg(angle,(double)direction) * kq;
		windspeed += (speed - windspeed) * kq;
		direction = Vector::normalizeDeg( direction );
	}
	ESP_LOGI(FNAME,"### NEW AGV CircleWind: %3.1f°/%.1fKm/h  KQ:%1.3f", direction, windspeed, kq );
	OV.sendWindChange( direction, windspeed, WA_CIRCLING );
	if( q > min_circle_wind_quality.get() && circleCount >= 2 )
		theWind.newCirclingWind( direction, windspeed );
}

void CircleWind::tick(){
	_age++;
}


void CircleWind::restartCycle( bool clean ){
	// ESP_LOGI(FNAME,"restartCycle( clean=%d)", clean  );
	if( clean )
		circleCount   = 0;
	circleDegrees = 0;
	lastHeading   = -1;
	minVector.setSpeedKmh( 370.0 );
	maxVector.setSpeedKmh( 0.0 );
}

void CircleWind::newConstellation( int numSat )
{
	if( wireless == WL_WLAN_CLIENT )
		return;
	ESP_LOGI(FNAME,"newConstellation num sat:%d", numSat );
	satCnt = numSat;

	if( satCnt < minSatCnt )
	{
		ESP_LOGW(FNAME,"Sat Constellation below minimum");
		// we are active, but the satellite count drops below minimum
		status = "< 5 Sat";
		restartCycle( true );
	}
}

void CircleWind::gpsStatusChange( bool newStatus )
{
	if( wireless == WL_WLAN_CLIENT )
		return;
	ESP_LOGI(FNAME,"gpsStatusChange status:%d", newStatus );
	if( gpsStatus != newStatus  )
	{
		// we are not active because we had no GPS fix but that has been
		// changed now. So we become active.
		// Initialize analyzer-parameters
		gpsStatus = newStatus;
		if( ! gpsStatus )
			status = "Bad GPS";
		restartCycle( true );
	}
}
