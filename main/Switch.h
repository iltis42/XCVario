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
	void begin( gpio_num_t sw );
	inline bool isClosed() {  return( !(bool)( gpio_get_level(_sw) ) ); }
	inline bool isOpen() {  return( (bool)( gpio_get_level(_sw) ) ); }
private:
	gpio_num_t _sw;
};

#endif /* MAIN_SWITCH_H_ */
