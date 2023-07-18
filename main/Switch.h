/*
 * Switch.h
 *
 *  Created on: Feb 24, 2019
 *      Author: iltis
 */

#ifndef MAIN_SWITCH_H_
#define MAIN_SWITCH_H_

#include "average.h"

#define GYRO_FILTER_SAMPLES 20

class Switch {
public:
	Switch( );
	virtual ~Switch();
	static void begin( gpio_num_t sw );
	static bool isClosed();
	static bool isOpen();
	static bool cruiseMode();
	static bool getCruiseState(){ return cruise_mode_final; };
	static void setCruiseModeXCV();
	static void tick();   // call al least every 100 mS
	static void setCruiseSpeed( float s ) { _cruise_speed_kmh = s; };
private:
	static gpio_num_t _sw;
	static Average<GYRO_FILTER_SAMPLES, float, float> filter;
	static bool _cruise_mode_sw;
	static bool _cruise_mode_xcv;
	static bool _cruise_mode_speed;
	static bool _cruise_mode_flap;
	static bool _cruise_mode_gyro;
	static bool _closed;
	static int _holddown;
	static float _cruise_speed_kmh;
	static int _tick;
	static bool cm_switch_prev;
	static bool cm_auto_prev;
	static bool cm_xcv_prev;
	static bool cruise_mode_final;
	static bool initial;
};

#endif /* MAIN_SWITCH_H_ */
