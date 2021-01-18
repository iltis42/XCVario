#ifndef FLAP_H
#define FLAP_H

#include "Flap.h"
#include "AnalogInput.h"

/*
 * This class handels flap display and Flap sensor
 *
 */

class Flap {
public:
	static void  init();
	static float getLeverPosition( int sensorreading );
	static void  progress();
	static void  initSensor();
	static inline float getSensor() { return wksensor; }
	static inline void setSensor( float wk ) { wksensor = wk; }
	static inline unsigned int getSensorRaw() { return AnalogInWk->getRaw(1000); }
	static inline bool haveSensor() { if( AnalogInWk != 0 ) return true; else return false; }

private:
	static AnalogInput *AnalogInWk;
	static float wksensor;
	static int wksenspos[9];
	static int wksensorold;
};

#endif
