/*
 * SetupMenu.cpp
 *
 *  Created on: Feb 4, 2018
 *      Author: iltis
 */

#include "setup/SetupMenuValFloat.h"

#include "setup/SetupMenu.h"
#include "IpsDisplay.h"
#include "BMPVario.h"
#include "glider/Polars.h"
#include "sensor.h"
#include "ESPAudio.h"
#include "logdef.h"


SetupMenuValFloat::SetupMenuValFloat( const char* title, const char *unit, int (*action)( SetupMenuValFloat *p ), 
	bool end_menu, SetupNG<float> *anvs, e_restart_mode_t restart, bool sync, bool live_update ) :
	MenuEntry(),
	_action(action),
	_nvs(anvs)
{
	// ESP_LOGI(FNAME,"SetupMenuValFloat( %s ) ", title.c_str() );
	_title.assign(title);
	if( unit != 0 && *unit != '\0' ) {
		_unit = unit;
	}
	else if ( _nvs ) {
		_unit = Units::unit( _nvs->quantityType() );
	}

	if ( _nvs ) {
		_value = _nvs->get();
		if ( _nvs->hasLimits() ) {
			_min = _nvs->getMin();
			_max = _nvs->getMax();
			_step = _nvs->getStep();
		}
	}

	bits._restart = restart;
	bits._end_menu = end_menu;
	bits._precision = 2;
	bits._live_update = live_update;
	if( _step >= 1 ) {
		bits._precision = 0;
	}
}

void SetupMenuValFloat::enter()
{
	_value_safe = _value;
	MenuEntry::enter();
}

const char *SetupMenuValFloat::value() const
{
	float uval = Units::value( _value, _nvs->quantityType() );
	// ESP_LOGI(FNAME,"value() ulen: %d val: %f, utype: %d unitval: %f", strlen( _unit ), _nvs->get(), _nvs->quantityType(), uval  );
	sprintf(_val_str,"%0.*f %s   ", bits._precision, uval, _unit );
	return _val_str;
}

void SetupMenuValFloat::setPrecision( int prec ){
	bits._precision = prec;
}

void SetupMenuValFloat::display(int mode)
{
	ESP_LOGI(FNAME,"display %s", _title.c_str());
	if ( bits._is_inline ) {
		indentHighlight(_parent->getHighlight());
	}
	else {
		menuPrintLn(_title.c_str(), 0, 5 );
		displayVal();
	}
	if( _action != 0 ) {
		(*_action)( this );
	}
}

void SetupMenuValFloat::displayVal()
{
	// ESP_LOGI(FNAME,"displayVal %s", v);
	if ( bits._is_inline ) {
		indentPrintLn(value());
	}
	else {
		MYUCG->setFont(ucg_font_fub25_hf, true);
		menuPrintLn(value(), 2);
		MYUCG->setFont(ucg_font_ncenR14_hr);
	}
}

// fixme use si units and adapt only the display to locales
float SetupMenuValFloat::step( float instep ){
	float step = 1.0;
	if( _nvs->quantityType() == QUANT_ALT && alt_unit.get() == ALT_UNIT_FT )
		step = 5.0;
	else
		step = instep;
	if( _nvs->quantityType() == QUANT_VSPEED && vario_unit.get() == SPEED_UNIT_KNOTS )
		step = Units::Vario2ms( instep*2 );
	// ESP_LOGI(FNAME,"instep: %f, ut:%d ostep: %f", instep, _nvs->quantityType(), step );
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

	BMPVario::setHolddown( 150 );  // so seconds stop average ?? fixme

	if( bits._end_menu ) {
		exit(-1);
		return;
	}
	exit();
}
