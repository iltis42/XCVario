/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "SetupNG.h"
#include "MenuEntry.h"

struct bitfield {
   bool _restart      :1;
   bool _end_menu     :1;
   bool _live_update  :1;
   uint8_t _precision :5;
};

class SetupMenuValFloat:  public MenuEntry {
public:
	SetupMenuValFloat() { _unit = ""; };
	SetupMenuValFloat(  const char *title, const char *unit, float min, float max, float step, int (*action)(SetupMenuValFloat *p) = 0, bool end_menu=false, SetupNG<float> *anvs = 0, bool restart=false, bool sync=false, bool life_update=false );
	virtual ~SetupMenuValFloat();
	void display( int mode=0 );
	void displayVal();
	void setPrecision( int prec );
	const char *value();
	static void showQnhMenu();
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	void setStep( float val ) { _step = val; };
    float _value;
    static SetupMenuValFloat * qnh_menu;

private:
    float step( float instep );
    static char _val_str[20];
	float _min, _max, _step;
	bitfield bits;
	float _value_safe;
	const char * _unit;
	int (*_action)( SetupMenuValFloat *p );
	SetupNG<float> * _nvs;
};
