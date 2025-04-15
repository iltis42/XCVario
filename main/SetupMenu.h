/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "MenuEntry.h"

constexpr const char* FORMATSTRING_AND_SPACE = "%s                ";

class SetupMenuValFloat;

typedef void (*SetupMenuCreator_t)(SetupMenu*);

class SetupMenu : public MenuEntry
{
public:
	SetupMenu() = delete;
	explicit SetupMenu(const char* title, SetupMenuCreator_t, int cont_id=0);
	virtual ~SetupMenu();
	void enter() override;
	void display( int mode=0 ) override;
	bool isLeaf() const override { return false; }
	const char *value() const override { return nullptr; };
	int getHighlight() const { return highlight; }
	int incHighlight() { return ++highlight; }
	void highlightTop() { highlight = -1; }
	void highlightLast() { highlight = _childs.size()-1; }
	void highlightEntry(MenuEntry*);
	int getNrChilds() const { return _childs.size(); }
	void setDynContent() { dyn_content = true; }
	void setDirty() { dirty = true; }
	int getContId() const { return content_id; }
	
	// the submenu structure
	MenuEntry* addEntry(MenuEntry* item);
	void       delEntry(MenuEntry* item);
	const MenuEntry* findMenu(const char *title) const;
	MenuEntry* getEntry(int n) const;

	void rot( int count ) override;
	void press() override;
	void longPress() override;

	static void catchFocus( bool activate );
	static bool focus;
	static gpio_num_t getGearWarningIO();

	static SetupMenu* createTopSetup();
	static SetupMenuValFloat *createQNHMenu();
	static SetupMenuValFloat *createVoltmeterAdjustMenu();

protected:
	SetupMenuCreator_t populateMenu;
	std::vector<MenuEntry*>  _childs;
	int highlight = -1;
	bool dyn_content = false; // reentrant create routine required, call it when dirty
	bool dirty = false; // need to refresh content/child list
	int content_id;
};

