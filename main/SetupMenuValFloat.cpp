/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "SetupMenuValFloat.h"

#include "IpsDisplay.h"
#include "BMPVario.h"
#include "Polars.h"
#include "sensor.h"
#include "ESPAudio.h"

#include <esp_log.h>

SetupMenuValFloat * SetupMenuValFloat::qnh_menu = 0;
char SetupMenuValFloat::_val_str[20];

SetupMenuValFloat::SetupMenuValFloat( const char* title, const char *unit, float min, float max, float step, int (*action)( SetupMenuValFloat *p ), bool end_menu, SetupNG<float> *anvs, bool restart, bool sync ) {
	// ESP_LOGI(FNAME,"SetupMenuValFloat( %s ) ", title.c_str() );
	attach(this);
	_title = title;
	highlight = -1;
	_nvs = 0;
	bits._restart = restart;
	if( unit != 0 )
		_unit = unit;
	else
		_unit = "";
	_min = min;
	_max = max;
	_step = step;
	_action = action;
	bits._end_menu = end_menu;
	bits._precision = 2;
	if( step >= 1 )
		bits._precision = 0;
	if( anvs ) {
		_nvs = anvs;
		_value = _nvs->get();
		_value_safe = _value;
	}
}

SetupMenuValFloat::~SetupMenuValFloat()
{
    detach(this);
}

const char *SetupMenuValFloat::value(){
	_value = _nvs->get();
	sprintf(_val_str,"%0.*f %s", bits._precision, _value, _unit );
	return _val_str;
}

void SetupMenuValFloat::setPrecision( int prec ){
	bits._precision = prec;
}

void SetupMenuValFloat::showQnhMenu(){
	ESP_LOGI(FNAME,"SetupMenuValFloat::showQnhMenu()");
	if( qnh_menu ) {
		ESP_LOGI(FNAME,"qnh_menu = true");
		inSetup = true;
		selected = qnh_menu;
		inSetup=true;
		qnh_menu->clear();
		qnh_menu->display();
		qnh_menu->pressed = true;
	}
}

void SetupMenuValFloat::display( int mode ){
	if( (selected != this) || !inSetup )
		return;
	ESP_LOGI(FNAME,"SetupMenuValFloat display() pressed=%d instance=%x mode=%d", pressed, (int)this, mode );
	int y= 75;
	if( mode == 0 ){ // normal mode
		uprintf( 5,25, selected->_title );
		displayVal();
		if( _action != 0 )
			(*_action)( this );
		showhelp( y );
	}
	else if (mode == 1){   // save mode, do show only "Saved"
		y+=24;
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 300 );
		ucg->print("Saved");
		xSemaphoreGive(spiMutex );
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

void SetupMenuValFloat::displayVal()
{
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos( 1, 70 );
	ucg->setFont(ucg_font_fub25_hf, true);
	char val[20];
	sprintf(val, "%0.*f", bits._precision, _value );
	ucg->print(val);
	if( _unit ) {
		ucg->setFont(ucg_font_fur25_hf, true);   // use different font for unit as of ° special char
		ucg->setPrintPos( 1+ ucg->getStrWidth(val), 70 );
		ucg->printf(" %s   ", _unit);
	}

	xSemaphoreGive(spiMutex );
	ucg->setFont(ucg_font_ncenR14_hr);
}

void SetupMenuValFloat::down( int count ){
	if( (selected != this) || !inSetup )
		return;
	// ESP_LOGI(FNAME,"val down %d times ", count );
	while( (_value > _min) && count ) {
		_value -= _step;
		count --;
	}
	if( _value < _min )
		_value = _min;
	displayVal();
	if( _action != 0 )
		(*_action)( this );
}

void SetupMenuValFloat::up( int count ){
	if( (selected != this) || !inSetup )
		return;
	// ESP_LOGI(FNAME,"val up %d times ", count );
	while( (_value < _max) && count ) {
		_value += _step;
		count--;
	}
	if( _value > _max )
		_value = _max;
	displayVal();
	if( _action != 0 )
		(*_action)( this );
}

void SetupMenuValFloat::longPress(){
	press(); // implicit trigger also on long press actions when in Setup menu.
}

void SetupMenuValFloat::press(){
	if( selected != this )
		return;
	// ESP_LOGI(FNAME,"SetupMenuValFloat press");
	if ( pressed ){
		// ESP_LOGI(FNAME,"pressed, value: %f", _value );
		_nvs->set( _value );
		display( 1 );
		if( bits._end_menu )
			selected = root;
		else if( _parent != 0 )
			selected = _parent;
		selected->highlight = -1;  // to topmost selection when back
		selected->pressed = true;
		if( _value != _value_safe ){
			if( _nvs )
				_nvs->commit();
			if( bits._restart ) {
				Audio::shutdown();
				clear();
				ucg->setPrintPos( 10, 50 );
				ucg->print("...rebooting now" );
				SetupCommon::commitNow();
				delay(2000);
				esp_restart();
			}
		}
		pressed = false;
		BMPVario::setHolddown( 150 );  // so seconds stop average
		if( bits._end_menu )
			selected->press();
	}
	else
	{
		// ESP_LOGI(FNAME,"NOT pressed, value: %f", _value );
		pressed = true;
		clear();
		display();
	}
}
