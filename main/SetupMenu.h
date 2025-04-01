/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "MenuEntry.h"

class SetupMenuValFloat;

class SetupMenu : public MenuEntry
{
public:
	SetupMenu() = delete;
	explicit SetupMenu(const char* title, void (menu_create)(SetupMenu* ptr));
	virtual ~SetupMenu();
	void enter() override;
	// void exit(int levels=0) override;
	void display( int mode=0 ) override;
	bool isLeaf() const override { return false; }
	const char *value() const override { return nullptr; };
	int getMenuPos() const { return highlight; }
	int menuPosInc() { return ++highlight; }
	void menuSetTop() { highlight = -1; }
	
	// the submenu structure
	MenuEntry* addEntry(MenuEntry* item);
	void       delEntry(MenuEntry* item);
	const MenuEntry* findMenu(const char *title) const;

	void up( int count ) override;
	void down( int count ) override;
	void press() override;
	void longPress() override {};

	static void catchFocus( bool activate );
	static bool focus;
	static gpio_num_t getGearWarningIO();

	static SetupMenu* createTopSetup();
	static SetupMenuValFloat *createQNHMenu();
	static SetupMenuValFloat *createVoltmeterAdjustMenu();

protected:
	void (*populateMenu)(SetupMenu*);
	std::vector<MenuEntry*>  _childs;
	int8_t  highlight = -1;
};

