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
	void begin();
	void press() override;
	void release() override {};
	void up( int count ) override {};
	void down( int count ) override {};
	void longPress() override;
	void escape() override {};
	void doSoftwareUpdate( IpsDisplay *p );
private:
    bool pressed;
    int  tick;
};

#endif /* MAIN_SWITCH_H_ */
