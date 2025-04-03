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
	if( unit != 0 ) {
		_unit = unit;
	}
	bits._restart = restart;
	bits._end_menu = end_menu;
	bits._precision = 2;
	bits._live_update = live_update;
	if( step >= 1 ) {
		bits._precision = 0;
	}
}

void SetupMenuValFloat::enter()
{
	if( _nvs ) {
		_value = _nvs->get();
		if( (_value > _max) || (_value < _min) ){
			_nvs->init();
			ESP_LOGI(FNAME,"SetupMenuValFloat( %s ), OOB: set to default value = %s", _title, value() );
		}
		_value_safe = _value;
	}
	if ( helptext ) {
		clear();
	}
	MenuEntry::enter();
}

const char *SetupMenuValFloat::value() const
{
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

void SetupMenuValFloat::display(int mode)
{
	ESP_LOGI(FNAME,"display %s", _title);
	uprintf( 5,25, _title );
	displayVal();
	if( _action != 0 )
		(*_action)( this );
	showhelp();
}

void SetupMenuValFloat::displayVal()
{
	ESP_LOGI(FNAME,"displayVal %s", value() );
	MYUCG->setColor(COLOR_WHITE);
	MYUCG->setPrintPos( 1, 70 );
	MYUCG->setFont(ucg_font_fub25_hf, true);
	MYUCG->print( value() );
	MYUCG->setFont(ucg_font_ncenR14_hr);
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

void SetupMenuValFloat::rot( int count )
{
	// ESP_LOGI(FNAME,"val rot %d times ", count );
	_value = _nvs->get();
	_value += std::pow( abs(count), _dynamic ) * step(_step) * sign(count);

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
		SavedDelay();
		ESP_LOGI(FNAME,"Yes restart:%d", bits._restart);
		_value_safe = _value;
		_nvs->commit();
		if( bits._restart == RST_ON_EXIT ) {
			_restart = true;
		}else if( bits._restart == RST_IMMEDIATE ){
			_nvs->commit();
			MenuEntry::reBoot();
		}
	}

	BMPVario::setHolddown( 150 );  // so seconds stop average

	_parent->menuSetTop();
	if( bits._end_menu ) {
		exit(-1);
		return;
	}
	exit();
}
