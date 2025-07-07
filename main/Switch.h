/*
 * Switch.h
 *
 *  Created on: Feb 24, 2019
 *      Author: iltis
 */

#pragma once

#include "protocol/ClockIntf.h"

#include <driver/gpio.h>

class Switch final : public Clock_I
{
public:
	Switch() = delete;
	Switch(gpio_num_t sw);
	virtual ~Switch();
	void updateSwitchSetup();
	void checkCruiseMode();
	bool tick() override; // polling the switch

private:
	// used gpio for switch
	gpio_num_t _sw;

	// active auto switch filter
	bool (*auto_plug)() = nullptr;
	bool _auto_state = true;
	int _auto_lag = 0;
	int _lag_counter = 0;

	// switch polling
	int _active_level = 1;
	bool _state = false;
	bool _lastButtonRead = false;
	int _debounceCount = 0;
};

extern Switch *S2FSWITCH;
