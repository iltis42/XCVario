#ifndef FLARM_H
#define FLARM_H
#include <stdlib.h> // abs
#include <string> // std::string
#include <locale> // std::locale, std::toupper
#include <Ucglib.h>
#include "RingBufCPP.h"  // SString, tbd: extra header


class Flarm {
public:
	static void setDisplay( Ucglib_ILI9341_18x240x320_HWSPI *theUcg ) { ucg = theUcg; };
	static void parsePFLAU( char *pflau );
	static void parsePFLAA( char *pflaa );
	static void parsePFLAX( SString &msg );
 	static inline int alarmLevel(){ return AlarmLevel; };
 	static void drawFlarmWarning();
 	static void initFlarmWarning();
 	static int bincom;

private:
 	static void drawClearTriangle( int x, int y, int rb, int dist, int size, int factor );
 	static void drawClearVerticalTriangle( int x, int y, int rb, int dist, int size, int factor );
 	static void drawTriangle( int x, int y, int rb, int dist, int size=15, int factor=2, bool erase=false );
 	static void drawAirplane( int x, int y, bool fromBehind=false );

 	static Ucglib_ILI9341_18x240x320_HWSPI* ucg;
	static int RX,TX,GPS,Power;
	static int AlarmLevel;
	static int RelativeBearing,RelativeVertical,RelativeDistance;
	static int AlarmType;
	static char ID[8];
};

#endif
