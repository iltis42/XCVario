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

static const int X=80;
static const int Y=220;

CenterAid::CenterAid( AdaptUGC *display ) {
	// TODO Auto-generated constructor stub
	ucg = display;
	last_heading = 0;
}

void CenterAid::drawThermal( int idir, bool draw_sink, bool erase ){
	if( erase )
		ucg->setColor( COLOR_BLACK );
	else
		ucg->setColor( COLOR_GREEN );
	if( draw_sink && thermals[idir] < 0 )
		ucg->setColor( COLOR_RED );

	int cy = X+sin(idir*CA_STEP)*30;
	int cx = Y+cos(idir*CA_STEP)*30;
	if( (thermals[idir]/32) > 0 )
		ucg->drawCircle(cx,cy, thermals[idir]/32 );
}

void CenterAid::headingChange( float dir ){
	int idir = rint(dir);
	if( idir/CA_STEP != (int)(last_heading/CA_STEP) ){
		float diff = Vector::angleDiffDeg( dir, last_heading );
		int idiff = rint(diff/CA_STEP);
		drawDiagram( X, Y, true );
		rotatePolar( idiff );
		drawDiagram( X, Y, false );
		last_heading = dir;
	}
}


void CenterAid::rotatePolar( int delta ){
	int8_t thermals_rotated[CA_NUM_DIRS];
	for( int i=0; i<CA_NUM_DIRS; i++ ){
		thermals_rotated[(i+delta)%CA_NUM_DIRS] = thermals[i];
	}
	memcpy( (void*)thermals, (void*)thermals_rotated, CA_NUM_DIRS);
}

// add one thermal and draw thermal
void CenterAid::addThermal( float dir, float teval ){
	int idir = rint(dir-CA_STEP_2);
	if( idir > CA_STEP )
		return;
	int ite = (int)(teval*10);
	if( thermals[ idir ] != ite  ){
		drawThermal( idir, true );
		thermals[ idir ] = ite;
		drawThermal( idir, false  );
	}
}

// draw full center aid
void CenterAid::drawDiagram( int x, int y, bool clear ){
	for( int i=0; i<CA_NUM_DIRS; i++ ){
		drawThermal( i, true, clear );
	}
}

