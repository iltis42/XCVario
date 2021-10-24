
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
#include <Ucglib.h>
#include "Colors.h"

#define TEGAP 26
#define TEMIN TEGAP
#define TEMAX DISPLAY_H-TEGAP
#define DISPLAY_LEFT 25

enum ips_display { ILI9341 };

typedef enum e_sreens { INIT_DISPLAY_NULL, INIT_DISPLAY_AIRLINER=1, INIT_DISPLAY_RETRO=2, INIT_DISPLAY_FLARM=4, INIT_DISPLAY_GLOAD=8, INIT_DISPLAY_UL=16 } e_screens_t;
extern int screens_init;

class IpsDisplay {
public:
	IpsDisplay( Ucglib_ILI9341_18x240x320_HWSPI *aucg );
	virtual ~IpsDisplay();
	static void begin();
	static void setup();
	static void bootDisplay();
	static void writeText( int line, String text );                          // airspeed,       TE,       aTE,       polar_sink,       alt, temperature, battery, s2f_delta, as2f, aCl, s2fmode
	static void drawDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt, float wksensor );
	static void drawWarning( const char *warn, bool push=false );
	static void drawLoadDisplay( float loadFactor );
	static void initDisplay();
	static void clear();   // erase whole display
	void doMenu( bool menu=true ) { _menu = menu; };
	static void drawArrowBox( int x, int y, bool are=true );
	static void redrawValues();
	static void drawCompass(int16_t x, int16_t y);
	static void drawWindArrow( float dir, float speed, int type );
	static inline Ucglib_ILI9341_18x240x320_HWSPI *getDisplay() { return ucg; };

private:
	static Ucglib_ILI9341_18x240x320_HWSPI *ucg;
	gpio_num_t _reset;
	gpio_num_t _cs;
	gpio_num_t _dc;
	static float _range;
	static int prev_heading;
	static int _divisons;
    static float _scale_k;
	static float _range_clip;
	static int _pixpmd;
	static float (*_gauge)(float);

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
	static int wksensoralt;

	static int yposalt;
	static int tyalt;
	static int pyalt;
	static int average_climb;
	static float average_climbf;
	// Battery Indicator related
	static int charge;
	static int red;
	static int yellow;
	static ucg_color_t wkcolor;
	static bool netto_old;
	static ucg_int_t char_width;
	static ucg_int_t char_height;

	// Pointer edges and alpha for analog display
	static float needle_pos_old;

	static void drawMC( float mc, bool large=false );
	static void drawS2FMode( int x, int y, bool cruise );
	static void drawArrow(int16_t x, int16_t y, int level, int16_t color);
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
	static void drawPolarIndicator(float a, int16_t l1, int16_t l2, int16_t w, ucg_color_t color);
    static void drawBow( float a, int16_t l1, bool netto);
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
	static void drawAltitude( float altitude, ucg_int_t x, ucg_int_t y, bool dirty, bool inc_unit = true );
	static void drawSmallSpeed(float v_kmh, ucg_int_t x, ucg_int_t y);
	static void drawSpeed(float speed, ucg_int_t x, ucg_int_t y, bool dirty, bool inc_unit=true);
	static void drawLegend( bool onlyLines=false );
	static void drawAvgVario( int16_t x, int16_t y, float ate );
	static float logGaugeIdx(const float val);
	static float linGaugeIdx(const float val);
	static void  initGauge(const float max, const bool log);
    static int16_t gaugeSin(const float idx, const int16_t len);
    static int16_t gaugeCos(const float idx, const int16_t len);
    static int16_t gaugeSin(const int16_t idx, const int16_t len);
    static int16_t gaugeCos(const int16_t idx, const int16_t len);
	static float gaugeValueFromIdx(const float val);
};


#endif /* IPS_DISPLAY_H */
