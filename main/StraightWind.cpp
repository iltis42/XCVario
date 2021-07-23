/*
 * Wind.cpp
 *
 *  Module to calculate straight wind
 *
 *  Created on: Mar 21, 2021
 *
 *  Author: Eckhard Völlm, Axel Pauli
 *
 *  Last update: 2021-04-21
 */
#include <algorithm>
#include <cmath>
#include "esp_system.h"
#include "Compass.h"
#include "Flarm.h"
#include "SetupNG.h"
#include "StraightWind.h"
#include "Units.h"
#include "sensor.h"
#include "math.h"
#include "CircleWind.h"

// degree to rad conversion
#define D2R(x) ((x)/57.2957795131)
#define R2D(x) ((x)*57.2957795131)

StraightWind::StraightWind() :
nunberOfSamples( 0 ),
measurementStart( 0 ),
tas( 0.0 ),
groundSpeed( 0.0 ),
trueCourse( 0.0 ),
trueHeading( -1.0 ),
sumTas( 0.0 ),
sumGroundSpeed( 0.0 ),
averageTH( 0.0 ),
averageTC( 0.0 ),
averageGS(0.0),
tcStart( 0.0 ),
mhStart( 0.0 ),
windDir( -1.0 ),
windSpeed( -1.0 ),
lowAirspeed( false ),
circlingWindDir( -1.0 ),
circlingWindSpeed( -1.0 ),
circlingWindAge( 0 ),
airspeedCorrection( 1.0 ),
_age (0),
airspeed_jitter(0),
groundspeed_jitter(0),
airspeed_jitter_tmp(0),
groundspeed_jitter_tmp(0),
gpsStatus(false),
deviation_cur(0),
magneticHeading(0),
status( "Initial" )
{
}

void StraightWind::begin(){
	if( compass_dev_auto.get() )
		airspeedCorrection = wind_as_calibration.get();
}

/**
 * Starts a new wind measurement cycle.
 */
void StraightWind::start()
{
	if( wireless == WL_WLAN_CLIENT || the_can_mode == CAN_MODE_CLIENT )
		return;
	if( Flarm::gpsStatus() == true ) {
		// Ground speed in Km/h
		groundSpeed = Units::knots2kmh(Flarm::getGndSpeedKnots());
		trueCourse = Flarm::getGndCourse();
		gpsStatus = true;
	}
	else {
		// Mark all values as invalid.
		groundSpeed = -1.0;
		trueCourse = -1.0;
		trueHeading = -1.0;
		gpsStatus = false;
		return;
	}
	nunberOfSamples = 1;
	measurementStart = getMsTime();
	tas = double( getTAS() );

	bool ok;
	trueHeading = Compass::rawHeading( &ok );

	if( ok == false ) {
		// Mark true heading as invalid
		trueHeading = -1.0;
		return;
	}

	sumTas = tas;
	sumGroundSpeed = groundSpeed;

	averageTC = trueCourse;
	averageTH = trueHeading;

	// Define start of TH and TC observation window
	mhStart = Vector::normalizeDeg( trueHeading );
    tcStart = Vector::normalizeDeg( trueCourse );
}

void StraightWind::tick(){
	_age++;
	circlingWindAge++;
}

/**
 * Measurement cycle for wind calculation in straight flight. Should be
 * triggered periodically, maybe once per second.
 *
 * Returns true, if a new wind was calculated.
 */
bool StraightWind::calculateWind()
{
	// ESP_LOGI(FNAME,"Straight wind, calculateWind()");
	if( wireless == WL_WLAN_CLIENT || the_can_mode == CAN_MODE_CLIENT ){
		ESP_LOGI(FNAME,"We are client device, get wind from master");
		return false;
	}
	// ESP_LOGI(FNAME,"calculateWind flightMode: %d", CircleStraightWind::getFlightMode() );
	if( CircleWind::getFlightMode() != straight ){
		// ESP_LOGI(FNAME,"In Circling, stop ");
		return false;
	}
	// Check if wind requirements are fulfilled
	if( compass_enable.get() == false || compass_calibrated.get() == false || wind_enable.get() == WA_OFF ) {
		ESP_LOGI(FNAME,"Compass issues: ENA:%d CAL:%d WIND_ENA:%d, abort", compass_enable.get(), compass_calibrated.get(), wind_enable.get() );
		if( !compass_enable.get() )
			status="Comps Dis";
		if( !compass_calibrated.get() )
			status="Comps NoCal";
		if( !wind_enable.get() )
			status="Wind NoEna";
		return false;
	}

	// Check GPRMC data status, GS, TC and TH
	if( Flarm::gpsStatus() == false ) {
		// GPS status not valid
		start();
		ESP_LOGI(FNAME,"Restart Cycle: GPS Status invalid");
		status="Bad GPS";
		gpsStatus = false;
		return false;
	}
	gpsStatus = true;

	if(  groundSpeed == -1.0 ) {
		// GS not valid
		start();
		ESP_LOGI(FNAME,"Restart Cycle: GS invalid");
		status="GS invalid";
		return false;
	}

	if( trueCourse == -1.0 ) {
		// TC are not valid
		start();
		ESP_LOGI(FNAME,"Restart Cycle: TC invalid");
		status="Bad TC";
		return false;
	}

	if( trueHeading == -1.0 ) {
		// TH start data are not valid
		start();
		ESP_LOGI(FNAME,"Restart Cycle: True Heading invalid");
		status="Bad TH";
		return false;
	}

	// Get current ground speed in km/h
	double cgs = Units::knots2kmh( Flarm::getGndSpeedKnots() );
	float gsdelta = fabs( groundSpeed - cgs );
	// Check, if given ground speed deltas are valid.
	if( gsdelta > Units::Airspeed2Kmh( wind_speed_delta.get() ) ) {
		// Condition violated, start a new measurements cycle.
		start();
		ESP_LOGI(FNAME,"Restart Cycle GS %3.1f - CGS: %3.1f > %3.1f", groundSpeed, cgs, Units::Airspeed2Kmh( wind_speed_delta.get() ) );
		status="GS Unstable";
		return false;
	}
	if( groundspeed_jitter_tmp < gsdelta )
		groundspeed_jitter_tmp = gsdelta;

	// Get current TAS in km/h
	double ctas = double( getTAS() );
	// check if given TAS deltas are valid.
	float tasdelta = fabs( tas - ctas );
	if( tasdelta > Units::Airspeed2Kmh( wind_speed_delta.get() ) ) {
		// Condition violated, start a new measurements cycle.
		start();
		ESP_LOGI(FNAME,"TAS %3.1f - CTAS: %3.1f  > delta %3.1f", tas, ctas, Units::Airspeed2Kmh( wind_speed_delta.get() ) );
		status="AS Unstable";
		return false;
	}
	// ESP_LOGI(FNAME,"Straight Wind calculate GS:%3.2f TC%3.2f TH:%3.2f TAS:%3.2f ", groundSpeed, trueCourse, trueHeading, ctas  );
	if( airspeed_jitter_tmp < tasdelta )
		airspeed_jitter_tmp = tasdelta;
	// ESP_LOGI(FNAME,"++++++ TAS %3.1f GS: %3.1f GSJ%3.2f  ASJ %3.2f", tas, cgs, groundspeed_jitter, airspeed_jitter );

	// Check, if we have a AS value > minimum, default is 25 km/h.
	// If GS is nearly zero, the measurement makes also sense (wave), hence if we are not flying it doesn't

	if( ctas < Units::Airspeed2Kmh( wind_as_min.get() ) )
	{
		// We start a new measurement cycle.
		start();
		if( !lowAirspeed ) {
			ESP_LOGI(FNAME,"Low Airspeed, stop wind calculation, AS %3.1f  < %3.1f Kmh", ctas,  Units::Airspeed2Kmh( wind_as_min.get() ) );
			lowAirspeed = true;
		}
		status="Low AS";
		return false;
	}else{
		if( lowAirspeed ) {
			ESP_LOGI(FNAME,"Airspeed OK, start wind calculation, AS %3.1f  < %3.1f Kmh", ctas,  Units::Airspeed2Kmh( wind_as_min.get() ) );
			lowAirspeed = false;
		}
	}
	// Get current true heading from compass.
	bool ok = true;
	double cth = Compass::rawHeading( &ok );

	if( ok == false ) {
		// No valid heading available
		start();
		status="No MH";
		ESP_LOGI(FNAME,"Restart Cycle: No magnetic heading");
		return false;
	}

	float diff = abs( Vector::angleDiffDeg( cth, mhStart ));
	if( diff >  wind_heading_delta.get() ) {
		// Condition violated, start a new measurements cycle.
		start();
		ESP_LOGI(FNAME,"Restart Cycle, CTH diff %3.1f outside max %3.1f", diff, wind_heading_delta.get()  );
		status="MH Unstable";
		return false;
	}

	// Get current true course from GPS
	double ctc = Flarm::getGndCourse();

	// The ground course check is only done, if the ground speed is >=10 Km/h.
	// Near speed zero, the ground course is not stable in its direction.
	// Check if given GPS true course deltas are valid.
	if( cgs >= 10 ) {
		float diff = abs( Vector::angleDiffDeg( ctc, tcStart ));
		if( diff > wind_heading_delta.get() ) {
			// Condition violated, start a new measurements cycle.
			start();
			ESP_LOGI(FNAME,"Restart Cycle, Ground Heading CTC diff: %3.1f outside delta: %3.1f", diff, wind_heading_delta.get() );
			status="TC Unstable";
			return false;
		}
	}

	// Take all as new sample
	nunberOfSamples++;

	// The given deltas are fulfilled, summarize values
	sumTas += ctas;
	sumGroundSpeed += cgs;

	// Calculate average true course TC
	averageTC +=  Vector::angleDiffDeg( ctc, averageTC ) * 0.1;

	// Calculate average true heading TH
	averageTH += Vector::angleDiffDeg( cth, averageTH ) * 0.1;

	averageTH = Vector::normalizeDeg( averageTH );
	averageTC = Vector::normalizeDeg( averageTC );

	ESP_LOGI(FNAME,"%d TC: %3.1f (avg:%3.1f) GS:%3.1f TH: %3.1f (avg:%3.1f) IAS: %3.1f", nunberOfSamples, ctc, averageTC, cgs, cth, averageTH, ctas );
	// ESP_LOGI(FNAME,"avTC: %3.1f avTH:%3.1f ",averageTC,averageTH  );

	status="Measuring";
	if( elapsed() >= wind_measurement_time.get() * 1000 )
	{
		status="Calculating";
		/**
       calculate wind by using wind triangle, see more here:
       http://klspublishing.de/downloads/KLSP%20061%20Allgemeine%20Navigation%20DREHMEIER.pdf

       The Wind Correction Angle is the angle between the Heading and the
       Desired Course:

       WCA = Heading - DesiredCourse
		 */
		double nos = double( nunberOfSamples );

		// WCA in radians
		double tas = sumTas / nos;
		averageGS = sumGroundSpeed / nos;
		airspeed_jitter = airspeed_jitter_tmp;
		groundspeed_jitter = groundspeed_jitter_tmp;
		airspeed_jitter_tmp = 0;
		groundspeed_jitter_tmp = 0;
		magneticHeading = averageTH;
		calculateWind( averageTC, averageGS, averageTH, tas  );
		measurementStart = getMsTime();  // it is enough to calculate every ten seconds a new wind
		start();
		return true;
	}
	return false;
}

// length (or speed) of third vector in windtriangle
double StraightWind::calculateSpeed( double angle1, double speed1, double angle2, double speed2  ){
	double delta = Vector::normalize( D2R( angle2 - angle1 ) );
	// Cosinus sentence: c^2 = a^2 + b^2 − 2 * a * b * cos( α ) for wind speed in km/h
	return sqrt( (speed2 * speed2) + (speed1 * speed1 ) - ( 2 * speed2 * speed1 * cos( delta ) ) );
}

// calculate WA (wind angle) in degree
// wind direction calculation taken from here:
// view-source:http://www.owoba.de/fliegerei/flugrechner.html
// direction in degrees of third vector in windtriangle
double StraightWind::calculateAngle( double angle1, double speed1, double angle2, double speed2  ){
	ESP_LOGI(FNAME,"calculateAngle: angle1:%3.1f speed1:%3.1f angle2:%3.1f speed2:%3.1f", angle1, speed1, angle2, speed2 );
	double tcrad = D2R( angle1 );
	double thrad = D2R( angle2 );
	double wca = Vector::normalize( thrad - tcrad );

	double ang = tcrad + atan2( speed2 * sin( wca ), speed2 * cos( wca ) - speed1 );
	return( Vector::normalizeDeg( R2D( ang ) ) );  // convert radian to degree
}

void StraightWind::calculateWind( double tc, double gs, double th, double tas  ){
	ESP_LOGI(FNAME,"calculateWind: TC:%3.1f GS:%3.1f TH:%3.1f TAS:%3.1f", tc, gs, th, tas );
	// Wind correction angle WCA
	if( gs < 5 ){
		tc = th;   // what will deliver heading and airspeed for wind
	}
	// Wind speed
	// Reverse calculate windtriangle for deviation and airspeed calibration
	bool devOK = true;
	if( circlingWindSpeed > 0 && compass_dev_auto.get() ){
		if( circlingWindAge > 1800 ){
			status = "OLD CIRC WIND";
		}else{
			float airspeed = calculateSpeed( circlingWindDir, circlingWindSpeed, tc, gs );
#ifdef VERBOSE_LOG
			ESP_LOGI(FNAME,"Using reverse circling wind dir %3.2f, reverse cal. airspeed=%3.3f, tas=%3.3f, delta %3.3f", circlingWindDir, airspeed, tas, airspeed-tas );
#endif
			float tH = calculateAngle( circlingWindDir, circlingWindSpeed, tc, gs );
			if( abs( airspeed/tas - 1.0 ) > 0.15 ){  // 15 percent max deviation
				status = "AS OOB";
				ESP_LOGI(FNAME,"Estimated Airspeed/Groundspeed OOB");
				return;
			}
			airspeedCorrection +=  (airspeed/tas - airspeedCorrection) *0.02;
			devOK = Compass::newDeviation( th, tH, airspeedCorrection );
			ESP_LOGI(FNAME,"Calculated TH/TAS: %3.1f°/%3.1f km/h  Measured TH/TAS: %3.1f°/%3.1f, asCorr:%2.3f, deltaAS:%3.2f, Age:%d",
					trueHeading, airspeed, averageTH, tas, airspeedCorrection , airspeed-tas, circlingWindAge );
		}
	}else
		status = "No Circ Wind";

	if( !devOK ){ // data is not plausible/useful
			ESP_LOGI( FNAME, "Calculated deviation out of bounds: Drop also this wind calculation");
			status = "Deviation OOB";
			return;
	}
	deviation_cur = Compass::getDeviation( th );
	ESP_LOGI(FNAME,"Deviation=%3.2f", deviation_cur );
	float thd = Vector::normalizeDeg( th+deviation_cur );

	windSpeed = calculateSpeed( tc, gs, thd, tas*airspeedCorrection );
	// wind direction
	windDir = calculateAngle( tc, gs, thd, tas*airspeedCorrection );

	ESP_LOGI(FNAME,"New WindDirection: %3.1f deg,  Strength: %3.1f km/h", windDir, windSpeed  );
	_age = 0;
	OV.sendWindChange( windDir, windSpeed, WA_STRAIGHT );
}


void StraightWind::newCirclingWind( float angle, float speed ){
	ESP_LOGI(FNAME,"New good circling wind %3.2f°/%3.2f", angle, speed );
	circlingWindDir = angle;
	circlingWindDir += 180;      // revers windvector
	if( circlingWindDir > 360 )
		circlingWindDir -= 360;
	// ESP_LOGI(FNAME,"Reverse circling wind dir %3.2f", circlingWindDir );
	circlingWindSpeed = speed;
	circlingWindAge = 0;
}

void StraightWind::test()
{    // Class Test, check here the results: http://www.owoba.de/fliegerei/flugrechner.html
	calculateWind( 90, 100, 0, 100 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 135 )
		ESP_LOGI(FNAME,"Failed");
	calculateWind( 270, 100, 0, 100 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 225 )
		ESP_LOGI(FNAME,"Failed");

	calculateWind( 0, 100, 90, 100 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 315 )
		ESP_LOGI(FNAME,"Failed");
	calculateWind( 0, 100, 270, 100 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 45 )
		ESP_LOGI(FNAME,"Failed");

	calculateWind( 90, 100, 180, 100 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 45 )
		ESP_LOGI(FNAME,"Failed");

	calculateWind( 180, 100, 270, 100 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 135  )
		ESP_LOGI(FNAME,"Failed");
}
