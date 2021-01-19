/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

// #include "SetupMenu.h"
#include "IpsDisplay.h"
#include "BMPVario.h"
#include "Polars.h"
#include <logdef.h>
#include <sensor.h>
#include "Units.h"
#include "SetupMenuValFloat.h"

SetupMenuValFloat * SetupMenuValFloat::qnh_menu = 0;

SetupMenuValFloat::SetupMenuValFloat( String title, float *value, const char *unit, float min, float max, float step, int (*action)( SetupMenuValFloat *p ), bool end_menu, SetupNG<float> *anvs ) {
	// ESP_LOGI(FNAME,"SetupMenuValFloat( %s ) ", title.c_str() );
	_rotary->attach(this);
	_title = title;
	highlight = -1;
	_nvs = 0;
	if( value )
		_value = value;
	_unit = unit;
	_min = min;
	_max = max;
	_step = step;
	_action = action;
	_end_menu = end_menu;
	_precision = 2;
	if( step >= 1 )
		_precision = 0;
	if( anvs ) {
		_nvs = anvs;
		_value = _nvs->getPtr();
	}
}

void SetupMenuValFloat::setPrecision( int prec ){
	_precision = prec;
}

void SetupMenuValFloat::showQnhMenu(){
	ESP_LOGI(FNAME,"SetupMenuValFloat::showQnhMenu()");
	if( qnh_menu ) {
		ESP_LOGI(FNAME,"qnh_menu = true");
		_menu_enabled = true;
		selected = qnh_menu;
		inSetup=true;
		qnh_menu->clear();
		qnh_menu->display();
		qnh_menu->pressed = true;
	}
}

void SetupMenuValFloat::display( int mode ){
	if( (selected != this) || !_menu_enabled )
		return;
	// ESP_LOGI(FNAME,"SetupMenuValFloat display() %d %x", pressed, (int)this);
	uprintf( 5,25, selected->_title.c_str() );
	displayVal();
	y= 75;
	if( _action != 0 )
		(*_action)( this );

	showhelp( y );
	if(mode == 1){
		y+=24;
		xSemaphoreTake(spiMutex,portMAX_DELAY );
		ucg->setPrintPos( 1, 300 );
		ucg->print("Saved");
		xSemaphoreGive(spiMutex );
	}
	y=0;

	if( mode == 1 )
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	ESP_LOGI(FNAME,"~SetupMenuValFloat display");
}

void SetupMenuValFloat::displayVal()
{
	ucg->setFont(ucg_font_fub25_hr);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos( 1, 70 );
	if( _unit )
		ucg->printf("%0.*f %s   ", _precision, *_value, _unit);
	xSemaphoreGive(spiMutex );
	ucg->setFont(ucg_font_ncenR14_hr);
}

void SetupMenuValFloat::down( int count ){
	if( (selected != this) || !_menu_enabled )
		return;
	ESP_LOGI(FNAME,"val down %d times ", count );
	while( (*_value > _min) && count ) {
		*_value -= _step;
		count --;
	}
	if( *_value < _min )
		*_value = _min;
	displayVal();
	if( _action != 0 )
		(*_action)( this );
}

void SetupMenuValFloat::up( int count ){
	if( (selected != this) || !_menu_enabled )
		return;
	ESP_LOGI(FNAME,"val up %d times ", count );
	while( (*_value < _max) && count ) {
		*_value += _step;
		count--;
	}
	if( *_value > _max )
		*_value = _max;
	displayVal();
	if( _action != 0 )
		(*_action)( this );
}

void SetupMenuValFloat::press(){
	if( selected != this )
		return;
	ESP_LOGI(FNAME,"SetupMenuValFloat press");
	if ( pressed ){
		display( 1 );
		if( _end_menu )
			selected = root;
		else if( _parent != 0 )
			selected = _parent;
		selected->highlight = -1;  // to topmost selection when back
		selected->pressed = true;
		if( _nvs )
			_nvs->commit();
		pressed = false;
		BMPVario::setHolddown( 150 );  // so seconds stop average
		if( _end_menu )
			selected->press();
	}
	else
	{
		pressed = true;
		clear();
		display();
	}
}
