/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "S2FBar.h"

#include "Colors.h"
#include "AdaptUGC.h"
#include "logdef.h"

#include <cstdio>
#include <cmath>

extern AdaptUGC *MYUCG;

S2FBar::S2FBar(int16_t cx, int16_t cy, int16_t width, int16_t gap) :
    ScreenElement(cx, cy),
    _width_h(width/2),
    _gap_h(gap/2)
{
    stepFromWidth(width);
    MYUCG->setFont(ucg_font_fub11_hn);
    _LFH_h = (MYUCG->getFontAscent() - MYUCG->getFontDescent() + 2) / 2;

}

// Accepts speed in kmh IAS/TAS, translates into configured unit
// right-aligned to value in 25 font size, no unit
void S2FBar::drawSpeed(float v)
{
	int airspeed = (int)std::roundf(v);
    if ( _prev_s2f_level == 0 ) {
	    MYUCG->setColor(COLOR_DGREEN);
    }
    else {
        MYUCG->setColor(COLOR_WGREY);
    }
    MYUCG->setFont(ucg_font_fub11_hn, true);
    char s[32];
    sprintf(s, " %3d ", airspeed);
    int16_t toleft = (_gap_h < 8) ? 2 * _width_h : 0;
    MYUCG->setPrintPos(_ref_x + toleft - MYUCG->getStrWidth(s) / 2, _ref_y + _LFH_h);
    MYUCG->print(s);
}

void S2FBar::drawArrow(int16_t x, int16_t y, int16_t level, bool del)
{
    constexpr int16_t gap = 2;
    int16_t height = _step;

    if (level == 0)
        return;

    int16_t init = 1;
    if (std::abs(level) == 4)
    {
        init = 2;
        if (del) {
            MYUCG->setColor(COLOR_WGREY);
        } else {
            MYUCG->setColor(COLOR_WHITE);
        }
    }
    else if (del) {
        MYUCG->setColor(COLOR_BLACK);
    }
    else {
        MYUCG->setColor(COLOR_WGREY);
    }

    int16_t l = level - init;
    if (level < 0) {
        height = -height;
        l = level + init;
    }
    MYUCG->drawTriangle(x, y + l * (_step + gap), x, y + l * (_step + gap) + height, x - _width_h, y + l * gap);
    MYUCG->drawTriangle(x, y + l * (_step + gap), x + _width_h, y + l * gap, x, y + l * (_step + gap) + height);
}

// speed to fly delta given in kmh, s2fd > 0 means speed up
// bars dice up 10 kmh steps
void S2FBar::draw(int s2fd)
{
    int16_t level = s2fd / 10; // dice up into 10 kmh steps

    // draw max. three bars, then change color of the last one to red
    if (level > 4) {
        level = 4;
    }
    else if (level < -4) {
        level = -4;
    }
    if ( _dirty ) {
        // force redraw of all
        _prev_s2f_level = 0;
        _dirty = false;
    }

    // ESP_LOGI(FNAME,"s2fbar %d %d", s2fd, level);
    if (level == _prev_s2f_level) {
        return;
    }

    int16_t inc = (level - _prev_s2f_level > 0) ? 1 : -1;
    for (int16_t i = _prev_s2f_level + ((_prev_s2f_level == 0 || _prev_s2f_level * inc > 0) ? inc : 0);
         i != level + ((i * inc < 0) ? 0 : inc); i += inc)
    {
        if (i != 0)
        {
            drawArrow(_ref_x, _ref_y + (i > 0 ? 1 : -1) * _gap_h, i, i * inc < 0);
            // ESP_LOGI(FNAME,"s2fbar draw %d,%d", i, (i*inc < 0)?0:inc);
        }
    }

    _prev_s2f_level = level;
}
