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
	// ESP_LOGI(FNAME,"drawThermal, th: %d, idir: %d, ds: %d er: %d", th, idir, draw_sink, erase );
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

void CenterAid::newHeading(){
	idir = rint(cur_heading-CA_STEP_2)/CA_STEP;
	//  ESP_LOGI(FNAME,"newHeading: %d", idir );
	int th = rint(te_vario.get()*10);
	// ESP_LOGI(FNAME,"newHeading: %.2f, int TE:%d", dir, th );
	addThermal( th  );
}


void CenterAid::drawCenterAid(){
	// int rotate = rint( Vector::angleDiffDeg( cur_heading, 0 ))/CA_STEP; // reference is always the current heading
	if( flightmode == circlingL ){
		for( int i=0; i<CA_NUM_DIRS; i++ ){
			drawThermal( thermals[(i+idir) % CA_NUM_DIRS], i );
		}
	}else if( flightmode == circlingR ){
		for( int i=CA_NUM_DIRS-1; i>=0; i-- ){
			drawThermal( thermals[(i+idir) % CA_NUM_DIRS], i );
		}
	}
}

// add one thermal and draw thermal
void CenterAid::addThermal( int teval ){
	if( idir > CA_NUM_DIRS )
		return;
	// ESP_LOGI(FNAME,"addThermal %.1f (%d), TE:%d", cur_heading, idir, teval );
	if( teval > thermals[ idir ]){
		thermals[ idir ] = teval;
	}
}


void CenterAid::ageThermal(){
	// ESP_LOGI(FNAME,"age: dir %d, TH: %d, FM: %d", agedir, thermals[agedir], flightmode );
	if( flightmode == circlingL ){
		agedir = idir-3;
	}
	else if( flightmode == circlingR ){
		agedir = idir+3;
	}
	else{
		agedir++;
		agedir = agedir %CA_NUM_DIRS;
	}
	if( thermals[agedir] != 0 ){
		thermals[agedir] = (int8_t)( (float)(thermals[agedir])*0.8 );
	}
}

void CenterAid::fmStraight(){
	for( int i=0; i<CA_NUM_DIRS; i++ ){
		thermals[i] = 0;
		drawThermal( 0, i );
	}
}

// every 100 mS
void CenterAid::tick(){
	_tick++;
	t_circling fm = CircleWind::getFlightMode();
	if( fm != flightmode ){
		if( fm == straight ){
			fmStraight();
		}
		flightmode = fm;
	}
	cur_heading = mag_hdt.get();
	if( cur_heading < 0 )  {          // fall back to GPS course
		if( Flarm::gpsStatus() ){
			// ESP_LOGI(FNAME,"Flarm GPS OK");
			cur_heading = Flarm::getGndCourse();
		}
	}
	if( cur_heading > 0 ) {
		newHeading();
	}
	if( !(_tick%2) ){  // 0.2 s per thermal = 4.8 seconds all 24 thermals aged by 0.1 m/s
		ageThermal();
	}
}

