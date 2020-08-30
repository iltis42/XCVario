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

S2F * OpenVario::_s2f = 0;

OpenVario::OpenVario(S2F * s2f) {
	_s2f = s2f;
}

OpenVario::~OpenVario() {

}


void OpenVario::makeNMEA(char* str, float baro, float dp, float te, float temp, float ias, float tas, float mc, int bugs, float aballast, bool cruise ){
	if( nmea_protocol.get() == OPENVARIO ) {
		sprintf(str,"$POV,P,%0.1f,Q,%0.1f,E,%0.1f,T,%0.1f",baro,dp,te,temp);
	}
	else if ( nmea_protocol.get() == BORGELT ) {
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
	else {
		ESP_LOGW(FNAME,"Not supported protocol %d", nmea_protocol.get() );
	}
	int cs = getCheckSum(&str[1]);
	int i = strlen(str);
	sprintf( &str[i], "*%02X\n", cs );
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
