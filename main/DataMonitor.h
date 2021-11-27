/*
 * Datamonitor.h
 *
 *  Created on: Nov, 2021
 *      Author: iltis
 */
#pragma once

#include "Setup.h"
#include "SString.h"
#include <Ucglib.h>
#include "ESPRotary.h"

typedef enum e_dir { DIR_RX, DIR_TX } e_dir_t;

class DataMonitor: public RotaryObserver
{
public:
	DataMonitor();
	void monitorString( int ch, e_dir_t dir, const char *s );
	void start();
	void stop();
	void press();
	void release() {};
	void up( int count ) {};
	void down( int count ) {};
	void longPress();
	int maxChar( const char *s, int pos );
	void begin(Ucglib_ILI9341_18x240x320_HWSPI *theUcg, ESPRotary * aRotary ) { ucg = theUcg; _rotary = aRotary; };
	bool active() { return mon_started; };

private:
	bool mon_started;
	Ucglib_ILI9341_18x240x320_HWSPI *ucg;
	ESPRotary * _rotary;
	int scrollpos;
	bool paused;
};
