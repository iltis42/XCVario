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
#include "logdefnone.h"

#include <cstdio>

extern AdaptUGC *MYUCG;

constexpr const int16_t BOX_WIDTH  = 28;
// constexpr const int16_t BOX_LENGTH = 100; // w/o corners
constexpr const int16_t BOX_CORNER = 8;
constexpr const int16_t LABEL_SPACING = 40;
// constexpr const float   PIX_PER_KMH = ((float)(BOX_LENGTH)-2*BOX_CORNER) / (26.f + std::max(BOX_LENGTH-100, 0))/30.f; // km/h range on flap box
constexpr const int     SOUND_LATENCY = 5; // frames

int16_t FlapsBox::BOX_LENGTH = 100;
float   FlapsBox::PIX_PER_KMH = 3.23f;


constexpr FBoxStateHash::FBoxStateHash(float f, float minvd, float maxvd) :
    wkidx10( (int)std::roundf(f*10.) )
{
    top_pix = static_cast<int16_t>(minvd * FlapsBox::PIX_PER_KMH);
    bottom_pix = static_cast<int16_t>(maxvd * FlapsBox::PIX_PER_KMH);
    top_exseed = (top_pix < -FlapsBox::BOX_LENGTH/2) ? 1 : 0;
    bottom_exseed = (bottom_pix > FlapsBox::BOX_LENGTH/2) ? 1 : 0;
}
constexpr bool FBoxStateHash::operator!=(const FBoxStateHash &other) const noexcept
{
    if ( wkidx10 != other.wkidx10 ) return true;
    if ( ((top_pix > -FlapsBox::BOX_LENGTH/2
            && top_pix < FlapsBox::BOX_LENGTH/2)
        || (other.top_pix > -FlapsBox::BOX_LENGTH/2
            && other.top_pix < FlapsBox::BOX_LENGTH/2))
        && top_pix != other.top_pix ) return true;
    if ( ((bottom_pix > -FlapsBox::BOX_LENGTH/2
            && bottom_pix < FlapsBox::BOX_LENGTH/2)
        || (other.bottom_pix > -FlapsBox::BOX_LENGTH/2
            && other.bottom_pix < FlapsBox::BOX_LENGTH/2))
        && bottom_pix != other.bottom_pix) return true;
    if ( top_exseed != other.top_exseed ) return true;
    if ( bottom_exseed != other.bottom_exseed ) return true;
    return false;
}


FlapsBox::FlapsBox(Flap* flap, int16_t cx, int16_t cy, bool vertical) :
    ScreenElement(cx, cy),
    _flap(flap),
    _last_event(0,0),
    _vertical(vertical)
{
    MYUCG->setFont(ucg_font_fub11_hn);
    _LFH = MYUCG->getFontAscent() - MYUCG->getFontDescent() + 4;
    ESP_LOGI(FNAME, "FlapsBox label height %d, a%d d%d", _LFH, MYUCG->getFontAscent(), MYUCG->getFontDescent());
}

void FlapsBox::setLength(int16_t length)
{
    BOX_LENGTH = length;
    PIX_PER_KMH = ((float)(BOX_LENGTH)-2*BOX_CORNER) / (26.f + std::max(BOX_LENGTH-100, 0)/30.f); // km/h range on flap box
}

void FlapsBox::drawLabels(FBoxStateHash cs)
{
    ESP_LOGI(FNAME, "draw wkf=%.1f, %d/%d", cs.wkidx10/10.f, cs.top_pix, cs.bottom_pix);
    int16_t boxx = _ref_x+2;
    int16_t boxy = _ref_y - BOX_LENGTH / 2 + 2;
    int16_t boxw = BOX_WIDTH - 4;
    int16_t boxh = BOX_LENGTH - 4;

    // colored speed range (background)
    // draw excess corners in green
    if ( cs.top_exseed != _state.top_exseed ) {
        if ( cs.top_exseed ) {
            MYUCG->setColor(COLOR_DGREEN);
        } else {
            MYUCG->setColor(COLOR_BLACK);
        }
        MYUCG->setClipRange(boxx, boxy - BOX_CORNER, boxw, BOX_CORNER);
        MYUCG->drawRBox(boxx+1, boxy - BOX_CORNER + 1, boxw-1, 2 * BOX_CORNER, BOX_CORNER - 2);
        MYUCG->undoClipRange();
    }
    if ( cs.bottom_exseed != _state.bottom_exseed ) {
        if ( cs.bottom_exseed ) {
            MYUCG->setColor(COLOR_DGREEN);
        } else {
            MYUCG->setColor(COLOR_BLACK);
        }
        MYUCG->setClipRange(boxx, boxy + boxh, boxw, BOX_CORNER);
        MYUCG->drawRBox(boxx+1, boxy + boxh - BOX_CORNER + 1, boxw-1, 2 * BOX_CORNER, BOX_CORNER - 2);
        MYUCG->undoClipRange();
    }

    // background speed band
    MYUCG->setClipRange(boxx, boxy, boxw, boxh);
    int16_t green_top =  _ref_y + cs.top_pix;
    if ( cs.top_pix > -BOX_LENGTH/2 ) {
        MYUCG->setColor(COLOR_WGREY);
        MYUCG->drawBox(boxx, boxy, boxw, BOX_LENGTH/2 + cs.top_pix);
    }
    if ( green_top < _ref_y + boxh ) {
        MYUCG->setColor(COLOR_DGREEN);
        if ( cs.bottom_pix < BOX_LENGTH/2 ) {
            MYUCG->drawBox(boxx, green_top, boxw, _ref_y - green_top + cs.bottom_pix);
            MYUCG->setColor(COLOR_WGREY);
            MYUCG->drawBox(boxx, _ref_y + cs.bottom_pix, boxw, BOX_LENGTH/2 - cs.bottom_pix);
        } else {
            MYUCG->drawBox(boxx, green_top, boxw, boxh);
        }
    }
    MYUCG->undoClipRange();

    // foreground labels
    MYUCG->setFont(ucg_font_fub11_hn);
    const int from = std::max((int)(std::floorf(cs.getWk())) - 1, 0);
    const int range = (from == (int)(std::floorf(cs.getWk()))) ? 1 : 2;
    const int to   = std::min(from + range, _flap->getNrPositions() - 1);
    for (int wk = from; wk <= to; wk++)
    {
        const char *label = _flap->getFL(wk)->label;
        int16_t pixoff = -(cs.getWk() - wk) * LABEL_SPACING; // 20 pixels per flap step
        // ESP_LOGI(FNAME, "wk %d, pixoff %d", wk, pixoff);
        int16_t lwidth = MYUCG->getStrWidth(label);
        MYUCG->setPrintPos(_ref_x + (BOX_WIDTH - lwidth)/2 + 1, _ref_y + pixoff + _LFH/2);
        MYUCG->setColor(COLOR_WHITE); // highlight the recommendation, or current position

        if ( (pixoff - _LFH/2) <= cs.top_pix && (pixoff + _LFH/2) >= cs.top_pix ) {
            // clipped top grey to green band
            MYUCG->setColor(1, COLOR_WGREY);
            MYUCG->setClipRange(boxx, boxy+1, boxw, std::min((int16_t)(cs.top_pix + BOX_LENGTH/2), (int16_t)(boxh-2)));
            MYUCG->print(label);
            MYUCG->setColor(1, COLOR_DGREEN);
            MYUCG->undoClipRange();
            int16_t top = std::max((int16_t)(_ref_y + cs.top_pix+1), (int16_t)(boxy+1));
            // int16_t bot = std::min(BOX_LENGTH/2 - cs.top_pix);
            MYUCG->setClipRange(boxx, top, boxw, _ref_y + BOX_LENGTH / 2 - 2 - top);
            MYUCG->setPrintPos(_ref_x + (BOX_WIDTH - lwidth)/2 + 1, _ref_y + pixoff + _LFH/2);
            MYUCG->print(label);
            MYUCG->undoClipRange();
        }
        else if ( (pixoff - _LFH/2) <= cs.bottom_pix && (pixoff + _LFH/2) >= cs.bottom_pix ) {
            // clipped bottom green to grey band
            MYUCG->setColor(1, COLOR_DGREEN);
            MYUCG->setClipRange(boxx, boxy+1, boxw, std::min((int16_t)(cs.bottom_pix + BOX_LENGTH/2), (int16_t)(boxh-2)));
            MYUCG->print(label);
            MYUCG->undoClipRange();
            int16_t top = std::max((int16_t)(_ref_y + cs.bottom_pix+1), (int16_t)(boxy+1));
            MYUCG->setClipRange(boxx, top, boxw, _ref_y + BOX_LENGTH / 2 - 2 - top);
            MYUCG->setColor(1, COLOR_WGREY);
            MYUCG->setPrintPos(_ref_x + (BOX_WIDTH - lwidth)/2 + 1, _ref_y + pixoff + _LFH/2);
            MYUCG->print(label);
            MYUCG->undoClipRange();
        }
        else {
            MYUCG->setClipRange(boxx, boxy, boxw, boxh);
            // no clipping, just choose the right background
            if ((pixoff + _LFH/2) < cs.top_pix || (pixoff - _LFH/2) > cs.bottom_pix) {
                MYUCG->setColor(1, COLOR_WGREY);
            }
            else {
                MYUCG->setColor(1, COLOR_DGREEN);
            }
            MYUCG->print(label);
            MYUCG->undoClipRange();
        }
    }

    MYUCG->setColor(1, g_col_background, g_col_background, g_col_background);

    _state = cs;
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
    if ( flap_sensor.get() != FLAP_SENSOR_DISABLE ) {
        wktarget = _flap->getFlapPosition();
    } else {
        wktarget = (int)std::ceilf(_flap->getOptimum(ias));
    }
    // damp speed of indicator to make it good readable
    _flaps_position = _flaps_position + (wktarget - _flaps_position) * 0.2f;

    float minv, maxv;
    minv = _flap->getSpeedBand(_flaps_position, maxv);
    if ( airborne.get() == false ) {
        // on ground, set a virtual green band for the correct start position (ias "0km/h")
        ias = _flap->getSpeed(flap_takeoff.get() - .3); // pretend start speed
    }
    minv -= ias;
    maxv -= ias;
    // the three variables that define the box state
    FBoxStateHash current_state( _flaps_position, minv, maxv);
    if ( current_state != _state || _dirty ) {
        ESP_LOGI(FNAME,"wkf:%.1f minv:%.1f maxv:%.1f", current_state.getWk(), minv, maxv);
        drawLabels(current_state);
    }

    // do sounds when stepping over the speed range (with sensor),
    // or when the recommended position changes (without sensor)
    int flap_idx = (int)std::roundf(_flaps_position);
    if ( flap_sensor.get() != FLAP_SENSOR_DISABLE ) {
        _last_flap_idx = flap_idx; // keep in sync with actual position, option to not play any sound
        if ( minv > 0. && flap_idx < _flap->getNrPositions()-1 ) { // slipped below the lower speed limit
            flap_idx++;
        }
        else if ( maxv < 0. && flap_idx > 0 ) { // exceeded the upper speed limit
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
                _same_event_to = 200;
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
        _last_event = SwitchEvent(0,0);
    }

    _dirty = false;
}
