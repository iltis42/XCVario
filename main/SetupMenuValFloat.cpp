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
SetupMenuValFloat * SetupMenuValFloat::meter_adj_menu = 0;
char SetupMenuValFloat::_val_str[20];

SetupMenuValFloat::SetupMenuValFloat( const char* title, const char *unit, float min, float max, float step, int (*action)( SetupMenuValFloat *p ), bool end_menu, SetupNG<float> *anvs, e_restart_mode_t restart, bool sync, bool live_update ):
	_dynamic(1.0) {
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
	bits._live_update = live_update;
	if( step >= 1 )
		bits._precision = 0;
	if( anvs ) {
		_nvs = anvs;
		_value = _nvs->get();
		if( (_value > max) || (_value < min) ){
			_nvs->init();
			ESP_LOGI(FNAME,"SetupMenuValFloat( %s ), OOB: set to default value = %s", title, value() );
		}
		_value_safe = _value;
	}
}

SetupMenuValFloat::~SetupMenuValFloat()
{
    detach(this);
}

const char *SetupMenuValFloat::value(){
	float uval = Units::value( _nvs->get(), _nvs->unitType() );
	// ESP_LOGI(FNAME,"value() ulen: %d val: %f, utype: %d unitval: %f", strlen( _unit ), _nvs->get(), _nvs->unitType(), uval  );
	const char * final_unit = _unit;
	if( strlen( _unit ) == 0 )
		final_unit = Units::unit( _nvs->unitType() );
	sprintf(_val_str,"%0.*f %s   ", bits._precision, uval, final_unit );
	return _val_str;
}

void SetupMenuValFloat::setPrecision( int prec ){
	bits._precision = prec;
}

void SetupMenuValFloat::showMenu( float val, SetupMenuValFloat * menu ){
	ESP_LOGI(FNAME,"showMenu()");
	if( menu ) {
		ESP_LOGI(FNAME,"menu found");
		gflags.inSetup = true;
		selected = menu;
		gflags.inSetup=true;
		menu->clear();
		menu->display();
		menu->pressed = true;
		menu->_value = val;
	}
}

void SetupMenuValFloat::display( int mode ){
	if( (selected != this) || !gflags.inSetup )
		return;
	// ESP_LOGI(FNAME,"display() pressed=%d instance=%x mode=%d", pressed, (int)this, mode );
	int y= 75;
	if( mode == 0 ){ // normal mode
		uprintf( 5,25, selected->_title );
		displayVal();
		if( _action != 0 )
			(*_action)( this );
		showhelp( y );
	}
	else if (mode == 1){   // save mode, do show only "Saved"true
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
	// ESP_LOGI(FNAME,"displayVal %s", value() );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setPrintPos( 1, 70 );
	ucg->setFont(ucg_font_fub25_hf, true);
	ucg->print( value() );
	xSemaphoreGive(spiMutex );
	ucg->setFont(ucg_font_ncenR14_hr);
}

float SetupMenuValFloat::step( float instep ){
	float step = 1.0;
	if( _nvs->unitType() == UNIT_ALT && alt_unit.get() == ALT_UNIT_FT )
		step = 5.0;
	else
		step = instep;
	if( _nvs->unitType() == UNIT_VARIO && vario_unit.get() == VARIO_UNIT_KNOTS )
		step = Units::Vario2ms( instep*2 );
	// ESP_LOGI(FNAME,"instep: %f, ut:%d ostep: %f", instep, _nvs->unitType(), step );
	return step;
}

void SetupMenuValFloat::down( int count ){
	if( (selected != this) || !gflags.inSetup )
		return;
	// ESP_LOGI(FNAME,"val down %d times ", count );
	_value = _nvs->get();
	// float start = _value;
	int _count = std::pow( count, _dynamic );
	while( (_value > _min) && _count ) {
			_value -= step( _step );
			_count --;
		}

	if( _value < _min )
		_value = _min;
	_nvs->set( _value );

	// ESP_LOGI(FNAME,"val down diff=%f", start-_value );

	displayVal();
	if( _action != 0 )
		(*_action)( this );
}

void SetupMenuValFloat::up( int count ){
	if( (selected != this) || !gflags.inSetup )
		return;
	// ESP_LOGI(FNAME,"val up %d times ", count );
	_value = _nvs->get();
	// float start = _value;
	int _count = std::pow( count, _dynamic );
	while( (_value < _max) && _count ) {
		_value += step( _step );
		_count--;
   }

	if( _value > _max )
		_value = _max;

	// ESP_LOGI(FNAME,"val up diff=%f", _value-start );
	_nvs->set(_value );
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
	ESP_LOGI(FNAME,"SetupMenuValFloat press %d", pressed );

	if ( pressed ){
		ESP_LOGI(FNAME,"pressed, value: %f", _value );
		_nvs->set( _value );
		display( 1 );
		if( bits._end_menu )
			selected = root;
		else if( _parent != 0 )
			selected = _parent;
		selected->highlight = -1;  // to topmost selection when back
		selected->pressed = true;
		ESP_LOGI(FNAME,"Check if _value: %f != _value_safe: %f", _value, _value_safe );
		if( _value != _value_safe ){
			ESP_LOGI(FNAME,"Yes restart:%d", bits._restart);
			_value_safe = _value;
			_nvs->commit();
			if( bits._restart == RST_ON_EXIT ) {
				_restart = true;
			}else if( bits._restart == RST_IMMEDIATE ){
				_nvs->commit();
				MenuEntry::restart();
			}
		}
		pressed = false;
		BMPVario::setHolddown( 150 );  // so seconds stop average
		if( bits._end_menu )
			selected->press();
	}
	else
	{
		ESP_LOGI(FNAME,"NOT pressed, value: %f", _value );
		pressed = true;
		clear();
		display();
	}
}
