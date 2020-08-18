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

Setup * OpenVario::_setup = 0;
S2F   * OpenVario::_s2f = 0;


OpenVario::OpenVario(Setup * setup, S2F * s2f) {
	_setup = setup;
	_s2f = s2f;
}

OpenVario::~OpenVario() {

}


void OpenVario::makeNMEA(char* str, float baro, float dp, float te, float temp, float ias, float tas, float mc, int bugs, float ballast, bool cruise ){
	if( _setup->get()->_nmea_protocol == OPENVARIO ) {
		printf("OpenVario\n");
		sprintf(str,"$POV,P,%0.1f,Q,%0.1f,E,%0.1f,T,%0.1f",baro,dp,te,temp);
	}
	else if ( _setup->get()->_nmea_protocol == BORGELT ) {
		printf("Borgelt\n");
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
	    sprintf(str,"$PBB50,%03d,%3.1f,%1.1f,%d,%d,%1.2f,%1d,%2d", (int)(tas*0.539957+0.5), te*1.94384, mc*1.94384, (int)((iaskn*iaskn)+0.5), bugs, (ballast+100)/100.0, cruise, (int)(temp+0.5) );
	}
	else {
		printf("Not supported protocol %d", _setup->get()->_nmea_protocol );
	}
	int cs = getCheckSum(&str[1]);
	int i = strlen(str);
	sprintf( &str[i], "*%02X\n", cs );
}

void OpenVario::parseNMEA( char *str ){
    printf("parseNMEA %s\n", str);
    if ( _setup->get()->_nmea_protocol == BORGELT ) {
		printf("parseNMEA, BORGELT\n");
		if (str[3] == 'b') {
			printf("parseNMEA, BORGELT, ballast modification\n");
			float ballast;
			sscanf(str, "!g,b%f", &ballast);
			ballast = ballast * 10; // There is a bug in XCSoar sending only 10% of the fraction (issue: 464)
			printf("New Ballast: %f %% of max \n", ballast);
			float liters = (ballast/100.0) * _setup->get()->_polar.max_ballast;
			printf("New Ballast in liters: %f \n", liters);
			float refw = _setup->get()->_polar.wingload * _setup->get()->_polar.wingarea;
			printf("Reference weight: %f \n", refw);
			float bal = (100.0*(liters+refw)/refw) - 100;
			printf("Final new ballast: %f \n", bal);
			_setup->get()->_ballast = bal;
			_s2f->change_mc_bal();
		}
		if (str[3] == 'm') {
			printf("parseNMEA, BORGELT, MC modification\n");
			float mc;
			sscanf(str, "!g,m%f", &mc);
			mc = mc*0.1 / 1.94384;
			printf("New MC: %1.1f m/s\n", mc);
			_setup->get()->_MC = mc;
			_s2f->change_mc_bal();
		}
		if (str[3] == 'u') {
			printf("parseNMEA, BORGELT, Bugs modification\n");
			int bugs;
			sscanf(str, "!g,u%d", &bugs);
			bugs = 100-bugs;
			printf("New Bugs: %d %%\n", bugs);
			_setup->get()->_bugs = bugs;
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
