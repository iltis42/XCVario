/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "MenuEntry.h"
#include "SetupMenuValCommon.h"
#include "SetupNG.h"

class SetupMenuSelect : public MenuEntry
{
public:
	SetupMenuSelect() = delete;
	explicit SetupMenuSelect( const char* title, e_restart_mode_t restart=RST_NONE, int (*action)(SetupMenuSelect *p) = 0, 
		bool save=true, SetupNG<int> *anvs=0, bool ext_handler=false, bool end_menu=false );
	virtual ~SetupMenuSelect() = default;
	void enter() override;
	void display(int mode=0) override;
	void rot(int count);
	void press() override;
	void longPress() override;
	const char *value() const override;

	bool existsEntry( std::string ent );
    void addEntry( const char* ent );
	void addEntryList( const char ent[][4], int size );
	void delEntry( const char * ent );
	void mkBinary(const char *what=nullptr);
	inline void updateEntry( const char *ent, int num ) { _values[ num ] = ent; }
	int getSelect();
	void setSelect( int sel );
	int numEntries() { return _values.size(); };

private:
	mutable int  _select = 0;
	int  _select_save = 0;
	int  _char_index = 0;   // position of character to be altered
	int  _col, _row;
	bitfield_select bits;
	std::vector<const char *> _values;
	int (*_action)( SetupMenuSelect *p );
	SetupNG<int> *_nvs;
	bool _show_inline;
};

