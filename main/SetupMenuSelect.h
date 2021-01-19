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
#include <set>
#include <list>
#include <stdio.h>
#include "IpsDisplay.h"
#include "ESPRotary.h"
#include "Setup.h"
#include "AnalogInput.h"
#include "MenuEntry.h"


class SetupMenuSelect:  public MenuEntry {
public:
	SetupMenuSelect();
	SetupMenuSelect(  String title, int *select, bool restart=false, int (*action)(SetupMenuSelect *p) = 0, bool save=true, SetupNG<int> *anvs=0 );
	void display(int mode=0);
	bool existsEntry( String ent ){
	for( std::vector<String>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
			if( *iter == ent )
				return true;
		return false;
	};
	inline void addEntry( String ent ) {  _values.push_back( ent );
										  _numval++;
	};
	void delEntry( String ent ) {
		for( std::vector<String>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
			if( *iter == ent )
			{
				_values.erase( iter );
				_numval--;
				if( *_select >= _numval )
					*_select = _numval-1;
				break;
			}
	};
	void updateEntry( String ent, int num ) {  _values[ num ] = ent; };
	void up( int count );  // step up to parent
	void down( int count );
	void press();
	virtual ~SetupMenuSelect() {};
	int getSelect() { return *_select; };

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
