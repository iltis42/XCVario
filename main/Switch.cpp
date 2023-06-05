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
#include "Flap.h"
#include "KalmanMPU6050.h"
#include "average.h"
#include "vector.h"

bool Switch::_cruise_mode_sw = false;
bool Switch::_cruise_mode_speed = false;
bool Switch::_closed = false;
int Switch::_holddown = 0;
float Switch::_cruise_speed_kmh = 100;
int Switch::_tick = 0;
bool Switch::cm_switch_prev = false;
bool Switch::cm_auto_prev = false;
bool Switch::cruise_mode_final = false;
bool Switch::_cruise_mode_xcv = false;
bool Switch::cm_xcv_prev = false;
bool Switch::initial = true;

Average<GYRO_FILTER_SAMPLES, float, float> Switch::filter;

gpio_num_t Switch::_sw = GPIO_NUM_0;

Switch::Switch() {
}

Switch::~Switch() {
}

bool Switch::cruiseMode() {
	if( s2f_switch_mode.get() == AM_EXTERNAL ){
		// just take from master and ignore setting
		_cruise_mode_xcv = cruise_mode.get(); // updated from master
		if( _cruise_mode_xcv != cm_xcv_prev  ){
			cruise_mode_final = _cruise_mode_xcv;
			cm_xcv_prev = _cruise_mode_xcv;
			ESP_LOGI(FNAME,"cruise_mode from change from Peer: %d", cruise_mode_final );
		}
	}
	else if( s2f_switch_mode.get() == AM_AUTOSPEED ){        // Let autospeed mode merge both states
		if( _cruise_mode_sw != cm_switch_prev  ){
			ESP_LOGI(FNAME,"cruise_mode change %d", cruise_mode_final );
			cm_switch_prev = _cruise_mode_sw;
			cruise_mode_final = _cruise_mode_sw;
			ESP_LOGI(FNAME,"cruise_mode change from Switch: %d", cruise_mode_final );
		}
		else if( _cruise_mode_speed != cm_auto_prev ){
			cm_auto_prev = _cruise_mode_speed;
			cruise_mode_final = _cruise_mode_speed;
		}
	}
	else if( s2f_switch_mode.get() == AM_FLAP  ){
		if( flap_enable.get() ){
			if( FLAP->getFlapPosition() > s2f_flap_pos.get() )
				cruise_mode_final = false;
			else
				cruise_mode_final = true;
		}
	}
	else if( s2f_switch_mode.get() == AM_AHRS ){
		float gr = (float)filter( (float)IMU::getGyroRate() );
		// ESP_LOGI( FNAME,"Gyro-Rate %.2f", gr );
		float ref = s2f_gyro_deg.get();
		if( cruise_mode_final )
			ref = ref*1.2;  // 20% hysteresis
		// ESP_LOGI( FNAME,"Gyro-Rate: %.2f  thres: %.2f", gr, ref );
		if( gr < ref )   // default 12° per second
			cruise_mode_final = true;
		else
			cruise_mode_final = false;
		// ESP_LOGI( FNAME,"Gyro-Rate: %.2f  thres: %.2f cmf: %d", gr, ref, cruise_mode_final  );
	}
	else if( s2f_switch_mode.get() == AM_SWITCH ){
		if( cruise_mode_final != _cruise_mode_sw ){
			cruise_mode_final = _cruise_mode_sw;
		}
	}
	else if( s2f_switch_mode.get() == AM_VARIO ) {
		if( cruise_mode_final != false ){
			cruise_mode_final = false;
		}
	}
	else if( s2f_switch_mode.get() == AM_S2F ){
		if( cruise_mode_final != true ){
			cruise_mode_final = true;
		}
	}
	// ESP_LOGI(FNAME,"cruise_mode_final %d", cruise_mode_final );

	if( (int)cruise_mode_final != (int)cruise_mode.get() || initial ){
		ESP_LOGI(FNAME,"New cruise mode: %d", cruise_mode_final );
		initial = false;
		cruise_mode.set( (int)cruise_mode_final );
	}
	return cruise_mode_final;
}

void Switch::begin( gpio_num_t sw ){
	_cruise_speed_kmh = Units::Airspeed2Kmh( s2f_speed.get() );
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

void Switch::setCruiseModeXCV(){
	_cruise_mode_xcv = cruise_mode.get();
}


void Switch::tick() {
	_tick++;
	if( s2f_switch_mode.get() == AM_AUTOSPEED  && !(_tick%10) ){ // its enough to check this every 10 tick
		// ESP_LOGI(FNAME,"mode: %d ias: %3.1f hyst: %3.1f", _cruise_mode_speed, ias.get(), s2f_hysteresis.get() );
		if( _cruise_mode_speed ){
			if ( ias.get() < (_cruise_speed_kmh - s2f_hysteresis.get()) ){
				if( _cruise_mode_speed != false  ){
					_cruise_mode_speed = false;
					ESP_LOGI(FNAME,"set cruise mode false");
				}
			}
		}
		else{ // vario mode
			if ( ias.get() > (_cruise_speed_kmh + s2f_hysteresis.get()) ){
				if( _cruise_mode_speed != true ){
					_cruise_mode_speed = true;
					ESP_LOGI(FNAME,"set cruise mode true");
				}
			}
		}
	}

	if( s2f_switch_mode.get() == AM_AUTOSPEED || s2f_switch_mode.get() == AM_SWITCH ){   // both of this modes consider switch
		if( s2f_switch_type.get() == S2F_HW_SWITCH || s2f_switch_type.get() == S2F_HW_SWITCH_INVERTED ){
			if( isClosed() ){
				if( s2f_switch_type.get() == S2F_HW_SWITCH_INVERTED ){
					_cruise_mode_sw = false;
				}
				else{
					_cruise_mode_sw = true;
				}
			}
			else{
				if( s2f_switch_type.get() == S2F_HW_SWITCH_INVERTED ){
					_cruise_mode_sw = true;
				}
				else{
					_cruise_mode_sw = false;
				}
			}
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
					if( _cruise_mode_sw )
						_cruise_mode_sw = false;
					else
						_cruise_mode_sw = true;
				}
			}
		}
	}
	if( !(_tick%100) )  // called every 10 mS, once in a second switch mode
		Switch::cruiseMode();

}
