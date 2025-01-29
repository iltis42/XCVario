/*
 * Datamonitor.h
 *
 *  Created on: Nov, 2021
 *      Author: iltis
 */
#pragma once

#include "Setup.h"
#include "SString.h"
#include "AdaptUGC.h"
#include "ESPRotary.h"
#include "SetupMenuSelect.h"
#include "comm/InterfaceCtrl.h"

typedef enum { DIR_RX, DIR_TX } e_dir_t;

class DataMonitor: public RotaryObserver
{
public:
	DataMonitor();
	void monitorString( ItfTarget ch, e_dir_t dir, const char *s, int len );
	void start(SetupMenuSelect * p, ItfTarget ch);
	void stop();
	void press();
	void release() {};
	void up( int count ) {};
	void down( int count ) {};
	void longPress();
	void escape() {};
	int maxChar( const char *s, int pos, int len, bool binary=false );
	void begin(AdaptUGC *theUcg) { ucg = theUcg; };
	bool active() { return mon_started; };

private:
	void printString( ItfTarget ch, e_dir_t dir, const char *s, bool binary, int len );
	void header( ItfTarget ch, bool binary=false, int len=0, e_dir_t dir=DIR_RX );
	void scroll(int scroll);
	bool mon_started;
	AdaptUGC *ucg;
	int scrollpos;
	bool paused;
	SetupMenuSelect * setup;
	ItfTarget channel;
	bool first;
	int rx_total;
	int tx_total;
	static SemaphoreHandle_t mutex;
};

extern DataMonitor DM;
