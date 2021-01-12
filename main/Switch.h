/*
 * Switch.h
 *
 *  Created on: Feb 24, 2019
 *      Author: iltis
 */

#ifndef MAIN_SWITCH_H_
#define MAIN_SWITCH_H_

class Switch {
public:
	Switch( );
	virtual ~Switch();
	static void begin( gpio_num_t sw );
	static bool isClosed();
	static bool isOpen();
	static bool cruiseMode( bool check_automode = true );
	static void tick();   // call al least every 100 mS
	static void setCruiseSpeed( float s ) { _cruise_speed_kmh = s; };
private:
	static gpio_num_t _sw;
	static bool _cruise_mode;
	static bool _closed;
	static int _holddown;
	static float _cruise_speed_kmh;
};

#endif /* MAIN_SWITCH_H_ */
