/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef _SetupMenu_H_
#define _SetupMenu_H_
#include "IpsDisplay.h"
#include "ESPRotary.h"
#include "Setup.h"
#include "AnalogInput.h"
#include "BME280_ESP32_SPI.h"
#include "MenuEntry.h"
#include "SetupMenuValFloat.h"

class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	SetupMenu(  String title );
	void begin( IpsDisplay* display, ESPRotary * rotary, BME280_ESP32_SPI * bmp, AnalogInput *adc );
	void setup();
	void display( int mode=0 );
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	virtual ~SetupMenu() {};
};

#endif
