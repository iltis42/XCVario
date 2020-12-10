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
#include "BatVoltage.h"
#include "BME280_ESP32_SPI.h"

class MenuEntry: public RotaryObserver{
public:
	MenuEntry(){ _parent = 0;
				 highlight = 0;
				 pressed = false;
				 y=0;
				 helptext=0;
				 long_pressed = false;
	};
	virtual void display( int mode=0 ) = 0;
	virtual void release() { display(); };
	virtual void longPress() {};
	virtual ~MenuEntry() {};
	MenuEntry* addMenu( MenuEntry * item);
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

public:
	std::vector<MenuEntry*>  _childs;
	MenuEntry *_parent;
	String _title;
	static MenuEntry *root;
	static MenuEntry *selected;
	static IpsDisplay* _display;
	static ESPRotary* _rotary;
	static BatVoltage* _adc;
	static BME280_ESP32_SPI *_bmp;
	static bool _menu_enabled;
	int    highlight;
	bool   pressed;
	bool   long_pressed;
	char   *helptext;
	int    hypos;
	unsigned char y;
	static Ucglib_ILI9341_18x240x320_HWSPI *ucg;
	static float volume;

private:
};

class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	SetupMenu(  String title );
	void begin( IpsDisplay* display, ESPRotary * rotary, BME280_ESP32_SPI * bmp, BatVoltage *adc );
	void setup();
	void display( int mode=0 );
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	virtual ~SetupMenu() {};
};


class SetupMenuValFloat:  public MenuEntry {
public:
	SetupMenuValFloat();
	SetupMenuValFloat(  String title, float *value, String unit, float min, float max, float step, int (*action)(SetupMenuValFloat *p) = 0, bool end_menu=false, SetupNG<float> *anvs = 0 );
	void display(int mode=0);
	void displayVal();
	void setPrecision( int prec );
	static void showQnhMenu();
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	virtual ~SetupMenuValFloat() {};
    float *_value;
    static SetupMenuValFloat * qnh_menu;

private:
	float _min, _max, _step;
	bool _end_menu;
	String _unit;
	int (*_action)( SetupMenuValFloat *p );
	int _precision;
	SetupNG<float> * _nvs;
};

class SetupMenuSelect:  public MenuEntry {
public:
	SetupMenuSelect();
	SetupMenuSelect(  String title, int *select, bool restart=false, int (*action)(SetupMenuSelect *p) = 0, bool save=true, SetupNG<int> *anvs=0 );
	void display(int mode=0);
	bool existsEntry( String ent ){
	for( std::vector<String>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
			if( *iter == ent )
				return true;
		return false;
	};
	inline void addEntry( String ent ) {  _values.push_back( ent );
										  _numval++;
	};
	void delEntry( String ent ) {
		for( std::vector<String>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
			if( *iter == ent )
			{
				_values.erase( iter );
				_numval--;
				break;
			}
	};
	void updateEntry( String ent, int num ) {  _values[ num ] = ent; };
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	virtual ~SetupMenuSelect() {};
	int getSelect() { return *_select; };

private:
	int  *_select;
	int  _select_save;
	int  _numval;
	bool _restart;
	bool _save;
	std::vector<String> _values;
	int (*_action)( SetupMenuSelect *p );
	SetupNG<int> * _nvs;
};


#endif /* MAIN_SetupMenu_H_ */
