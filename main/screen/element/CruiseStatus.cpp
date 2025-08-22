/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "CruiseStatus.h"

#include "setup/CruiseMode.h"
#include "Colors.h"
#include "AdaptUGC.h"
#include "logdefnone.h"

#include <cstdio>

extern AdaptUGC *MYUCG;


void CruiseStatus::draw() {
    // if ( !_dirty ) { return; }
    // _dirty = false;
    // dirty not used, call only when a refresh is needed

    int16_t text_offset = 0;

    // optional symbol
    if ( _use_symbol ) {
        // clear the symbol area
        MYUCG->setColor(COLOR_BLACK);
        MYUCG->drawBox(_ref_x - (SYMBOL_SIZE+3), _ref_y - (SYMBOL_SIZE+3), 2*SYMBOL_SIZE+6, 2*SYMBOL_SIZE+6);

        MYUCG->setColor(COLOR_WHITE);
        if (VCMode.getCMode()) {
            // draw litte arrow
            MYUCG->drawTetragon(_ref_x - SYMBOL_SIZE, _ref_y - 5, _ref_x - SYMBOL_SIZE-1, _ref_y - 1,
                                _ref_x + SYMBOL_SIZE-4, _ref_y + 5, _ref_x + SYMBOL_SIZE-4, _ref_y + 1);
            MYUCG->drawTriangle(_ref_x, _ref_y + 7, _ref_x + 3, _ref_y - 4, _ref_x + SYMBOL_SIZE+2, _ref_y + 4);
        } else {
            // draw circle
            MYUCG->drawCircle(_ref_x, _ref_y, SYMBOL_SIZE, UCG_DRAW_ALL);
            MYUCG->drawCircle(_ref_x, _ref_y, SYMBOL_SIZE - 1, UCG_DRAW_ALL);
            int tm = _ref_x - SYMBOL_SIZE + 1;
            constexpr const int16_t S2FTS = SYMBOL_SIZE/2;
            MYUCG->drawTriangle(tm - S2FTS, _ref_y + 2, tm + S2FTS, _ref_y + 2, tm, _ref_y - S2FTS);
        }

        text_offset = 3*SYMBOL_SIZE;
    }

    // write netto mode in case set
    if (VCMode.isNetto()) {
        MYUCG->setColor(COLOR_WHITE);
    } else {
        MYUCG->setColor(COLOR_BLACK);
    }
    const char *s = "   net";
    if (VCMode.getVMode() == CruiseMode::MODE_REL_NETTO) {
        s = "s-net";
    }
    ESP_LOGI(FNAME, "print netto stat %d - %s", VCMode.isNetto(), s);
    ESP_LOGI(FNAME, "print x/y %d/%d", _ref_x + SYMBOL_SIZE + 3 - MYUCG->getStrWidth(s), _ref_y + text_offset);
    MYUCG->setFont(ucg_font_fub11_hr, true);
    MYUCG->setPrintPos(_ref_x + SYMBOL_SIZE+3 - MYUCG->getStrWidth(s), _ref_y+text_offset);
    MYUCG->print(s);
}
