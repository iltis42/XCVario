/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include <logdef.h>
#include <sensor.h>
#include "Units.h"
#include "SetupMenuSelect.h"
#include "ESPAudio.h"

const char * SetupMenuSelect::getEntry() const
{
	// ESP_LOGI(FNAME,"getEntry() select:%d", _select );
	return _values.at(_select);
}

const char *SetupMenuSelect::value() {
	getSelect();
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
	_values.push_back( ent );
	_numval++;
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

void SetupMenuSelect::setSelectNVS( int sel ) {
	if( _nvs )
		_select = _nvs->set( sel );
}

void SetupMenuSelect::setSelect( int sel ) {
	if (sel < 0)
		sel = 0;
	if (sel > _numval-1)
		sel = _numval-1;
	_select = (int16_t)sel;
	setSelectNVS( sel );
}

int SetupMenuSelect::getSelect() {
	if( _nvs )
		_select = _nvs->get() > _numval-1 ? _numval-1 : _nvs->get();
	return (int)_select;
}

int SetupMenuSelect::getSelectCode() {
	return _select;   // the value of the index is also the code for nvs
}

// >>> what is the meaning of the [4]?  The strings must be 4-chars-long?
void SetupMenuSelect::addEntryList( const char ent[][4], int size )
{
	// ESP_LOGI(FNAME,"addEntryList() char ent[][4]");
	for( int i=0; i<size; i++ ) {
		_values.push_back( (char *)ent[i] );
		_numval++;
#ifdef DEBUG_MAX_ENTRIES
		if( num_max < _numval ){
			ESP_LOGI(FNAME,"addEntryList:%s  num:%d", (char *)ent[i], _numval );
			num_max = _numval;
		}
#endif
	}
}

void SetupMenuSelect::delEntry( const char* ent ) {
	for( std::vector<const char *>::iterator iter = _values.begin(); iter != _values.end(); ++iter ) {
		if( std::string(*iter) == std::string(ent) )
		{
			_values.erase( iter );
			_numval--;
			if( _select >= _numval )
				_select = _numval-1;
			break;
		}
	}
}

void SetupMenuSelect::initSelect() {
	if (! bits._sel_init) {  // _select not initialized yet
		getSelect();         // sets _select to the index of the code that equals nvs value
		bits._sel_init = true;
	}
}

SetupMenuSelect::SetupMenuSelect( const char* title, e_restart_mode_t restart, int (*action)(SetupMenuSelect *p), bool save, SetupNG<int> *anvs, bool ext_handler, bool end_menu ) {
	// ESP_LOGI(FNAME,"SetupMenuSelect( %s ) action: %x", title, (int)action );
	attach(this);
	bits._ext_handler = ext_handler;
	_title = title;
	_nvs = 0;
	_select = 0;
	_select_save = 0;
	bits._end_menu = end_menu;
	highlight = -1;
	_numval = 0;
	bits._restart = restart;
	_action = action;
	bits._save = save;
	if( anvs ) {
		_nvs = anvs;
		// ESP_LOGI(FNAME,"_nvs->key(): %s val: %d", _nvs->key(), (int)(_nvs->get()) );
// >>> If codes, Before the entries are added, there is nothing to set _select to!
//     Instead use initSelect()
// >>> BTW why is _select set via getPtr()?
//		_select = (int16_t)(*(int *)(_nvs->getPtr()));
		_select_save = (int16_t)_nvs->get();
	}
	bits._sel_init = false;
}

SetupMenuSelect::~SetupMenuSelect()
{
	detach(this);
}

void SetupMenuSelect::display( int mode ){
	if( (selected != this) || !gflags.inSetup  )
		return;
	initSelect();
	ESP_LOGI(FNAME,"display() pressed:%d title:%s action: %x hl:%d", pressed, _title, (int)(_action), highlight );
	clear();
	if( bits._ext_handler ){  // handling is done only in action method
		ESP_LOGI(FNAME,"ext handler");
		selected = _parent;
	}else
	{
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos(1,25);
		ESP_LOGI(FNAME,"Title: %s ", _title );
		ucg->printf("<< %s",_title);
		xSemaphoreGive(spiMutex );
		if( _select > _values.size() )
			_select = _numval-1;
		// ESP_LOGI(FNAME,"select=%d numval=%d size=%d val=%s", _select, _numval, _values.size(), _values[_select]  );
		if( _numval > 9 ){
			xSemaphoreTake(spiMutex,portMAX_DELAY );
			ucg->setPrintPos( 1, 50 );
			ucg->printf( "%s                ", _values[_select] );
			xSemaphoreGive(spiMutex );
		}else
		{
			xSemaphoreTake(spiMutex,portMAX_DELAY );
			for( int i=0; i<_numval && i<+10; i++ )	{
				ucg->setPrintPos( 1, 50+25*i );
				ucg->print( _values[i] );
			}
			ucg->drawFrame( 1,(_select+1)*25+3,238,25 );
			xSemaphoreGive(spiMutex );
		}

		int y=_numval*25+50;
		showhelp( y );
		if(mode == 1 && bits._save == true ){
			xSemaphoreTake(spiMutex,portMAX_DELAY );
			ucg->setColor( COLOR_BLACK );
			ucg->drawBox( 1,280,240,40 );
			ucg->setPrintPos( 1, 300 );
			ucg->setColor( COLOR_WHITE );
			ucg->print( "Saved" );
			xSemaphoreGive(spiMutex );
		}
		if( mode == 1 )
			delay(1000);
	}
}

void SetupMenuSelect::down(int count){
	if( (selected != this) || !gflags.inSetup )
		return;
	initSelect();
	if( _numval > 9 ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		while( count > 0 ) {
			if( (_select) > 0 )
				(_select)--;
			count--;
		}
		ucg->setPrintPos( 1, 50 );
		ucg->setFont(ucg_font_ncenR14_hr, true );
		ucg->printf("%s                  ",_values[_select] );
		xSemaphoreGive(spiMutex );
	}else {
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setColor(COLOR_BLACK);
		ucg->drawFrame( 1,(_select+1)*25+3,238,25 );  // blank old frame
		ucg->setColor(COLOR_WHITE);
		while( (_select) >  0 && count){
			(_select)--;
			count--;
		}
		ESP_LOGI(FNAME,"val down %d", _select );
		ucg->drawFrame( 1,(_select+1)*25+3,238,25 );  // draw new frame
		xSemaphoreGive(spiMutex );
	}
}

void SetupMenuSelect::up(int count){
	if( (selected != this) || !gflags.inSetup )
		return;
	initSelect();
	if( _numval > 9 )
	{
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		while( count > 0 ) {
			if( (_select) <  _numval-1 )
				(_select)++;
			count--;
		}
		ucg->setPrintPos( 1, 50 );
		ucg->setFont(ucg_font_ncenR14_hr, true );
		ucg->printf("%s                   ", _values[_select] );
		xSemaphoreGive(spiMutex );
	}else {
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setColor(COLOR_BLACK);
		ucg->drawFrame( 1,(_select+1)*25+3,238,25 );  // blank old frame
		ucg->setColor(COLOR_WHITE);
		while ( (_select) < _numval-1 && count ){
			(_select)++;
			count--;
		}
		ESP_LOGI(FNAME,"val up %d", _select );
		ucg->drawFrame( 1,(_select+1)*25+3,238,25 );  // draw new frame
		xSemaphoreGive(spiMutex );
	}
}

void SetupMenuSelect::longPress(){
	press();
}

void SetupMenuSelect::press(){
	if( (selected != this) || !gflags.inSetup  )
		return;
	initSelect();
	ESP_LOGI(FNAME,"press() ext handler: %d press: %d _select: %d selected %p", bits._ext_handler, pressed, _select, selected );
	if ( pressed ){
		if( _select_save != getSelectCode() )
			display( 1 );
		//else
		//	display();
		if( bits._end_menu ){
			ESP_LOGI(FNAME,"press() end_menu");
			selected = root;
		}
		else if( _parent != 0) {
			ESP_LOGI(FNAME,"go to parent");
			selected = _parent;
		}
		selected->highlight = -1;
		selected->pressed = true;
		if( _nvs ){
			_nvs->set( getSelectCode(), false );   // do sync in next step
			_nvs->commit();
		}
		pressed = false;
		if( _action != 0 ){
			ESP_LOGI(FNAME,"calling action in press %d", _select );
			(*_action)( this );
			// - in the derived class SetupMenuSelectCodes,
			//   "this" points to the derived class object
			// - tested working
		}
		if( _select_save !=  getSelectCode() ) {
			if( bits._restart == RST_ON_EXIT ) {
				_restart = true;
			}else if( bits._restart == RST_IMMEDIATE ){
				_nvs->commit();
				MenuEntry::restart();
			}
			_select_save = getSelectCode();
		}
		if( bits._end_menu ){
			selected->press();
		}
	}
	else{
		pressed = true;
	}
}

// ------------------------------------------------------------------------------

// derived class that stores the specified codes (MB, 2024):

void SetupMenuSelectCodes::addEntryCode( const char* ent, int code ) {
	_values.push_back( ent );
	if (code < 0)
		_codes.push_back( _numval );   // code is same as index into vector
	else
		_codes.push_back( code );      // code was specified
	_numval++;
}

void SetupMenuSelectCodes::updateEntryCode( const char * ent, int num, int code ) {
	ESP_LOGI(FNAME,"updateEntry ent:%s  num:%d total%d", ent, num, _numval );
	_values.at(num) = ent;
	if (code >= 0)      // code was specified
		_codes.at(num) = code;
}

void SetupMenuSelectCodes::setSelect( int sel ) {
	if (sel < 0)
		sel = 0;
	if (sel > _numval-1)
		sel = _numval-1;
	_select = (int16_t)sel;
	setSelectNVS( _codes.at(_select) );
}

void SetupMenuSelectCodes::setSelectCode( int code ) {
	_select = 0;
	for (int i=0; i<_numval; i++) {
		if (_codes.at(i) == code) {
			_select = i;
			break;
		}
	}
	setSelectNVS( _codes.at(_select) );
}

int SetupMenuSelectCodes::getSelect() {
	_select = 0;
	if( _nvs ) {
		int code = _nvs->get();
		for (int i=0; i<_numval; i++) {
			if (_codes.at(i) == code) {
				_select = i;
				break;
			}
		}
	}
	return (int)_select;
}

int SetupMenuSelectCodes::getSelectCode() {
	return _codes.at(_select);
}

#if 0    // the functions below are not likely to be actually used

void SetupMenuSelectCodes::addEntryList( const char ent[][4], int size )
{
	// ESP_LOGI(FNAME,"addEntryList() char ent[][4]");
	for( int i=0; i<size; i++ ) {
		_values.push_back( (char *)ent[i] );
		_codes.push_back( _numval );    // note: code=position, like the base class
		_numval++;
	}
}

void SetupMenuSelectCodes::delEntry( const char* ent ) {
	int i=0;
	for( std::vector<const char *>::iterator iter = _values.begin(); iter != _values.end(); ++iter ) {
		if( std::string(*iter) == std::string(ent) )
		{
			bool found = false;
			int code = _codes.at(i);
			for( std::vector<int>::iterator iter2 = _codes.begin(); iter2 != _codes.end(); ++iter2 ) {
				if ( *iter2 == code ) {
					_codes.erase( iter2 );
					found = true;
					break;
				}
			}
			if (! found)
				return;
			_values.erase( iter );
			_numval--;
			if( _select >= _numval )
				_select = _numval-1;
			break;
		}
		++i;
	}
}

void SetupMenuSelectCodes::delEntryByCode( const int code ) {
	int i=0;
	for( std::vector<int>::iterator iter = _codes.begin(); iter != _codes.end(); ++iter ) {
		if( *iter == code )
		{
			bool found = false;
			const char *p = _values.at(i);
			for( std::vector<const char *>::iterator iter2 = _values.begin(); iter2 != _values.end(); ++iter2 ) {
				if ( std::string(*iter2) == std::string(p) ) {
					_values.erase( iter2 );
					found = true;
					break;
				}
			}
			if (! found)
				return;
			_codes.erase( iter );
			_numval--;
			if( _select >= _numval )
				_select = _numval-1;
			break;
		}
		++i;
	}
}

#endif
