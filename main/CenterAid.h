/*
 * CenterAid.h
 *
 *  Created on: Jan 23, 2022
 *      Author: gittest3
 */
#pragma once

// #include "types.h"
// #include "IpsDisplay.h"
#include "Colors.h"
#include "wind/CircleWind.h"

#include <cinttypes>

constexpr int CA_NUM_DIRS = 24;  // every 15°

class PolarGauge;

class CenterAid {
public:
    static CenterAid *create(PolarGauge &g);
    static void remove();
	void drawThermal( int th, int idir, bool draw_red=false );
	void tick();
	void drawCenterAid();
    void setGeometry(int r);

private:
    const PolarGauge &_gauge;
	CenterAid(PolarGauge &g);
	void ageThermal();
	void addThermal( int teval );
	bool maxClimb();
	void checkThermal();
	void calcFlightMode( float headingDiff );
	int maxClimbIndex();
	int8_t thermals[CA_NUM_DIRS];  // every 5 °  +-127 in steps of 0.1 m/S
	int8_t drawn_thermals[CA_NUM_DIRS];
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

extern CenterAid  *theCenteraid;
