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
#include "setup/SetupNG.h"
#include "math.h"
#include "S2F.h"
#include <logdef.h>

#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "KalmanMPU6050.h"
#include "Atmosphere.h"
#include "Flarm.h"
#include "Units.h"
#include "Flap.h"
#include "Switch.h"
#include "Blackboard.h"
#include "setup/SetupNG.h"
#include "CircleWind.h"

S2F *   Protocols::_s2f = 0;
bool    Protocols::_can_send_error = false;
Protocols::Protocols(S2F * s2f) {
	_s2f = s2f;
}

Protocols::~Protocols() {
}

// The XCVario Protocol or Cambridge CAI302 protocol to adjust MC,Ballast,Bugs.


void Protocols::parseNMEA( const char *str ){
	// ESP_LOGI(FNAME,"parseNMEA: %s, len: %d", str,  strlen(str) );



	if( strncmp( str, "$g,", 3 ) == 0 ) {
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
				}else {
					ESP_LOGI(FNAME,"Volume change limit reached steps: %d volume: %.0f", steps, v );
				}
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
					Rotary->sendPress();
					Rotary->sendRelease();
				}else if( func == 'l' ){
					ESP_LOGI(FNAME,"Long Press" );
					Rotary->sendLongPress();
					Rotary->sendRelease();
				}else if( func == 'u' ){
					ESP_LOGI(FNAME,"Up");
					Rotary->sendRot(1);
				}else if( func == 'd' ){
					ESP_LOGI(FNAME,"Down");
					Rotary->sendRot(-1);
				}
				else if( func == 'x' ){
					ESP_LOGI(FNAME,"Escape");
					Rotary->sendEscape();
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
