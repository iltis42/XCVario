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
	bool isClosed();
	bool isOpen();
private:
	gpio_num_t _sw;
};

#endif /* MAIN_SWITCH_H_ */
