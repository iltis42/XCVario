/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "MenuEntry.h"
#include "SetupMenuValFloat.h"

class IpsDisplay;
class ESPRotary;
class PressureSensor;
class AnalogInput;

class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	explicit SetupMenu( const char* title, void (menu_create)(SetupMenu* ptr)  );
	virtual ~SetupMenu();
	void begin( IpsDisplay* display, PressureSensor * bmp, AnalogInput *adc );
	void display( int mode=0 );
	const char *value() { return 0; };
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	void escape();
	void showMenu();
	void create_subtree();
	void delete_subtree();

	static void catchFocus( bool activate );
	static bool focus;
	static gpio_num_t getGearWarningIO();

	static SetupMenuValFloat * createQNHMenu();

private:
	void (*menu_create_ptr)(SetupMenu*);
};

