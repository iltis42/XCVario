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
	explicit SetupMenuSelect( const char* title, e_restart_mode_t restart=RST_NONE, int (*action)(SetupMenuSelect *p) = 0, 
		bool save=true, SetupNG<int> *anvs=0, bool ext_handler=false, bool end_menu=false );
	virtual ~SetupMenuSelect() = default;
	void enter() override;
	void display(int mode=0) override;
	void rot(int count);
	bool existsEntry( std::string ent );
    void addEntry( const char* ent );
	void addEntryList( const char ent[][4], int size );
	void delEntry( const char * ent );
	void updateEntry( const char * ent, int num );
	void press() override;
	void longPress() override;
	const char *value() const override;
	int getSelect();
	void setSelect( int sel );
	const char * getEntry() const ;
	int numEntries() { return _numval; };

private:
	mutable uint8_t  _select = 0;       // limit to maximum 255 entries, as of today there are e.g. 134 different polars
	uint8_t  _select_save = 0;
	uint8_t  _numval = 0;
	bitfield_select bits;
	std::vector<const char *> _values;
	int (*_action)( SetupMenuSelect *p );
	SetupNG<int> *_nvs;
};

