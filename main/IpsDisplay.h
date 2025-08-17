
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

#define TEGAP 26
#define TEMIN TEGAP
#define TEMAX DISPLAY_H-TEGAP
#define DISPLAY_LEFT 25

enum ips_display { ILI9341 };

typedef enum e_sreens { INIT_DISPLAY_NULL, INIT_DISPLAY_AIRLINER=1, INIT_DISPLAY_RETRO=2, INIT_DISPLAY_FLARM=4, INIT_DISPLAY_GLOAD=8, INIT_DISPLAY_UL=16, INIT_DISPLAY_HORIZON=32 } e_screens_t;
extern int screens_init;

class PolarGauge;
class WindIndicator;
class McCready;
class Battery;
class Altimeter;
class CruiseStatus;

// fixme needs a home
float getHeading();

class IpsDisplay {
public:
	IpsDisplay( AdaptUGC *aucg );
	virtual ~IpsDisplay();
	static void begin();
	static void setup();
	static void bootDisplay();
	static void writeText( int line, const char *text );
	static void writeText( int line, std::string &text );
	                          // airspeed,       TE,       aTE,       polar_sink,       alt, temperature, battery, s2f_delta, as2f, aCl, s2fmode
	static void drawDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt, float wksensor );
	static void drawWarning( const char *warn, bool push=false );
	static void drawLoadDisplay( float loadFactor );
	static void drawHorizon( float pitch, float roll, float yaw );
	static void drawLoadDisplayTexts();
	static void initDisplay();
	static void clear();   // erase whole display
	static void drawArrowBox( int x, int y, bool are=true );
	static void redrawValues();
	// static bool drawCompass(int16_t x, int16_t y, bool wind_dirty, bool compass_dirty );
	// static void drawWindArrow( float dir, float speed, int type );
	static void setBottomDirty();
	static void setCruiseChanged();

	static inline AdaptUGC *getDisplay() { return ucg; };
	static AdaptUGC *ucg;

private:
	static float _range;
	static int prev_winddir;
	static int prev_heading;
	static int prev_windspeed;
	static int _divisons;
	static int _pixpmd;

	enum ips_display _dtype;
	static int tick;
	static ucg_color_t colors[];
	static ucg_color_t colorsalt[];

	// local variabls for dynamic display
	static int s2falt;
	static int s2fdalt;
    static int s2f_level_prev;
	static bool wireless_alive;
	static int tempalt;
	static bool s2fmodealt;
	static int s2fclipalt;
	static int as_prev;

	static int wkalt;
	static int wkoptalt;

	static int tyalt;
	static int pyalt;
	static float average_climbf;

	static ucg_color_t wkcolor;
	static float old_polar_sink;
	static float flt_altitude;

	static PolarGauge* MAINgauge;
    static PolarGauge* WNDgauge;
	static McCready*   MCgauge;
	static Battery*    BATgauge;
	static Altimeter*  ALTgauge;
    static CruiseStatus* STATgauge;
	static temp_status_t siliconTempStatusOld;

	static void setArrowColor( bool upper );
	static void drawArrow(int16_t x, int16_t y, int16_t level, bool del);
	static void drawS2FBar(int16_t x, int16_t y, int s2fd);
	static void drawBT();
	static void drawCable(int16_t x, int16_t y);
	static void drawWifi( int x, int y );
	static void drawConnection( int16_t x, int16_t y );
	static void drawTemperature( int x, int y, float t );
	static void drawThermometer( int x, int y );

private:
	static void initRetroDisplay();
	static void initLoadDisplay();
	static void drawRetroDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, float wksensor );
	static void drawAirlinerDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt, float wksensor );
	static void setTeBuf( int y1, int y2, int r, int g, int b );
	static void drawTeBuf();
	static void drawGaugeTriangle( int y, int r, int g, int b, bool s2f=false );
	static void drawAvgSymbol( int y, int r, int g, int b, int x=DISPLAY_LEFT );
	static void drawAvg( float mps, float delta );
	static void drawSmallSpeed(float v_kmh, int16_t x, int16_t y);
	static bool drawTopGauge(int val, int16_t x, int16_t y, bool inc_unit=false);
	static void drawLegend( bool onlyLines=false );
	static void drawNetto( int16_t x, int16_t y, bool netto );
};
