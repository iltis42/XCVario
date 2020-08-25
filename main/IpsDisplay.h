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

// For display with invers Color

#define COLOR_HEADER 255-101,255-108,255-255  // Azureish gray
#define COLOR_WHITE 0,0,0
#define COLOR_BLACK 255,255,255
#define COLOR_GREEN 255, 30, 255
#define COLOR_RED   0,255,255
#define LIGHT_GREEN 127,0,255
#define COLOR_YELLOW 0, 0, 255
#define DARK_GREY    230, 230, 230
#define COLOR_MGREY  180, 180, 180
#define COLOR_BLUE   255, 255, 0
#define COLOR_LBLUE  200, 200, 0

/*
#define COLOR_HEADER 65,105,225  // royal blue
#define COLOR_WHITE  255,255,255
#define COLOR_BLACK  0,0,0
#define COLOR_GREEN  0, 225, 0
#define COLOR_RED    255,80,80
#define LIGHT_GREEN  127,255,0
#define COLOR_YELLOW 255,255,0
#define DARK_GREY    25,25,25
#define COLOR_MGREY  75,75,75
#define COLOR_BLUE    0,0,255

*/
#define DISPLAY_H 320
#define DISPLAY_W 240

#define TEGAP 26
#define TEMIN TEGAP
#define TEMAX DISPLAY_H-TEGAP


enum ips_display { ILI9341 };

typedef enum bars { TEBAR, S2FBAR } t_bar;



class IpsDisplay {
public:
	IpsDisplay( Ucglib_ILI9341_18x240x320_HWSPI *aucg );
	virtual ~IpsDisplay();
	static void begin();
	static void setup();
	static void bootDisplay();
	static void writeText( int line, String text );
	                          // ias,       TE,       aTE,       polar_sink,       alt, temperature, battery, s2f_delta, as2f, aCl, s2fmode
	static void drawDisplay( int ias, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt );
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
	static int _divisons;
	static float _range_clip;
	static int _pixpmd;

	static bool _menu;
	enum ips_display _dtype;
	static int tick;
	static ucg_color_t colors[TEMAX+1];
	static ucg_color_t colorsalt[TEMAX+1];

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
	static int iasalt;

	static int wkalt;
	static char wkss[6];
	static int  wkspeeds[6];
	static bool wkbox;
	static int wkposalt;
	static int wkialt;

	static int yposalt;
	static int tyalt;
	static int pyalt;
	static int average_climb;
	// Battery Indicator related
	static int charge;
	static int red;
	static int yellow;
	static ucg_color_t wkcolor;


	static void setTeBuf( int y1, int y2, int r, int g, int b );
	static void drawTeBuf();
	static void drawGaugeTriangle( int y, int r, int g, int b, bool s2f=false );
	static void drawAvgSymbol( int y, int r, int g, int b );
	static void drawLegend( bool onlyLines=false );
	static void drawWkBar( int ypos, float wk );
	static void drawWkSymbol( int ypos, int wk, int wkalt );
	static int getWk( int wks );
};


#endif /* IPS_DISPLAY_H */
