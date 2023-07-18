
/*
 * IpsDisplay.h
 *
 *  Created on: Jan 25, 2018
 *      Author: iltis
 */

#ifndef IPS_DISPLAY_H
#define IPS_DISPLAY_H

#include "esp_system.h"
#include "Setup.h"
#include <SPI.h>
#include <AdaptUGC.h>
#include "Colors.h"

#define TEGAP 26
#define TEMIN TEGAP
#define TEMAX DISPLAY_H-TEGAP
#define DISPLAY_LEFT 25

enum ips_display { ILI9341 };

typedef enum e_sreens { INIT_DISPLAY_NULL, INIT_DISPLAY_AIRLINER=1, INIT_DISPLAY_RETRO=2, INIT_DISPLAY_FLARM=4, INIT_DISPLAY_GLOAD=8, INIT_DISPLAY_UL=16, INIT_DISPLAY_HORIZON=32 } e_screens_t;
extern int screens_init;

class PolarIndicator;

class IpsDisplay {
public:
	IpsDisplay( AdaptUGC *aucg );
	virtual ~IpsDisplay();
	static void begin();
	static void setup();
	static void bootDisplay();
	static void writeText( int line, String text );                          // airspeed,       TE,       aTE,       polar_sink,       alt, temperature, battery, s2f_delta, as2f, aCl, s2fmode
	static void drawDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt, float wksensor );
	static void drawWarning( const char *warn, bool push=false );
	static void drawLoadDisplay( float loadFactor );
	static void drawHorizon( float pitch, float roll, float yaw );
	static void drawLoadDisplayTexts();
	static void drawBow( float a, int16_t &old_a_level, int16_t l1, ucg_color_t color );
	static void initDisplay();
	static void clear();   // erase whole display
	void doMenu( bool menu=true ) { _menu = menu; };
	static bool inMenu() { return _menu; }
	static void drawArrowBox( int x, int y, bool are=true );
	static void redrawValues();
	static float getHeading();
	static bool drawCompass(int16_t x, int16_t y, bool wind_dirty, bool compass_dirty );
	static void drawWindArrow( float dir, float speed, int type );

	static inline AdaptUGC *getDisplay() { return ucg; };
	static AdaptUGC *ucg;

private:
	gpio_num_t _reset;
	gpio_num_t _cs;
	gpio_num_t _dc;
	static float _range;
	static int prev_winddir;
	static int prev_heading;
	static int prev_windspeed;
	static int _divisons;
	static float _range_clip;
	static int _pixpmd;

	static bool _menu;
	enum ips_display _dtype;
	static int tick;
	static ucg_color_t colors[TEMAX+1+TEGAP];
	static ucg_color_t colorsalt[TEMAX+1+TEGAP];

	// local variabls for dynamic display
	static int _te;
	static int _ate;
	static int s2falt;
	static int s2fdalt;
    static int s2f_level_prev;
	static int16_t bow_level_prev;
	static int16_t psink_level_prev;
	static int alt_prev;
	static float pref_qnh;
	static int chargealt;
	static int btqueue;
	static int tempalt;
	static int mcalt;
	static bool s2fmodealt;
	static int s2fclipalt;
	static int as_prev;
	static int s2fmode_prev;

	static int wkalt;
	static int wkoptalt;

	static int yposalt;
	static int tyalt;
	static int pyalt;
	static float average_climbf;
	// Battery Indicator related
	static int charge;
	static int red;
	static int yellow;
	static ucg_color_t wkcolor;
	static bool netto_old;
	static int16_t char_width;
	static int16_t char_height;
	static float old_polar_sink;
	static float flt_altitude;

	static int last_avg;
	static int  x_start;
	static PolarIndicator* indicator;
	static temp_status_t siliconTempStatusOld;

	static void drawMC( float mc, bool large=false );
	static void drawS2FMode( int x, int y, bool cruise );
	static void setArrowColor( bool upper );
	static void drawArrow(int16_t x, int16_t y, int16_t level, bool del);
	static void drawS2FBar(int16_t x, int16_t y, int s2fd);
	static void drawCircling( int x, int y, bool draw );
	static void drawCruise( int x, int y, bool draw );
	static void drawBT();
	static void drawCable(int16_t x, int16_t y);
	static void drawFlarm( int x, int y, bool flarm );
	static void drawWifi( int x, int y );
	static void drawConnection( int16_t x, int16_t y );
	static void drawBat( float volt, int x, int y, bool blank );
	static void drawTemperature( int x, int y, float t );
	static void drawThermometer( int x, int y );
	static void drawOneScaleLine(float a, int16_t l1, int16_t l2, int16_t w, uint8_t r, uint8_t g, uint8_t b);

	static void initRetroDisplay( bool ulmode );
	static void initLoadDisplay();
	static void drawRetroDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt, float wksensor, bool ulmode );
	static void drawAirlinerDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt, float wksensor );
	static void drawScale( int16_t max_pos, int16_t max_neg, int16_t pos, int16_t offset, int16_t at=-1000 );
	static void drawOneLabel( float val, int16_t labl, int16_t pos, int16_t offset );
	static void setTeBuf( int y1, int y2, int r, int g, int b );
	static void drawTeBuf();
	static void drawGaugeTriangle( int y, int r, int g, int b, bool s2f=false );
	static void drawAvgSymbol( int y, int r, int g, int b, int x=DISPLAY_LEFT );
	static void drawAvg( float mps, float delta );
	static bool drawAltitude( float altitude, int16_t x, int16_t y, bool dirty, bool inc_unit=true );
	static void drawSmallSpeed(float v_kmh, int16_t x, int16_t y);
	static bool drawSpeed(float speed, int16_t x, int16_t y, bool dirty, bool inc_unit=false);
	static void drawLegend( bool onlyLines=false );
	static void drawAvgVario( int16_t x, int16_t y, float ate, bool large=false );
	static void drawNetto( int16_t x, int16_t y, bool netto );
};


#endif /* IPS_DISPLAY_H */
