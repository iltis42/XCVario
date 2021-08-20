/*
 * Wind.cpp
 *
 *  Module to calculate straight wind
 *
 *  Created on: Mar 21, 2021
 *
 *  Author: Eckhard Völlm, Axel Pauli
 *
 *
       Calculate wind continuously by using wind triangle, see more here:
       http://klspublishing.de/downloads/KLSP%20061%20Allgemeine%20Navigation%20DREHMEIER.pdf

       The Wind Correction Angle is the angle between the Heading and the
       Desired Course:

       WCA = Heading - DesiredCourse
 *
 *
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
#include "Router.h"

// degree to rad conversion
#define D2R(x) ((x)/57.2957795131)
#define R2D(x) ((x)*57.2957795131)


Vector StraightWind::windVectors[NUM_STRAIGHT_RESULTS];

StraightWind::StraightWind() :
nunberOfSamples( 0 ),
averageTas(0),
averageTH( 0.0 ),
averageTC( 0.0 ),
averageGS(0.0),
windDir( -1.0 ),
windSpeed( -1.0 ),
lastWindSpeed( -1.0 ),
lowAirspeed( false ),
circlingWindDir( -1.0 ),
circlingWindDirReverse( -1.0 ),
circlingWindSpeed( -1.0 ),
circlingWindAge( 0 ),
airspeedCorrection( 1.0 ),
_age (0),
_tick(0),
gpsStatus(false),
deviation_cur(0),
magneticHeading(0),
status( "Initial" ),
jitter(0),
curVectorNum(0),
newWindSpeed(0),
newWindDir(0)
{
}

void StraightWind::begin(){
	if( compass_dev_auto.get() )
		airspeedCorrection = wind_as_calibration.get();
}

void StraightWind::tick(){
	_age++;
	circlingWindAge++;
	_tick++;
	// bool ok;
	// double cth = Compass::rawHeading( &ok );
	// ESP_LOGI(FNAME,"Heading: %.1f", cth );
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

	if( Flarm::gpsStatus() == false ) {
		// GPS status not valid
		ESP_LOGI(FNAME,"Restart Cycle: GPS Status invalid");
		status="Bad GPS";
		gpsStatus = false;
	}
	gpsStatus = true;
	// ESP_LOGI(FNAME,"calculateWind flightMode: %d", CircleStraightWind::getFlightMode() );

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

	// Get current ground speed in km/h
	double cgs = Units::knots2kmh( Flarm::getGndSpeedKnots() );

	// Get current TAS in km/h
	double ctas = double( getTAS() );

	// Check, if we have a AS value > minimum, default is 25 km/h.
	// If GS is nearly zero, the measurement makes also sense (wave), hence if we are not flying it doesn't

	if( ctas < Units::Airspeed2Kmh( wind_as_min.get() ) )
	{
		// We start a new measurement cycle.
		if( !lowAirspeed ) {
			ESP_LOGI(FNAME,"Low Airspeed, stop wind calculation, AS %3.1f  < %3.1f Kmh", ctas,  Units::Airspeed2Kmh( wind_as_min.get() ) );
			lowAirspeed = true;
		}
		status="Low AS";
	}else{
		if( lowAirspeed ) {
			ESP_LOGI(FNAME,"Airspeed OK, start wind calculation, AS %3.1f  < %3.1f Kmh", ctas,  Units::Airspeed2Kmh( wind_as_min.get() ) );
			lowAirspeed = false;
		}
	}
	// Get current true heading from compass.
	bool THok = true;
	double cth = Compass::filteredRawHeading( &THok );
	if( THok == false ) {
		// No valid heading available
		status="No MH";
		ESP_LOGI(FNAME,"Restart Cycle: No magnetic heading");
	}

	// Get current true course from GPS
	double ctc = Flarm::getGndCourse();
	averageTC += (ctc - averageTC) * 1/wind_gps_lowpass.get();

	averageTas = ctas;

	averageGS += (cgs -averageGS) * 1/wind_gps_lowpass.get();

	// Calculate average true course TC
	averageTC = ctc;

	// Calculate average true heading TH
	averageTH = cth;

	// WCA in radians
	magneticHeading = averageTH;

	if( wind_logging.get() ){
		char log[120];
		float dev = Compass::getDeviation( averageTH );
		float thd = Vector::normalizeDeg( averageTH+dev );
		sprintf( log, "$WIND;%d;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.4f,%d,%d,%.1f\n", _tick, averageTC, cgs, cth, thd, newWindDir, newWindSpeed, windDir, windSpeed, circlingWindDir, circlingWindSpeed, airspeedCorrection, CircleWind::getFlightMode(), gpsStatus, dev );
		Router::sendXCV( log );
	}

	if( (CircleWind::getFlightMode() != straight) || lowAirspeed || !THok || !gpsStatus ){
		// ESP_LOGI(FNAME,"In Circling, stop ");
		return false;
	}

	status="Calculating";
	ESP_LOGI(FNAME,"%d TC: %3.1f (avg:%3.1f) GS:%3.1f TH: %3.1f (avg:%3.1f) TAS: %3.1f", nunberOfSamples, ctc, averageTC, cgs, cth, averageTH, ctas );
	calculateWind( averageTC, averageGS, averageTH, averageTas  );

	return true;
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
	// ESP_LOGI(FNAME,"calculateWind: TC:%3.1f GS:%3.1f TH:%3.1f TAS:%3.1f", tc, gs, th, tas );
	// Wind correction angle WCA
	if( gs < 5 ){
		tc = th;   // what will deliver heading and airspeed for wind
	}
	// Wind speed
	// Reverse calculate windtriangle for deviation and airspeed calibration
	bool devOK = true;
	if( circlingWindSpeed > 0 && compass_dev_auto.get() ){
		if( circlingWindAge > 900 ){
			status = "OLD CIRC WIND";
		}else{
			float airspeed = calculateSpeed( circlingWindDirReverse, circlingWindSpeed, tc, gs );
#ifdef VERBOSE_LOG
			ESP_LOGI(FNAME,"Using reverse circling wind dir %3.2f, reverse cal. airspeed=%3.3f, tas=%3.3f, delta %3.3f", circlingWindDirReverse, airspeed, tas, airspeed-tas );
#endif
			float tH = calculateAngle( circlingWindDirReverse, circlingWindSpeed, tc, gs );
			if( abs( airspeed/tas - 1.0 ) > 0.30 ){  // 30 percent max deviation
				status = "AS OOB";
				ESP_LOGI(FNAME,"Estimated Airspeed/Groundspeed OOB");
				//return;
			}
			airspeedCorrection +=  (airspeed/tas - airspeedCorrection) * wind_as_filter.get();
			if( airspeedCorrection > 1.05 ) // we consider 5% as maximum needed correction
				airspeedCorrection = 1.05;
			else if( airspeedCorrection < 0.95 )
				airspeedCorrection = 0.95;
			devOK = Compass::newDeviation( th, tH, airspeedCorrection );
			ESP_LOGI(FNAME,"Calculated TH/TAS: %3.1f°/%3.1f km/h  Measured TH/TAS: %3.1f°/%3.1f, asCorr:%2.3f, deltaAS:%3.2f, Age:%d",
					tH, airspeed, averageTH, tas, airspeedCorrection , airspeed-tas, circlingWindAge );
		}
	}else{
		status = "No Circ Wind";
		// float airspeed = calculateSpeed( windDir, windSpeed, tc, gs );
		// airspeedCorrection +=  (airspeed/tas - airspeedCorrection) * wind_as_filter.get();
	}

	if( !devOK ){ // data is not plausible/useful
			ESP_LOGI( FNAME, "Calculated deviation out of bounds: Drop also this wind calculation");
			status = "Deviation OOB";
			return;
	}
	deviation_cur = Compass::getDeviation( th );
	// ESP_LOGI(FNAME,"Deviation=%3.2f", deviation_cur );
	float thd = Vector::normalizeDeg( th+deviation_cur );

	newWindSpeed = calculateSpeed( tc, gs, thd, tas*airspeedCorrection );


	ESP_LOGI( FNAME, "Calculated raw windspeed %.1f jitter:%.1f", newWindSpeed, jitter );

	// wind direction
	newWindDir = calculateAngle( tc, gs, thd, tas*airspeedCorrection );

	windVectors[curVectorNum].setAngle( newWindDir );
	windVectors[curVectorNum].setSpeedKmh( newWindSpeed );
	// Take all as new sample
	nunberOfSamples++;

	int max_samples = wind_filter_lowpass.get();

	Vector result;
	for( int i=0; (i<max_samples) && (i<nunberOfSamples); i++ ){
		result.add( windVectors[i] );
		// ESP_LOGI(FNAME,"i=%d, angle %.1f speed %.1f", i, result.getAngleDeg(), result.getSpeed() );
	}
	int cur_samples=nunberOfSamples;
	if( cur_samples  > max_samples )
		cur_samples = max_samples;

	windDir = result.getAngleDeg(); // Vector::normalizeDeg( result.getAngleDeg()/circle_wind_lowpass.get() );
	windSpeed = result.getSpeed() / cur_samples;

	curVectorNum++;
	if( curVectorNum > max_samples )
		curVectorNum=0;

	ESP_LOGI(FNAME,"New WindDirection: %3.1f deg,  Strength: %3.1f km/h JI:%2.1f", windDir, windSpeed, jitter );
	_age = 0;
	lastWindSpeed = windSpeed;


	OV.sendWindChange( windDir, windSpeed, WA_STRAIGHT );
}


void StraightWind::newCirclingWind( float angle, float speed ){
	ESP_LOGI(FNAME,"New good circling wind %3.2f°/%3.2f", angle, speed );
	circlingWindDirReverse = angle;
	circlingWindDir = angle;
	circlingWindDirReverse += 180;      // revers windvector
	if( circlingWindDirReverse > 360 )
		circlingWindDirReverse -= 360;
	// ESP_LOGI(FNAME,"Reverse circling wind dir %3.2f", circlingWindDirReverse );
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
