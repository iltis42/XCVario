/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "MenuEntry.h"
#include "SetupMenuValFloat.h"

class IpsDisplay;
class ESPRotary;
class PressureSensor;
class AnalogInput;

class SetupMenu:  public MenuEntry {
public:
	SetupMenu();
	explicit SetupMenu( const char* title, void (menu_create)(SetupMenu* ptr)  );
	virtual ~SetupMenu();
	void begin( IpsDisplay* display, PressureSensor * bmp, AnalogInput *adc );
	void display( int mode=0 ) override;
	bool isLeaf() const override { return false; }
	const char *value() { return 0; };
	// the submenu structure
    MenuEntry* getFirst() const;
	MenuEntry* addEntry(MenuEntry* item);
	void       delEntry(MenuEntry* item);
	const MenuEntry* findMenu(const char *title) const;

	void up( int count ) override;
	void down( int count ) override;
	void press() override;
	void longPress() override;
	void escape() override;
	void showMenu();
	void create_subtree();
	void delete_subtree();

	static void catchFocus( bool activate );
	static bool focus;
	static gpio_num_t getGearWarningIO();

	static SetupMenuValFloat * createQNHMenu();

private:
	void (*menu_create_ptr)(SetupMenu*);
	std::vector<MenuEntry*>  _childs;
	uint8_t subtree_created;
};

