/*
 * Protocols.cpp
 *
 *  Created on: Dec 20, 2017
 *      Author: iltis
 */

#include "Protocols.h"
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
#include "KalmanMPU6050.h"
#include "Router.h"
#include "Atmosphere.h"
#include "Flarm.h"
#include "Units.h"
#include "Flap.h"
#include "Switch.h"
#include "SetupNG.h"
#include "CircleWind.h"

S2F * Protocols::_s2f = 0;
float Protocols::ballast_percent=0;

Protocols::Protocols(S2F * s2f) {
	_s2f = s2f;
}

Protocols::~Protocols() {

}

void Protocols::sendTemperatureChange( float temp ){
	char str[20];
	sprintf( str,"!xt,%2.1f\n", std::round(temp*10)/10 );
	// ESP_LOGI(FNAME,"New Temperature: %f, cmd: %s", temp, str );
	Router::sendXCV(str);
}

void Protocols::sendWkChange( float wk ){
	char str[20];
	sprintf( str,"!xw,%1.1f\n", wk );
	ESP_LOGI(FNAME,"New WK pos: %f, cmd: %s", wk, str );
	Router::sendXCV(str);
}

void Protocols::sendWindChange( float dir, float speed, e_windanalyser_mode_t type  ){
	char str[20];
	sprintf( str,"!xd,%3.1f,%3.1f,%d\n", dir, speed, type );
	ESP_LOGI(FNAME,"New wind cmd: %s", str );
	Router::sendXCV(str);
}

void Protocols::sendQNHChange( float qnh ){
	char str[20];
	sprintf( str,"!xq,%4.2f\n", qnh );
	ESP_LOGI(FNAME,"New QNH: %4.2f, cmd: %s", qnh, str );
	Router::sendXCV(str);
}

void Protocols::sendBallastChange( float ballast, bool external ){
	char str[20];
	ESP_LOGI(FNAME,"Send new Ballast %f ", ballast );
	float refw = polar_wingload.get() * polar_wingarea.get();
	ESP_LOGI(FNAME,"Reference weight: %f kg", refw);
	float liters = (1+ (ballast/100))*refw -refw;
	ESP_LOGI(FNAME,"New Ballast in liters: %f ", liters);
	float max_bal = polar_max_ballast.get();
	if( (int)(polar_max_ballast.get()) == 0 ) { // We use 100 liters as default once its not with the polar
		max_bal = 100;
	}
	ESP_LOGI(FNAME,"Max ballast %f", max_bal );
	float bal = (liters/max_bal)*10;
	ballast_percent = bal*10;
	if( external ){
		sprintf( str,"!g,b%d\n\r", (int)(std::round(bal)));
		ESP_LOGI(FNAME,"New ballast %f, cmd: %s", bal, str );
		Router::sendXCV(str);
	}
	sprintf( str,"!xb,%3.1f\n", ballast);
	ESP_LOGI(FNAME,"New ballast internal %f, cmd: %s", ballast, str );
	Router::sendXCV(str);
}

void Protocols::sendBugsChange( float bugs ){
	char str[20];
	sprintf( str,"!g,u%d\n", (int)(std::round(100-bugs)));   // Cambridge protocol uses 100-%bugs notation, 25% bugs = !g,u75
	ESP_LOGI(FNAME,"New bugs cmd: %s", str );
	Router::sendXCV(str);
}

void Protocols::sendClientBugsChange( float bugs ){
	char str[20];
	sprintf( str,"!xu,%d\n", (int)(std::round(bugs)));
	ESP_LOGI(FNAME,"New client bugs cmd: %s", str );
	Router::sendXCV(str);
}


void Protocols::sendMcChange( float mc ){
	char str[20];
	sprintf( str,"!g,m%d\n", (int)std::round(Units::ms2knots(mc)*10) );
	ESP_LOGI(FNAME,"New MC %f, cmd: %s", mc , str );
	Router::sendXCV(str);
}

void Protocols::sendClientMcChange( float mc ){
	char str[20];
	sprintf( str,"!xm,%2.1f\n", std::roundf(mc*10)/10 );
	ESP_LOGI(FNAME,"New Client MC %f, cmd: %s", mc , str );
	Router::sendXCV(str);
}

int last_climb=-1000;

void Protocols::sendMeanClimb( float climb ){
	if( std::round(climb*10) != last_climb )
	{
		last_climb=std::round(climb*10);
		char str[20];
		sprintf( str,"!xa,%1.1f\n", std::roundf(climb*10)/10 );
		ESP_LOGI(FNAME,"New mean climb cmd:%s", str );
		Router::sendXCV(str);
	}
}

/*
HDM - Heading - Magnetic

        1   2 3
        |   | |
 $--HDM,x.x,M*hh<CR><LF>

 Field Number:
  1) Heading Degrees, magnetic
  2) M = magnetic
  3) Checksum
 */
void Protocols::sendNmeaHDM( float heading ) {
	char str[21];
	sprintf( str,"$HCHDM,%3.1f,M", heading );
	// ESP_LOGI(FNAME,"Magnetic Heading: %3.1f", heading );

	int cs = calcNMEACheckSum(&str[1]);
	int i = strlen(str);
	sprintf( &str[i], "*%02X\r\n", cs );
	Router::sendXCV(str);
}

/*
HDT - Heading - True

        1   2 3
        |   | |
 $--HDT,x.x,T*hh<CR><LF>

 Field Number:
  1) Heading Degrees, true
  2) T = True
  3) Checksum
 */
void Protocols::sendNmeaHDT( float heading ) {
	char str[21];
	sprintf( str,"$HCHDT,%3.1f,T", heading );
	// ESP_LOGI(FNAME,"True Heading: %3.1f", heading );

	int cs = calcNMEACheckSum(&str[1]);
	int i = strlen(str);
	sprintf( &str[i], "*%02X\r\n", cs );
	Router::sendXCV(str);
}


void Protocols::sendNMEA( proto_t proto, char* str, float baro, float dp, float te, float temp, float ias, float tas,
		float mc, int bugs, float aballast, bool cruise, float alt, bool validTemp, float acc_x, float acc_y, float acc_z, float gx, float gy, float gz  ){
	if( !validTemp )
		temp=0;

	if( proto == P_XCVARIO ){
		/*
				Sentence has following format:
				$PXCV,
				BBB.B = Vario, -30 to +30 m/s, negative sign for sink,
				C.C = MacCready 0 to 10 m/s
				EE = bugs degradation, 0 = clean to 30 %,
				F.FF = Ballast 1.00 to 1.60,
				G = 0 in climb, 1 in cruise,
				HH.H = Outside airtemp in degrees celcius ( may have leading negative sign ),
				QQQQ.Q = QNH e.g. 1013.2,
				PPPP.P: static pressure in hPa,
				QQQQ.Q: dynamic pressure in Pa,
				RRR.R: roll angle,
				III.I: pitch angle,
				X.XX:   acceleration in X-Axis,
				Y.YY:   acceleration in Y-Axis,
				Z.ZZ:   acceleration in Z-Axis,
		 *CHK = standard NMEA checksum
		 */
		sprintf(str,"$PXCV,%3.1f,%1.1f,%d,%1.2f,%d,%2.1f,%4.1f,%4.1f,%.1f", te, Units::Vario2ms(mc), bugs, (aballast+100)/100.0, cruise, std::roundf(temp*10.f)/10.f, QNH.get(), baro, dp );
		int append_idx = strlen(str);
		if( haveMPU && attitude_indicator.get() ){
			float roll = IMU::getRoll();
			float pitch = IMU::getPitch();
			sprintf(str+append_idx,",%3.1f,%3.1f,%1.2f,%1.2f,%1.2f", roll, pitch, acc_x, acc_y, acc_z );

		}else{
			sprintf(str+append_idx,",,,,,");
		}
	}
	else if( proto == P_OPENVARIO ) {
		if( validTemp )
			sprintf(str,"$POV,P,%0.1f,Q,%0.1f,E,%0.1f,T,%0.1f",baro,dp,te,temp);
		else
			sprintf(str,"$POV,P,%0.1f,Q,%0.1f,E,%0.1f",baro,dp,te);
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
		 */
		float iaskn = Units::kmh2knots( ias );
		sprintf(str,"$PBB50,%03d,%3.1f,%1.1f,%d,%d,%1.2f,%1d,%2d", (int)(Units::kmh2knots(tas)+0.5), Units::ms2knots(te), Units::mcval2knots(mc), (int)((iaskn*iaskn)+0.5), bugs, (aballast+100)/100.0, cruise, (int)(temp+0.5) );
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

		sprintf(str, "!w,0,0,0,0,%d,%d,%d,%d,0,0,%d,%d,%d", int(alt+1000), (int)(QNH.get()), int(Units::kmh2ms(tas)*100), int((Units::ms2knots(te)*10)+200), int( Units::mcval2knots(mc)*10 ), int( ballast_percent ), (int)(100-bugs) );
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
		float roll = IMU::getRoll();
		float pitch = IMU::getPitch();
		// ESP_LOGI(FNAME,"roll %.2f pitch %.2f yaw %.2f", roll, pitch, yaw  );
		/*
			$PEYI,%.2f,%.2f,,,,%.2f,%.2f,%.2f,,%.2f,
			lbank,         // Bank == roll    (deg)           SRC
			lpitch,         // pItch           (deg)
			x,
			y,
			z,
			);
		 */
		sprintf(str, "$PEYI,%.2f,%.2f,,,,%.2f,%.2f,%.2f,,", roll, pitch,acc_x,acc_y,acc_z );
	}
	else if( proto == P_AHRS_APENV1 ) { // experimental
		sprintf(str, "$APENV1,%.2f,%.2f,0,0,0,%.2f,", ias,alt,te );
	}
	else if( proto == P_AHRS_RPYL ) {   // experimental
		sprintf(str, "$RPYL,%.2f,%.2f,0,0,,%.2f,0,",
				IMU::getRoll(),         // Bank == roll    (deg)           SRC
				IMU::getPitch(),         // pItch           (deg)
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
		sprintf(str, "$PTAS1,%d,%d,%d,%d", int((Units::ms2knots(te)*10)+200), 0, int(Units::meters2feet(alt)+2000), int(Units::kmh2knots(tas)+0.5) );
	}
	else {
		ESP_LOGW(FNAME,"Not supported protocol %d", nmea_protocol.get() );
	}
	int cs = calcNMEACheckSum(&str[1]);
	int i = strlen(str);
	sprintf( &str[i], "*%02X\r\n", cs );
	Router::sendXCV(str);
}

int countPFLAU=0;

int tickNMEA=0;
float Protocols::_mc_prev = 0.5;
float Protocols::_qnh_prev = 0.5;

// The XCVario Protocol or Cambridge CAI302 protocol to adjust MC,Ballast,Bugs.

char * mystrtok(char *s)
{
	int i, c;
	for (i = 0; i < strlen(s); i++) {
		c = (unsigned char)s[i];
		if (c == '$' || c == '!')
			return &s[i];
	}
	return 0;
}


void Protocols::parseNMEA( char *astr ){
	// ESP_LOGI(FNAME,"parseNMEA: %s", astr );
	char *str = mystrtok(astr);
	while( str ){
		ESP_LOGV(FNAME,"parseNMEA token: %s", str);
		tickNMEA++;
		if ( strncmp( str, "!xw,", 4 ) == 0 ) {
			float wkcmd;
			sscanf( str,"!xw,%f", &wkcmd );  // directly scan into sensor variable
			Flap::setLever( wkcmd );
			// ESP_LOGI(FNAME,"XW command detected wk=%f", wkcmd );
		}
		else if( strncmp( str, "!xd,", 4 ) == 0 ) {
			float dir, speed;
			int type;
			sscanf( str,"!xd,%f,%f,%d", &dir,&speed, &type );
			ESP_LOGI(FNAME,"New wind type: %d detected %3.1f/%3.1f", type, dir, speed );
			if( type == WA_STRAIGHT ){
				theWind.setWind( dir, speed );
			}
			else if( type == WA_CIRCLING ){
				CircleWind::setWind( dir, speed );
			}
		}
		else if ( strncmp( str, "!xt,", 4 ) == 0 ) {
			float temp;
			sscanf( str,"!xt,%f", &temp );  // directly scan into sensor variable
			temperature = temp;
			validTemperature=true;
			ESP_LOGI(FNAME,"T change detected T=%2.1f", temp );
		}
		else if ( strncmp( str, "!xa,", 4 ) == 0 ) {
			float climb;
			sscanf( str,"!xa,%f", &climb );  // directly scan into sensor variable
			meanClimb = climb;
			// ESP_LOGI(FNAME,"mean climb change detected mean climb=%f", climb );
		}
		else if ( strncmp( str, "!xm,", 4 ) == 0 ) {
			float mc;
			sscanf( str,"!xm,%f", &mc );
			ESP_LOGI(FNAME,"MC change %s detected, new MC %f", str, mc );
			if( MC.get() != mc ){
				MC.set( mc );
			}
		}
		else if ( strncmp( str, "!xu,", 4 ) == 0 ) {
			float _bugs;
			sscanf( str,"!xu,%f", &_bugs );
			ESP_LOGI(FNAME,"bugs change from client detected=%f", _bugs );
			if( bugs.get() != _bugs ){
				bugs.set( _bugs );
			}
		}
		else if ( strncmp( str, "!xc,", 4 ) == 0 ) {
				float h;
				sscanf( str,"!xc,%f", &h );
				// ESP_LOGI(FNAME,"Compass heading detected=%3.1f", h );
				Compass::setHeading( h );
		}
		else if ( strncmp( str, "!xq,", 4 ) == 0 ) {
			float qnh;
			sscanf( str,"!xq,%f", &qnh );  // directly scan into sensor variable
			if( QNH.get() != qnh )
				QNH.set( qnh );
			ESP_LOGI(FNAME,"QNH change cmd new QNH: %f, cmd: %s", qnh, str );
		}
		else if ( strncmp( str, "!xb,", 4 ) == 0 ) {
			float aballast;
			sscanf(str, "!xb,%f", &aballast);
			ballast.set( aballast );
			ESP_LOGI(FNAME,"parseNMEA, internal ballast modification %s new ballast: %f", str, aballast );
			_s2f->change_mc_bal();
		}
		else if ( strncmp( str, "!xsM,", 5 ) == 0 && wireless == WL_WLAN_CLIENT ) {
			ESP_LOGI(FNAME,"parseNMEA %s", str );
			char key[20];
			char type;
			int  length;
			sscanf(str, "!xsM,%[^,],%c", key,&type );
			ESP_LOGI(FNAME,"parseNMEA type=%c key=%s", type , key );
			if( type == 'F' ){
				float valf;
				sscanf(str, "!xsM,%[^,],%c,%d,%f", key,&type,&length, &valf );
				ESP_LOGI(FNAME,"parseNMEA float value=%f", valf );
				SetupNG<float> *item = (SetupNG<float> *)SetupCommon::getMember( key );
				if( item != 0 ){
					item->set( valf );
				}else
					ESP_LOGW(FNAME,"Setup item with key %s not found", key );
			}
			else if( type == 'I' ){
				int vali;
				sscanf(str, "!xsM,%[^,],%c,%d,%d", key,&type,&length, &vali );
				ESP_LOGI(FNAME,"parseNMEA %s val=%d", str, vali );
			}
		}

		else if ( (strncmp( str, "!g,", 3 ) == 0)    ) {
			ESP_LOGI(FNAME,"parseNMEA, Cambridge C302 style command !g detected: %s",str);
			if (str[3] == 'b' && wireless != WL_WLAN_CLIENT ) {  // we run own protocol between Master and Client as of bad precision in official
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
				if( wireless == WL_WLAN ) {// update also client from Master
					delay( 500 );
					OV.sendBallastChange( ballast.get(), false );
				}
			}
			if (str[3] == 'm' ) {
				ESP_LOGI(FNAME,"parseNMEA, BORGELT, MC modification");
				float mc;
				sscanf(str, "!g,m%f", &mc);
				mc = mc*0.1;   // comes in knots*10, unify to knots
				float mc_ms =  std::roundf(Units::knots2ms(mc)*10.f)/10.f; // hide rough knot resolution
				ESP_LOGI(FNAME,"New MC: %1.1f knots, %f", mc, mc_ms );
				MC.set( Units::Vario( mc_ms ) );  // set mc according corresponding vario units
				_s2f->change_mc_bal();
				if( wireless == WL_WLAN ) {// update also client from Master
					delay( 500 );
					OV.sendMcChange( MC.get() );
					ESP_LOGI(FNAME,"MC BG change detected, new MC %f", mc_ms );
				}
			}
			if (str[3] == 'u') {
				int mybugs;
				sscanf(str, "!g,u%d", &mybugs);
				mybugs = 100-mybugs;
				ESP_LOGI(FNAME,"New Bugs: %d %%", mybugs);
				bugs.set( mybugs );
				_s2f->change_mc_bal();
				if( wireless == WL_WLAN ) {// update also client from Master
					delay( 500 );
					OV.sendBugsChange( bugs.get() );
				}
			}
		}
		else if( !strncmp( str, "$PXCV,", 5 ) ){   // $PXCV,-0.0,0.5,0,1.00,0,24.4,1013.2,990.8, 0.0,0.2,-29.2,-0.45,0.01,0.80*2C
			/*
				$PXCV,
				BBB.B = Vario, -30 to +30 m/s, negative sign for sink,
				C.C = MacCready 0 to 10 m/s
				EE = bugs degradation, 0 = clean to 30 %,
				F.FF = Ballast 1.00 to 1.60,
				G = 0 in climb, 1 in cruise,
				HH.H = Outside airtemp in degrees celcius ( may have leading negative sign ),
				QQQQ.Q = QNH e.g. 1013.2,
				PPPP.P: static pressure in hPa,
				QQQQ.Q: dynamic pressure in Pa,
				RRR.R: roll angle,
				III.I: pitch angle,
				X.XX:   acceleration in X-Axis,
				Y.YY:   acceleration in Y-Axis,
				Z.ZZ:   acceleration in Z-Axis,
			 *CHK = standard NMEA checksum
			 */
			// tbd: checksum check
			// ESP_LOGI(FNAME,"parseNMEA, PXCV %s", str);
			float _te, _mc, _bugs,_ballast, _temp, _qnh, _baro, _pitot;
			int _cs, _cruise;
			sscanf( str, "$PXCV,%f,%f,%f,%f,%d,%f,%f,%f,%f%*[^*]*%2x", &_te, &_mc, &_bugs, &_ballast,&_cruise, &_temp, &_qnh, &_baro, &_pitot, &_cs  );
			int calc_cs=calcNMEACheckSum( str );
			if( _cs != calc_cs )
				ESP_LOGW(FNAME,"CHECKSUM ERROR: %s; calculcated CS: %d != delivered CS %d", str, calc_cs, _cs );
			else{
				TE = _te;
				baroP = _baro;
				dynamicP = _pitot;
				float iasraw= Atmosphere::pascal2kmh( dynamicP );
				Switch::setCruiseModeXCV( _cruise );
				ias = ias + (iasraw - ias)*0.25;
				tas += ( Atmosphere::TAS( iasraw , _baro, _temp) - tas)*0.25;
				alt=Atmosphere::calcAltitude( QNH.get(), _baro );
				aTE += (TE - aTE)* (1/(10*vario_av_delay.get()));
			}
			// ESP_LOGI(FNAME,"parseNMEA, $PXCV TE=%2.1f T=%2.1f Baro=%4.1f Pitot=%4.1f IAS:%3.1f", _te, _temp, _baro, _pitot, ias);
		}
		else if( !strncmp( str, "$PFLAU,", 6 )) {
			Flarm::parsePFLAU( str );
			if( Flarm::bincom  ) {
				Flarm::bincom--;
				ESP_LOGI(FNAME,"Flarm::bincom %d", Flarm::bincom  );
			}
		}
		else if( !strncmp( str, "$GPRMC,", 6 )) {
			Flarm::parseGPRMC( str );
			if( Flarm::bincom  ) {
				Flarm::bincom--;
				ESP_LOGI(FNAME,"Flarm::bincom %d", Flarm::bincom  );
			}
		}
		else if( !strncmp( str, "$GPGGA,", 6 )) {
			Flarm::parseGPGGA( str );
			if( Flarm::bincom  ) {
				Flarm::bincom--;
				ESP_LOGI(FNAME,"Flarm::bincom %d", Flarm::bincom  );
			}
		}
		else if( !strncmp( str, "!w,", 2 ) ){
			/*
			 * Cambridge 302 Format
			!W,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>,<13>*hh<CR><LF>
			<1>    Vector wind direction in degrees
			<2>    Vector wind speed in 10ths of meters per second
			<3>    Vector wind age in seconds
			<4>    Component wind in 10ths of Meters per second + 500 (500 = 0, 495 = 0.5 m/s tailwind)
			<5>    True altitude in Meters + 1000
			<6>    Instrument QNH setting
			<7>    True airspeed in 100ths of Meters per second  ( kilometers per second )
			<8>    Variometer reading in 10ths of knots + 200
			<9>    Averager reading in 10ths of knots + 200
			<10>   Relative variometer reading in 10ths of knots + 200
			<11>   Instrument MacCready setting in 10ths of knots
			<12>   Instrument Ballast setting in percent of capacity
			<13>   Instrument Bug setting
			 *hh   Checksum, XOR of all bytes of the sentence after the ‘!’ and before the ‘*’
			 */
			float _alt, _qnh, _tas, _te, _mc, _ballast, _bugs;
			int _cs;
			sscanf(str, "!w,0,0,0,0,%f,%f,%f,%f,0,0,%f,%f,%f*%02x", &_alt, &_qnh, &_tas, &_te, &_mc, &_ballast, &_bugs, &_cs );
			int calc_cs=calcNMEACheckSum( str );
			if( _cs != calc_cs )
				ESP_LOGW(FNAME,"CHECKSUM ERROR: %s; calculcated CS: %02x != delivered CS %02x", str, calc_cs, _cs );
			else{
				alt = _alt-1000;
				if( _qnh_prev != _qnh ){
					QNH.set( _qnh );
					_qnh_prev = _qnh;
				}
				tas = Units::ms2kmh( _tas )/100;    // True airspeed in 100ths of Meters per second
				ias = Atmosphere::IAS( tas , alt, temperature );
				TE = Units::knots2ms( _te - 200 )/10;
				aTE += (TE - aTE)* (1/(10*vario_av_delay.get()));
				// ESP_LOGI(FNAME,"parseNMEA/Cambridge %s: alt=%4.1f QNH=%4.1f TE=%2.1f TAS:%3.1f",str, alt, _qnh, TE, tas);
			}
		}
		str++;
		str = mystrtok(str);
	}
}


// Calculate the checksum and output it as an int
// is required as HEX in the NMEA data set
// between $ or! and * character
int Protocols::calcNMEACheckSum(char * nmea) {
	int i, XOR, c;
	for (XOR = 0, i = 0; i < strlen(nmea); i++) {
		c = (unsigned char)nmea[i];
		if (c == '*') break;
		if ((c != '$') && (c != '!')) XOR ^= c;
	}
	return XOR;
}

int Protocols::getNMEACheckSum(char * nmea) {
	int i, cs, c;
	for (i = 0; i < strlen(nmea); i++) {
		c = (unsigned char)nmea[i];
		if (c == '*') break;
	}
	sscanf( &nmea[i],"*%02x", &cs );
	return cs;
}

