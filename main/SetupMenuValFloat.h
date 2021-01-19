/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef _SetupMenuValFloat_H_
#define _SetupMenuValFloat_H_
#include <string>
#include <vector>
#include <set>
#include <list>
#include <stdio.h>
#include "IpsDisplay.h"
#include "ESPRotary.h"
#include "Setup.h"
#include "AnalogInput.h"

class SetupMenuValFloat:  public MenuEntry {
public:
	SetupMenuValFloat() { _unit = 0; };
	SetupMenuValFloat(  String title, float *value, const char *unit, float min, float max, float step, int (*action)(SetupMenuValFloat *p) = 0, bool end_menu=false, SetupNG<float> *anvs = 0 );
	void display(int mode=0);
	void displayVal();
	void setPrecision( int prec );
	static void showQnhMenu();
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	virtual ~SetupMenuValFloat() {};
    float *_value;
    static SetupMenuValFloat * qnh_menu;

private:
	float _min, _max, _step;
	bool _end_menu;
	const char * _unit;
	int (*_action)( SetupMenuValFloat *p );
	int _precision;
	SetupNG<float> * _nvs;
};

#endif
