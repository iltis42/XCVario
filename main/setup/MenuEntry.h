/*
 * SetupEntry.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "ESPRotary.h"
#include "setup/SetupNG.h"
#include <string>

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
	virtual bool isLeaf() const { return true; }
	virtual const char* value() const = 0; // content as string

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
	int freeBottomLines() const;
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
	bool        is_inline = false;
	static int16_t cur_indent;
	static int16_t cur_row;

private:
	static MenuEntry *selected;
};
