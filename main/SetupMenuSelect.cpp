/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "SetupMenuSelect.h"
#include "SetupMenu.h"

#include "sensor.h"
#include "Units.h"
#include "ESPAudio.h"
#include "logdef.h"


SetupMenuSelect::SetupMenuSelect( const char* title, e_restart_mode_t restart, int (*action)(SetupMenuSelect *p), 
									bool save, SetupNG<int> *anvs, bool ext_handler, bool end_menu ) :
	MenuEntry(),
	_action(action),
	_nvs(anvs)
{
	// ESP_LOGI(FNAME,"SetupMenuSelect( %s ) action: %x", title, (int)action );
	_title = title;
	bits._ext_handler = ext_handler;
	bits._end_setup = end_menu;
	bits._restart = restart;
	bits._save = save;
	if( _nvs ) {
		// ESP_LOGI(FNAME,"_nvs->key(): %s val: %d", _nvs->key(), (int)(_nvs->get()) );
		_select = _nvs->get(); // > _values.size()-1) ? _values.size()-1 : _nvs->get();
	}
}

void SetupMenuSelect::enter()
{
	if ( bits._locked ) { return; }

	_select_save = _select;
	_select = (_select > _values.size()-1) ? _values.size()-1 : _select;
	_show_inline = _values.size() > 9 || !helptext;
	MenuEntry::enter();
}

void SetupMenuSelect::display(int mode)
{
	_col = 1;
	_row = 50;
    ESP_LOGI(FNAME,"display title:%s action: %x", _title, (int)(_action));
	if ( !helptext ) {
		_row = (_parent->getMenuPos() + 1) * 25;
		MYUCG->setColor(COLOR_BLACK);
		MYUCG->drawFrame(1, _row + 3, 238, 25);
		MYUCG->setColor(COLOR_WHITE);
		_col = MYUCG->getStrWidth(_title) + 4;
		MYUCG->drawFrame(_col, _row + 3, 238, 25);
		_col += 7;
		_row += 25;
	}
	else if( bits._ext_handler ){  // handling is done only in action method
		ESP_LOGI(FNAME,"ext handler");
	}
	else
	{
		MYUCG->setPrintPos(1,25);
		ESP_LOGI(FNAME,"Title: %s ", _title );
		MYUCG->printf("<< %s",_title);
		if( _select > _values.size() )
			_select = _values.size()-1;
		ESP_LOGI(FNAME,"select=%d size=%d val=%s", _select, _values.size(), _values[_select].first );
		if( _show_inline ){
			MYUCG->setPrintPos( _col, _row );
			MYUCG->printf(FORMATSTRING_AND_SPACE, _values[_select].first );
		}else
		{
			for( int i=0; i<_values.size() && i<+10; i++ )	{
				MYUCG->setPrintPos( _col, _row+25*i );
				MYUCG->print( _values[i].first );
			}
			MYUCG->drawFrame( _col,(_select+1)*25+3,238,25 );
		}
	}
}

void SetupMenuSelect::rot(int count)
{
	int prev_select = _select;
	_select += count;
	if ( _select < 0 ) { _select = 0; }
	else if ( _select >= _values.size() ) { _select = _values.size()-1; }

	if( _show_inline )
	{
		MYUCG->setPrintPos(_col, _row);
		MYUCG->setFont(ucg_font_ncenR14_hr, true );
		MYUCG->printf(FORMATSTRING_AND_SPACE,_values[_select].first);
	}else {
		MYUCG->setColor(COLOR_BLACK);
		MYUCG->drawFrame( _col,(prev_select+1)*25+3,238,25 );  // blank old frame
		MYUCG->setColor(COLOR_WHITE);
		ESP_LOGI(FNAME,"rot %d", _select );
		MYUCG->drawFrame( _col,(_select+1)*25+3,238,25 );  // draw new frame
	}
}

void SetupMenuSelect::press()
{
	ESP_LOGI(FNAME,"press() ext handler: %d _select: %d", bits._ext_handler, _select );

	if( _nvs ) {
		_nvs->set((int)_select, false ); // do sync in next step
	}
	if ( helptext ) {
		SavedDelay(bits._save);
	}
	if( _action ){
		ESP_LOGI(FNAME,"calling action in press %d", _select );
		if ( (*_action)( this ) != 0 ) {
			return; // Menu got hijacked
		}
	}
	if( _select_save != _select ){
		if( bits._restart == RST_ON_EXIT ) {
			_restart = true;
		}else if( bits._restart == RST_IMMEDIATE ){
			if( _nvs ) {
				_nvs->commit();
			}
			MenuEntry::reBoot();
		}
		_select_save = _select;
	}
	if( bits._end_setup ){
		exit(-1);
		return;
	}
	exit();
}

void SetupMenuSelect::longPress(){
	press();
}

const char *SetupMenuSelect::value() const 
{
	if ( _values.size() > 0 ) {
		return _values[_select].first;
	}
	return "";
}

bool SetupMenuSelect::existsEntry( std::string ent ){
	for( std::vector<ITEM_t>::iterator iter = _values.begin(); iter != _values.end(); ++iter ) {
		if( std::string(iter->first) == ent ) {
			return true;
		}
	}
	return false;
}

void SetupMenuSelect::addEntry(const char* ent, int v)
{
	_values.push_back(ITEM_t(ent,v));

}

void SetupMenuSelect::addEntry(const char* ent)
{
	_values.push_back(ITEM_t(ent, _values.size()));
}

void SetupMenuSelect::addEntryList( const char ent[][4], int size )
{
	// ESP_LOGI(FNAME,"addEntryList() char ent[][4]");
	for( int i=0; i<size; i++ ) {
		_values.push_back(ITEM_t((char *)ent[i], _values.size()));
#ifdef DEBUG_MAX_ENTRIES
		if( num_max < _values.size() ){
			ESP_LOGI(FNAME,"addEntryList:%s  num:%d", (char *)ent[i], _values.size() );
			num_max = _values.size();
		}
#endif
	}
}

void SetupMenuSelect::delEntry( const char* ent )
{
	for( std::vector<ITEM_t>::iterator iter = _values.begin(); iter != _values.end(); ++iter ) {
		if( strcmp(iter->first, ent) == 0 )
		{
			_values.erase( iter );
			break;
		}
	}
	if( _select >= _values.size() ) {
		_select = _values.size()-1;
	}
}

void SetupMenuSelect::delAllEntries()
{
	_values.clear();
	_select = 0;
}

void SetupMenuSelect::mkEnable(const char *what)
{
	// precondition: _values is empty
	addEntry("disable");
	addEntry(what? what : "enable");
}

void SetupMenuSelect::mkConfirm()
{
	// precondition: _values is empty
	addEntry("cancel");
	addEntry("yes");
}

void SetupMenuSelect::setSelect( int sel ) {
	_select = (int16_t)sel;
	// if( _nvs ) // fixme
	// 	_select = _nvs->set( sel );
}






