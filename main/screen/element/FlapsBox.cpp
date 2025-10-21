/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "FlapsBox.h"

#include "Flap.h"
#include "Colors.h"
#include "AdaptUGC.h"
#include "ESPAudio.h"
#include "setup/SetupNG.h"
#include "logdef.h"

#include <cstdio>

extern AdaptUGC *MYUCG;

constexpr const int16_t BOX_WIDTH  = 28;
constexpr const int16_t BOX_LENGTH = 100; // w/o corners
constexpr const int16_t BOX_CORNER = 8;
constexpr const int16_t LABEL_SPACING = 40;
constexpr const float   PIX_PER_KMH = ((float)(BOX_LENGTH)-2*BOX_CORNER) / 26; // km/h range on flap box
constexpr const int     SOUND_LATENCY = 5; // frames 

FlapsBox::FlapsBox(int16_t cx, int16_t cy, bool vertical) :
    ScreenElement(cx, cy),
    _last_event(0,0),
    _vertical(vertical)
{
    MYUCG->setFont(ucg_font_fub11_hn);
	_LFH = MYUCG->getFontAscent() - MYUCG->getFontDescent();
    ESP_LOGI(FNAME, "FlapsBox label height %d, a%d d%d", _LFH, MYUCG->getFontAscent(), MYUCG->getFontDescent());
}

void FlapsBox::drawLabels(float lwk_speed, float uwk_speed)
{
    ESP_LOGI(FNAME, "draw wkf=%.2f, %.1f/%.1f", _flaps_position, lwk_speed, uwk_speed);
    int16_t boxx = _ref_x+2,
    boxy = _ref_y - BOX_LENGTH / 2 + 2,
    boxw = BOX_WIDTH - 4,
    boxh = BOX_LENGTH - 4;
    MYUCG->setClipRange(boxx, boxy, boxw, boxh);
    MYUCG->setFont(ucg_font_fub11_hn);

    // colored speed range (background)
    int16_t top_pix = static_cast<int16_t>(lwk_speed * PIX_PER_KMH);
    int16_t bottom_pix = static_cast<int16_t>(uwk_speed * PIX_PER_KMH);
    int16_t green_top =  _ref_y + top_pix;
    if ( top_pix > -BOX_LENGTH/2 ) {
        MYUCG->setColor(COLOR_WGREY);
        MYUCG->drawBox(boxx, boxy, boxw, BOX_LENGTH/2 + top_pix);
    }
    if ( green_top < _ref_y + boxh ) {
        MYUCG->setColor(COLOR_DGREEN);
        if ( bottom_pix < BOX_LENGTH/2 ) {
            MYUCG->drawBox(boxx, green_top, boxw, _ref_y - green_top + bottom_pix);
            MYUCG->setColor(COLOR_WGREY);
            MYUCG->drawBox(boxx, _ref_y + bottom_pix, boxw, BOX_LENGTH/2 - bottom_pix);
        } else {
            MYUCG->drawBox(boxx, green_top, boxw, boxh);
        }
    }

    // foreground labels
    // const int from = std::max((int)std::roundf(_flaps_position-1), 0);
    // const int to   = std::min((int)std::roundf(_flaps_position+1), FLAP->getNrPositions()-1);
    // for (int wk = from; wk <= to; wk++)
    int wk = (int)std::roundf(_flaps_position);
    {
        const char *label = FLAP->getFL(wk)->label;
        int16_t pixoff = (_flaps_position - wk) * LABEL_SPACING; // 20 pixels per flap step
        // ESP_LOGI(FNAME, "wk %d, pixoff %d", wk, pixoff);
        int16_t lwidth = MYUCG->getStrWidth(label);
        MYUCG->setPrintPos(_ref_x + (BOX_WIDTH - lwidth)/2 + 1, _ref_y - pixoff + _LFH/2);
        // if (abs(pixoff) < LABEL_SPACING/2) {
            // MYUCG->setColor(1, COLOR_DGREEN);
            MYUCG->setColor(COLOR_WHITE); // highlight the recommendation, or current position
        // } else {
        //     // MYUCG->setColor(1, COLOR_WGREY);
        //     MYUCG->setColor(COLOR_MGREY);
        // }

        // simplistic
        // MYUCG->printf(label);

        // or cool alternatively, but not correctly working, because of a eglib bug with clipping and text output
        if ( top_pix < (pixoff + _LFH/2) && top_pix > (pixoff - _LFH/2) ) {
            // clipped top grey to green band
            MYUCG->setColor(1, COLOR_WGREY);
            MYUCG->undoClipRange();
            MYUCG->setClipRange(boxx, boxy, boxw, BOX_LENGTH/2 + top_pix);
            MYUCG->print(label);
            MYUCG->setColor(1, COLOR_DGREEN);
            MYUCG->undoClipRange();
            // MYUCG->setColor(COLOR_WHITE);
            MYUCG->setPrintPos(_ref_x + (BOX_WIDTH - lwidth)/2 + 1, _ref_y - pixoff + _LFH/2);
            MYUCG->print(label);
        }
        else if ( bottom_pix < (pixoff + _LFH/2) && bottom_pix > (pixoff - _LFH/2) ) {
            // clipped bottom green to grey band
            MYUCG->setColor(1, COLOR_DGREEN);
            MYUCG->undoClipRange();
            MYUCG->setClipRange(boxx, boxy, boxw, BOX_LENGTH/2 + bottom_pix);
            MYUCG->print(label);
            MYUCG->undoClipRange();
            MYUCG->setClipRange(boxx, _ref_y + bottom_pix+1, boxw, BOX_LENGTH/2 - bottom_pix);
            MYUCG->setColor(1, COLOR_WGREY);
            MYUCG->setPrintPos(_ref_x + (BOX_WIDTH - lwidth)/2 + 1, _ref_y - pixoff + _LFH/2);
            MYUCG->print(label);
        }
        else {
            // no clipping, just choose the right background
            if (pixoff < top_pix || pixoff > bottom_pix) {
                MYUCG->setColor(1, COLOR_WGREY);
            }
            else {
                MYUCG->setColor(1, COLOR_DGREEN);
            }
            MYUCG->print(label);
        }
    }


    MYUCG->setColor(1, g_col_background, g_col_background, g_col_background);
    MYUCG->undoClipRange();
}

void FlapsBox::draw(float ias)
{
    if ( _dirty ) {
        MYUCG->setColor(COLOR_HEADER);
        MYUCG->drawRFrame(_ref_x, _ref_y-BOX_LENGTH/2-BOX_CORNER, BOX_WIDTH, BOX_LENGTH + 2*BOX_CORNER, BOX_CORNER);
        MYUCG->drawDisc(_ref_x, _ref_y + 10*PIX_PER_KMH, 2, UCG_DRAW_ALL);
        MYUCG->drawDisc(_ref_x, _ref_y - 10*PIX_PER_KMH, 2, UCG_DRAW_ALL);
        MYUCG->setColor(COLOR_WHITE);
        MYUCG->drawDisc(_ref_x, _ref_y, 3, UCG_DRAW_ALL);
    }
    
    float wktarget;
    if ( FLAP->haveSensor() ) {
        wktarget = FLAP->getFlapPosition();
    } else {
        wktarget = (int)std::ceilf(FLAP->getOptimum(ias));
    }
    // damp speed of indicator to make it good readable
    _flaps_position = _flaps_position + (wktarget - _flaps_position) * 0.2f;
    float minv, maxv;
    minv = FLAP->getSpeedBand(_flaps_position, maxv) - ias;
    maxv -= ias;
    drawLabels(minv, maxv);

    // do sounds when stepping over the speed range (with sensor),
    // or when the recommended position changes (without sensor)
    int flap_idx = (int)std::roundf(_flaps_position);
    if ( FLAP->haveSensor() ) {
        _last_flap_idx = flap_idx; // keep in sync with actual position, option to not play any sound
        if ( minv > 0. ) { // slipped below the lower speed limit
            flap_idx++;
        } 
        else if ( maxv < 0. ) { // exceeded the upper speed limit
            flap_idx--;
        }

    }

    // play sounds after some latency
    if ( flap_idx != _last_flap_idx ) {
        SwitchEvent current_event = SwitchEvent(_last_flap_idx, flap_idx);
        if ( current_event != _last_event ) {
            _snd_latency_cnt++;
            if ( _snd_latency_cnt > SOUND_LATENCY ) {
                ESP_LOGI(FNAME, "flap_idx changed from %d to %d", _last_flap_idx, flap_idx);
                if ( flap_idx > _last_flap_idx ) {
                    // flap back sound
                    AUDIO->startSound(AUDIO_FLAP_BACK, true);
                } else {
                    // flap forward sound
                    AUDIO->startSound(AUDIO_FLAP_FORWARD, true);
                }
                _last_event = current_event;
                _same_event_to = 100;
                _last_flap_idx = flap_idx;
            }
        }
        else {
            if ( _same_event_to > 0 ) {
                _same_event_to--;
            } else {
                // reset to allow sound again
                _last_event = SwitchEvent(0,0);
            }
        }
    }
    else {
        _snd_latency_cnt = 0;
    }
    
    _dirty = false;
}

