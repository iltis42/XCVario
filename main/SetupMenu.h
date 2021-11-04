/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef _SetupMenu_H_
#define _SetupMenu_H_
#include "MenuEntry.h"
#include <string>

extern std::string vunit;
extern std::string sunit;

class IpsDisplay;
class ESPRotary;
class PressureSensor;
class AnalogInput;

class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	SetupMenu( std::string title );
	virtual ~SetupMenu();
	void begin( IpsDisplay* display, ESPRotary * rotary, PressureSensor * bmp, AnalogInput *adc );
	void setup();
	void display( int mode=0 );
	const char *value() const override { return 0; };
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	void showMenu( bool pressed );
	static void catchFocus( bool activate );
};

#endif
