/*
 * OTA.h
 *
 *  Created on: Feb 24, 2019
 *      Author: iltis
 */
#ifndef MAIN_OTA_H_
#define MAIN_OTA_H_

#include "ESPRotary.h"

class OTA: public RotaryObserver
{
public:
	OTA();
	~OTA() {};
	void begin( ESPRotary * aRotary );
	void press();
	void release();
	void up( int count ) {};
	void down( int count ) {};
	void longPress() {};
	void doSoftwareUpdate( IpsDisplay *p );
private:
    bool pressed;
    ESPRotary * _rotary;
};

#endif /* MAIN_SWITCH_H_ */
