/*
 * OTA.h
 *
 *  Created on: Feb 24, 2019
 *      Author: iltis
 */
#pragma once

#include "setup/SetupMenuDisplay.h"

class OTA: public SetupMenuDisplay
{
public:
	OTA() : SetupMenuDisplay("") {}
	~OTA() = default;
	void display( int ) override;
	void press() override { _abort = true; }
	void longPress() override {}

private:
	int tick = 0;
	bool _abort = false;
};
