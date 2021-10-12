/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef _SetupMenu_H_
#define _SetupMenu_H_
#include "Setup.h"
#include "MenuEntry.h"
#include "SetupMenuValFloat.h"

extern String vunit;
extern String sunit;

class IpsDisplay;
class ESPRotary;
class PressureSensor;
class AnalogInput;

class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	SetupMenu(  String title );
	virtual ~SetupMenu();
	void begin( IpsDisplay* display, ESPRotary * rotary, PressureSensor * bmp, AnalogInput *adc );
	void setup();
	void display( int mode=0 );
	char *value() { return 0; };
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	void showMenu( bool pressed );
};

#endif
