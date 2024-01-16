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
#include "SetupMenuValCommon.h"

struct bitfield_select {
   e_restart_mode_t _restart :2;
   bool _ext_handler         :1;
   bool _save                :1;
   bool _end_menu            :1;
   bool _sel_init            :1;
};

class SetupMenuSelect:  public MenuEntry
{
public:
	SetupMenuSelect( const char* title, e_restart_mode_t restart=RST_NONE, int (*action)(SetupMenuSelect *p) = 0, bool save=true, SetupNG<int> *anvs=0, bool ext_handler=false, bool end_menu=false );
	virtual ~SetupMenuSelect();
	void display( int mode=0 );
	bool existsEntry( std::string ent );
	void addEntry( const char* ent );
	virtual void addEntryList( const char ent[][4], int size );
	virtual void delEntry( const char * ent );
	void updateEntry( const char * ent, int num );
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	void escape() {};
	const char *value();
	virtual int getSelect();
	virtual void setSelect( int sel );
	void initSelect();
	const char * getEntry() const ;
	int numEntries() { return _numval; };
	virtual int getSelectCode();

	friend class SetupMenuSelectCodes;  // to be able to access private variables below

private:
	uint8_t  _select;       // limit to maximum 255 entries, as of today there are e.g. 134 different polars
	uint8_t  _select_save;
	uint8_t  _numval;
	bitfield_select bits;
	std::vector<const char *> _values;
	int (*_action)( SetupMenuSelect *p );
	SetupNG<int> *_nvs;
};

// derived class that stores the specified codes:

class SetupMenuSelectCodes: public SetupMenuSelect
{
public:
	SetupMenuSelectCodes( const char* title, e_restart_mode_t restart=RST_NONE,
	   int (*action)(SetupMenuSelect *p) = 0, bool save=true, SetupNG<int> *anvs=0,
	   bool ext_handler=false, bool end_menu=false ) :
	       SetupMenuSelect(title, restart, action, save, anvs, ext_handler, end_menu){};
	void addEntryCode( const char* ent, const int code );
	void updateEntryCode( const char * ent, int num, const int code );
	void addEntryList( const char ent[][4], int size );
	void delEntry( const char* ent );
	void delEntryByCode( const int code );
	void setSelect( int sel );
	int getSelect();
	int getSelectCode();
private:
	std::vector<int> _codes;
};

#endif
