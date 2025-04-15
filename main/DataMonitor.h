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

class SetupAction;

typedef enum { DIR_RX, DIR_TX } e_dir_t;

class DataMonitor: public MenuEntry
{
public:
	DataMonitor();
	void monitorString(e_dir_t dir, const char *s, int len );
	void start(SetupAction *p, ItfTarget ch);
	void display(int) override {}
	const char* value() const override { return ""; }
	void press() override;
	void rot( int count ) override {};
	void longPress() override;
	void escape() override {};
	int maxChar( const char *s, int pos, int len);

private:
	void printString(e_dir_t dir, const char *s, int len );
	void header(int len=0, e_dir_t dir=DIR_RX);
	void scroll(int scroll);
	int scrollpos;
	bool paused = true;
	ItfTarget channel = {};
	int rx_total = 0;
	int tx_total = 0;
	bool bin_mode = false;
};

extern DataMonitor *DM;
