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
#include "SetupNG.h"


/*
  About Wind analysis

  Currently, the wind is analyzed by finding the minimum and the maximum
  ground speeds measured while flying a circle. The direction of the wind is taken
  to be the direction in which the speed reaches it's maximum value, the speed
  is half the difference between the maximum and the minimum speeds measured.
  A jitter parameter, based on the number of circles already flown (the first
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
  The jitter indication we are calculation can also be approached differently,
  by calculating how constant the speed in the circle would be if corrected for
  the wind speed we just derived. The more constant, the better. This is again
  more CPU intensive, but may produce better results.

  Some of the errors made here will be averaged-out by the WindStore, which keeps
  a number of wind measurements and calculates a weighted average based on jitter.

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
float CircleWind::jitter = 0;
int CircleWind::num_samples = 0;
float CircleWind::direction = 0;
float CircleWind::windspeed = 0;
float CircleWind::lastWindSpeed = 0;
float CircleWind::headingDiff = 0;
int CircleWind::_age = 9000;
const char * CircleWind::status = "idle";
t_circling CircleWind::flightMode = undefined;
Vector CircleWind::windVectors[NUM_RESULTS];
int CircleWind::curVectorNum = 0;
uint8_t CircleWind::turn_left=0;
uint8_t CircleWind::turn_right=0;
uint8_t CircleWind::fly_straight=0;

CircleWind::~CircleWind()
{}

/** Called if a new sample is available in the sample list. */
void CircleWind::newSample( Vector curVec )
{
	// circle detection
	if( lastHeading != -1 )
	{
		headingDiff += ( Vector::angleDiffDeg( curVec.getAngleDeg(), lastHeading ) - headingDiff) * 0.3;  // filter a bit jittering headings
		calcFlightMode( headingDiff, curVec.getSpeed() );
		if( flightMode == circlingL || flightMode == circlingR )
			circleDegrees += abs(headingDiff);
	}
	if( SetupCommon::isClient() )
		return;

	lastHeading = curVec.getAngleDeg();
	if( flightMode != circlingL && flightMode != circlingR ){
		// ESP_LOGI(FNAME,"FlightMode not circling %d", flightMode );
		status = "Not Circling";
		circleDegrees = 0;
		return;
	}
	status = "Sampling";
	ESP_LOGI(FNAME,"GPS Sample, dir:%3.2f° speed:%3.2f", curVec.getAngleDeg(), curVec.getSpeed() );

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
		circleCount++;  // increase the number of circles flown (used to determine the jitter)
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
			ESP_LOGI(FNAME,"New flightmode: undefined, no GS");
		}
	}
	else{
		if( headingDiff > MINTURNANGDIFF ){
				if( turn_right < 4 )  // hold down
					turn_right++;
				fly_straight = 0;
				if( flightMode != circlingR && turn_right > 2 ) {
					newFlightMode( circlingR );
					flightMode = circlingR;
					// ESP_LOGI(FNAME,"New flightmode: circle right");
				}
		}
		else if( headingDiff < -MINTURNANGDIFF ) {
			if( turn_left < 4)
				turn_left++;
			fly_straight = 0;
			if( flightMode != circlingL && turn_left > 2 ){
				newFlightMode( circlingL );
				flightMode = circlingL;
				// ESP_LOGI(FNAME,"New flightmode: circle left");
			}
		}
		else{
			turn_left = turn_right = 0;
			if( fly_straight < 4 )
				fly_straight++;
			if( flightMode != straight && fly_straight > 2 ) {
				newFlightMode( straight );
				flightMode = straight;
				// ESP_LOGI(FNAME,"New flightmode: straight");
			}
		}
	}
}


/** Called if the flight mode changes */
void CircleWind::newFlightMode( t_circling newFlightMode )
{
	if( SetupCommon::isClient() )
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

const char * CircleWind::getFlightModeStr(){
	if( flightMode == straight )
		return "straight";
	else if( flightMode == circlingL )
		return "circle left";
	else if( flightMode == circlingR )
		return "circle right";
	else
		return "undefined";
}

bool CircleWind::getWind( int *dir, float *speed, int * age )
{
	*dir=rint(cwind_dir.get());
	*speed=cwind_speed.get();
	*age=_age;
	if( _age < 1800 && !(*speed == 0) && !(*dir == 0)  )
		return true;
	else
		return false;
}

void CircleWind::resetAge(){
	ESP_LOGI(FNAME,"resetAge");
	_age = 0;
}

void CircleWind::_calcWind()
{
	if( SetupCommon::isClient() )
		return;

	// Invert maxVector angle
	maxVector.setAngle( maxVector.getAngleDeg() + 180 );

	float aDiff = Vector::angleDiffDeg( minVector.getAngleDeg(), maxVector.getAngleDeg() );
	ESP_LOGI(FNAME,"calcWind, min/max diff %3.2f", aDiff );

	float delta = abs( aDiff );   // 90 degree diff is considered zero jitter

	if( delta > (max_circle_wind_diff.get()) )
	{
		ESP_LOGI(FNAME,"true course jitter bad %3.1f > %3.1f: Drop Sample ", delta, max_circle_wind_diff.get() );
		status = "Too Low Qual";
		return; // Measurement jitter too low
	}

	// take both directions for min and max vector into account
	ESP_LOGI(FNAME, "maxAngle=%3.1f°/%.0f   minAngle=%3.1f°/%.0f Jitter=%2.1f°",
			maxVector.getAngleDeg(), maxVector.getSpeed(),
			minVector.getAngleDeg(), minVector.getSpeed(), aDiff  );


	// the direction of the wind is the direction where the greatest speed occurred
	result.setAngle( (maxVector.getAngleDeg() + minVector.getAngleDeg()) / 2.0 );

	// The speed of the wind is half the difference between the minimum and the maximum speeds.
	result.setSpeedKmh( (maxVector.getSpeed() - minVector.getSpeed()) / 2.0 );

	// Let the world know about our measurement!
	ESP_LOGI(FNAME,"### RAW CircleWind: %3.1f°/%.1fKm/h", result.getAngleDeg(), result.getSpeed() );
	newWind( result.getAngleDeg(), result.getSpeed() );
	// _age = 0;
}


// Jitter in the signal leads to higher windspeed measures as delta's grow
// The new algorithm considers jitter and corrects windspeed according to measured jitter value
void CircleWind::newWind( double angle, double speed ){
	num_samples++;
	windVectors[curVectorNum].setAngle( angle );
	windVectors[curVectorNum].setSpeedKmh( speed );
	ESP_LOGI(FNAME,"New Wind Vector angle %.1f speed %.1f", windVectors[curVectorNum].getAngleDeg(), windVectors[curVectorNum].getSpeed() );

	int max_samples = (int)circle_wind_lowpass.get();

	Vector result;
	for( int i=0; (i<max_samples) && (i<num_samples); i++ ){
		result.add( windVectors[i] );
		// ESP_LOGI(FNAME,"i=%d, angle %.1f speed %.1f", i, result.getAngleDeg(), result.getSpeed() );
	}
	int cur_samples=num_samples;
	if( cur_samples  > max_samples )
		cur_samples = max_samples;

	direction = result.getAngleDeg(); // Vector::normalizeDeg( result.getAngleDeg()/circle_wind_lowpass.get() );
	windspeed = result.getSpeed() / cur_samples;

	lastWindSpeed = windspeed;
	curVectorNum++;
	if( curVectorNum > max_samples )
		curVectorNum=0;
	ESP_LOGI(FNAME,"### NEW AGV CircleWind: %3.1f°/%.1fKm/h  JI:%2.1f", direction, windspeed, jitter  );

	if( (int)direction != (int)cwind_dir.get()  ){
		cwind_dir.set( direction );
	}
	if( (int)windspeed != (int)cwind_speed.get() ){
		cwind_speed.set( windspeed );
	}
	if( circleCount >= 2 )
		theWind.newCirclingWind( direction, windspeed );
}

void CircleWind::tick(){
	_age++;
	// ESP_LOGI(FNAME,"age: %d CWD: %.1f CWS %.1f", _age, cwind_dir.get(), cwind_speed.get() );
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
	if( SetupCommon::isClient()  )
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
	if( SetupCommon::isClient() )
		return;
	if( gpsStatus != newStatus  )
	{
		ESP_LOGI(FNAME,"gpsStatusChange status:%d", newStatus );
		// we are not active because we had no GPS fix but that has been
		// changed now. So we become active.
		// Initialize analyzer-parameters
		gpsStatus = newStatus;
		if( ! gpsStatus )
			status = "Bad GPS";
		restartCycle( true );
	}
}
