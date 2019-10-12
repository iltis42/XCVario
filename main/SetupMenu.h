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
#include "IpsDisplay.h"
#include "ESPRotary.h"
#include "Setup.h"
#include "SetupVolt.h"
#include "BatVoltage.h"
#include "BME280_ESP32_SPI.h"

class MenuEntry: public RotaryObserver{
public:
	MenuEntry(){ _parent = 0;
				 highlight = 0;
				 pressed = false;
				 y=0;
				 helptext=0;
	};
	virtual void display( int mode=0 ) = 0;
	virtual void release() { display(); };
	virtual void longP() {};
	virtual ~MenuEntry() {};
	MenuEntry* addMenu( MenuEntry * item);
	MenuEntry* findMenu( String title, MenuEntry* start=root  );
	void togglePressed() {
		if( pressed )
			pressed = false;
		else
			pressed = true;
	}
	void setHelp( const char *txt ) { helptext = (char*)txt; };
	void showhelp( int y );
	void clear();

public:

	std::vector<MenuEntry*>  _childs;
	MenuEntry *_parent;
	String _title;
	static MenuEntry *root;
	static MenuEntry *selected;
	static IpsDisplay* _display;
	static ESPRotary* _rotary;
	static Setup *_setup;
	static SetupVolt *_setupv;
	static BatVoltage* _adc;
	static BME280_ESP32_SPI *_bmp;
	static bool _menu_enabled;
	int    highlight;
	bool   pressed;
	char   *helptext;
	unsigned char y;

private:
};

class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	SetupMenu(  String title );
	void begin( IpsDisplay* display, ESPRotary * rotary, Setup * setup, SetupVolt * setupv, BME280_ESP32_SPI * bmp, BatVoltage *adc );
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
	SetupMenuValFloat(  String title, float *value, String unit, float min, float max, float step, int (*action)(SetupMenuValFloat *p) = 0 );
	void display(int mode=0);

	void up();  // step up to parent
	void down();
	void press();
	virtual ~SetupMenuValFloat() {};
    float *_value;

private:
	float _min, _max, _step;
	String _unit;
	int (*_action)( SetupMenuValFloat *p );
};

class SetupMenuSelect:  public MenuEntry {
public:
	SetupMenuSelect();
	SetupMenuSelect(  String title, uint8_t *select, bool restart=false, int (*action)(SetupMenuSelect *p) = 0 );
	void display(int mode=0);

	void addEntry( String ent ) {  _values.push_back( ent );
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
	std::vector<String> _values;
	int (*_action)( SetupMenuSelect *p );
};



#endif /* MAIN_SetupMenu_H_ */
