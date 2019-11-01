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

enum ips_display { ILI9341 };


class IpsDisplay {
public:
	IpsDisplay( Ucglib_ILI9341_18x240x320_HWSPI *aucg );
	virtual ~IpsDisplay();
	static void begin( Setup* setup );
	static void setup();
	static void drawDisplay( int ias, float te, float ate, float tealt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode );
	static void initDisplay();
	void doMenu( bool menu=true ) { _menu = menu; };
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
};


#endif /* IPS_DISPLAY_H */
