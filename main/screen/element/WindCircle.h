/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <cinttypes>

// Wind indicator around gauge center
class WindCircle {
public:
	WindCircle(int cx, int cy);
	// API
	void setGeometry(int16_t r) { _radius = r; }
	void forceRedraw() { dirty = true; }
	void draw(int sdir, int sw, int idir, int iw);

private:
	enum DRAWTYP { ERASE=0x10, SYNAPT=1, INST=2 };
	void drawPolarWind(int a, int w, uint8_t type);

	// attributes
private:
	int16_t _center_x;
	int16_t _center_y;
	int16_t _radius = 0; // distance to wind number
	int _sytic_dir = 0.; // deg
	int _sytic_w = 0;
	int _inst_dir = 0; // deg
	int _inst_w = 0;
	int _cheight;
	int _cwidth;
	bool dirty = true;
};