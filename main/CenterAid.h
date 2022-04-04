/*
 * CenterAid.h
 *
 *  Created on: Jan 23, 2022
 *      Author: gittest3
 */

#include "types.h"
#include "IpsDisplay.h"
#include "Colors.h"
#include "CircleWind.h"

#define CA_NUM_DIRS 24  // every 10°
#define CA_STEP (360/CA_NUM_DIRS)
#define CA_STEP_2 (CA_STEP/2.0)
#define MAX_DISK_RAD 6

#pragma once

class CenterAid final {
public:
	CenterAid( AdaptUGC *display );
	void drawThermal( int th, int idir, bool draw_sink=false );
	void tick();
	void drawCenterAid();

private:
	void ageThermal();
	void addThermal( int teval );
	void checkThermal();

	int8_t thermals[CA_NUM_DIRS];  // every 5 °  +-127 in steps of 0.1 m/S
	int8_t drawn_thermals[CA_NUM_DIRS];
	AdaptUGC * ucg;
	float cur_heading;
	float last_heading;
	int idir;
	int agedir;
	int _tick;
	t_circling flightmode;
};

