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


enum ips_display { ILI9341 };

typedef enum bars { TEBAR, S2FBAR } t_bar;



class IpsDisplay {
public:
	IpsDisplay( Ucglib_ILI9341_18x240x320_HWSPI *aucg );
	virtual ~IpsDisplay();
	static void begin( Setup* setup );
	static void setup();
	                          // ias,       TE,       aTE,       polar_sink,       alt, temperature, battery, s2f_delta, as2f, aCl, s2fmode
	static void drawDisplay( int ias, float te, float ate, float polar_sink, float alt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode );
	static void initDisplay();
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
	float _clipte;

	static Setup *_setup;
	static bool _menu;
	enum ips_display _dtype;
	static int tick;
	static void drawBar( t_bar bar, short int origin, short int y, uint8_t r, uint8_t g, uint8_t b );
	static void drawClipBox( short int ori, short int y );
};


#endif /* IPS_DISPLAY_H */
