/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "SetupMenuChar.h"
#include "SetupMenu.h"

#include "sensor.h"
#include "Units.h"
#include "ESPAudio.h"
#include "logdef.h"


SetupMenuChar::SetupMenuChar( const char* title, e_restart_mode_t restart, int (*action)(SetupMenuChar *p), 
								bool save, char *achar, uint32_t index, bool ext_handler, bool end_menu ) :
	MenuEntry()
{
	ESP_LOGI(FNAME,"SetupMenuChar( %s ), char: %c", title, *achar );
	bits._ext_handler = ext_handler;
	_title = title;
	_select = 0;
	_select_save = 0;
	bits._end_setup = end_menu;
	_mychar = 0;
	_char_index = index;
	if( achar ) {
		_mychar = achar+_char_index;
	}
	_numval = 0;
	bits._restart = restart;
	_action = action;
	bits._save = save;

}

void SetupMenuChar::enter()
{
	if ( helptext ) {
		clear();
	}
	MenuEntry::enter();
}

void SetupMenuChar::display(int mode)
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
		ESP_LOGI(FNAME,"select=%d numval=%d size=%d val=%s", _select, _numval, _values.size(), _values[_select] );
		if( _numval > 9 ){
			MYUCG->setPrintPos( _col, _row );
			MYUCG->printf( "%s                ", _values[_select] );
		}else
		{
			for( int i=0; i<_numval && i<+10; i++ )	{
				MYUCG->setPrintPos( _col, _row+25*i );
				MYUCG->print( _values[i] );
			}
			MYUCG->drawFrame( _col,(_select+1)*25+3,238,25 );
		}

		showhelp();
	}
}

void SetupMenuChar::rot(int count)
{
	if ( _numval <= 9 ) {
		count = sign(count);
	}
	_select += count;
	if ( _select < 0 ) { _select = 0; }
	else if ( _select > _numval ) { _select = _numval; }

	if( _numval > 9 )
	{
		MYUCG->setPrintPos(_col, _row);
		MYUCG->setFont(ucg_font_ncenR14_hr, true );
		MYUCG->printf("%s                   ", _values[_select] );
	}else {
		MYUCG->setColor(COLOR_BLACK);
		MYUCG->drawFrame( _col,(_select+1)*25+3,238,25 );  // blank old frame
		MYUCG->setColor(COLOR_WHITE);
		ESP_LOGI(FNAME,"rot %d", _select );
		MYUCG->drawFrame( _col,(_select+1)*25+3,238,25 );  // draw new frame
	}
}

void SetupMenuChar::up(int count) {
	rot(count);
}
void SetupMenuChar::down(int count) {
	rot(-count);
}

void SetupMenuChar::longPress(){
	press();
}

void SetupMenuChar::press()
{
	ESP_LOGI(FNAME,"press() ext handler: %d _select: %d", bits._ext_handler, _select );

	SavedDelay(bits._save);
	if ( helptext ) {
		_parent->menuSetTop();
	}
	if( _action != 0 ){
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

const char *SetupMenuChar::value() const
{
	// ESP_LOGI(FNAME,"getEntry() select:%d", _select );
	return _values[ _select ];
}

bool SetupMenuChar::existsEntry( std::string ent ){
	for( std::vector<const char*>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
		if( std::string(*iter) == ent )
			return true;
	return false;
}

void SetupMenuChar::addEntry( const char* ent ) {
	_values.push_back( ent ); _numval++;

}

void SetupMenuChar::addEntryList( const char ent[][4], int size )
{
	// ESP_LOGI(FNAME,"addEntryList() char ent[][4]");
	for( int i=0; i<size; i++ ) {
		_values.push_back( (char *)ent[i] ); _numval++;

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
	for( std::vector<const char *>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
		if( std::string(*iter) == std::string(ent) )
		{
			_values.erase( iter );
			_numval--;
			if( _select >= _numval )
				_select = _numval-1;
			break;
		}
}

int SetupMenuChar::getSelect() {
	return (int)_select;
}

void SetupMenuChar::setSelect( int sel ) {
	_select = (int16_t)sel;
}

