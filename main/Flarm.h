#ifndef FLARM_H
#define FLARM_H
#include <cstdlib> // abs
#include <string> // std::string
#include <locale> // std::locale, std::toupper
#include <AdaptUGC.h>
#include "RingBufCPP.h"  // SString, tbd: extra header
#include "Units.h"


class Flarm {
public:
	static void setDisplay( AdaptUGC *theUcg ) { ucg = theUcg; };
	static void parsePFLAU( const char *pflau );
	static void parsePFLAA( const char *pflaa );
	static void parsePFLAX( SString &msg );
	static void parseGPRMC( const char *gprmc );
	static void parseGPGGA( const char *gpgga );
	static void parsePGRMZ( const char *pgrmz );
	static void drawAirplane( int x, int y, bool fromBehind=false, bool smallSize=false );
 	static inline int alarmLevel(){ return AlarmLevel; };
 	static void drawDownloadInfo();
 	static void drawFlarmWarning();
 	static void initFlarmWarning();
 	static void progress();
 	static bool connected(); // returns true if Flarm is connected
 	static inline bool getGPS( double &gndSpeedKmh, double &gndTrack ) { if( gpsOK ) {
 																			gndSpeedKmh = Units::knots2kmh(gndSpeedKnots);
 																			gndTrack = gndCourse;
 																			return true; }
 																		else
 																			return false;
 	                                                                   }
 	static bool gpsStatus() { return gpsOK; }
    static double getGndSpeedKnots() { return gndSpeedKnots; }
    static double getGndCourse() { return gndCourse; }
 	static int bincom;
 	static void tick();
 	static bool validExtAlt() { if( ext_alt_timer )
 									return true;
 								else
 									return false;
 	}

private:
 	static void drawClearTriangle( int x, int y, int rb, int dist, int size, int factor );
 	static void drawClearVerticalTriangle( int x, int y, int rb, int dist, int size, int factor );
 	static void drawTriangle( int x, int y, int rb, int dist, int size=15, int factor=2, bool erase=false );

 	static AdaptUGC* ucg;
	static int RX,TX,GPS,Power;
	static int AlarmLevel;
	static int RelativeBearing,RelativeVertical,RelativeDistance;
	static double gndSpeedKnots;
	static double gndCourse;
	static bool   gpsOK;
	static int AlarmType;
	static char ID[8];
	static int oldDist;
	static int oldVertical;
	static int oldBear;
	static int alarmOld;
	static int _tick;
	static int timeout;
	static int ext_alt_timer;
	static int _numSat;
};

#endif
