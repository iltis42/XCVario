/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef _MenuEntry_H_
#define _MenuEntry_H_
#include <string>
#include <vector>
#include <set>
#include <list>
#include <stdio.h>
#include "IpsDisplay.h"
#include "ESPRotary.h"
#include "Setup.h"
#include "AnalogInput.h"
#include "BMPVario.h"


class MenuEntry: public RotaryObserver {
public:
	MenuEntry() : RotaryObserver() {
	       _parent = 0;
				 highlight = 0;
				 pressed = false;
				 y=0;
				 helptext=0;
				 long_pressed = false;
				 hypos=0;
	}

	virtual void display( int mode=0 ) = 0;
	virtual void release() { display(); };
	virtual void longPress() {};
	virtual char* value() = 0;
	virtual ~MenuEntry() {};
	MenuEntry* addMenu( MenuEntry * item );
	void       delMenu( MenuEntry * item );
	MenuEntry* findMenu( String title, MenuEntry* start=root  );
	void togglePressed() {
		if( pressed )
			pressed = false;
		else
			pressed = true;
	}
	void setHelp( const char *txt, int y=180 ) { helptext = (char*)txt; hypos = y; };
	void showhelp( int y );
	void clear();
	void uprintf( int x, int y, const char* format, ...);
	void uprint( int x, int y, const char* str );
	bool menuEnabled() { return _menu_enabled; };
	void semaphoreTake();
  void semaphoreGive();
public:
	std::vector<MenuEntry*>  _childs;
	MenuEntry *_parent;
	String _title;
	static MenuEntry *root;
	static MenuEntry *selected;
	static IpsDisplay* _display;
	static ESPRotary* _rotary;
	static AnalogInput* _adc;
	static PressureSensor *_bmp;
	static bool _menu_enabled;
	int    highlight;
	bool   pressed;
	bool   long_pressed;
	char   *helptext;
	int    hypos;
	unsigned char y;
	static Ucglib_ILI9341_18x240x320_HWSPI *ucg;
	// static float volume;

private:
};

#endif
