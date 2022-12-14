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
#include <driver/gpio.h>

class IpsDisplay;
class ESPRotary;
class PressureSensor;
class AnalogInput;

class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	SetupMenu( const char* title );
	virtual ~SetupMenu();
	void begin( IpsDisplay* display, PressureSensor * bmp, AnalogInput *adc );
	void setup();
	void display( int mode=0 );
	const char *value() { return 0; };
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	void escape();
	void showMenu();

	static void catchFocus( bool activate );
	static bool focus;
	static gpio_num_t getGearWarningIO();

	static void vario_menu_create( MenuEntry *top );
	static void vario_menu_create_damping( MenuEntry *top );
	static void vario_menu_create_meanclimb( MenuEntry *top );
	static void vario_menu_create_s2f( MenuEntry *top );
	static void vario_menu_create_ec( MenuEntry *top );

	static void audio_menu_create( MenuEntry *top );
	static void audio_menu_create_tonestyles( MenuEntry *top );
	static void audio_menu_create_deadbands( MenuEntry *top );

	static void glider_menu_create( MenuEntry *top );
	static void options_menu_create( MenuEntry *top );
	static void system_menu_create( MenuEntry *top );
};

#endif
