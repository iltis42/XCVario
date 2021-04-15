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

// degree to rad conversion
#define D2R(x) ((x)/57.2957795131)
#define R2D(x) ((x)*57.2957795131)

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
averageTH( 0.0 ),
averageTC( 0.0 ),
hMin( 0.0 ),
hMax( 0.0 ),
hMin_magn( 0.0 ),
hMax_magn( 0.0 ),
windDir( -1.0 ),
windSpeed( -1.0 )
{
}

double Wind::normAngle( double angle ){
	while( angle < 0 )
		angle += 360.;
	while( angle >= 360. )
		angle -= 360;
	return angle;
}


double Wind::meanAngle( double angle, double average ){
	bool neg=false;
	if( angle > 180 ){  // check if angle is in the second half and covernt to -+ 180° representation in case
		angle = angle - 360;
		neg=true;
	}
	if( average > 180 ){
		average = average - 360;
		neg=true;
	}
	double radangle=D2R(angle);
	double radaverage=D2R(average);
	double result = R2D(atan2( sin(radangle)+sin(radaverage), cos(radangle)+cos(radaverage)));
	if( neg ) // convert back from -+180° representation
		result = result + 360;
	if( result > 360 )
		result = result - 360;
	return result;
}

/**
 * Starts a new wind measurement cycle.
 */
void Wind::start()
{
	if( Flarm::gpsStatus() == true ) {
		// Ground speed in Km/h
		groundSpeed = Units::knots2kmh(Flarm::getGndSpeedKnots());
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

	averageTC = trueCourse;
	averageTH = trueHeading;

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
	// ESP_LOGI(FNAME,"calculateWind");
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
		ESP_LOGI(FNAME,"Restart Cycle: GPS Status bad or GS invalid");
		return false;
	}

	// Get current ground speed in km/h
	double cgs = Units::knots2kmh( Flarm::getGndSpeedKnots() );

	// Check, if given ground speed deltas are valid.
	if( fabs( groundSpeed - cgs ) > Units::Airspeed2Kmh( wind_speed_delta.get() ) ) {
		// Condition violated, start a new measurements cycle.
		start();
		ESP_LOGI(FNAME,"Restart Cycle GS %3.1f - CGS: %3.1f > %3.1f", groundSpeed, cgs, Units::Airspeed2Kmh( wind_speed_delta.get() ) );
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

	// Check, if we have a AS value > minimum, default is 25 km/h.
	// If GS is too, the measurement makes also sense, hence if we are not flying it doesn't

	if( ctas < Units::Airspeed2Kmh( wind_as_min.get() ) )
	{
		// We start a new measurement cycle.
		start();
		ESP_LOGI(FNAME,"Restart Cycle, AS %3.1f  < %3.1f Kmh", ctas,  Units::Airspeed2Kmh( wind_as_min.get() ) );
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
		ESP_LOGI(FNAME,"Restart Cycle, CTH %3.1f outside min: %3.1f max %3.1f", cth, hMin_magn, hMax_magn );
		return false;
	}

	// Get current true course
	double ctc = Flarm::getGndCourse();

	// Check if given GPS true course deltas are valid.
	if( (ctc < hMin || ctc > hMax) && (cgs > 5.0)  ) { // below a minimum Groundspeed, we ignore this vector
		start();
		ESP_LOGI(FNAME,"Restart Cycle, Ground Heading CTC: %3.1f outside min: %3.1f max: %3.1f", ctc, hMin, hMax );
		return false;
	}

	// Take all as new sample
	nunberOfSamples++;

	// The given deltas are fulfilled, summarize values
	sumTas += ctas;
	sumGroundSpeed += cgs;

	// Calculate average true course TC
	averageTC = averageTC + (meanAngle( ctc, averageTC ) - averageTC)*0.1;

	// Calculate average true heading TH
	averageTH = averageTH + (meanAngle( cth, averageTH ) - averageTH)*0.1;

	ESP_LOGI(FNAME,"%d TC: %3.1f (avg:%3.1f) GS:%3.1f TH: %3.1f (avg:%3.1f) IAS: %3.1f", nunberOfSamples, ctc, averageTC, cgs, cth, averageTH, ctas );
	// ESP_LOGI(FNAME,"avTC: %3.1f avTH:%3.1f ",averageTC,averageTH  );

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

		double tc = normAngle( averageTC );

		double th = normAngle( averageTH );

		// WCA in radians
		double tas = sumTas / nos;
		double gs = sumGroundSpeed / nos;

		calculateWind( tc, gs, th, tas  );
		measurementStart = getMsTime();  // it is enough to calculate every ten seconds a new wind
		return true;
	}
	return false;
}

void Wind::calculateWind( double tc, double gs, double th, double tas  ){
	ESP_LOGI(FNAME,"calculateWind: TC:%3.1f GS:%3.1f TH:%3.1f TAS:%3.1f", tc, gs, th, tas );
	// Wind correction angle WCA
	double wca = D2R( th - tc );  // catch the case when in wave there is no groundspeed and ground course is nonsense
	if( gs < 5 ){
		wca = 0;   // with invalid GPS directions we assume wca to be zero and tc=th
		tc = th;   // what will deliver heading and airspeed for wind
	}
	// Apply the Cosinus sentence: c^2 = a^2 + b^2 − 2 * a * b * cos( α )
	double ws = sqrt( (tas * tas) + (gs * gs ) - ( 2 * tas * gs * cos( wca ) ) );
	// WS / sin(WCA)
	double term = ws / sin( wca );
	// calculate WA (wind angle) in degree
	double wa = R2D(asin( tas / term ) );
	// Wind direction: W = TC - WA
	double wd = normAngle(tc - wa);
	// Alternative Method using TH
	// Wind direction: W = TH +WCA - WA
	// double wd = normAngle( (th + wca - wa) );
	// store calculated results
	windSpeed = ws;// wind speed in km/h
	windDir = wd;  // wind direction in degrees
	ESP_LOGI(FNAME,"New WindDirection: %3.1f deg,  Strength: %3.1f km/h  WCA: %3.1f  WA: %3.1f", wd, ws, R2D(wca), wa );
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
	if( int( windSpeed ) != 141 || int(windDir +0.5) != 135  )
		ESP_LOGI(FNAME,"Failed");

}



