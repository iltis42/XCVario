/*
 * Datamonitor.h
 *
 *  Created on: Nov, 2021
 *      Author: iltis
 */
#pragma once

#include "Setup.h"
#include "SString.h"
#include <AdaptUGC.h>
#include "ESPRotary.h"
#include "SetupMenuSelect.h"

typedef enum e_dir { DIR_RX, DIR_TX } e_dir_t;

class DataMonitor: public RotaryObserver
{
public:
	DataMonitor();
	void monitorString( int ch, e_dir_t dir, const char *s );
	void start(SetupMenuSelect * p);
	void stop();
	void press();
	void release() {};
	void up( int count ) {};
	void down( int count ) {};
	void longPress();
	int maxChar( const char *s, int pos );
	void begin(AdaptUGC *theUcg) { ucg = theUcg; };
	bool active() { return mon_started; };
	void header( int ch );

private:
	void printString( int ch, e_dir_t dir, const char *s );
	bool mon_started;
	AdaptUGC *ucg;
	int scrollpos;
	bool paused;
	SetupMenuSelect * setup;
	int channel;
};
