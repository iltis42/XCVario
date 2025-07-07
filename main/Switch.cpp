/*
 * Switch.cpp
 *
 *  Created on: Feb 24, 2019
 *      Author: iltis
 *
 */

#include "Switch.h"
#include "protocol/Clock.h"
#include "setup/SetupNG.h"
#include "screen/UiEvents.h"
#include "screen/DrawDisplay.h"
#include "sensor.h"
#include "Flap.h"
#include "KalmanMPU6050.h"
#include "logdefnone.h"


// auto switch filters
static bool Extern();
static bool Speed();
static bool Flap();
static bool Omega();
static bool VarioFix();
static bool S2fFix();

Switch *S2FSWITCH = nullptr;

// clock tick timer (task context)
bool IRAM_ATTR Switch::tick()
{
	// called every 10msec
	bool buttonRead = gpio_get_level(_sw) == _active_level; // button active

	_debounceCount = (buttonRead == _lastButtonRead) ? (_debounceCount+1) : 0;
	_lastButtonRead = buttonRead;

	if ( _debounceCount < 2 || (buttonRead == _state) ) {
		return false;
	}

	// A valid edge detected
	_state = buttonRead;
	int gotEvent;
	ESP_LOGI(FNAME, "cruise_mode _state: %d", _state );
	if ( s2f_switch_type.get() == S2F_HW_SWITCH ) {
		// switch
		if (_state) {
			gotEvent = ModeEvent(ModeEvent::MODE_VARIO).raw; // Button active
		}
		else {
			gotEvent = ModeEvent(ModeEvent::MODE_S2F).raw;
		}
	}
	else if (_state) {
		// toggle
		gotEvent = ModeEvent(ModeEvent::MODE_SV_TOGGLE).raw;

	}
	xQueueSend(uiEventQueue, &gotEvent, 0);

	return false;
}

Switch::Switch(gpio_num_t sw) :
	Clock_I(1),
	_sw(sw)
{
	// prepare switch gpio
	gpio_set_direction(_sw, GPIO_MODE_INPUT);

	// init
	updateSwitchSetup();
}


Switch::~Switch() {
	Clock::stop(this);
}

void Switch::updateSwitchSetup()
{
	// setup the switch
	if ( (s2f_switch_type.get() != S2F_SWITCH_DISABLE) ) {
		_active_level = (s2f_switch_type.get() == S2F_HW_SWITCH_INVERTED) ? 0 : 1;
		gpio_set_pull_mode(_sw, GPIO_PULLUP_ONLY);
		Clock::start(this);
	}
	else {
		gpio_set_pull_mode(_sw, GPIO_FLOATING);
		Clock::stop(this);
	}

	// setup auto switches
	_auto_lag = 0;
	_auto_state = cruise_mode.get(); // init event signalling state
	switch( s2f_switch_mode.get() ) {
	case AM_EXTERNAL:
	case AM_SWITCH:
		auto_plug = nullptr; // Extern;
		break;
	case AM_AUTOSPEED:
		_auto_lag = static_cast<int>(s2f_auto_lag.get());
		auto_plug = Speed;
		break;
	case AM_FLAP:
		_auto_lag = static_cast<int>(s2f_auto_lag.get());
		auto_plug = Flap;
		break;
	case AM_AHRS:
		_auto_lag = static_cast<int>(s2f_auto_lag.get());
		auto_plug = Omega;
		break;
	case AM_VARIO:
		auto_plug = VarioFix;
		break;
	case AM_S2F:
		auto_plug = S2fFix;
		break;
	default:
		auto_plug = nullptr;
		break;
	}
}

//////////////
// auto switch filter
bool Extern() {
	return cruise_mode.get() != 0; // updated from peer, or switch
}
bool Speed() {
	if (ias.get() < s2f_threshold.get() ) {
		return false; // vario mode
	}
	else {
		return true;
	}
}
bool Flap() {
	if( flap_enable.get() ) {
		return FLAP->getFlapPosition() < s2f_flap_pos.get();
	}
	return true;
}
bool Omega() {
	float ref = s2f_gyro_deg.get() * 1.2; // 20% hysteresis
	if ( (abs(IMU::getVerticalOmega())*RAD_TO_DEG) < ref ) {
		return true;
	}
	else {
		return false;
	}
}
bool VarioFix() {
	return false;
}
bool S2fFix() {
	return true;
}

// call once a second
void Switch::checkCruiseMode()
{
	if ( auto_plug ) {
		bool cm = (*auto_plug)();
		if ( cm != _auto_state ) {
			if ( _lag_counter == _auto_lag ) {
				_auto_state = cm;
				ESP_LOGI(FNAME,"New S2F auto mode: %d", cm );
				int gotEvent = ModeEvent(cm ? ModeEvent::MODE_VARIO : ModeEvent::MODE_S2F).raw;
				xQueueSend(uiEventQueue, &gotEvent, 0);
			}
			_lag_counter++;
		}
		else {
			_lag_counter = 0;
		}
	}
}

