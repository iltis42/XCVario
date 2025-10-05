/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "CruiseMode.h"

CruiseMode VCMode; // a global variable

CruiseMode::CruiseMode()
{
    updateCache();
}

void CruiseMode::setCMode(bool cruise)
{
    if ( ! _lock ) {
        if ( cruise != _cmode ) {
            cruise_mode.set(cruise);
        }
    }
}

void CruiseMode::lockTo(bool cruise)
{
    _lock = true;
    cruise_mode.set(cruise);
}

// callback to any change on cruise_mode setup
void CruiseMode::updateCache()
{
    // set the cruise mode cache
    _cmode = cruise_mode.get();

    // set the vario mode cache
    if (vario_mode.get() == VARIO_NETTO || (_cmode && (vario_mode.get() == CRUISE_ONLY_NETTO))) {
        if (netto_mode.get() == NETTO_NORMAL) {
            _vmode = MODE_NORMAL_NETTO;
        } else { // Super Netto
            _vmode = MODE_REL_NETTO;
        }
    } else {
        _vmode = MODE_BRUTTO;
    }

    // set the audio mode cache
    _audio_vario = ( ! _cmode || (cruise_audio_mode.get() != AUDIO_S2F) );
    // set the audio chopping style cache
    // breaks can be filled with silence or with dual tone
    _audio_chopping = (_audio_vario && (chopping_mode.get() & VARIO_CHOP) ) ||
            (!_audio_vario && (chopping_mode.get() & S2F_CHOP) );
}
