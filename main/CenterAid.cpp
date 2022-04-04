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


static const int X=75;
static const int Y=215;

#define SCALE_DOWN 4.0

CenterAid::CenterAid( AdaptUGC *display ) {
	ucg = display;
	last_heading = 0;
	_tick = 0;
	for( int i=0; i<CA_NUM_DIRS; i++ ){
		thermals[i] = 0;
		drawn_thermals[i] = 0;
	}
	idir = 0;
	agedir = 0;
	flightmode = undefined;
}

void CenterAid::drawThermal( int th, int idir, bool draw_sink ){
	// ESP_LOGI(FNAME,"drawThermal, th: %d, idir: %d, ds: %d", th, idir, draw_sink );
	if( idir > CA_NUM_DIRS || idir < 0 ){
		ESP_LOGE(FNAME,"index out of range: %d", agedir );
		return;
	}
	short int cy = Y-cos(D2R(idir*CA_STEP))*30;
	short int cx = X+sin(D2R(idir*CA_STEP))*30;
	if( drawn_thermals[idir] && (th <  drawn_thermals[idir]) ){
		ucg->setColor( COLOR_BLACK );
		int td = rint(drawn_thermals[idir]/SCALE_DOWN);
		td = td > MAX_DISK_RAD ? MAX_DISK_RAD : td;
		ucg->drawDisc(cx,cy, td, UCG_DRAW_ALL );
	}
	if( th > 0 ){
		int td = rint(th/SCALE_DOWN);
		td = td > MAX_DISK_RAD ? MAX_DISK_RAD : td;
		if( td > 0 ){
			// ESP_LOGI(FNAME,"drawThermal dir:%d, s:%d", idir, td );
			ucg->setColor( COLOR_GREEN );
			ucg->drawDisc(cx,cy, td, UCG_DRAW_ALL );
		}
		else if( draw_sink ){
			ucg->setColor( COLOR_RED );
			ucg->drawDisc(cx,cy, -td, UCG_DRAW_ALL );
		}
		drawn_thermals[idir] = th;
	}

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
	if( flightmode == circlingL ){
		agedir = idir-3;
		if( agedir < 0 )
			agedir += CA_NUM_DIRS;
	}
	else if( flightmode == circlingR ){
		agedir = idir+3;
	}
	else{
		agedir++;
	}
	agedir = agedir %CA_NUM_DIRS;
	if( agedir > CA_NUM_DIRS || agedir < 0 ){
		ESP_LOGE(FNAME,"index out of range: %d", agedir );
		return;
	}
	if( thermals[agedir] != 0 ){
		thermals[agedir] = (int8_t)( (float)(thermals[agedir])*0.8 );
	}
}

// every 100 mS
void CenterAid::tick(){
	_tick++;
	if( !(_tick%5) ) { // every 100 mS
		flightmode = CircleWind::getFlightMode();
		// ESP_LOGI(FNAME,"CenterAid tick %d fm: %d", _tick, flightmode );
		cur_heading = mag_hdt.get();
		if( cur_heading < 0 )  {          // fall back to GPS course
			if( Flarm::gpsStatus() ){
				// ESP_LOGI(FNAME,"Flarm GPS OK");
				cur_heading = Flarm::getGndCourse();
			}
		}
		checkThermal();
		ageThermal();  // 0.2 s per thermal = 4.8 seconds all 24 thermals aged by 0.1 m/s
	}
}

