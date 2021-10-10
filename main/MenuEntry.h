/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef _MenuEntry_H_
#define _MenuEntry_H_
#include <vector>
// #include <string>
// #include <set>
#include <list>
// #include <cstdio>
// #include "IpsDisplay.h"
#include "ESPRotary.h"
#include "Setup.h"
// #include "AnalogInput.h"
// #include "BMPVario.h"

class IpsDisplay;
class AnalogInput;
class PressureSensor;
class Ucglib_ILI9341_18x240x320_HWSPI;

class MenuEntry: public RotaryObserver {
public:
	MenuEntry() : RotaryObserver() {}
	virtual ~MenuEntry();
	virtual void display( int mode=0 ) = 0;
	virtual void release() { display(); };
	virtual void longPress() {};
	virtual char* value() = 0;
    MenuEntry* getFirst() const { return _childs.front(); }
	MenuEntry* addEntry( MenuEntry * item );
	MenuEntry* addEntry( MenuEntry * item, const MenuEntry* after );
	void       delEntry( MenuEntry * item );
	MenuEntry* findMenu( String title, MenuEntry* start=root  );
	void togglePressed() { pressed = ! pressed; }
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
	MenuEntry *_parent = 0;
	String _title;
	static MenuEntry *root;
	static MenuEntry *selected;
	static IpsDisplay* _display;
	static ESPRotary* _rotary;
	static AnalogInput* _adc;
	static PressureSensor *_bmp;
	static bool _menu_enabled;
	int    highlight;
	bool   pressed = false;
	bool   long_pressed = false;
	char   *helptext = 0;
	int    hypos = 0;
	unsigned char y = 0;
    int    idx = 0;
	static Ucglib_ILI9341_18x240x320_HWSPI *ucg;
// private:
//     class MenuRotary : public RotaryObserver {
//     public:
//         virtual void up( int count ) { selected->up(count); }
//         virtual void down( int count ) { selected->down(count); }
//         virtual void press() { selected->press(); }
//         virtual void release() { selected->release(); }
//         virtual void longPress() { selected->longPress(); }
//     };
// public:
//     static MenuRotary menu_rotary_handler;
};

#endif
