/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "McCready.h"

#include "Colors.h"
#include "Units.h"
#include "AdaptUGC.h"

#include <cstdio>

extern AdaptUGC *MYUCG;


void McCready::draw(float mc)
{
    if ( std::abs(mc-_mcval) > 0.05 || _dirty ) {
        _mcval = mc;
        _dirty = false;
        MYUCG->setPrintPos(_ref_x, _ref_y);
        MYUCG->setColor(COLOR_WHITE);
        if( _large ) {
            MYUCG->setFont(ucg_font_fub20_hn, false);
        } else {
            MYUCG->setFont(ucg_font_fub14_hn, false);
        }
        char s[32];
        std::sprintf(s, "%1.1f", Units::Vario(mc) );
        MYUCG->print(s);
        int16_t fl = MYUCG->getStrWidth(s);
        MYUCG->setFont(ucg_font_fub11_hr, false);
        MYUCG->setColor(COLOR_HEADER);
        MYUCG->setPrintPos(_ref_x+fl+2, _ref_y);
        MYUCG->print("MC");
    }
}

