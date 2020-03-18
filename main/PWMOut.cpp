/*
 * PWMOut.cpp
 *
 *  Created on: Dec 25, 2018
 *      Author: iltis
 */

#include "PWMOut.h"
#include "driver/gpio.h"
#include <stdio.h>

bool PWMOut::_init = false;

PWMOut::PWMOut() {

}

void PWMOut::init(  gpio_num_t pin=GPIO_NUM_18 ) {
	if( _init == false )
	{
		// sigmadelta_set_pin( SIGMADELTA_CHANNEL_0, pin );
		// sigmadelta_set_prescale( SIGMADELTA_CHANNEL_0, 255 );
		// sigmadelta_set_duty( SIGMADELTA_CHANNEL_0, 0 );
		_init = true;
	}
}


PWMOut::~PWMOut() {
	// sigmadelta_set_duty( SIGMADELTA_CHANNEL_0, 0 );
}

void PWMOut::setDuty( int8_t duty )
{
	printf("setDuty( %d ) \n", duty );
	// sigmadelta_set_duty( SIGMADELTA_CHANNEL_0, duty-127 );
}

void PWMOut::setContrast( float val )
{
	printf("setContrast( %f ) \n", val );
	// val = 2.55 * ( ((val/100.0)*14.0) + 62.0 );
	// setDuty( val );
}





