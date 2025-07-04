/*
 * Switch.h
 *
 *  Created on: Feb 24, 2019
 *      Author: iltis
 */

#pragma once

#include "protocol/ClockIntf.h"
#include "average.h"

#include <driver/gpio.h>

#define GYRO_FILTER_SAMPLES 20

class Switch final : public Clock_I
{
public:
	Switch(gpio_num_t sw);
	virtual ~Switch();
	bool isClosed();
	bool isOpen();
	bool cruiseMode();
	bool getCruiseState(){ return cruise_mode_final; };
	void setCruiseModeXCV();
	void ticktack();
	void setCruiseThreshold( float s ) { _cruise_threshold_kmh = s; };
	bool tick() override;

private:
	gpio_num_t _sw;
	Average<GYRO_FILTER_SAMPLES, float, float> filter;
	bool _cruise_mode_sw = false;
	bool _cruise_mode_xcv = false;
	bool _cruise_mode_speed = false;
	bool _cruise_mode_flap = false;
	bool _cruise_mode_gyro = false;
	bool _closed = false;
	int _holddown = 0;
	float _cruise_threshold_kmh = 100;
	int _tick = 0;
	bool cm_switch_prev = false;
	bool cm_auto_prev = false;
	bool cm_xcv_prev = false;
	bool cruise_mode_final = false;
	bool initial = true;
	int _active_level = 1;
	bool _pullup_on = true;
	bool state = false;
	int holdCount = 0; // timer timeout set the hold state
	bool lastButtonRead = false;
	int debounceCount = 0;
};

extern Switch *S2FSWITCH;
