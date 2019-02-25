/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef MAIN_SetupMenu_H_
#define MAIN_SetupMenu_H_
#include <string>
#include <vector>
#include <list>
#include <stdio.h>
#include "DotDisplay.h"
#include "ESPRotary.h"
#include "Setup.h"
#include "BME280_ESP32_SPI.h"

class MenuEntry: public RotaryObserver{
public:
	MenuEntry(){ _parent = 0; highlight = 0; pressed = false; y=0; u8g2=0;};
	virtual void display( int mode=0 ) = 0;
	virtual void release() { display(); };
	virtual void longP() {};
	virtual ~MenuEntry() {};
	std::vector<MenuEntry*>  _childs;
	MenuEntry *_parent;
	std::string _title;
	static MenuEntry *root;
	static MenuEntry *selected;
	MenuEntry* addMenu( MenuEntry * item);
	MenuEntry* findMenu( std::string title, MenuEntry* start=root  );
	void togglePressed() {
		if( pressed )
			pressed = false;
		else
			pressed = true;
	}

	static DotDisplay* _display;
	static ESPRotary* _rotary;
	static Setup *_setup;
	static BME280_ESP32_SPI *_bmp;
	static bool _menu_enabled;
	int    highlight;
	bool   pressed;
	u8g2_t *u8g2;
	unsigned char y;

private:
};


class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	SetupMenu(  std::string title );
	void begin( DotDisplay* display, ESPRotary * rotary, Setup * setup, BME280_ESP32_SPI * bmp );
	void setup();
	void display( int mode=0 );

	void up();  // step up to parent
	void down();
	void press();

	virtual ~SetupMenu() {};

};


class SetupMenuValFloat:  public MenuEntry {
public:
	SetupMenuValFloat();
	SetupMenuValFloat(  std::string title, float *value, std::string unit, float min, float max, float step, int (*action)(SetupMenuValFloat *p) = 0 );
	void display(int mode=0);

	void up();  // step up to parent
	void down();
	void press();
	virtual ~SetupMenuValFloat() {};
    float *_value;

private:
	float _min, _max, _step;
	std::string _unit;
	int (*_action)( SetupMenuValFloat *p );
};

class SetupMenuSelect:  public MenuEntry {
public:
	SetupMenuSelect();
	SetupMenuSelect(  std::string title, uint8_t *select, bool restart=false );
	void display(int mode=0);

	void addEntry( std::string ent ) {  _values.push_back( ent );
										_numval++; };
	void up();  // step up to parent
	void down();
	void press();
	virtual ~SetupMenuSelect() {};

private:
	uint8_t *_select;
	uint8_t _select_save;
	uint8_t _numval;
	bool    _restart;
	std::vector<std::string> _values;
};



#endif /* MAIN_SetupMenu_H_ */
