/*
 * OTA.h
 *
 *  Created on: Feb 24, 2019
 *      Author: iltis
 */
#pragma once

class OTA
{
public:
	OTA() = default;
	~OTA() = default;
	void doSoftwareUpdate( IpsDisplay *p );

private:
	int  tick = 0;
};


