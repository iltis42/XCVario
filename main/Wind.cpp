/*
 * Wind.cpp
 *
 *  Created on: Mar 21, 2021
 *
 *  Author: Eckhard Völlm, Axel Pauli
 *
 *  Last update: 2021-04-14
 */
#include <algorithm>
#include <cmath>
#include "esp_system.h"
#include "Compass.h"
#include "Flarm.h"
#include "SetupNG.h"
#include "Wind.h"
#include "Units.h"
#include "sensor.h"
#include "math.h"


Wind::Wind() :
_drift(0),
nunberOfSamples( 0 ),
measurementStart( 0 ),
tas( 0.0 ),
groundSpeed( 0.0 ),
trueCourse( 0.0 ),
trueHeading( -1.0 ),
sumTas( 0.0 ),
sumGroundSpeed( 0.0 ),
sumTHDeviation( 0.0 ),
sumTCDeviation( 0.0 ),
hMin( 0.0 ),
hMax( 0.0 ),
hMin_magn( 0.0 ),
hMax_magn( 0.0 ),
windDir( -1.0 ),
windSpeed( -1.0 )
{
}

/**
 * Starts a new wind measurement cycle.
 */
void Wind::start()
{
	if( Flarm::gpsStatus() == true ) {
		// Ground speed in Km/h
		groundSpeed = Units::knots2kmh(Flarm::getGndSpeedKnots());
		ESP_LOGI(FNAME,"start GS %3.1f", groundSpeed );
		trueCourse = Flarm::getGndCourse();
	}
	else {
		// Mark values as invalid.
		groundSpeed = -1.0;
		trueCourse = -1.0;
		trueHeading = -1.0;
		return;
	}

	nunberOfSamples = 1;
	measurementStart = getMsTime();
	tas = double( getTAS() );

	bool ok;
	trueHeading = Compass::trueHeading( &ok );

	if( ok == false ) {
		// Mark true heading as invalid
		trueHeading = -1.0;
		return;
	}

	sumTas = tas;
	sumGroundSpeed = groundSpeed;
	sumTCDeviation = 0.0;
	sumTHDeviation = 0.0;

	// Define limit of TH observation window
	hMin_magn = trueHeading - wind_heading_delta.get();

	if( hMin_magn < 0.0 ) {
		hMin_magn += 360.0;
	}

	hMax_magn = trueHeading + wind_heading_delta.get();

	if( hMax_magn >= 360.0 ) {
		hMax_magn -= 360.0;
	}

	// Define limit of TC observation window
	hMin = trueCourse - wind_heading_delta.get();

	if( hMin < 0.0 ) {
		hMin += 360.0;
	}

	hMax = trueCourse + wind_heading_delta.get();

	if( hMax >= 360.0 ) {
		hMax -= 360.0;
	}

	// windDir = -1.0;
	// windSpeed = -1.0;
}

/**
 * Measurement cycle for wind calculation in straight flight. Should be
 * triggered periodically, maybe once per second.
 *
 * Returns true, if a new wind was calculated.
 */
bool Wind::calculateWind()
{
	// Check if wind requirements are fulfilled
	if( compass_enable.get() == false || compass_calibrated.get() == false ||
			wind_enable.get() == false ) {
		return false;
	}

	// Check GPRMC data status, GS, TC and TH
	if( Flarm::gpsStatus() == false || groundSpeed == -1.0 ||
			trueCourse == -1.0 || trueHeading == -1.0 ) {
		// GPS status or GS, TC and TH start data are not valid
		start();
		ESP_LOGI(FNAME,"GPS Status bad or GS invalid");
		return false;
	}

	// Get current ground speed in km/h
	double cgs = Units::knots2kmh( Flarm::getGndSpeedKnots() );

	// Check, if we have a GS value > 25 km/h. GS can be nearly zero in the wave.
	// If GS is to low, the measurement make no sense.
	/*
	if( cgs < 25.0 )
	{
		// We start a new measurement cycle.
		start();
		ESP_LOGI(FNAME,"GS %3.1f  < 25.0", cgs );
		return false;
	}
	 */

	// Check, if given ground speed deltas are valid.
	if( fabs( groundSpeed - cgs ) > Units::Airspeed2Kmh( wind_speed_delta.get() ) ) {
		// Condition violated, start a new measurements cycle.
		start();
		ESP_LOGI(FNAME,"GS %3.1f - CGS: %3.1f > %3.1f", groundSpeed, cgs, Units::Airspeed2Kmh( wind_speed_delta.get() ) );
		return false;
	}

	// Get current TAS in km/h
	double ctas = double( getTAS() );

	// check if given TAS deltas are valid.
	if( fabs( tas - ctas ) > Units::Airspeed2Kmh( wind_speed_delta.get() ) ) {
		// Condition violated, start a new measurements cycle.
		start();
		ESP_LOGI(FNAME,"TAS %3.1f - CTAS: %3.1f  > delta %3.1f", tas, ctas, Units::Airspeed2Kmh( wind_speed_delta.get() ) );
		return false;
	}

	// Get current true heading
	bool ok = true;
	double cth = Compass::trueHeading( &ok );

	if( ok == false ) {
		// No valid heading available
		start();
		return false;
	}

	// Check if given magnetic heading deltas are valid.
	if( hMin_magn < hMax_magn && ( cth < hMin_magn || cth > hMax_magn ) ) {
		// Heading outside of observation window
		ok = false;
	}
	else if( hMin_magn > hMax_magn && cth < hMin_magn && cth > hMax_magn ) {
		// Heading outside of observation window
		ok = false;
	}

	if( ok == false ) {
		// Condition violated, start a new measurements cycle.
		start();
		ESP_LOGI(FNAME,"CTH %3.1f outside min: %3.1f max %3.1f", cth, hMin_magn, hMax_magn );
		return false;
	}

	// Get current true course
	double ctc = Flarm::getGndCourse();

	ESP_LOGI(FNAME,"GND-Track: %3.1f GS:%3.1f MGN-Track: %3.1f IAS: %3.1f", ctc, cgs, cth, ctas );

	// Check if given true course deltas are valid.
	if( hMin < hMax && ( ctc < hMin || ctc > hMax ) ) {
		// Heading outside of observation window
		ok = false;
	}
	else if( hMin > hMax && ctc < hMin && ctc > hMax ) {
		// Heading outside of observation window
		ok = false;
	}

	if( ok == false ) {
		// Condition violated, start a new measurements cycle.
		start();
		ESP_LOGI(FNAME,"Ground Heading CTC: %3.1f outside min: %3.1f max: %3.1f", ctc, hMin, hMax );
		return false;
	}

	// Take all as new sample
	nunberOfSamples++;

	// The given deltas are fulfilled, summarize values
	sumTas += ctas;
	sumGroundSpeed += cgs;

	// Calculate course deviations
	double deviation = ctc - trueCourse;

	if( deviation < -180. ) { deviation += 360.; }

	sumTCDeviation += deviation;

	deviation = cth - trueHeading;

	if( deviation < -180. ) { deviation += 360.; }

	sumTHDeviation += deviation;

	if( elapsed() >= wind_measurement_time.get() * 1000 )
	{
		/**
       calculate wind by using wind triangle, see more here:
       http://klspublishing.de/downloads/KLSP%20061%20Allgemeine%20Navigation%20DREHMEIER.pdf

       The Wind Correction Angle is the angle between the Heading and the
       Desired Course:

       WCA = Heading - DesiredCourse
		 */
		double nos = double( nunberOfSamples );

		double tc = sumTCDeviation / nos;

		// normalize angle
		tc += trueCourse; // Average of TC

		while(tc >= 360.)
			tc -= 360.;
		while(tc < 0.)
			tc += 360.;

		double th = sumTHDeviation / nos;

		// normalize angle
		th += trueHeading; // Average of TH

		while(th >= 360.)
			th -= 360.;
		while(th < 0.)
			th += 360.;

		// WCA in radians
		double tas = sumTas / nos;
		double gs = sumGroundSpeed / nos;

		calculateWind( tc, gs, th, tas  );
		return true;

	}
	return false;
}

void Wind::calculateWind( double tc, double gs, double th, double tas  ){
	ESP_LOGI(FNAME,"calculateWind: TC:%3.1f GS:%3.1f TH:%3.1f TAS:%3.1f", tc, gs, th, tas );

	// Wind correction angle WCA
	double wca = ( th - tc ) * M_PI / 180.0;
	ESP_LOGI(FNAME,"WCA:%3.1f°", wca*180/M_PI );

	// Apply the Cosinus sentence: c^2 = a^2 + b^2 − 2 * a * b * cos( α )
	double ws = sqrt( (tas * tas) + (gs * gs ) - ( 2 * tas * gs * cos( wca ) ) );
	// WS / sin(WCA)
	double term = ws / sin( wca );

	// calculate WA (wind angle) in degree
	double wa = asin( tas / term ) * 180. / M_PI;

	// Wind direction: W = TC - WA
	double wd = (tc - wa);

	while( wd < 0 )
		wd += 360.;
	while( wd >= 360. )
		wd -= 360;

	// store calculated results
	windSpeed = ws;// wind speed in km/h
	windDir = wd;  // wind direction in degrees
	ESP_LOGI(FNAME,"New WindDirection: %3.1f deg,  Strength: %3.1f km/h", wd, ws );
}

void Wind::test(){    // Class Test
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
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 135 )
		ESP_LOGI(FNAME,"Failed");

}



