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
	static inline float getLever() { return lever; }
	static inline void setLever( float l ) { lever = l; }

	static inline unsigned int getSensorRaw() {
		if( haveSensor() )
			return AnalogInWk->getRaw(1000);
		else
			return 0;
	}
	static inline bool haveSensor() { if( AnalogInWk != 0 ) return true; else return false; }

private:
	static AnalogInput *AnalogInWk;
	static float lever;
	static int wksenspos[9];
	static int  leverold;
};

#endif
