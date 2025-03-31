/*
 * SetupEntry.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "ESPRotary.h"
#include "SetupNG.h"

class PressureSensor;
class SetupMenu;
class SetupRoot;

constexpr const int DEFAULT_HELP_Y_POS = 180;

class MenuEntry : public RotaryObserver
{
	friend class SetupRoot;

public:
	MenuEntry() : RotaryObserver() {}
	virtual ~MenuEntry() = default;

	// from rotary
	void release() override {}
	void escape() override {}

	// own API
	virtual void enter();
	virtual void exit(int ups=1);
	virtual void display( int mode=0 ) = 0;
	virtual bool isLeaf() const { return true; }
	virtual const char* value() const = 0; // content as string
	//
	const char* getTitle() const { return _title; }
	SetupMenu* getParent() const { return _parent; }
	void regParent(SetupMenu* p);
	bool isActive() const { return selected == this; }
	void setHelp( const char *txt, int y=DEFAULT_HELP_Y_POS ) { helptext = (char*)txt; hypos = y; };
	void showhelp();
	void clear();
	const MenuEntry* findMenu(const char *title) const;
	void uprintf( int x, int y, const char* format, ...);
	void uprint( int x, int y, const char* str );
	void SavedDelay(bool showit=true);
	void reBoot();

protected:
	static bool _restart;
private:
	static MenuEntry *selected;

protected:
	SetupMenu  *_parent = nullptr;
	const char *_title = nullptr;
	const char *helptext = nullptr;
	int16_t     hypos = DEFAULT_HELP_Y_POS;
};
