/*
 * Switch.cpp
 *
 *  Created on: Feb 24, 2019
 *      Author: iltis
 *
 *      Example:
 *      sw = Switch();
 *      sw.begin( GPIO_NUM_1 );
 *      if( sw.state() )
 *         ESP_LOGI(FNAME,"Open");
 *
 */

#include "esp32-hal.h"
#include "driver/gpio.h"
#include "Switch.h"
#include <logdef.h>
#include "Setup.h"
#include "sensor.h"

bool Switch::_cruise_mode = false;
bool Switch::_closed = false;
int Switch::_holddown = 0;
float Switch::_cruise_speed_kmh = 100;
gpio_num_t Switch::_sw = GPIO_NUM_0;

Switch::Switch() {
}

Switch::~Switch() {
}

bool Switch::cruiseMode( bool check_automode ) {
	if( check_automode )
		if( audio_mode.get() == AM_AUTOSPEED )
			if ( ias > _cruise_speed_kmh )
				return true;
	return _cruise_mode;
};

void Switch::begin( gpio_num_t sw ){
	_sw = sw;
	gpio_set_direction(_sw, GPIO_MODE_INPUT);
	gpio_set_pull_mode(_sw, GPIO_PULLDOWN_ONLY);
}

bool Switch::isClosed() {
	gpio_set_pull_mode(_sw, GPIO_PULLUP_ONLY);
	delay(10);
	int level = gpio_get_level(_sw );
	gpio_set_pull_mode(_sw, GPIO_PULLDOWN_ONLY);
	if( level )
		return false;
	else
		return true;
}

bool Switch::isOpen() {
	return( !isClosed() );
}

void Switch::tick() {
	if( s2f_switch_type.get() == S2F_HW_SWITCH || s2f_switch_type.get() == S2F_HW_SWITCH_INVERTED ){
		if( isClosed() )
			if( s2f_switch_type.get() == S2F_HW_SWITCH_INVERTED )
				_cruise_mode = false;
			else
				_cruise_mode = true;
		else
			if( s2f_switch_type.get() == S2F_HW_SWITCH_INVERTED )
				_cruise_mode = true;
			else
				_cruise_mode = false;
	}
	else if( s2f_switch_type.get() == S2F_HW_PUSH_BUTTON ){
		if( _holddown ){   // debouncing
			_holddown--;
			return;
		}
		if( _closed ) {
			if( !isClosed() )
				_closed = false;
		}
		else {
			if( isClosed() ) {
				_closed = true;
				_holddown = 5;
				if( _cruise_mode )
					_cruise_mode = false;
				else
					_cruise_mode = true;
			}
		}
	}
}
