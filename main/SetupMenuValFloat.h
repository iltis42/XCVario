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
   uint8_t _precision :6;
};

class SetupMenuValFloat:  public MenuEntry {
public:
	SetupMenuValFloat() { _unit = ""; };
	SetupMenuValFloat(  const char *title, float *value, const char *unit, float min, float max, float step, int (*action)(SetupMenuValFloat *p) = 0, bool end_menu=false, SetupNG<float> *anvs = 0, bool restart=false, bool sync=false );
	virtual ~SetupMenuValFloat();
	void display( int mode=0 );
	void displayVal();
	void setPrecision( int prec );
	const char *value() const override {  sprintf(_val_str,"%0.*f %s", bits._precision, _nvs?_nvs->getGui():*_value, _unit ); return _val_str; };
	static void showQnhMenu();
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
    float *_value;
    static SetupMenuValFloat * qnh_menu;

private:
    static char _val_str[20];
	float _min, _max, _step;
	bitfield bits;
	float _value_safe;
	const char * _unit;
	int (*_action)( SetupMenuValFloat *p );
	SetupNG<float> * _nvs;
};
