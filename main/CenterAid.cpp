/*
 * CenterAid.cpp
 *
 *  Created on: Jan 23, 2022
 *      Author: Eckhard Völlm
 */

#include "CenterAid.h"
#include "types.h"
#include "vector.h"
#include "string.h"
#include "Flarm.h"
#include "sensor.h"
#include "AdaptUGC.h"
#include "KalmanMPU6050.h"
#include "sensor_processing_lib.h"
#include "sensor.h"

static const int X=75;
static const int Y=215;

#define SCALE_DOWN 4.0

CenterAid::CenterAid( AdaptUGC *display ) {
	ucg = display;
	_tick = 0;
	for( int i=0; i<CA_NUM_DIRS; i++ ){
		thermals[i] = 0;
		drawn_thermals[i] = 0;
	}
	idir = 0;
	agedir = 0;
	flightmode = undefined;
	turn_left = 0;
	turn_right = 0;
	fly_straight = 0;
	cur_heading = 0;
	gyro_last = 0;
}

void CenterAid::drawThermal( int th, int idir, bool draw_sink ){
	// ESP_LOGI(FNAME,"drawThermal, th: %d, idir: %d, ds: %d", th, idir, draw_sink );
	if( idir > CA_NUM_DIRS || idir < 0 ){
		ESP_LOGE(FNAME,"index out of range: %d", agedir );
		return;
	}
	short int cy = Y-cos(D2R(idir*CA_STEP))*30;
	short int cx = X+sin(D2R(idir*CA_STEP))*30;
	int td = rint(drawn_thermals[idir]/SCALE_DOWN);
	td = td > MAX_DISK_RAD ? MAX_DISK_RAD : td;
	int tn = rint(th/SCALE_DOWN);
	tn = tn > MAX_DISK_RAD ? MAX_DISK_RAD : tn;

	if( td && (tn <  td) ){
		ucg->setColor( COLOR_BLACK );
		int td = rint(drawn_thermals[idir]/SCALE_DOWN);
		td = td > MAX_DISK_RAD ? MAX_DISK_RAD : td;
		// ESP_LOGI(FNAME,"erase TH, dir:%d, TE:%d", idir, td );
		ucg->drawDisc(cx,cy, td, UCG_DRAW_ALL );
	}
	if( tn > 0 ){
		// ESP_LOGI(FNAME,"draw TH, dir:%d, TE:%d", idir, td );
		ucg->setColor( COLOR_GREEN );
		ucg->drawDisc(cx,cy, tn, UCG_DRAW_ALL );
	}
	else if( draw_sink ){
		ucg->setColor( COLOR_RED );
		ucg->drawDisc(cx,cy, -tn, UCG_DRAW_ALL );
	}
	drawn_thermals[idir] = th;
}

void CenterAid::checkThermal(){
	// ESP_LOGI(FNAME,"checkThermal");
	idir = rint(cur_heading-CA_STEP_2)/CA_STEP;
	if( idir > CA_NUM_DIRS || idir < 0 ){
		ESP_LOGE(FNAME,"index out of range: %d", idir );
		return;
	}
	int th = rint(te_vario.get()*10);
	// ESP_LOGI(FNAME,"newThermal int:%d, TE:%d", idir, th );
	addThermal( th  );
}

void CenterAid::drawCenterAid(){
	// ESP_LOGI(FNAME,"drawCenterAid");
	for( int i=0; i<CA_NUM_DIRS; i++ ){
		int d = (i+idir) % CA_NUM_DIRS;
		// ESP_LOGI(FNAME,"dir:%d TE:%d", d, thermals[d] );
		if( d < CA_NUM_DIRS && d >= 0 ){
			drawThermal( thermals[d], i );
		}else{
			ESP_LOGE(FNAME,"index out of range: %d", d );
		}
	}
	if( wind_display.get() == WD_NONE ){
		// ESP_LOGI(FNAME,"draw Airplane");
		ucg->setColor( COLOR_WHITE );
		Flarm::drawAirplane( X, Y, false, true );
	}
}

// add one thermal and draw thermal
void CenterAid::addThermal( int teval ){
	// ESP_LOGI(FNAME,"addThermal %.1f (%d), TE:%d", cur_heading, idir, teval );
	if( teval > thermals[ idir ]){
		if( idir > CA_NUM_DIRS || idir < 0 ){
			ESP_LOGE(FNAME,"index out of range: %d", idir );
		}else{
			thermals[ idir ] = teval;
		}
	}
}

void CenterAid::ageThermal(){
	// ESP_LOGI(FNAME,"age: dir %d, TH: %d, FM: %d", agedir, thermals[agedir], flightmode );
	float lambda = 0.6; // age faster in straight flight: we leaf quickly the place of lift
	if( flightmode == circlingL ){
		agedir--;
		if( agedir < 0 )
			agedir += CA_NUM_DIRS;
		lambda = 0.9;
	}
	else if( flightmode == circlingR ){
		agedir++;
		lambda = 0.9;
	}
	else{
		agedir++;
	}
	if( agedir < 0 )
		agedir += CA_NUM_DIRS;
	agedir = agedir %CA_NUM_DIRS;
	if( agedir > CA_NUM_DIRS || agedir < 0 ){
		ESP_LOGE(FNAME,"index out of range: %d", agedir );
		return;
	}
	if( thermals[agedir] != 0 ){
		thermals[agedir] = (int8_t)( (float)(thermals[agedir])*lambda);
	}
}


void CenterAid::calcFlightMode( float headingDiff ){
	// ESP_LOGI(FNAME,"calcFlightMode cur_head: %.1f hdiff:%.1f gs:%.0f GPSS:%d MH:%.1f FM:%d", cur_heading, headingDiff, getTAS(), Flarm::gpsStatus(), mag_hdt.get(), flightmode  );
	if( getTAS() < 25 ){
		if( flightmode != undefined ) {
			flightmode = undefined;
			ESP_LOGI(FNAME,"New fm: undefined, no AS");
		}
	}
	else{
		if( headingDiff > 4 ){
				if( turn_right < 4 )
					turn_right++;
				fly_straight = 0;
				if( flightmode != circlingR && turn_right > 2 ) {
					flightmode = circlingR;
					ESP_LOGI(FNAME,"New fm: circle right");
				}
		}
		else if( headingDiff < -4 ) {
			if( turn_left < 4 )
				turn_left++;
			fly_straight = 0;
			if( flightmode != circlingL && turn_left > 2 ){
				flightmode = circlingL;
				ESP_LOGI(FNAME,"New fm: circle left");
			}
		}
		else{
			turn_left = turn_right = 0;
			if( fly_straight < 4 )
				fly_straight++;
			if( flightmode != straight && fly_straight > 2 ) {
				flightmode = straight;
				ESP_LOGI(FNAME,"New fm: straight");
			}
		}
	}
}

// every 100 mS
void CenterAid::tick(){
	_tick++;
	if( !(_tick%5) ) { // every 100 mS
		// ESP_LOGI(FNAME,"CenterAid tick %d fm: %d", _tick, flightmode );
		float new_heading = -1.0;
		if( compass_enable.get() )  // this is the best source for a heading, use this when avail
			new_heading = mag_hdt.get();
		// ESP_LOGI(FNAME,"MH %f", new_heading );
		if( new_heading < 0 )  {         // fall back to GPS course and fuse gps heading with gyro
			if( Flarm::gpsStatus() ){
				if( gyro_last == 0 ){
					gyro_last = IMU::getYaw();
				}
				float gpshead = Flarm::getGndCourse();
				float gyro = IMU::getYaw();
				float gyro_delta =  gyro - gyro_last;
				gyro_last = gyro;
				float diff = Vector::angleDiffDeg( gpshead, gps_heading );
				gps_heading += (diff*0.01 + gyro_delta*1.07);
				new_heading=Vector::normalizeDeg( gps_heading );
				// ESP_LOGI(FNAME,"GPS OK TC:%f gdY:%f fused:%f diff:%f", gpshead, gyro_delta, new_heading, diff );
			}else{     // trust as last resort just only gyro for Center Aid
				new_heading = IMU::getYaw();
				// ESP_LOGI(FNAME,"Gyro yaw %f", new_heading );
			}
		}
		float diff = Vector::angleDiffDeg( new_heading, cur_heading );
		// ESP_LOGI(FNAME,"new heading %.1f diff:%.1f", new_heading, diff );
		if( new_heading != cur_heading ){
			uint64_t rts = esp_timer_get_time();
			float dt = (float)(rts - last_rts)/1000000.0;
			last_rts = rts;
			calcFlightMode( diff/dt ); // we calculate own flight mode here
			cur_heading = new_heading;
		}
		checkThermal();
		ageThermal();  // 0.2 s per thermal = 4.8 seconds all 24 thermals aged by 0.1 m/s
	}
}

