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
#include "Blackboard.h"
#include "SetupNG.h"
#include "CircleWind.h"

S2F *   Protocols::_s2f = 0;
uint8_t Protocols::_protocol_version = 1;
bool    Protocols::_can_send_error = false;

char stream[100];

Protocols::Protocols(S2F * s2f) {
	_s2f = s2f;
}

Protocols::~Protocols() {

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

void Protocols::sendItem( const char *key, char type, void *value, int len, bool ack ){
	if( Flarm::bincom )  // do not sent to client in case
		return;
	// ESP_LOGI(FNAME,"sendItem: %s", key );
	char str[40];
	char sender = 'U';
	if( SetupCommon::isMaster()  )
		sender='M';
	else if( SetupCommon::isClient() ){
		if( ack )
			sender='A'; // ack from client
		else
			sender='C';
	}
	// ESP_LOGI(FNAME,"sender: %c", sender );
	if( sender != 'U' ) {
		int l = sprintf( str,"!xs%c,%s,%c,", sender, key, type );
		if( type == 'F' )
			sprintf( str+l,"%.3f", *(float*)(value) );
		else if( type == 'I' )
			sprintf( str+l,"%d", *(int*)(value) );

		int cs = calcNMEACheckSum(&str[1]);
		int i = strlen(str);
		sprintf( &str[i], "*%02X\r\n", cs );
		// ESP_LOGI(FNAME,"sendNMEAString: %s", str );
		SString nmea( str );
		if( !Router::forwardMsg( nmea, can_tx_q ) ){
			if( !_can_send_error ){
				_can_send_error = true;
				ESP_LOGW(FNAME,"Permanent send msg to XCV client XCV (%d bytes) failure", nmea.length() );
			}
		}
		else if( _can_send_error ){
			_can_send_error = false;
			ESP_LOGI(FNAME,"Okay again send msg to XCV client (%d bytes)", nmea.length() );
		}
	}
}

void Protocols::sendNmeaXCVCmd( const char *item, float value ){
	if( Flarm::bincom )  // do not sent to client in case
		return;
	// ESP_LOGI(FNAME,"sendNMEACmd: %s: %f", item, value );
	char str[40];
	sprintf( str,"!xcv,%s,%d", item, (int)(value+0.5) );
	int cs = calcNMEACheckSum(&str[1]);
	int i = strlen(str);
	sprintf( &str[i], "*%02X\r\n", cs );
	ESP_LOGI(FNAME,"sendNMEACmd: %s", str );
	SString nmea( str );
	if( !Router::forwardMsg( nmea, xcv_tx_q ) ){
		ESP_LOGW(FNAME,"Warning: Overrun in send to XCV tx queue %d bytes", nmea.length() );
	}
}

void Protocols::sendNMEA( proto_t proto, char* str, float baro, float dp, float te, float temp, float ias, float tas,
		float mc, int bugs, float aballast, bool cruise, float alt, bool validTemp, float acc_x, float acc_y, float acc_z, float gx, float gy, float gz ){
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
		float bal = (aballast+100)/100.0;
		// ESP_LOGW(FNAME,"Ballast: %f %1.2f", bal, bal );
		if( Protocols::getXcvProtocolVersion() > 1 ){
			// omit ballast overload field
			sprintf(str,"$PXCV,%3.1f,%1.2f,%d,,%d,%2.1f,%4.1f,%4.1f,%.1f", te, mc, bugs, !cruise, std::roundf(temp*10.f)/10.f, QNH.get() , baro, dp );

		}else{
			// legacy send ballast overload
			sprintf(str,"$PXCV,%3.1f,%1.2f,%d,%1.3f,%d,%2.1f,%4.1f,%4.1f,%.1f", te, mc, bugs, bal, !cruise, std::roundf(temp*10.f)/10.f, QNH.get() , baro, dp );
		}
		int append_idx = strlen(str);
		if( gflags.haveMPU && attitude_indicator.get() ){
			float roll = IMU::getRoll();
			float pitch = IMU::getXCSPitch();
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
		sprintf(str,"$PBB50,%03d,%3.1f,%1.1f,%d,%d,%1.2f,%1d,%2d", (int)(Units::kmh2knots(tas)+0.5), Units::ms2knots(te), Units::mcval2knots(mc), (int)((iaskn*iaskn)+0.5), bugs, (aballast+100)/100.0, !cruise, (int)(temp+0.5) );
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
		int cballast = int((100*ballast_kg.get() / polar_max_ballast.get()));
		sprintf(str, "!w,0,0,0,0,%d,%4.2f,%d,%d,0,0,%d,%d,%d", int(alt+1000), QNH.get(), int(Units::kmh2ms(tas)*100), int((Units::ms2knots(te)*10)+200), int( Units::mcval2knots(mc)*10 ), cballast, (int)(100-bugs) );
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
		float pa = alt - 8.92*( QNH.get() - 1013.25);


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
	else if( gflags.haveMPU && attitude_indicator.get() && (proto == P_AHRS_APENV1) ) {  // LEVIL_AHRS
		sprintf(str, "$APENV1,%d,%d,0,0,0,%d", (int)(Units::kmh2knots(ias)+0.5),(int)(Units::meters2feet(alt)+0.5),(int)(Units::ms2fpm(te)+0.5));
	}
	else if( gflags.haveMPU && attitude_indicator.get() && (proto == P_AHRS_RPYL) ) {   // LEVIL_AHRS  $RPYL,Roll,Pitch,MagnHeading,SideSlip,YawRate,G,errorcode,
		sprintf(str, "$RPYL,%d,%d,%d,0,0,%d,0",
				(int)(IMU::getRoll()*10+0.5),         // Bank == roll     (deg)
				(int)(IMU::getPitch()*10+0.5),        // Pitch            (deg)
				(int)(IMU::getYaw()*10+0.5),          // Magnetic Heading (deg)
				(int)(acc_z*1000.0+0.5)
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

// The XCVario Protocol or Cambridge CAI302 protocol to adjust MC,Ballast,Bugs.

void Protocols::parseXS( const char *str ){
	// ESP_LOGI(FNAME,"parseXS %s", str );
	char key[20];
	char type;
	char role; // M | C
	int cs;
	float val;
	sscanf(str, "!xs%c,%[^,],%c,%f*%02x", &role, key, &type, &val, &cs );  // !xsM,FLPS,F,1.5000*27
	int calc_cs=calcNMEACheckSum( str );
	if( cs == calc_cs ){
		// ESP_LOGI(FNAME,"parsed NMEA: role=%c type=%c key=%s val=%f vali=%d", role, type , key, val, (int)val );
		if( type == 'F' ){
			SetupNG<float> *item = (SetupNG<float> *)SetupCommon::getMember( key );
			if( item != 0 ){
				if( role == 'A' )
					item->ack( val );
				else
					item->set( val, false );
			}else
				ESP_LOGW(FNAME,"Setup item with key %s not found", key );
		}
		else if( type == 'I' ){
			SetupNG<int> *item = (SetupNG<int> *)SetupCommon::getMember( key );
			if( item != 0 ){
				if( role == 'A' && val == item->get() )
					item->ack( val );
				else
					item->set( (int)val, false );
			}else
				ESP_LOGW(FNAME,"Setup item with key %s not found", key );
		}

	}else{
		ESP_LOGW(FNAME,"!xs messgae CS error got:%X != calculated:%X", cs, calc_cs );
		ESP_LOG_BUFFER_HEXDUMP(FNAME,str,32, ESP_LOG_WARN);
	}
}


void Protocols::parseNMEA( const char *str ){
	// ESP_LOGI(FNAME,"parseNMEA: %s, len: %d", str,  strlen(str) );

	if ( strncmp( str, "!xc,", 4 ) == 0 ) { // need this to support Wind Simulator with Compass simulation
		float heading;
		float TAS;
		sscanf( str,"!xc,%f,%f", &heading, &TAS );
		ESP_LOGI(FNAME,"Compass heading detected=%3.1f TAS: %3.1f NMEA:%s", heading, TAS, str );
		if( compass )
			compass->setHeading( heading );
		tas = TAS;
	}
	else if ( strncmp( str, "!xcs,", 5 ) == 0 ) {
		if( strncmp( str+5, "crew-weight,", 12 ) == 0 ){
			ESP_LOGI(FNAME,"Detected crew-weight cmd");
			int weight;
			int cs;
			sscanf(str+17, "%d*%02x", &weight, &cs);
			int calc_cs=calcNMEACheckSum( str );
			if( calc_cs != cs ){
				ESP_LOGW(FNAME,"CS Error in %s; %d != %d", str, cs, calc_cs );
			}
			else{
				ESP_LOGI(FNAME,"New crew-weight: %d", weight );
				crew_weight.set( (float)weight );
			}
		}
		else if( strncmp( str+5, "empty-weight,", 13 ) == 0 ){
			ESP_LOGI(FNAME,"Detected empty-weight cmd");
			int weight;
			int cs;
			sscanf(str+18, "%d*%02x", &weight, &cs);
			int calc_cs=calcNMEACheckSum( str );
			if( calc_cs != cs ){
				ESP_LOGW(FNAME,"CS Error in %s; %d != %d", str, cs, calc_cs );
			}
			else{
				ESP_LOGI(FNAME,"New empty_weight: %d", weight );
				empty_weight.set( (float)weight );
			}
		}
		else if( strncmp( str+5, "bal-water,", 10 ) == 0 ){
			ESP_LOGI(FNAME,"Detected bal_water cmd");
			int weight;
			int cs;
			sscanf(str+15, "%d*%02x", &weight, &cs);
			int calc_cs=calcNMEACheckSum( str );
			if( calc_cs != cs ){
				ESP_LOGW(FNAME,"CS Error in %s; %d != %d", str, cs, calc_cs );
			}
			else{
				ESP_LOGI(FNAME,"New ballast: %d", weight );
				ballast_kg.set( (float)weight );
			}
		}
		else if( strncmp( str+5, "version,", 8 ) == 0 ){
			ESP_LOGI(FNAME,"Detected version cmd");
			int version;
			int cs;
			sscanf(str+13, "%d*%02x", &version, &cs);
			int calc_cs=calcNMEACheckSum( str );
			if( calc_cs != cs ){
				ESP_LOGW(FNAME,"CS Error in %s; %d != %d", str, cs, calc_cs );
			}
			else{
				ESP_LOGI(FNAME,"Got xcv protocol version: %d", version );
				_protocol_version = version;
				sendNmeaXCVCmd( "version", 2 );
			}
		}
	}
	else if ( (strncmp( str, "!g,", 3 ) == 0)    ) {
		ESP_LOGI(FNAME,"parseNMEA, Cambridge C302 style command !g detected: %s",str);
		if (str[3] == 'b') {  // this may reach master or client with an own navi
			ESP_LOGI(FNAME,"parseNMEA, BORGELT, ballast modification");
			float aballast;
			sscanf(str, "!g,b%f", &aballast);
			aballast = aballast * 10; // Its obviously only possible to change in fraction's by 10% in CA302 (issue: 464)
			ESP_LOGI(FNAME,"New Ballast: %.1f %% of max ", aballast);
			float liters = (aballast/100.0) * polar_max_ballast.get();
			ESP_LOGI(FNAME,"New Ballast in liters/kg: %.1f ", liters);
			ballast_kg.set( liters );
		}
		if (str[3] == 'm' ) {
			ESP_LOGI(FNAME,"parseNMEA, BORGELT, MC modification");
			float mc;
			sscanf(str, "!g,m%f", &mc);
			mc = mc*0.1;   // comes in knots*10, unify to knots
			float mc_ms =  std::roundf(Units::knots2ms(mc)*10.f)/10.f; // hide rough knot resolution
			if( vario_unit.get() == VARIO_UNIT_KNOTS )
				mc_ms =  std::roundf(Units::knots2ms(mc)*100.f)/100.f; // higher resolution for knots
			ESP_LOGI(FNAME,"New MC: %1.1f knots, %f m/s", mc, mc_ms );
			MC.set( mc_ms );  // set mc in m/s
		}
		if (str[3] == 'u') {
			int mybugs;
			sscanf(str, "!g,u%d", &mybugs);
			mybugs = 100-mybugs;
			ESP_LOGI(FNAME,"New Bugs: %d %%", mybugs);
			bugs.set( mybugs );
		}
		if (str[3] == 'q') {  // nonstandard CAI 302 extension for QNH setting in XCVario in int or float e.g. 1013 or 1020.20
			float qnh;
			sscanf(str, "!g,q%f", &qnh);
			ESP_LOGI(FNAME,"New QNH: %.2f", qnh);
			QNH.set( qnh );
		}
	}
	else if( strncmp( str, "$g,", 3 ) == 0 ) {
		ESP_LOGI(FNAME,"Remote cmd %s", str );
		if (str[3] == 's') {  // nonstandard CAI 302 extension for S2F mode switch, e.g. for XCNav remote stick
			ESP_LOGI(FNAME,"Detected S2F cmd");
			int mode;
			int cs;
			sscanf(str, "$g,s%d*%02x", &mode, &cs);
			int calc_cs=calcNMEACheckSum( str );
			if( calc_cs != cs ){
				ESP_LOGW(FNAME,"CS Error in %s; %d != %d", str, cs, calc_cs );
			}
			else{
				ESP_LOGI(FNAME,"New S2F mode: %d", mode );
				cruise_mode.set( mode );
			}
		}
		if (str[3] == 'v') {  // nonstandard CAI 302 extension for volume Up/Down, e.g. for XCNav remote stick
			int steps;
			int cs;
			ESP_LOGI(FNAME,"Detected volume cmd");
			sscanf(str, "$g,v%d*%02x", &steps, &cs);
			int calc_cs=calcNMEACheckSum( str );
			if( calc_cs != cs ){
				ESP_LOGW(FNAME,"CS Error: in %s; %d != %d", str, cs, calc_cs );
			}
			else{
				float v = audio_volume.get() + steps;
				if( v<=100.0 && v >= 0.0 ){
					audio_volume.set( v );
					ESP_LOGI(FNAME,"Volume change: %d steps, new volume: %.0f", steps, v );
				}else
					ESP_LOGI(FNAME,"Volume change limit reached steps: %d volume: %.0f", steps, v );
			}
		}
		if (str[3] == 'r') {  // nonstandard CAI 302 extension for Rotary Movement, e.g. for XCNav remote stick to navigate
			char func;
			int cs;
			ESP_LOGI(FNAME,"Detected rotary message");
			sscanf(str, "$g,r%c*%02x", &func, &cs);
			int calc_cs=calcNMEACheckSum( str );
			if( calc_cs != cs ){
				ESP_LOGW(FNAME,"CS Error: in %s; %x != %x", str, cs, calc_cs );
			}
			else{
				if( func == 'p' ){
					ESP_LOGI(FNAME,"Short Press");
					ESPRotary::sendPress();
					ESPRotary::sendRelease();
				}else if( func == 'l' ){
					ESP_LOGI(FNAME,"Long Press" );
					ESPRotary::sendLongPress();
					ESPRotary::sendRelease();
				}else if( func == 'u' ){
					ESP_LOGI(FNAME,"Up");
					ESPRotary::sendUp(1);
				}else if( func == 'd' ){
					ESP_LOGI(FNAME,"Down");
					ESPRotary::sendDown(1);
				}
				else if( func == 'x' ){
					ESP_LOGI(FNAME,"Escape");
					ESPRotary::sendEsc();
				}
			}
		}
		if (str[3] == 'w') {  // nonstandard CAI 302 extension for gear warning enable/disable
			int gear;
			int cs;
			ESP_LOGI(FNAME,"Detected gear warning message");
			sscanf(str, "$g,w%d*%02x", &gear, &cs);
			int calc_cs=calcNMEACheckSum( str );
			if( calc_cs != cs ){
				ESP_LOGW(FNAME,"CS Error: in %s; %x != %x", str, cs, calc_cs );
			}
			else{
				if( gear == 0 ){
					ESP_LOGI(FNAME,"Gear warning disable");
					gflags.gear_warn_external = false;
				}else if( gear == 1 ){
					ESP_LOGI(FNAME,"Gear warning enable");
					gflags.gear_warn_external = true;
				}
			}
		}
	}
	else if( !strncmp( str+1, "PFLAE,", 5 )) {  // On Task declaration or re-connect
		Flarm::parsePFLAE( str );
		ageBincom();
	}
	else if( !strncmp( str+1, "PFLAU,", 5 )) {
		Flarm::parsePFLAU( str );
		ageBincom();
	}
	else if( !strncmp( str+3, "RMC,", 3 ) ) {
		Flarm::parseGPRMC( str );
		ageBincom();
	}
	else if( !strncmp( str+3, "GGA,", 3 )) {
		Flarm::parseGPGGA( str );
		ageBincom();
	}
	else if( !strncmp( str+3, "RMZ,", 3 )) {
		Flarm::parsePGRMZ( str );
		ageBincom();
	}
	else if( !strncmp( str, "$FT", 3 ) ) {
		if (str[3] == '0') {
			IMUstream = false; // no FT stream
			SENstream = false;
			CALstream = false;
		}
		else if (str[3] == '1') {
			IMUstream = true; // IMU stream
			SENstream = false;
		}
		else if (str[3] == '2') {
			IMUstream = false; // SEN stream
			SENstream = true;
		}
		else if (str[3] == '3') {
			IMUstream = true; // IMU and SEN stream
			SENstream = true;
		}
		else if (str[3] == '4') {
			IMUstream = false;
			SENstream = false;			
			CALstream = true; // Accel calibration stream
		}
	} else if( !strncmp( str, "$ACC", 4 ) ) {
		mpud::float_axes_t AccBias;	
		mpud::float_axes_t AccGain;
		sscanf( str,"$ACC,%f,%f,%f,%f,%f,%f",&AccBias.x,&AccBias.y,&AccBias.z,&AccGain.x,&AccGain.y,&AccGain.z);
			
			if ( (abs(AccBias.x) < 1) && (abs(AccBias.y) < 1) && (abs(AccBias.z) < 1) && (abs(AccGain.x-1) < 0.2) && (abs(AccGain.y-1) < 0.2) && (abs(AccGain.z-1) < 0.2) ) {
			accl_bias.set(AccBias);
			accl_gain.set(AccGain);
			delay(100);
			AccBias = accl_bias.get();
			AccGain = accl_gain.get();
			sprintf(stream,"$ACC IN FLASH, %1.4f, %1.4f, %1.4f, %1.4f, %1.4f, %1.4f\r\n",AccBias.x,AccBias.y,AccBias.z,AccGain.x,AccGain.y,AccGain.z );
			Router::sendXCV(stream);
			// reboot Vario so that new bias/gains are taken into effect
			delay(100);
			esp_restart();
		} else {
			sprintf(stream,"$ACC format error. Need to type: $ACC,bias_x,bias_y,bias_z,gain_x,gain_y,gain_z\r\n");
			Router::sendXCV(stream);
			sprintf(stream,"example: $ACC,0.02,-0.001,0.004,1.02,0.98,1.003\r\n");
			Router::sendXCV(stream);
		}
	} else if( !strncmp( str, "$INST", 5 ) ) {
		float xcv_tilt, xcv_sway, xcv_distCG;	
		sscanf( str,"$INST,%f,%f,%f",&xcv_tilt,&xcv_sway,&xcv_distCG);
		if ( (abs(xcv_tilt) < 0.4) && (abs(xcv_sway) < 0.4) && (xcv_distCG < 3) ) {
			tilt.set(xcv_tilt);
			sway.set(xcv_sway);
			distCG.set(xcv_distCG);
			delay(100);
			xcv_tilt = tilt.get();
			xcv_sway = sway.get();
			xcv_distCG = distCG.get();
			sprintf(stream,"$INST IN FLASH, %1.3f, %1.3f, %1.3f\r\n",xcv_tilt,xcv_sway,xcv_distCG);
			Router::sendXCV(stream);
			// reboot Vario so that new tilt, sway and distance to CG are taken in effect
			delay(100);
			esp_restart();
		} else {
			sprintf(stream,"$INST format error. Need to type: $INST,tilt,sway,dist_to_cg respectively in rad, rad and meter\r\n");
			Router::sendXCV(stream);
			sprintf(stream,"example: $INST,0.14,0.52,1.1\r\n");
			Router::sendXCV(stream);
		}		
	} else if( !strncmp( str, "$CAL", 4 ) ) {
		sscanf( str,"$INST,%f",&localGravity);
		if  (abs(localGravity - 9.807) < 0.1) {
			IMUstream = false;
			SENstream = false;			
			CALstream = true; // Accel calibration stream
		} else {
			sprintf(stream,"$CAL format error. Need to type: $CAL, local_gravity_value\r\n");
			Router::sendXCV(stream);
			sprintf(stream,"example: $CAL,9.803\r\n");
			Router::sendXCV(stream);
		}
	}
}

void Protocols::ageBincom(){
	if( Flarm::bincom  ) {
		Flarm::bincom--;
		ESP_LOGI(FNAME,"Flarm::bincom %d", Flarm::bincom  );
	}
}

// Calculate the checksum and output it as an int
// is required as HEX in the NMEA data set
// between $ or! and * character
int Protocols::calcNMEACheckSum(const char *nmea) {
	int i, XOR, c;
	for (XOR = 0, i = 0; i < strlen(nmea); i++) {
		c = (unsigned char)nmea[i];
		if (c == '*') break;
		if ((c != '$') && (c != '!')) XOR ^= c;
	}
	return XOR;
}

int Protocols::getNMEACheckSum(const char *nmea) {
	int i, cs, c;
	for (i = 0; i < strlen(nmea); i++) {
		c = (unsigned char)nmea[i];
		if (c == '*') break;
	}
	sscanf( &nmea[i],"*%02x", &cs );
	return cs;
}
