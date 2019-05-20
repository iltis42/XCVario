/*
 * PWMOut.h
 *
 *  Created on: Dec 25, 2018
 *      Author: iltis
 */

#ifndef MAIN_PWMOUT_H_
#define MAIN_PWMOUT_H_

extern "C" {
#include <driver/sigmadelta.h>
}
#include "driver/gpio.h"

class PWMOut {
public:
	void init(  gpio_num_t pin );
	PWMOut();
	virtual ~PWMOut();
	void setDuty( int8_t duty );
	void setContrast( float val );
    static bool _init;
};


#endif /* MAIN_PWMOUT_H_ */
