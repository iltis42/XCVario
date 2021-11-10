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


class SetupMenuSelect:  public MenuEntry
{
public:
	SetupMenuSelect();
	SetupMenuSelect( std::string title, bool restart=false, int (*action)(SetupMenuSelect *p) = 0, bool save=true, SetupNG<int> *anvs=0 );
	virtual ~SetupMenuSelect();
	void display( int mode=0 );
	bool existsEntry( std::string ent );
	inline void addEntry( const char* ent ) { addEntry( std::string(ent) ); }
	void addEntry( std::string ent ) { _values.push_back( ent ); _numval++; }
	void addEntryList( char ent[][4], int size );
	void delEntry( std::string ent );
	inline void updateEntry( std::string ent, int num ) { _values[ num ] = ent; }
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	void longPress();
	const char *value() const override;
	inline int getSelect() { return (int)_select; };
	inline void setSelect( int sel ) { _select = (int16_t)sel; };
	const char * getEntry() const ;

private:
	static char _val_str[20];
	int16_t  _select;
	int16_t  select_intern;
	int16_t  _select_save;
	int16_t  _numval;
	int8_t   _restart;
	int8_t   _save;
	std::vector<std::string> _values;
	int (*_action)( SetupMenuSelect *p );
	SetupNG<int> *_nvs;
};


#endif
