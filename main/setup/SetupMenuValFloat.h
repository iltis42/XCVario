/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "setup/SetupNG.h"
#include "setup/MenuEntry.h"


class SetupMenuValFloat:  public MenuEntry {
public:
	SetupMenuValFloat() { _unit = ""; };
	SetupMenuValFloat(  const char *title, const char *unit, int (*action)(SetupMenuValFloat *p), 
		bool end_menu, SetupNG<float> *anvs, e_restart_mode_t restart=RST_NONE, bool sync=false, bool life_update=false );
	virtual ~SetupMenuValFloat() = default;
	void enter() override;
	void display(int mode=0) override;
	void refresh() override;
	void displayVal();
	void setPrecision( int prec );
	const char *value() const override;
	float getFloat() const { return _nvs->get(); }
	void rot( int count );
	void press();
	void longPress();
	void setStep( float val ) { _step = val; };
	void setMax( float max ) { _max = max; };
	float _value = .0;

private:
    float step( float instep );
    mutable char _val_str[20]; // buffer for returned string
	float _min = 0., _max = 1., _step = .1;
	float _value_safe = 0;
	const char *_unit = "";
	int (*_action)( SetupMenuValFloat *p );
	SetupNG<float> * _nvs;
};
