/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "WindCircle.h"

#include "IpsDisplay.h"
#include "math/Trigenometry.h"
#include "Colors.h"
#include "AdaptUGC.h"
#include "logdef.h"

#include <cmath>
#include <cstdio>

extern AdaptUGC *MYUCG;

WindCircle::WindCircle(int cx, int cy) :
	ScreenElement(cx, cy)
{
	_ref_x = cx;
	_ref_y = cy;
	_radius = 49;
	MYUCG->setFont(ucg_font_fub11_hr);
	_cheight = MYUCG->getFontAscent() - MYUCG->getFontDescent();
	_cwidth = MYUCG->getStrWidth("0");
	ESP_LOGI(FNAME, "asc %d %d", MYUCG->getFontAscent(), MYUCG->getFontDescent());
	ESP_LOGI(FNAME, "sw %d", _cwidth);
}


static void format3digits(int value, char *out) {
    if (value > 199) value = 199;
	if (value < 0) value = 0;
    sprintf(out, "%d", value);
}

// draw a number at the spot where the wind is comeing from
// a - wind in deg 0 - 360
void WindCircle::drawPolarWind( int a, int speed, uint8_t type )
{
	// ESP_LOGI(FNAME, "polWind (%f,%d)", a, speed);
	float si=fast_sin_deg(a);
	float co=fast_cos_deg(a);

	int16_t x0 = -si * _radius;
	int16_t y0 =  co * _radius;
	int16_t x1 = x0/4;
	int16_t y1 = y0/4;
	if ( type & ERASE ) {
		MYUCG->setColor( COLOR_BLACK );
	}
	else if ( type & INST ) {
		MYUCG->setColor( COLOR_ORANGE );
	}
	else {
		MYUCG->setColor( COLOR_BBLUE );
	}
	// a number at where the wind is coming from
	int16_t sw = _cwidth * count_digits(speed);
	int16_t xshift = sw/2;
	if ( type & ERASE ) {
		MYUCG->drawBox(_ref_x-x0-x1/2-xshift, _ref_y-y0-y1/2-_cheight/2, sw, _cheight);
	}
	else {
		char buf[6];
		format3digits(speed, buf);
		MYUCG->setFont(ucg_font_fub11_hr);
		MYUCG->setPrintPos(_ref_x-x0-x1/2-xshift, _ref_y-y0-y1/2+_cheight/2);

		MYUCG->print(buf);
	}

	// a tip in direction the wind is blowing
	x0 += _ref_x;
	y0 += _ref_y;
	int16_t ws = std::roundf(si);
	int16_t wc = std::roundf(co);
	if ( type & INST ) {
		int16_t bs = std::roundf(si*4);
		int16_t bc = std::roundf(co*4);
		// ">" like arrow tip
		MYUCG->drawLine(x0+bc, y0+bs, x0+x1, y0+y1);
		MYUCG->drawLine(x0+bc+wc, y0+bs+ws, x0+wc+x1, y0+ws+y1);
		MYUCG->drawLine(x0-bc, y0-bs, x0+x1, y0+y1);
		MYUCG->drawLine(x0-bc-wc, y0-bs-ws, x0-wc+x1, y0-ws+y1);
	}
	else {
		// "-" like arrow tip 
		MYUCG->drawLine(x0, y0, x0+x1, y0+y1);
		MYUCG->drawLine(x0+wc, y0+ws, x0+wc+x1, y0+ws+y1); // one step to the left
		MYUCG->drawLine(x0-wc, y0-ws, x0-wc+x1, y0-ws+y1); // one step to the right
		// ESP_LOGI(FNAME, "x0, y0 (%d,%d)", x0, y0);
	}
}


void WindCircle::draw(int sdir, int sw, int idir, int iw)
{
	int heading = static_cast<int>(std::round(IpsDisplay::getHeading()));
	sdir = sdir - heading;
	idir = idir - heading;
	bool redraw_s = dirty,
		redraw_i = dirty;
	if ( sdir != _sytic_dir || sw != _sytic_w ) {
		drawPolarWind(_sytic_dir, _sytic_w, ERASE);
		_sytic_dir = sdir;
		_sytic_w = sw;
		redraw_s = true;
	}
	if ( idir != _inst_dir || iw != _inst_w ) {
		drawPolarWind(_inst_dir, _inst_w, INST | ERASE);
		_inst_dir = idir;
		_inst_w = iw;
		redraw_i = true;
	}
	if (redraw_s || redraw_i) {
		MYUCG->setColor(COLOR_MGREY);
		MYUCG->drawCircle(_ref_x, _ref_y, _radius+4);
		if ( redraw_s ) {
			drawPolarWind(_sytic_dir, _sytic_w, SYNAPT);
		}
		if ( redraw_i || (angleDiffDeg(idir, sdir)%180)<25 ) {
			drawPolarWind(_inst_dir, _inst_w, INST);
		}
	}
}

void WindCircle::clear() const
{
	MYUCG->setColor(COLOR_BLACK);
	MYUCG->drawDisc(_ref_x, _ref_y, _radius+_radius/4, UCG_DRAW_ALL);
}