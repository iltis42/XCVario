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
#include "sensor.h"
#include "KalmanMPU6050.h"
#include "vector.h"  // D2R, R2D


StraightWind::StraightWind() :
averageTas(0),
averageTH( 0.0 ),
averageTC( 0.0 ),
averageGS(0.0),
windDir( -1.0 ),
windSpeed( -1.0 ),
lowAirspeed( false ),
circlingWindDir( -1.0 ),
circlingWindDirReverse( -1.0 ),
circlingWindSpeed( -1.0 ),
circlingWindAge( 0 ),
airspeedCorrection( 1.0 ),
_tick(0),
gpsStatus(false),
deviation_cur(0),
magneticHeading(0),
status( "Initial" ),
jitter(0),
newWindSpeed(0),
newWindDir(0),
slipAverage(0),
lastHeading(0),
lastGroundCourse(0)
{
}

void StraightWind::begin(){
	if( compass_dev_auto.get() )
		airspeedCorrection = wind_as_calibration.get();
}

int StraightWind::_age = 10000;

void StraightWind::tick(){
	_age++;
	circlingWindAge++;
	_tick++;
}

bool StraightWind::getWind( int* direction, float* speed, int *age )
{
	*direction = int( swind_dir.get() + 0.5 );
	*speed = float( swind_speed.get() );
	*age = _age;
	return ( true );
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
	if( SetupCommon::isClient()  ){
		ESP_LOGI(FNAME,"We are client device, get wind from master");
		return false;
	}

	if( Flarm::gpsStatus() == false ) {
		// GPS status not valid
		if( (wind_enable.get() == WA_STRAIGHT) || (wind_enable.get() == WA_BOTH) ){
			ESP_LOGI(FNAME,"Restart Cycle: GPS Status invalid");
		}
		status="Bad GPS";
		gpsStatus = false;
	}else{
		gpsStatus = true;
	}
	// ESP_LOGI(FNAME,"calculateWind flightMode: %d", CircleStraightWind::getFlightMode() );

	// Check if wind requirements are fulfilled
	if( (compass_enable.get() != CS_I2C &&  compass_enable.get() != CS_CAN) || compass_calibrated.get() == false || wind_enable.get() == WA_OFF ) {
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
	float cgs = Units::knots2kmh( Flarm::getGndSpeedKnots() );

	// Get current TAS in km/h
	float ctas = float( getTAS() );

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
	bool THok = false;
	if( compass )
		averageTH = compass->filteredTrueHeading( &THok, false ); // no deviation considered here (we add ourselfs as for reverse calculation we need also the pure heading)
	if( THok == false ) {
		// No valid heading available
		status="No Compass";
		if( (wind_enable.get() == WA_STRAIGHT) || (wind_enable.get() == WA_BOTH) ){
			ESP_LOGI(FNAME,"Restart Cycle: No magnetic heading");
		}
	}

	// Get current true course from GPS
	float ctc = Flarm::getGndCourse();
	// averageTC = Vector::normalize( averageTC + (ctc - averageTC) * 1/wind_gps_lowpass.get());
	averageTas = ctas;
	averageGS += (cgs -averageGS) * 1/wind_gps_lowpass.get();

	// Calculate average true course TC
	averageTC = ctc;

	// WCA in radians
	magneticHeading = averageTH;

	float deviation = compass->getDeviation( averageTH );

	if( (wind_logging.get() != WLOG_DISABLE) && compass ){
		char log[SSTRLEN];
		sprintf( log, "$WIND;");
		int pos = strlen(log);
		if( wind_logging.get() & WLOG_WIND ){
			sprintf( log+pos, "%d;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%.1f;%d;%d;%.1f;%1.1f", _tick, averageTC, cgs, averageTH, ctas, newWindDir, newWindSpeed, windDir, windSpeed, circlingWindDir, circlingWindSpeed,
					                                                                                       (airspeedCorrection-1)*100, CircleWind::getFlightMode(), gpsStatus, deviation, slipAngle );
		}
		pos=strlen(log);
		if( wind_logging.get() & WLOG_GYRO_MAG ){
			sprintf( log+pos, ";%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f;%.3f",
					compass->rawX()/16384.0,compass->rawY()/16384.0,compass->rawZ()/16384.0,
					IMU::getRawAccelX(), IMU::getRawAccelY(), IMU::getRawAccelZ(),
					IMU::getRawGyroX(), IMU::getRawGyroY(), IMU::getRawGyroZ()  );
		}
		pos = strlen(log);
		sprintf( log+pos, "\n");
		Router::sendXCV( log );
		// ESP_LOGI( FNAME,"%s", log );
	}

	if( (CircleWind::getFlightMode() != straight) || lowAirspeed || !THok || !gpsStatus ){
		// ESP_LOGI(FNAME,"In Circling, stop ");
		return false;
	}

	status="Calculating";
	// ESP_LOGI(FNAME,"%d TC: %3.1f (avg:%3.1f) GS:%3.1f TH: %3.1f (avg:%3.1f) TAS: %3.1f", nunberOfSamples, ctc, averageTC, cgs, cth, averageTH, ctas );
	calculateWind( averageTC, averageGS, averageTH, averageTas, deviation  );
	return true;
}

// length (or speed) of third vector in windtriangle
// and calculate WA (wind angle) in degree
// wind direction calculation taken from here:
// view-source:http://www.owoba.de/fliegerei/flugrechner.html
// direction in degrees of third vector in windtriangle
void StraightWind::calculateSpeedAndAngle( float angle1, float speed1, float angle2, float speed2, float& speed, float& angle ){
	float tcrad = D2R( angle1 );
	float thrad = D2R( angle2 );
	float wca = Vector::angleDiff( thrad, tcrad );
	float s2wca = speed2 * cos( wca );
	float ang = tcrad + atan2( speed2 * sin( wca ), s2wca - speed1 );
	// Cosinus sentence: c^2 = a^2 + b^2 − 2 * a * b * cos( α ) for wind speed in km/h
	speed = sqrt( (speed2 * speed2) + (speed1 * speed1 ) - ( 2 * s2wca * speed1  ) );
	angle = Vector::normalizeDeg( R2D( ang ) );  // convert radian to degree
	ESP_LOGI(FNAME,"calcAngleSpeed( A1/S1=%3.1f°/%3.1f km/h  A2/S2=%3.1f°/%3.1f km/h): A/S: %3.2f°/%3.2f km/h", angle1, speed1, angle2, speed2, angle, speed  );
}

float StraightWind::getAngle() { return swind_dir.get(); };
float StraightWind::getSpeed() { return swind_speed.get(); };

void StraightWind::calculateWind( float tc, float gs, float th, float tas, float deviation  ){
	ESP_LOGI(FNAME,"calculateWind: TC:%3.1f GS:%3.1f TH:%3.1f TAS:%3.1f Dev:%2.2f", tc, gs, th, tas, deviation );
	// Wind correction angle WCA
	if( gs < 5 ){
		tc = th;   // what will deliver heading and airspeed for wind
	}
	// Wind speed
	// Reverse calculate windtriangle for deviation and airspeed calibration
	bool devOK = true;

	// Gating for proper heading alignment
	slipAverage += (slipAngle -slipAverage)*0.0005;
	if( abs( slipAngle - slipAverage) > swind_sideslip_lim.get() ){
		status = "Side Slip";
		ESP_LOGI( FNAME, "Slip overrun %.2f, average %.2f", slipAngle, slipAverage );
		return;
	}
	float headingDelta = Vector::angleDiffDeg( th , lastHeading );
	lastHeading = th;
	if(  abs(headingDelta) > wind_straight_course_tolerance.get() ){
		ESP_LOGI(FNAME,"Not really straight flight, heading delta: %f", headingDelta );
		return;
	}
	float groundCourseDelta = Vector::angleDiffDeg( tc , lastGroundCourse );
	lastGroundCourse = tc;
	if(  abs(groundCourseDelta) > 7.5  ){
			ESP_LOGI(FNAME,"Not really straight flight, GND course delta: %f", groundCourseDelta );
			return;
	}

	if( circlingWindSpeed > 0 && compass_dev_auto.get() ){
		if( circlingWindAge > 1200 ){
			status = "OLD CIRC WIND";
			ESP_LOGI(FNAME,"Circling Wind exired");
		}else{
			float airspeed = 0;
			float heading = 0;
			Vector wind( circlingWindDir, circlingWindSpeed );
			Vector groundTrack( tc, gs );
			groundTrack.add( wind );
			airspeed = groundTrack.getSpeed();
			heading = groundTrack.getAngleDeg();
//#ifdef VERBOSE_LOG
			ESP_LOGI(FNAME,"Using CWind: %.2f°/%.2f, TC/GS: %.1f°/%.1f, HD/AS: %.2f°/%.2f, tas=%.2f, ASdelta %.3f", circlingWindDir, circlingWindSpeed, tc, gs, heading, airspeed, tas, airspeed-tas );
// #endif
			if( abs( airspeed - tas ) > wind_straight_speed_tolerance.get() ){  // 30 percent max deviation
				status = "AS OOB";
				ESP_LOGI(FNAME,"Estimated Airspeed/Groundspeed OOB, max delta: %f km/h, delta: %f km/h", wind_straight_speed_tolerance.get(), abs( airspeed - tas ) );
				return;
			}
			airspeedCorrection +=  (airspeed/tas - airspeedCorrection) * wind_as_filter.get();
			if( airspeedCorrection > 1.01 ) // we consider 1% as maximum needed correction
				airspeedCorrection = 1.01;
			else if( airspeedCorrection < 0.99 )
				airspeedCorrection = 0.99;
			if( abs( wind_as_calibration.get() - airspeedCorrection )*100 > 0.5 )
					wind_as_calibration.set( airspeedCorrection );
			if( compass )
				devOK = compass->newDeviation( th, heading );
			else{
				status = "No Compass";
				return;
			}
			// ESP_LOGI(FNAME,"Calculated TH/TAS: %3.1f°/%3.1f km/h  Measured TH/TAS: %3.1f°/%3.1f, asCorr:%2.3f, deltaAS:%3.2f, Age:%d", tH, airspeed, averageTH, tas, airspeedCorrection , airspeed-tas, circlingWindAge );
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

    // wind speed and direction
	calculateSpeedAndAngle( tc, gs, th+deviation, tas*airspeedCorrection, newWindSpeed, newWindDir );
	// ESP_LOGI( FNAME, "Calculated raw windspeed %.1f jitter:%.1f", newWindSpeed, jitter );

	Vector v;
	v.setAngle( newWindDir );
	v.setSpeedKmh( newWindSpeed );

	windVectors.push_back( v );
	while( windVectors.size() > wind_filter_lowpass.get() ){
		windVectors.pop_front();
	}

	Vector result = Vector( 0.0, 0.0 );
	for( auto it=std::begin(windVectors); it != std::end(windVectors); it++ ){
			result.add( *it );
			// ESP_LOGI(FNAME,"angle %.1f speed %.1f", it->getAngleDeg(), it->getSpeed() );
	}

	windDir   = result.getAngleDeg(); // Vector::normalizeDeg( result.getAngleDeg()/circle_wind_lowpass.get() );
	windSpeed = result.getSpeed() / windVectors.size();

	ESP_LOGI(FNAME,"New AVG WindDirection: %3.1f deg,  Strength: %3.1f km/h JI:%2.1f", windDir, windSpeed, jitter );
	_age = 0;
	if( (int)windDir != (int)swind_dir.get()  ){
		swind_dir.set( windDir );
	}
	if( (int)windSpeed != (int)swind_speed.get() ){
		swind_speed.set( windSpeed );
	}
}


void StraightWind::newCirclingWind( float angle, float speed ){
	ESP_LOGI(FNAME,"New good circling wind %3.2f°/%3.2f", angle, speed );
	circlingWindDir = angle;
	circlingWindDirReverse = Vector::reverse( angle );      // revers windvector
	ESP_LOGI(FNAME,"Wind dir %3.2f, reverse circling wind dir %3.2f", angle, circlingWindDirReverse );
	circlingWindSpeed = speed;
	circlingWindAge = 0;
}

void StraightWind::test()
{    // Class Test, check here the results: http://www.owoba.de/fliegerei/flugrechner.html
	calculateWind( 90, 100, 0, 100, 0 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 135 )
		ESP_LOGI(FNAME,"Failed");
	calculateWind( 270, 100, 0, 100, 0 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 225 )
		ESP_LOGI(FNAME,"Failed");

	calculateWind( 0, 100, 90, 100, 0 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 315 )
		ESP_LOGI(FNAME,"Failed");
	calculateWind( 0, 100, 270, 100, 0 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 45 )
		ESP_LOGI(FNAME,"Failed");

	calculateWind( 90, 100, 180, 100, 0 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 45 )
		ESP_LOGI(FNAME,"Failed");

	calculateWind( 180, 100, 270, 100, 0 );
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 135  )
		ESP_LOGI(FNAME,"Failed");
}
