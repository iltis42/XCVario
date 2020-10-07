/*
 * OpenVario.cpp
 *
 *  Created on: Dec 20, 2017
 *      Author: iltis
 */

#include "OpenVario.h"
#include <stdio.h>
#include <string.h>
#include "sensor.h"
#include "Setup.h"
#include "math.h"
#include "S2F.h"
#include <logdef.h>

#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "DallasRmt.h"

S2F * OpenVario::_s2f = 0;

OpenVario::OpenVario(S2F * s2f) {
	_s2f = s2f;
}

OpenVario::~OpenVario() {

}

float lbank = 0;
float lpitch = 0;


void OpenVario::makeNMEA( proto_t proto, char* str, float baro, float dp, float te, float temp, float ias, float tas,
		float mc, int bugs, float aballast, bool cruise, float alt, bool validTemp, float acc_x, float acc_y, float acc_z ){
	if( !validTemp )
		temp=0;

	if( proto == P_OPENVARIO ) {
		sprintf(str,"$POV,P,%0.1f,Q,%0.1f,E,%0.1f,T,%0.1f",baro,dp,te,temp);
	}
	else if ( proto == P_BORGELT ) {
		/*
		Sentence has following format:
		$PBB50,AAA,BBB.B,C.C,DDDDD,EE,F.FF,G,HH*CHK crlf
		AAA = TAS 0 to 150 knots
		BBB.B = Vario, -10 to +15 knots, negative sign for sink
		C.C = MacCready 0 to 8.0 knots
		DDDDD = IAS squared 0 to 22500
		EE = bugs degradation, 0 = clean to 30 %
		F.FF = Ballast 1.00 to 1.60
		G = 0 in climb, 1 in cruise
		HH = Outside airtemp in degrees celcius ( may have leading negative sign )
		CHK = standard NMEA checksum
		  // $PBB50,100,0,10,1,10000,0,1,0,20*4A..
		  // $PBB50,0,.0,.0,0,0,1.07,0,-228*58
		  // $PBB50,14,-.2,.0,196,0,.92,0,-228*71

		 */
		float iaskn = ias*0.539957;
		sprintf(str,"$PBB50,%03d,%3.1f,%1.1f,%d,%d,%1.2f,%1d,%2d", (int)(tas*0.539957+0.5), te*1.94384, mc*1.94384, (int)((iaskn*iaskn)+0.5), bugs, (aballast+100)/100.0, cruise, (int)(temp+0.5) );
	}
	else if( proto == P_CAMBRIDGE ){
		/*
		 * Cambridge 302 Format
		!W,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>,<13>*hh<CR><LF>
		<1>    Vector wind direction in degrees
		<2>    Vector wind speed in 10ths of meters per second
		<3>    Vector wind age in seconds
		<4>    Component wind in 10ths of Meters per second + 500 (500 = 0, 495 = 0.5 m/s tailwind)
		<5>    True altitude in Meters + 1000
		<6>    Instrument QNH setting
		<7>    True airspeed in 100ths of Meters per second
		<8>    Variometer reading in 10ths of knots + 200
		<9>    Averager reading in 10ths of knots + 200
		<10>   Relative variometer reading in 10ths of knots + 200
		<11>   Instrument MacCready setting in 10ths of knots
		<12>   Instrument Ballast setting in percent of capacity
		<13>   Instrument Bug setting
		 *hh   Checksum, XOR of all bytes of the sentence after the ‘!’ and before the ‘*’
		 */
		sprintf(str, "!w,0,0,0,0,%d,%d,%d,%d,0,0,%d,%d,%d", int(alt+1000), (int)(QNH.get()), int(tas*0.539957*100), int((te*1.94384*10)+200), int( mc*1.94384*10 ), int( aballast ), (int)bugs );
	}
	else if( proto == P_EYE_PEYA ){
		// Static pressure from aircraft pneumatic system [hPa] (i.e. 1015.5)
		// Total pressure from aircraft pneumatic system [hPA] (i.e. 1015.5)
		// Pressure altitude [m] (i.e. 10260)
		// Calculated local QNH [mbar] (i.e. 1013.2)
		// Direction from were the wind blows [°] (0 - 359)
		// Wind speed [km/h]
		// True air speed [km/h] (i.e. 183)
		// Vertical speed from anemometry (m/s) (i.e. +05.4)
		// Outside Air Temperature (?C) (i.e. +15.2)
		// Relative humidity [%] (i.e. 095)
		float pa = alt - 8.92*(QNH.get() - 1013.25);

		if( validTemp )
			sprintf(str, "$PEYA,%.2f,%.2f,%.2f,%.2f,,,%.2f,%.2f,%.2f,,", baro, baro+(dp/100),pa, QNH.get(),tas,te,temp);
		else
			sprintf(str, "$PEYA,%.2f,%.2f,%.2f,%.2f,,,%.2f,%.2f,0,,", baro, baro+(dp/100),alt, QNH.get(),tas,te);

	}
	else if( proto == P_EYE_PEYI ){
		// mathematical way to describe the tilt using precise data from an accelerometer
		float roll = atan2(acc_y, acc_z) * 57.3;
		float xpitch = atan2((acc_x) , sqrt(acc_y * acc_y + acc_z * acc_z)) * 57.3;
		/*
			$PEYI,%.2f,%.2f,,,,%.2f,%.2f,%.2f,,%.2f,
			lbank,         // Bank == roll    (deg)           SRC
			lpitch,         // pItch           (deg)
			x,
			y,
			z,
			);
		 */
		sprintf(str, "$PEYI,%.2f,%.2f,,,,%.2f,%.2f,%.2f,,", roll,xpitch,acc_x,acc_y,acc_z );
	}
	else if( proto == P_AHRS_APENV1 ) { // experimental
		sprintf(str, "$APENV1,%.2f,%.2f,0,0,0,%.2f,", ias,alt,te );
	}
	else if( proto == P_AHRS_RPYL ) {   // experimental
		float roll = atan2(acc_y, acc_z) * 57.3;
		float xpitch = atan2((acc_x) , sqrt(acc_y * acc_y + acc_z * acc_z)) * 57.3;
		lbank += roll;   // 5 times a second
		lpitch += xpitch;
		sprintf(str, "$RPYL,%.2f,%.2f,0,0,,%.2f,0,",
				lbank,         // Bank == roll    (deg)           SRC
				lpitch,         // pItch           (deg)
				acc_z
		);
	}
	else if( proto == P_GENERIC ) {
		/*
		 * $PTAS1,xxx,yyy,zzzzz,aaa*CS<CR><LF>
		 * xxx:   CV or current vario. =vario*10+200 range 0-400(display +/-20.0 knots)
		 * yyy:   AV or average vario. =vario*10+200 range 0-400(display +/-20.0 knots)
		 * zzzzz: Barometric altitude in feet +2000
		 * aaa:   TAS knots 0-200
		 */
		sprintf(str, "$PTAS1,%d,%d,%d,%d", int((te*1.94384*10)+200), 0, int(alt*3.28084+2000), int(ias*0.539957+0.5) );
	}
	else {
		ESP_LOGW(FNAME,"Not supported protocol %d", nmea_protocol.get() );
	}
	int cs = getCheckSum(&str[1]);
	int i = strlen(str);
	sprintf( &str[i], "*%02X\r\n", cs );
}

void OpenVario::parseNMEA( char *str ){
	ESP_LOGI(FNAME,"parseNMEA %s", str);
	if ( nmea_protocol.get() == BORGELT ) {
		ESP_LOGI(FNAME,"parseNMEA, BORGELT");
		if (str[3] == 'b') {
			ESP_LOGI(FNAME,"parseNMEA, BORGELT, ballast modification");
			float aballast;
			sscanf(str, "!g,b%f", &aballast);
			aballast = aballast * 10; // Its obviously only possible to change in fraction's by 10% in CA302 (issue: 464)
			ESP_LOGI(FNAME,"New Ballast: %f %% of max ", aballast);
			float liters = (aballast/100.0) * polar_max_ballast.get();
			ESP_LOGI(FNAME,"New Ballast in liters: %f ", liters);
			float refw = polar_wingload.get() * polar_wingarea.get();
			ESP_LOGI(FNAME,"Reference weight: %f ", refw);
			float bal = (100.0*(liters+refw)/refw) - 100;
			ESP_LOGI(FNAME,"Final new ballast: %f ", bal);
			ballast.set( bal );
			_s2f->change_mc_bal();
		}
		if (str[3] == 'm') {
			ESP_LOGI(FNAME,"parseNMEA, BORGELT, MC modification");
			float mc;
			sscanf(str, "!g,m%f", &mc);
			mc = mc*0.1 / 1.94384;
			ESP_LOGI(FNAME,"New MC: %1.1f m/s", mc);
			MC.set( mc );
			_s2f->change_mc_bal();
		}
		if (str[3] == 'u') {
			ESP_LOGI(FNAME,"parseNMEA, BORGELT, Bugs modification");
			int mybugs;
			sscanf(str, "!g,u%d", &mybugs);
			mybugs = 100-mybugs;
			ESP_LOGI(FNAME,"New Bugs: %d %%", mybugs);
			bugs.set( mybugs );
			_s2f->change_mc_bal();
		}
	}
}


// Checksum berechnen und als int ausgeben
// wird als HEX benötigt im NMEA Datensatz
// zwischen $ oder ! und * rechnen
int OpenVario::getCheckSum(char * s) {
	int i, XOR, c;
	for (XOR = 0, i = 0; i < strlen(s); i++) {
		c = (unsigned char)s[i];
		if (c == '*') break;
		if ((c != '$') && (c != '!')) XOR ^= c;
	}
	return XOR;
}
