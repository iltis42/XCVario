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
#include "MenuEntry.h"
#include "comm/InterfaceCtrl.h"

class SetupMenuSelect;

typedef enum { DIR_RX, DIR_TX } e_dir_t;

class DataMonitor: public MenuEntry
{
public:
	DataMonitor();
	void monitorString( ItfTarget ch, e_dir_t dir, const char *s, int len );
	void start(SetupMenuSelect * p, ItfTarget ch);
	// void enter() override;
	void exit(int ups=1) override;
	void display(int) override {}
	const char* value() const override { return ""; }
	void press() override;
	void up( int count ) override {};
	void down( int count ) override {};
	void longPress() override;
	void escape() override {};
	int maxChar( const char *s, int pos, int len, bool binary );
	bool active() { return mon_started; };

private:
	void printString( ItfTarget ch, e_dir_t dir, const char *s, bool binary, int len );
	void header( ItfTarget ch, bool binary, int len=0, e_dir_t dir=DIR_RX );
	void scroll(int scroll);
	bool mon_started;
	int scrollpos;
	bool paused;
	SetupMenuSelect *setup;
	ItfTarget channel;
	bool first;
	int rx_total;
	int tx_total;
};

extern DataMonitor DM;
