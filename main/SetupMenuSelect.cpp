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

const char * SetupMenuSelect::getEntry() const
{
	// ESP_LOGI(FNAME,"getEntry() select:%d", _select );
	return _values[ _select ];
}

const char *SetupMenuSelect::value() const 
{
	if( _nvs ){
		_select = _nvs->get() > _numval-1 ? _numval-1 : _nvs->get();
	}
	return getEntry();
}

bool SetupMenuSelect::existsEntry( std::string ent ){
	for( std::vector<const char*>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
		if( std::string(*iter) == ent )
			return true;
	return false;
}

#ifdef DEBUG_MAX_ENTRIES
// static int num_max = 0;
#endif

void SetupMenuSelect::addEntry( const char* ent ) {
	_values.push_back( ent ); _numval++;
#ifdef DEBUG_MAX_ENTRIES
	if( num_max < _numval ){
		ESP_LOGI(FNAME,"add ent:%s  num:%d", ent, _numval );
		num_max = _numval;
	}
#endif
}

void SetupMenuSelect::updateEntry( const char * ent, int num ) {
	ESP_LOGI(FNAME,"updateEntry ent:%s  num:%d total%d", ent, num, _numval );
	_values.at(num) = ent;
}

void SetupMenuSelect::setSelect( int sel ) {
	_select = (int16_t)sel;
	if( _nvs )
		_select = _nvs->set( sel );
}

int SetupMenuSelect::getSelect() {
	if( _nvs )
		_select = _nvs->get();
	return (int)_select;
}

void SetupMenuSelect::addEntryList( const char ent[][4], int size )
{
	// ESP_LOGI(FNAME,"addEntryList() char ent[][4]");
	for( int i=0; i<size; i++ ) {
		_values.push_back( (char *)ent[i] ); _numval++;
#ifdef DEBUG_MAX_ENTRIES
		if( num_max < _numval ){
			ESP_LOGI(FNAME,"addEntryList:%s  num:%d", (char *)ent[i], _numval );
			num_max = _numval;
		}
#endif
	}
}

void SetupMenuSelect::delEntry( const char* ent ) {
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

SetupMenuSelect::SetupMenuSelect( const char* title, e_restart_mode_t restart, int (*action)(SetupMenuSelect *p), 
									bool save, SetupNG<int> *anvs, bool ext_handler, bool end_menu ) :
	MenuEntry(),
	_action(action),
	_nvs(anvs)
{
	// ESP_LOGI(FNAME,"SetupMenuSelect( %s ) action: %x", title, (int)action );
	bits._ext_handler = ext_handler;
	_title = title;
	bits._end_setup = end_menu;
	bits._restart = restart;
	bits._save = save;
}

void SetupMenuSelect::enter()
{
	if( _nvs ) {
		// ESP_LOGI(FNAME,"_nvs->key(): %s val: %d", _nvs->key(), (int)(_nvs->get()) );
		_select = (int16_t)(*(int *)(_nvs->getPtr()));
		_select_save = (int16_t)_nvs->get();
	}
	MenuEntry::enter();
}

void SetupMenuSelect::display(int mode)
{

	ESP_LOGI(FNAME,"display() title:%s action: %x", _title, (int)(_action));
	clear();
	if( bits._ext_handler ){  // handling is done only in action method
		ESP_LOGI(FNAME,"ext handler");
	}
	else
	{
		MYUCG->setPrintPos(1,25);
		ESP_LOGI(FNAME,"Title: %s ", _title );
		MYUCG->printf("<< %s",_title);
		if( _select > _values.size() )
			_select = _numval-1;
		// ESP_LOGI(FNAME,"select=%d numval=%d size=%d val=%s", _select, _numval, _values.size(), _values[_select]  );
		if( _numval > 9 ){
			MYUCG->setPrintPos( 1, 50 );
			MYUCG->printf( "%s                ", _values[_select] );
		}else
		{
			for( int i=0; i<_numval && i<+10; i++ )	{
				MYUCG->setPrintPos( 1, 50+25*i );
				MYUCG->print( _values[i] );
			}
			MYUCG->drawFrame( 1,(_select+1)*25+3,238,25 );
		}

		showhelp();

	}
}

void SetupMenuSelect::down(int count)
{
	if( _numval > 9 ){
		while( count ) {
			if( (_select) > 0 )
				(_select)--;
			count--;
		}
		MYUCG->setPrintPos( 1, 50 );
		MYUCG->setFont(ucg_font_ncenR14_hr, true );
		MYUCG->printf("%s                  ",_values[_select] );
	}else {
		MYUCG->setColor(COLOR_BLACK);
		MYUCG->drawFrame( 1,(_select+1)*25+3,238,25 );  // blank old frame
		MYUCG->setColor(COLOR_WHITE);
		while( (_select) >  0 && count){
			(_select)--;
			count--;
		}
		ESP_LOGI(FNAME,"val down %d", _select );
		MYUCG->drawFrame( 1,(_select+1)*25+3,238,25 );  // draw new frame
	}
}

void SetupMenuSelect::up(int count)
{
	if( _numval > 9 )
	{
		while( count ) {
			if( (_select) <  _numval-1 )
				(_select)++;
			count--;
		}
		MYUCG->setPrintPos( 1, 50 );
		MYUCG->setFont(ucg_font_ncenR14_hr, true );
		MYUCG->printf("%s                   ", _values[_select] );
	}else {
		MYUCG->setColor(COLOR_BLACK);
		MYUCG->drawFrame( 1,(_select+1)*25+3,238,25 );  // blank old frame
		MYUCG->setColor(COLOR_WHITE);
		while ( (_select) < _numval-1 && count ){
			(_select)++;
			count--;
		}
		ESP_LOGI(FNAME,"val up %d", _select );
		MYUCG->drawFrame( 1,(_select+1)*25+3,238,25 );  // draw new frame
	}
}

void SetupMenuSelect::longPress(){
	press();
}

void SetupMenuSelect::press()
{
	ESP_LOGI(FNAME,"press() ext handler: %d _select: %d", bits._ext_handler, _select );

	if( _nvs ) {
		_nvs->set((int)_select, false ); // do sync in next step
		_nvs->commit();
	}
	SavedDelay();
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
	_parent->menuSetTop();
	if( bits._end_setup ){
		ESP_LOGI(FNAME,"press() end_menu");
		exit(-1);
	}
	exit();
}
