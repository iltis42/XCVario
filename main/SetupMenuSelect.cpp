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
		_select = _nvs->get();
		if ( _select < -1 ) { _select = -1; }
		else if ( _select > _values.size()-1) { _select = _values.size()-1; }
	}
	setRotDynamic(1.f);
}

void SetupMenuSelect::enter()
{
	if ( bits._locked ) { return; }

	_select_save = _select;
	_select = (_select > _values.size()-1) ? _values.size()-1 : _select;
	_show_inline = _values.size() > 9 || canInline();
	MenuEntry::enter();
}

void SetupMenuSelect::display(int mode)
{
    ESP_LOGI(FNAME,"display title:%s action: %x", _title, (int)(_action));
	if ( _show_inline ) {
		indentHighlight(_parent->getHighlight());
	}
	else if( bits._ext_handler ){  // handling is done only in action method
		ESP_LOGI(FNAME,"ext handler");
	}
	else
	{
		ESP_LOGI(FNAME,"Title: %s ", _title );
		menuPrintLn("<<", 0);
		menuPrintLn(_title, 0, 30);
		if( _select > _values.size() )
			_select = _values.size()-1;
		ESP_LOGI(FNAME,"select=%d size=%d val=%s", _select, _values.size(), _values[_select].first );
		for( int i=0; i<_values.size() && i<+10; i++ )	{
			menuPrintLn(_values[i].first, i+1);
		}
		doHighlight(_select);
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
		char buf[320];
		sprintf(buf, FORMATSTRING_AND_SPACE, _values[_select].first);
		indentPrintLn(buf);
	}else {
		unHighlight(prev_select);
		ESP_LOGI(FNAME,"rot %d", _select );
		doHighlight(_select);
	}
}

void SetupMenuSelect::press()
{
	ESP_LOGI(FNAME,"press() ext handler: %d _select: %d", bits._ext_handler, _select );

	if( _nvs ) {
		_nvs->set(_values[_select].second, false ); // do sync in next step
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
	else if ( bits._end_menu ) {
		exit(2);
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

void SetupMenuSelect::mkEnable(const char *what, int val)
{
	// precondition: _values is empty
	// val can be read out to know "what" to en/disable
	addEntry("disable", val);
	addEntry(what? what : "enable", val);
}

void SetupMenuSelect::mkConfirm()
{
	// precondition: _values is empty
	addEntry("cancel");
	addEntry("yes");
}

void SetupMenuSelect::updateEntry(const char *ent, int num)
{
	if (_select >=0  && _select < _values.size()) {
		_values.at(num).first = ent;
	}
	ESP_LOGI(FNAME, "Out of bounds");
}

int SetupMenuSelect::getValue() const
{
	if (_select >=0  && _select < _values.size()) {
		return _values[_select].second;
	}
	ESP_LOGW(FNAME, "Out of bounds");
	return 0;
}

void SetupMenuSelect::setSelect(int sel)
{
	_select = sel;
}
