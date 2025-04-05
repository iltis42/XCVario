/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "SetupMenuValFloat.h"

#include "SetupMenu.h"
#include "IpsDisplay.h"
#include "BMPVario.h"
#include "Polars.h"
#include "sensor.h"
#include "ESPAudio.h"
#include "logdef.h"


SetupMenuValFloat::SetupMenuValFloat( const char* title, const char *unit, float min, float max, float step, 
	int (*action)( SetupMenuValFloat *p ), bool end_menu, SetupNG<float> *anvs, e_restart_mode_t restart, bool sync, bool live_update ) :
	MenuEntry(),
	_min(min),
	_max(max),
	_step(step),
	_action(action),
	_nvs(anvs)
{
	// ESP_LOGI(FNAME,"SetupMenuValFloat( %s ) ", title.c_str() );
	_title = title;
	if( unit != 0 && *unit != '\0' ) {
		_unit = unit;
	}
	else {
		_unit = Units::unit( _nvs->unitType() );
	}
	bits._restart = restart;
	bits._end_menu = end_menu;
	bits._precision = 2;
	bits._live_update = live_update;
	if( step >= 1 ) {
		bits._precision = 0;
	}
	if ( _nvs ) {
		_value = _nvs->get();
	}
}

void SetupMenuValFloat::enter()
{
	_value_safe = _value;
	MenuEntry::enter();
}

const char *SetupMenuValFloat::value() const
{
	float uval = Units::value( _value, _nvs->unitType() );
	// ESP_LOGI(FNAME,"value() ulen: %d val: %f, utype: %d unitval: %f", strlen( _unit ), _nvs->get(), _nvs->unitType(), uval  );
	sprintf(_val_str,"%0.*f %s   ", bits._precision, uval, _unit );
	return _val_str;
}

void SetupMenuValFloat::setPrecision( int prec ){
	bits._precision = prec;
}

void SetupMenuValFloat::display(int mode)
{
	ESP_LOGI(FNAME,"display %s", _title);
	if ( helptext ) {
		uprintf( 5,25, _title );
		displayVal();
	}
	else {
		int col = MYUCG->getStrWidth(_title) + 4;
		int row = (_parent->getMenuPos() + 1) * 25;
		MYUCG->setColor(COLOR_BLACK);
		MYUCG->drawFrame(1, row + 3, 238, 25);
		MYUCG->setColor(COLOR_WHITE);
		MYUCG->drawFrame(col, row + 3, 238, 25);
	}
	if( _action != 0 ) {
		(*_action)( this );
	}
}

void SetupMenuValFloat::displayVal()
{
	int col = 1, row = 70;
	const char *v = value();
	ESP_LOGI(FNAME,"displayVal %s", v);
	MYUCG->setColor(COLOR_WHITE);
	if ( helptext ) {
		MYUCG->setFont(ucg_font_fub25_hf, true);
	}
	else {
		col = MYUCG->getStrWidth(_title) + 11;
		row = (_parent->getMenuPos() + 2) * 25;
	}
	MYUCG->setPrintPos( col, row );
	MYUCG->print( value() );
	if ( helptext ) {
		MYUCG->setFont(ucg_font_ncenR14_hr);
	}
}

// fixme use si units and adapt only the display to locales
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

void SetupMenuValFloat::rot( int count )
{
	// ESP_LOGI(FNAME,"val rot %d times ", count );
	_value = _nvs->get();
	_value += step(_step) * count;

	if( _value < _min )
		_value = _min;
	else if( _value > _max )
		_value = _max;

	_nvs->set(_value );
	displayVal();
	if( _action != 0 ) {
		(*_action)( this );
	}
}

void SetupMenuValFloat::longPress()
{
	press(); // implicit trigger also on long press actions when in Setup menu.
}

void SetupMenuValFloat::press()
{
	ESP_LOGI(FNAME,"SetupMenuValFloat value: %f", _value );
	ESP_LOGI(FNAME,"Check if _value: %f != _value_safe: %f", _value, _value_safe );
	if( _value != _value_safe ){
		_nvs->set( _value );
		_nvs->commit();
		if ( helptext ) {
			SavedDelay();
		}
		ESP_LOGI(FNAME,"Yes restart:%d", bits._restart);
		_value_safe = _value;
		if( bits._restart == RST_ON_EXIT ) {
			_restart = true;
		}else if( bits._restart == RST_IMMEDIATE ){
			MenuEntry::reBoot();
		}
	}

	BMPVario::setHolddown( 150 );  // so seconds stop average

	if( bits._end_menu ) {
		exit(-1);
		return;
	}
	exit();
}
