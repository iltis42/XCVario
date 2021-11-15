/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "ESPRotary.h"
#include "SetupNG.h"

#include <vector>
#include <string>

class IpsDisplay;
class AnalogInput;
class PressureSensor;
class AdaptUGC;

class MenuEntry: public RotaryObserver {
public:
	MenuEntry() : RotaryObserver() {}
	virtual ~MenuEntry();
	virtual void display( int mode=0 ) = 0;
	virtual void release() { display(); };
	virtual void longPress() {};
	virtual const char* value() const = 0;
    MenuEntry* getFirst() const { return _childs.front(); }
	MenuEntry* addEntry( MenuEntry * item );
	MenuEntry* addEntry( MenuEntry * item, const MenuEntry* after );
	void       delEntry( MenuEntry * item );
	MenuEntry* findMenu( std::string title, MenuEntry* start=root  );
	void togglePressed() { pressed = ! pressed; }
	void setHelp( const char *txt, int y=180 ) { helptext = (char*)txt; hypos = y; };
	void showhelp( int y );
	void clear();
	void uprintf( int x, int y, const char* format, ...);
	void uprint( int x, int y, const char* str );
	void semaphoreTake();
    void semaphoreGive();
public:
	std::vector<MenuEntry*>  _childs;
	MenuEntry *_parent = 0;
	std::string _title;
	static MenuEntry *root;
	static MenuEntry *selected;
	static IpsDisplay* _display;
	static ESPRotary* _rotary;
	static AnalogInput* _adc;
	static PressureSensor *_bmp;
	int    highlight;
	bool   pressed = false;
	bool   long_pressed = false;
	char   *helptext = 0;
	int    hypos = 0;
	unsigned char y = 0;
    int    idx = 0;
	static AdaptUGC *ucg;

};
