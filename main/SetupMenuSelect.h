/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef _SetupMenuSelect_H_
#define _SetupMenuSelect_H_
#include "SetupNG.h"
#include "MenuEntry.h"

struct bitfield_select {
   bool _restart      :1;
   bool _ext_handler  :1;
   bool _save         :1;
};

class SetupMenuSelect:  public MenuEntry
{
public:
	SetupMenuSelect();
	SetupMenuSelect( const char* title, bool restart=false, int (*action)(SetupMenuSelect *p) = 0, bool save=true, SetupNG<int> *anvs=0, bool ext_handler=false);
	virtual ~SetupMenuSelect();
	void display( int mode=0 );
	bool existsEntry( std::string ent );
    void addEntry( const char* ent );
	void addEntryList( const char ent[][4], int size );
	void delEntry( const char * ent );
	inline void updateEntry( const char * ent, int num ) { _values[ num ] = ent; }
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	const char *value();
	int getSelect();
	void setSelect( int sel );
	const char * getEntry() const ;

private:
	uint8_t  _select;       // limit to maximum 255 entries, as of today there are e.g. 134 different polars
	uint8_t  _select_save;
	uint8_t  _numval;
	bitfield_select bits;
	std::vector<const char *> _values;
	int (*_action)( SetupMenuSelect *p );
	SetupNG<int> *_nvs;
};

#endif
