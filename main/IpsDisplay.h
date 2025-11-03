
/*
 * IpsDisplay.h
 *
 *  Created on: Jan 25, 2018
 *      Author: iltis
 */

#pragma once

#include "esp_system.h"
#include "setup/SetupNG.h"
#include "AdaptUGC.h"
#include "Colors.h"
#include <string>

typedef enum e_sreens { INIT_DISPLAY_NULL, INIT_DISPLAY_RETRO=1, INIT_DISPLAY_FLARM=2, INIT_DISPLAY_GLOAD=4, INIT_DISPLAY_HORIZON=8 } e_screens_t;
extern int screens_init;

class PolarGauge;
class WindIndicator;
class McCready;
class S2FBar;
class Battery;
class Altimeter;
class CruiseStatus;
class FlapsBox;

// fixme needs a home
float getHeading();

class IpsDisplay {
public:
	IpsDisplay( AdaptUGC *aucg );
	virtual ~IpsDisplay();
	static void begin();
	static void bootDisplay();
	static void setGlobalColors();
	static void writeText( int line, const char *text );
	static void writeText( int line, std::string &text );
	                          // airspeed,       TE,       aTE,       polar_sink,       alt, temperature, battery, s2f_delta, as2f, acl, wkf
	static void drawDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, float wksensor );

	static void drawLoadDisplay( float loadFactor );
	static void drawHorizon( float pitch, float roll, float yaw );
	static void drawLoadDisplayTexts();
	static void initDisplay();
	static void clear();   // erase whole display
	static void redrawValues();
	static void setBottomDirty();
	static void setCruiseChanged();

	static inline AdaptUGC *getDisplay() { return ucg; };
	static AdaptUGC *ucg;

private:
	// static int prev_winddir;
	// static int prev_heading;
	// static int prev_windspeed;

	static int tick;

	// local variabls for dynamic display
	static int s2falt;
	static int s2fdalt;
	static bool wireless_alive;
	static int tempalt;
	static int as_prev;

	static float average_climbf;

	static PolarGauge* MAINgauge;
    static PolarGauge* WNDgauge;
	static McCready*   MCgauge;
	static S2FBar*     S2FBARgauge;
	static Battery*    BATgauge;
	static Altimeter*  ALTgauge;
    static CruiseStatus* VCSTATgauge;
	static FlapsBox*   FLAPSgauge;
	static temp_status_t siliconTempStatusOld;

	static void drawBT();
	static void drawCable(int16_t x, int16_t y);
	static void drawWifi( int x, int y );
	static void drawConnection( int16_t x, int16_t y );
	static void drawTemperature( int x, int y, float t );

private:
	static void initLoadDisplay();
	static void drawAvg( float mps, float delta );
	static bool drawTopGauge(int val, int16_t x, int16_t y, bool inc_unit=false);
};

extern IpsDisplay *Display;
