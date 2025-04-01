/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#pragma once

#include "SetupNG.h"
#include "MenuEntry.h"
#include "SetupMenuValCommon.h"

class SetupMenuChar:  public MenuEntry
{
public:
	SetupMenuChar() = delete;
	SetupMenuChar( const char* title, e_restart_mode_t restart=RST_NONE, int (*action)(SetupMenuChar *p) = 0, bool save=true, 
		char *achar=0, uint32_t index=0, bool ext_handler=false, bool end_menu=false );
	virtual ~SetupMenuChar() = default;
	void enter() override;
	void display(int mode=0) override;
	void rot(int count);
	void up( int count ) override;
	void down( int count ) override;
	void press() override;
	void longPress() override;
	void escape() override {};
	const char *value() const override;

	bool existsEntry( std::string ent );
    void addEntry( const char* ent );
	void addEntryList( const char ent[][4], int size );
	void delEntry( const char * ent );
	inline void updateEntry( const char * ent, int num ) { _values[ num ] = ent; }
	int getSelect();
	void setSelect( int sel );
	inline uint32_t getCharIndex() const { return (uint32_t)_char_index; };
	int numEntries() { return _numval; };

private:
	int  _select;       // limit to maximum 255 entries, as of today there are e.g. 134 different polars
	int  _select_save;
	int  _numval;
	int  _char_index;   // position of character to be altered
	int  _col, _row;
	bitfield_select bits;
	std::vector<const char *> _values;
	int (*_action)( SetupMenuChar *p );
	char *_mychar;
};

