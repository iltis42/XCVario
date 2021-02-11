#ifndef LEAK_TEST_H
#define LEAK_TEST_H


#include "sensor.h"
#include "PressureSensor.h"
#include "AirspeedSensor.h"

class LeakTest{

public:
	static void start( PressureSensor* bmpBA, PressureSensor* bmpTE, AirspeedSensor *as );

};



#endif
