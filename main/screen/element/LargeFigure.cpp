/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "LargeFigure.h"

#include "Colors.h"
#include "AdaptUGC.h"
#include "logdef.h"

#include <cmath>
#include <cstdio>

extern AdaptUGC *MYUCG;


////////////////////////////
// LargeFigure

LargeFigure::LargeFigure(int16_t x, int16_t y) :
    ScreenElement(x, y)
{
    MYUCG->setFont(ucg_font_fub35_hn, false);
    _yoff = MYUCG->getFontAscent()/2 - MYUCG->getFontDescent() - 2;
    _ref_y += _yoff;
}

void LargeFigure::draw(float val) {
    int16_t ival = std::rint(val * 10); // integer value in steps of 10th
    if (_value != ival || _dirty) {
        // only print if there is a change in rounded numeric string
        char s[32];
        MYUCG->setFont(ucg_font_fub35_hn, false);
        // MYUCG->setFontPosCenter(); // corrects wrongly for (asc-desc)/2 -> we want asc/2 + desc
        sprintf(s, _format[std::abs(ival) > 100], float(abs(ival) / 10.));
        int16_t tmp = MYUCG->getStrWidth(s)/2;
        if (val < 0) {
            MYUCG->setColor(COLOR_BBLUE);
        } else {
            MYUCG->setColor(COLOR_WHITE);
        }
        MYUCG->setPrintPos(_ref_x - tmp, _ref_y);
        MYUCG->print(s);
        _value = ival;
    }
}
