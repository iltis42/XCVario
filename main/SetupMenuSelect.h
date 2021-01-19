/*
 * SetupMenu.h
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#ifndef _SetupMenuSelect_H_
#define _SetupMenuSelect_H_
#include <string>
#include <vector>
#include <stdio.h>
#include "Setup.h"
#include "MenuEntry.h"


class SetupMenuSelect:  public MenuEntry {
public:
	SetupMenuSelect();
	SetupMenuSelect(  String title, int *select, bool restart=false, int (*action)(SetupMenuSelect *p) = 0, bool save=true, SetupNG<int> *anvs=0 );
	void display( int mode=0 );
	bool existsEntry( String ent );
	inline void addEntry( String ent ) {  _values.push_back( ent );
										  _numval++;
	};
	void delEntry( String ent );
	inline void updateEntry( String ent, int num ) {  _values[ num ] = ent; };
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	virtual ~SetupMenuSelect() {};
	inline int getSelect() { return *_select; };

private:
	int  *_select;
	int  _select_save;
	int  _numval;
	bool _restart;
	bool _save;
	std::vector<String> _values;
	int (*_action)( SetupMenuSelect *p );
	SetupNG<int> * _nvs;
};


#endif
