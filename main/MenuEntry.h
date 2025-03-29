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
class SetupMenu;

constexpr const int DEFAULT_HELP_Y_POS = 180;

class MenuEntry: public RotaryObserver {
public:
	MenuEntry() : RotaryObserver() {
		_parent = 0;
		pressed = false;
		helptext = 0;
		hypos = DEFAULT_HELP_Y_POS;
		_title = 0;
	};
	virtual ~MenuEntry();
	virtual void display( int mode=0 ) = 0;
	virtual bool isLeaf() const { return true; }
	void release() override { display(); };
	void longPress() override {};
	virtual const char* value() = 0;
	void togglePressed() { pressed = ! pressed; }
	void setHelp( const char *txt, int y=DEFAULT_HELP_Y_POS ) { helptext = (char*)txt; hypos = y; };
	void showhelp();
	void clear();
	const MenuEntry* findMenu(const char *title) const;
	void uprintf( int x, int y, const char* format, ...);
	void uprint( int x, int y, const char* str );
    void restart();
    bool get_restart() { return _restart; };
    static void setRoot( MenuEntry *root ) { selected = root; };
public:
	SetupMenu *_parent;
	const char *_title;
	bool      pressed;

	static AdaptUGC *ucg;
	static MenuEntry *root;
	static MenuEntry *selected;
	static IpsDisplay* _display;
	static AnalogInput* _adc;
	static PressureSensor *_bmp;
	static bool _restart;

protected:
	const char *helptext;
	int16_t    	hypos;
};
