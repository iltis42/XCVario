


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

char SetupMenuSelect::_val_str[20];

bool SetupMenuSelect::existsEntry( String ent ){
	for( std::vector<String>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
		if( *iter == ent )
			return true;
	return false;
}

void SetupMenuSelect::delEntry( String ent ) {
	for( std::vector<String>::iterator iter = _values.begin(); iter != _values.end(); ++iter )
		if( *iter == ent )
		{
			_values.erase( iter );
			_numval--;
			if( *_select >= _numval )
				*_select = _numval-1;
			break;
		}
}

SetupMenuSelect::SetupMenuSelect( String title, bool restart, int (*action)(SetupMenuSelect *p), bool save, SetupNG<int> *anvs ) {
	ESP_LOGI(FNAME,"SetupMenuSelect( %s ) ", title.c_str() );
	_rotary->attach(this);
	_title = title;
	_nvs = 0;
	_select = 0;
	_select_save = 0;
	highlight = -1;
	select_intern = 0;
	if( !anvs ) {
		_select = &select_intern;
		_select_save = select_intern;
	}
	_numval = 0;
	_restart = restart;
	_action = action;
	_save = save;
	if( anvs ) {
		_nvs = anvs;
		ESP_LOGI(FNAME,"_nvs->key(): %s val: %d", _nvs->key(), (int)(_nvs->get()) );
		_select = _nvs->getPtr();
		_select_save = _nvs->get();
	}

}


void SetupMenuSelect::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	ESP_LOGI(FNAME,"SetupMenuSelect display() %d %x", pressed, (int)this);
	clear();
	if( _action != 0 )
		(*_action)( this );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos(1,25);
	ESP_LOGI(FNAME,"Title: %s y=%d", _title.c_str(),y );
	ucg->printf("<< %s",_title.c_str());
	xSemaphoreGive(spiMutex );
	ESP_LOGI(FNAME,"select=%d numval=%d size=%d val=%s", *_select, _numval, _values.size(), _values[*_select].c_str() );
	if( _numval > 9 ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 50 );
		ucg->printf( "%s                ", _values[*_select].c_str() );
		xSemaphoreGive(spiMutex );
	}else
	{
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		for( int i=0; i<_numval && i<+10; i++ )	{
			ucg->setPrintPos( 1, 50+25*i );
			ucg->print( _values[i].c_str() );
		}
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );
		xSemaphoreGive(spiMutex );
	}

	y=_numval*25+50;
	showhelp( y );
	if(mode == 1 && _save == true ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 300 );
		ucg->print("Saved !" );
		if( _select_save != *_select )
			if( _restart ) {
				ucg->setColor(COLOR_BLACK);
				ucg->drawBox( 0,160,240,160 );
				ucg->setPrintPos( 20, 250  );
				ucg->setColor(COLOR_WHITE);
				ucg->print("...rebooting now" );
			}
		xSemaphoreGive(spiMutex );
	}
	if( mode == 1 )
		delay(1000);
}

void SetupMenuSelect::down(int count){
	if( (selected != this) || !_menu_enabled )
		return;

	if( _numval > 9 ){
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		while( count ) {
			if( (*_select) > 0 )
				(*_select)--;
			count--;
		}
		ucg->setPrintPos( 1, 50 );
		ucg->printf("%s                  ",_values[*_select].c_str());
		xSemaphoreGive(spiMutex );
	}else {
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setColor(COLOR_BLACK);
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // blank old frame
		ucg->setColor(COLOR_WHITE);
		if( (*_select) >  0 )
			(*_select)--;
		ESP_LOGI(FNAME,"val down %d", *_select );
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // draw new frame
		xSemaphoreGive(spiMutex );
	}
}

void SetupMenuSelect::up(int count){
	if( (selected != this) || !_menu_enabled )
		return;
	if( _numval > 9 )
	{
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		while( count ) {
			if( (*_select) <  _numval-1 )
				(*_select)++;
			count--;
		}
		ucg->setPrintPos( 1, 50 );
		ucg->printf("%s                   ", _values[*_select].c_str());
		xSemaphoreGive(spiMutex );
	}else {
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setColor(COLOR_BLACK);
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // blank old frame
		ucg->setColor(COLOR_WHITE);
		if ( (*_select) < _numval-1 )
			(*_select)++;
		ESP_LOGI(FNAME,"val up %d", *_select );
		ucg->drawFrame( 1,(*_select+1)*25+3,238,25 );  // draw new frame
		xSemaphoreGive(spiMutex );
	}
}

void SetupMenuSelect::press(){
	if( selected != this )
		return;
	ESP_LOGI(FNAME,"SetupMenuSelect press");
	if ( pressed ){
		display( 1 );
		if( _parent != 0) {
			selected = _parent;
			_parent->highlight = -1;  // to topmost selection when back
		}
		selected->pressed = true;
		if( _nvs )
			_nvs->commit();
		pressed = false;
		if( _action != 0 )
			(*_action)( this );
		if( _select_save != *_select )
			if( _restart ) {
                SetupCommon::commitNow();
				Audio::shutdown();
				sleep( 2 );
				esp_restart();
			}
	}
	else
	{
		pressed = true;
	}
}
