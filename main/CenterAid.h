/*
 * CenterAid.h
 *
 *  Created on: Jan 23, 2022
 *      Author: gittest3
 */

#include "types.h"
#include "IpsDisplay.h"
#include "Colors.h"

#define CA_NUM_DIRS 36  // every 10°
#define CA_STEP 360/CA_NUM_DIRS
#define CA_STEP_2 CA_STEP/2.0

#pragma once

class CenterAid final {
public:
	CenterAid( AdaptUGC *display );
	void addThermal( float dir, float teval );
	void headingChange( float dir );
	void drawDiagram( int x, int y, bool clear );
	void drawThermal( int idir, bool draw_sink, bool erase=false );
	void rotatePolar( int delta );

private:
	int8_t thermals[CA_NUM_DIRS];  // every 5 °  +-127 in steps of 0.1 m/S
	AdaptUGC * ucg;
	float last_heading;
};

