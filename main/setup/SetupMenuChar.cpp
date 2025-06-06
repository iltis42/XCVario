/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "setup/SetupMenuChar.h"
#include "setup/SetupMenu.h"

#include "sensor.h"
#include "Units.h"
#include "ESPAudio.h"
#include "logdef.h"


SetupMenuChar::SetupMenuChar( const char* title, e_restart_mode_t restart, int (*action)(SetupMenuChar *p), 
								char *achar, uint32_t index, bool ext_handler, bool end_menu ) :
	MenuEntry(),
	_char_index(index),
	_action(action)
{
	ESP_LOGI(FNAME,"SetupMenuChar( %s ), char: %c", title, *achar );
	_title.assign(title);
	if( achar ) {
		_mychar = achar+_char_index;
	}
	bits._ext_handler = ext_handler;
	bits._end_setup = end_menu;
	bits._restart = restart;
}

void SetupMenuChar::display(int mode)
{
    ESP_LOGI(FNAME,"display title:%s action: %x", _title.c_str(), (int)(_action));
	if ( _values.size() > 9 || canInline() ) {
		indentHighlight(_parent->getHighlight());
	}
	else if( bits._ext_handler ){  // handling is done only in action method
		ESP_LOGI(FNAME,"ext handler");
	}
	else
	{
		ESP_LOGI(FNAME,"Title: %s ", _title.c_str() );
		menuPrintLn("<<", 0);
		menuPrintLn(_title.c_str(), 0, 30);
		ESP_LOGI(FNAME,"select=%d numval=%d size=%d val=%s", _select, _values.size(), _values.size(), _values[_select] );
		for( int i=0; i<_values.size() && i<10; i++ )	{
			menuPrintLn(_values[i], i+1);
		}
		doHighlight(_select);
	}
}

void SetupMenuChar::rot(int count)
{
	int prev_select = _select;
	_select += count;
	if ( _select < 0 ) { _select = 0; }
	else if ( _select > _values.size() ) { _select = _values.size(); }

	if( _values.size() > 9 )
	{
		char buf[320];
		sprintf(buf, FORMATSTRING_AND_SPACE, _values[_select]);
		indentPrintLn(buf);
	}else {
		unHighlight(prev_select);
		ESP_LOGI(FNAME,"rot %d", _select );
		doHighlight(_select);
	}
}

void SetupMenuChar::press()
{
	ESP_LOGI(FNAME,"press() ext handler: %d _select: %d", bits._ext_handler, _select );

	if ( helptext ) {
		SavedDelay(_select_save != _select);
	}
	if( _action ){
		ESP_LOGI(FNAME,"calling action in press %d", _select );
		(*_action)( this );
	}
	if( _select_save != _select && bits._restart ) {
		_restart = true;
	}
	if( bits._end_setup ) {
		exit(-1);
		return;
	}
	exit();
}

void SetupMenuChar::longPress(){
	press();
}

const char *SetupMenuChar::value() const
{
	// ESP_LOGI(FNAME,"getEntry() select:%d", _select );
	if ( _select > 0 && _select < _values.size() ) {
		return _values[_select];
	}
	return "";
}

bool SetupMenuChar::existsEntry( std::string ent ){
	for( std::vector<const char*>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
		if( std::string(*iter) == ent )
			return true;
	return false;
}

void SetupMenuChar::addEntry( const char* ent ) {
	_values.push_back( ent );

}

void SetupMenuChar::addEntryList( const char ent[][4], int size )
{
	// ESP_LOGI(FNAME,"addEntryList() char ent[][4]");
	for( int i=0; i<size; i++ ) {
		_values.push_back( (char *)ent[i] );
	}
	if( _mychar ) {
		for( int i=0; i<_values.size(); i++ ){
			if( *(_values[i]) == *_mychar ){
				_select_save = _select = i;
				break;
				ESP_LOGI(FNAME,"addEntryList, found %c at i=%d", *_mychar, i );
			}
		}
	}
}

void SetupMenuChar::delEntry( const char* ent ) {
	for( std::vector<const char *>::iterator iter = _values.begin(); iter != _values.end(); ++iter ) {
		if( std::string(*iter) == std::string(ent) )
		{
			_values.erase( iter );
			break;
		}
	}
	if( _select >= _values.size() ) {
		_select = _values.size()-1;
	}
}

int SetupMenuChar::getSelect() {
	return (int)_select;
}

void SetupMenuChar::setSelect( int sel ) {
	_select = (int16_t)sel;
}

