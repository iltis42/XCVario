/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "SetupNG.h"
#include "MenuEntry.h"
#include "SetupMenuValCommon.h"

struct bitfield {
   e_restart_mode_t _restart :2;
   bool _end_menu            :1;
   bool _live_update         :1;
   uint8_t _precision        :4;
};

class SetupMenuValFloat:  public MenuEntry {
public:
	SetupMenuValFloat() { _unit = ""; };
	SetupMenuValFloat(  const char *title, const char *unit, float min, float max, float step, int (*action)(SetupMenuValFloat *p) = 0, bool end_menu=false, SetupNG<float> *anvs = 0, e_restart_mode_t restart=RST_NONE, bool sync=false, bool life_update=false );
	virtual ~SetupMenuValFloat();
	void display( int mode=0 );
	void displayVal();
	void setPrecision( int prec );
	const char *value();
	static void showMenu( float val, SetupMenuValFloat *menu  );
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	void escape() {}; // ignore, base class takes care
	void setStep( float val ) { _step = val; };
	void setMax( float max ) { _max = max; };
	void setDynamic( float value ) { _dynamic = value; }
    float _value;
    static SetupMenuValFloat * qnh_menu;
    static SetupMenuValFloat * meter_adj_menu;

private:
    float step( float instep );
    static char _val_str[20];
	float _min, _max, _step;
	bitfield bits;
	float _value_safe;
	const char * _unit;
	int (*_action)( SetupMenuValFloat *p );
	SetupNG<float> * _nvs;
    float _dynamic;
};
