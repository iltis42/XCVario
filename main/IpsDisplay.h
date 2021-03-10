
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
	static inline Ucglib_ILI9341_18x240x320_HWSPI *getDisplay() { return ucg; };

private:
	static Ucglib_ILI9341_18x240x320_HWSPI *ucg;
	gpio_num_t _reset;
	gpio_num_t _cs;
	gpio_num_t _dc;
	static float _range;
	static int prev_heading;
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
	static int prefalt;
	static int pref_qnh;
	static int chargealt;
	static int btqueue;
	static int tempalt;
	static int mcalt;
	static bool s2fmodealt;
	static int s2fclipalt;
	static int as_prev;
	static int s2fmode_prev;

	static int wkalt;
	static int wkposalt;
	static int wksensoralt;
	static int wkialt;

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

	// Pointer edges and alpha for analog display
	static float old_a;
	static int x_0;
	static int y_0;
	static int x_1;
	static int y_1;
	static int x_2;
	static int y_2;
	static int x_3;
	static int y_3;

	static void drawMC( float mc, bool large=false );
	static void drawS2FMode( int x, int y, bool cruise );
	static void drawCircling( int x, int y, bool draw );
	static void drawCruise( int x, int y, bool draw );
	static void drawBT();
	static void drawFlarm( int x, int y, bool flarm );
	static void drawWifi( int x, int y );
	static void drawBat( float volt, int x, int y, bool blank );
	static void drawTemperature( int x, int y, float t );
	static void drawThermometer( int x, int y );
	static void drawTetragon( float a, int x0, int y0, int l1, int l2, int w, int r, int g, int b, bool del=true );
	static void initRetroDisplay();
	static void initULDisplay();
	static void initLoadDisplay();
	static void drawRetroDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt, float wksensor );
	static void drawULDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt, float wksensor );
	static void drawAirlinerDisplay( int airspeed, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt, float wksensor );
	static void drawAnalogScale( int val, int pos, float range, int offset=0 );
	static void drawScaleLines( bool full, float max_pos, float max_neg );
	static void setTeBuf( int y1, int y2, int r, int g, int b );
	static void drawTeBuf();
	static void drawGaugeTriangle( int y, int r, int g, int b, bool s2f=false );
	static void drawAvgSymbol( int y, int r, int g, int b, int x=DISPLAY_LEFT );
	static void drawAvg( float mps, float delta );
	static void drawAltitude( float altitude, int x, int y );
	static void drawLegend( bool onlyLines=false );
	static void drawAvgVario( int x, int y, float ate );

};


#endif /* IPS_DISPLAY_H */


