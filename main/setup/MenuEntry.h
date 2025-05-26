/*
 * SetupEntry.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "ESPRotary.h"
#include "setup/SetupNG.h"
#include "setup/SetupMenuCommon.h"
#include <string>

struct bitfield {
    e_restart_mode_t _restart :2;
    bool _ext_handler         :1; // ??
    bool _end_setup           :1; // roll setup on exit
    bool _end_menu            :1; // just terminate the containing menu (two levels up)
    bool _locked              :1; // cannot enter
    bool _is_inline           :1; // inline menu
    bool _live_update         :1;
    uint8_t _precision        :4;
};


class PressureSensor;
class SetupMenu;
class SetupRoot;

constexpr const int DEFAULT_HELP_Y_POS = 180;

class MenuEntry : public RotaryObserver
{
	friend class SetupRoot;

public:
	MenuEntry();
	virtual ~MenuEntry() = default;

	// from Observer
	void release() override {} // not used
	void escape() override {} // not used

	// own API
	virtual void enter();
	virtual void exit(int ups=1);
	virtual void display( int mode=0 ) = 0;
	virtual void refresh() {} // reread temp values coping with side efects on refreshing the display
	virtual bool isLeaf() const { return true; }
	virtual const char* value() const = 0; // content as string
	virtual int freeBottomLines() const;

	void lock() { bits._locked = true; }
	void unlock() { bits._locked = false; }
	bool isLocked() const { return bits._locked; }

	// helper
	const char* getTitle() const { return _title.c_str(); }
	SetupMenu* getParent() const { return _parent; }
	void regParent(SetupMenu* p);
	bool isRoot() const;
	bool isActive() const { return selected == this; }
	void setHelp( const char *txt, int y=DEFAULT_HELP_Y_POS ) { helptext = (char*)txt; };
	void doHighlight(int sel) const;
    void unHighlight(int sel) const;
    void indentHighlight(int sel);
	void indentPrintLn(const char *str) const;
	bool canInline() const;
	int nrOfHelpLines() const;
	bool showhelp(bool inln=false);
	void clear();
	void clearHelpLines() const;
	const MenuEntry* findMenu(const char *title) const;
	void menuPrintLn(const char* str, int sel, int x=1) const;
	void uprint( int x, int y, const char* str );
	void SavedDelay(bool showit=true);
	void scheduleReboot() { _restart = true; }
	void reBoot();

public:
	static int16_t dwidth;
	static int16_t dheight;

protected:
	static bool _restart;

protected:
	SetupMenu  *_parent = nullptr;
	std::string _title;
	const char *helptext = nullptr;
	bitfield bits = {};
	// bool        is_inline = false;
	static int16_t cur_indent;
	static int16_t cur_row;

private:
	static MenuEntry *selected;
};
