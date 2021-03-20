#ifndef _FLAP_H
#define _FLAP_H

#include "AnalogInput.h"
#include <Ucglib.h>
#include "SetupMenu.h"

/*
 * This class handels flap display and Flap sensor
 *
 */

class Flap {
public:
	static void  init( Ucglib_ILI9341_18x240x320_HWSPI *theUcg );
	static float getLeverPosition( int sensorreading );
	static void  progress();
	static void  initSensor();
	static void  initSpeeds();
	static inline float getLever() { return lever; }
	static inline void setLever( float l ) { lever = l; }
	// recommendations
	static float getOptimum( float wks, int wki );
	static int   getOptimumInt( float wks );
	static void configureADC();
	static inline unsigned int getSensorRaw(int oversampling=1) {
		if( haveSensor() )
			return sensorAdc->getRaw(oversampling);
		else
			return 0;
	}
	static inline bool haveSensor() { if( sensorAdc != 0 ) return true; else return false; }
	static void drawSmallBar( int ypos, int xpos, float wkf );
	static void drawBigBar( int ypos, int xpos, float wkf, float wksens );
	static void drawLever( int xpos, int ypos, int oldypos, bool warn );
	static void drawWingSymbol( int ypos, int xpos, int wk, int wkalt );
	static void redraw() { sensorOldY = -1000; surroundingBox=false; };
	static void redrawLever() { sensorOldY = -1000; };
	static void setupMenue( SetupMenu *parent );

private:
	static Ucglib_ILI9341_18x240x320_HWSPI* ucg;
	static AnalogInput *sensorAdc;
	static float lever;
	static int   senspos[9];
	static int   leverold;
	static int   flapSpeeds[9];
	static bool  surroundingBox;
	static int   optPosOldY;
	static int   sensorOldY;
	static int   rawFiltered;
	static int   tick;
	static int   tickopt;
	static bool  warn_color;
};

#endif
