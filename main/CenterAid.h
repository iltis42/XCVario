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

#define CA_NUM_DIRS 24  // every 15°
#define CA_STEP (360/CA_NUM_DIRS) // 15
#define CA_STEP_2 (CA_STEP/2.0)   // 30
#define MAX_DISK_RAD 6
#define PEAK_STORAGE 120
#define DRAW_SCALE (PEAK_STORAGE/MAX_DISK_RAD)

#pragma once

class CenterAid {
public:
	CenterAid( AdaptUGC *display );
	void drawThermal( int th, int idir, bool draw_red=false );
	void tick();
	void drawCenterAid();

private:
	void ageThermal();
	void addThermal( int teval );
	bool maxClimb();
	void checkThermal();
	void calcFlightMode( float headingDiff );
	int maxClimbIndex();
	int8_t thermals[CA_NUM_DIRS];  // every 5 °  +-127 in steps of 0.1 m/S
	int8_t drawn_thermals[CA_NUM_DIRS];
	AdaptUGC * ucg;
	float cur_heading;
	float gps_heading;
	float gyro_last;
	int8_t idir;
	int8_t agedir;
	int _tick;
	t_circling flightmode;
	uint8_t turn_left;
	uint8_t turn_right;
	uint8_t fly_straight;
	uint64_t last_rts;
	float peak_value;
	float scale;
};

