/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "setup/SetupNG.h"

#include <cinttypes>

// A wrapper to the logic of the setup variables:
// vario_mode - persistent info, but individual for master/client, about brutto/netto
// netto_mode - persistent info, but individual for master/client, about used netto flavor (in case)
// cruise_mode - volatile status of the s2f mode, whereas syncronized bidirectional
//
// This class as a combinated logic gives straight access to
//  - S2F/Vario status queries
//  - Vario gauge display status
// and allows the straight modification of e.g. S2F mode respecting the implication
// to the vario gauge display status.


class CruiseMode
{
public:
    using VarioModeBits = enum { MODE_BRUTTO=0, MODE_NORMAL_NETTO=1, MODE_REL_NETTO=3 }; // vario mode bits
    CruiseMode();
    bool getCMode() const { return _cmode; };
    bool isNetto() const { return _vmode != MODE_BRUTTO; }
    bool audioIsVario() const { return _audio_vario; }
    bool audioIsChopping() const { return _audio_chopping; }
    int16_t getVMode() const { return _vmode; }

    // setter
    void setCMode(bool cruise);
    void lockTo(bool cruise);
    void unlockCMode() { _lock = false; }
    void updateCache();

private:
    bool _cmode = false; // cached cruise mode
    bool _audio_vario = true; // cached audio mode; true == vario mode, false == s2f mode
    bool _audio_chopping = true; // true for chopping mode (silent or dual)
    int16_t _vmode = 0; // cached vario mode
    bool _lock = false; // lock the cruise mode
};


extern CruiseMode VCMode;
