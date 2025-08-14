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
	inline bool getCMode() const { return _cmode; };
    inline bool isNetto() const { return _vmode; }
    inline int16_t getVMode() const { return _vmode; }

	// setter
	void setCMode(bool cruise);
	void lockTo(bool cruise);
	void unlockCMode() { _lock = false; }
	void updateCache();

private:
    bool _cmode = false; // cached cruise mode
    int16_t _vmode = 0; // cached vario mode
    bool _lock = false; // lock the cruise mode

	// SemaphoreHandle_t mutex; // 
};


extern CruiseMode VCMode;