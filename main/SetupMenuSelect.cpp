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
	return _values[ _select ];
}

const char *SetupMenuSelect::value() {
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

SetupMenuSelect::SetupMenuSelect( const char* title, bool restart, int (*action)(SetupMenuSelect *p), bool save, SetupNG<int> *anvs, bool ext_handler, bool end_menu ) {
	// ESP_LOGI(FNAME,"SetupMenuSelect( %s ) action: %x", title, (int)action );
	attach(this);
	bits._ext_handler = ext_handler;
	_title = title;
	_nvs = 0;
	_select = 0;
	_select_save = 0;
	bits._end_menu = end_menu;
	highlight = -1;
	if( !anvs ) {
		_select_save = _select;
	}
	_numval = 0;
	bits._restart = restart;
	_action = action;
	bits._save = save;
	if( anvs ) {
		_nvs = anvs;
		// ESP_LOGI(FNAME,"_nvs->key(): %s val: %d", _nvs->key(), (int)(_nvs->get()) );
		_select = (int16_t)(*(int *)(_nvs->getPtr()));
		_select_save = (int16_t)_nvs->get();
	}

}
SetupMenuSelect::~SetupMenuSelect()
{
	detach(this);
}

void SetupMenuSelect::display( int mode ){
	if( (selected != this) || !gflags.inSetup  )
		return;
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
		ESP_LOGI(FNAME,"select=%d numval=%d size=%d val=%s", _select, _numval, _values.size(), _values[_select] );
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
			ucg->drawBox( 0,280,240,40 );
			ucg->setPrintPos( 1, 300 );
			ucg->print("Saved" );
			xSemaphoreGive(spiMutex );
		}
		if( mode == 1 )
			delay(1000);
	}
}

void SetupMenuSelect::down(int count){
	if( (selected != this) || !gflags.inSetup )
		return;
	if( _numval > 9 ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		while( count ) {
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
		if( (_select) >  0 )
			(_select)--;
		ESP_LOGI(FNAME,"val down %d", _select );
		ucg->drawFrame( 1,(_select+1)*25+3,238,25 );  // draw new frame
		xSemaphoreGive(spiMutex );
	}
}

void SetupMenuSelect::up(int count){
	if( (selected != this) || !gflags.inSetup )
		return;
	if( _numval > 9 )
	{
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		while( count ) {
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
		if ( (_select) < _numval-1 )
			(_select)++;
		ESP_LOGI(FNAME,"val up %d", _select );
		ucg->drawFrame( 1,(_select+1)*25+3,238,25 );  // draw new frame
		xSemaphoreGive(spiMutex );
	}
}

void SetupMenuSelect::longPress(){
	press();
}

void SetupMenuSelect::press(){
	if( selected != this )
		return;
	ESP_LOGI(FNAME,"press() ext handler: %d press: %d _select: %d", bits._ext_handler, pressed, _select );
	if ( pressed ){
		display( 1 );
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
			_nvs->set((int)_select, false ); // do sync in next step
			_nvs->commit();
		}
		pressed = false;
		if( _action != 0 ){
			ESP_LOGI(FNAME,"calling action in press %d", _select );
			(*_action)( this );
		}
		if( _select_save != _select )
			if( bits._restart ) {
				restart();
			}
		if( bits._end_menu ){
			selected->press();
		}
	}
	else{
		pressed = true;
	}
}
