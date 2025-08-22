/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "WindIndicator.h"

#include "PolarGauge.h"
#include "IpsDisplay.h"
#include "Colors.h"
#include "AdaptUGC.h"
#include "logdef.h"

#include <cmath>
#include <cstdio>

extern AdaptUGC *MYUCG;

int16_t WindIndicator::_cheight = 0;
int16_t WindIndicator::_cwidth = 0;


WindIndicator::WindIndicator(PolarGauge &g, bool live) :
    _gauge(g),
    _live(live)
{
    MYUCG->setFont(ucg_font_fub11_hr);
    _cheight = MYUCG->getFontAscent() - MYUCG->getFontDescent();
    _cwidth = MYUCG->getStrWidth("0");
    ESP_LOGI(FNAME, "asc %d %d", MYUCG->getFontAscent(), MYUCG->getFontDescent());
    ESP_LOGI(FNAME, "sw %d", _cwidth);

    if ( _live ) {
        _color = { COLOR_ORANGE };
    }
    else {
        _color = { COLOR_BBLUE };
    }
}

// Check if changed
// -> clear, and take over new values (but not draw)
bool WindIndicator::changed(int16_t wdir, int16_t wval)
{
    if (wdir != _dir || wval != _val) {
        drawWind(true);
        _gauge.drawRose(_dir);
        _val = wval;
        if ( wval < 0 ) {
            return false;
        }
        _dir = wdir;
        return true;
    }
    return false;
}

// direction [°], northwind as 0°; strength [any]
// wval < 0 just removes the
bool WindIndicator::draw(int16_t wdir, int16_t wval)
{
    // ESP_LOGI(FNAME, "Wind (%d,%d)", wdir, wval);
    if (wdir != _dir || wval != _val) {
        drawWind(true);
        _gauge.drawRose(_dir);
        _val = wval;
        if ( wval < 0 ) {
            return false;
        }
        _dir = wdir;
        drawWind(false);
        return true;
    }
    return false;
}

// 0° reference on top of the compass rose
void WindIndicator::drawWind(bool erase)
{
    // if ( ! erase ) ESP_LOGI(FNAME, "wind deg:%d, a:%d ", _dir, _val);
    float si = -fast_sin_idx(_dir*2);
    float co = fast_cos_idx(_dir*2);

    int16_t x0 = si * _gauge._radius;
    int16_t y0 = co * _gauge._radius;
    int16_t x1 = x0 / 4;
    int16_t y1 = y0 / 4;
    if (erase) {
        MYUCG->setColor(COLOR_BLACK);
    } else {
        MYUCG->setColor(_color.color[0], _color.color[1], _color.color[2]);
    }
    // a number at where the wind is coming from
    int16_t sw = _cwidth * count_digits(_val);
    int16_t xshift = sw / 2;
    if (erase) {
        MYUCG->drawBox(_gauge._ref_x - x0 - x1 / 2 - xshift, _gauge._ref_y - y0 - y1 / 2 - _cheight / 2, sw, _cheight);
    } else {
        char buf[6];
        int16_t value = _val;
        if (_val > 399) value = -1;
        if (_val < 0) value = -1;
        sprintf(buf, "%d", value);
        MYUCG->setFont(ucg_font_fub11_hr);
        MYUCG->setPrintPos(_gauge._ref_x - x0 - x1 / 2 - xshift, _gauge._ref_y - y0 - y1 / 2 + _cheight / 2);
        MYUCG->print(buf);
    }

    // a tip in direction the wind is blowing (180° other direction)
    x0 += _gauge._ref_x;
    y0 += _gauge._ref_y;
    int16_t wx = std::roundf(si);
    int16_t wy = std::roundf(co);
    if (_live) {
        int16_t bx = si * 4;
        int16_t by = co * 4;
        // ">" like arrow tip
        MYUCG->drawLine(x0 + by, y0 - bx, x0 + x1, y0 + y1); // +(by,-bx)
        MYUCG->drawLine(x0 + by + wy, y0 - bx - wx, x0 + x1 + wy, y0 + y1 - wx);
        MYUCG->drawLine(x0 - by, y0 + bx, x0 + x1, y0 + y1); // +(-by,bx)
        MYUCG->drawLine(x0 - by - wy, y0 + bx + wx, x0 + x1 - wy, y0 + y1 + wx);
    } else {
        // "-" like arrow tip
        MYUCG->drawTetragon(x0 + wy, y0 - wx,
            x0 + x1 + wy, y0 + y1 - wx, // +(wy,-wx)
            x0 + x1 - wy, y0 + y1 + wx, // +(-wy,wx)
            x0 - wy, y0 + wx);
        // ESP_LOGI(FNAME, "x0, y0 (%d,%d)", x0, y0);
    }
}
