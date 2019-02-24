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
 *         printf("Open");
 *
 */

#include "driver/gpio.h"
#include "Switch.h"

Switch::Switch() {
	_sw = GPIO_NUM_0;
}

Switch::~Switch() {
}

void Switch::begin( gpio_num_t sw ){
	_sw = sw;
	gpio_set_direction(_sw, GPIO_MODE_INPUT);
	gpio_set_pull_mode(_sw, GPIO_PULLUP_ONLY);
}
