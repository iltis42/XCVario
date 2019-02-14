/*
 * DotDisplay.h
 *
 *  Created on: Jan 25, 2018
 *      Author: iltis
 */

#ifndef MAIN_DOTDISPLAY_H_
#define MAIN_DOTDISPLAY_H_

#include "esp_system.h"
#include <u8g2.h>
#include "u8g2_esp32_hal.h"
#include "SetupCMD.h"

enum display { SSD1306, ST7565, UC1701 };

class DotDisplay {
public:
	DotDisplay( gpio_num_t mosi,
			 gpio_num_t miso,
			 gpio_num_t scl,
			 gpio_num_t dc,
			 gpio_num_t reset,
			 gpio_num_t cs  );
	virtual ~DotDisplay();
	void begin( SetupCMD* setup );
	void setup();
	static void drawDisplay( float te, float ate, float tealt, float temperature, float volt, float s2fd, float s2f, float acl, bool s2fmode );
	static void test(void *arg);
	void doMenu( bool menu=true ) { _menu = menu; };
	u8g2_t * getBuffer() { return &u8g2; };
private:
	static void drawDisplayPartial( float te, float ate );
	static void drawConstant();
	static u8g2_t u8g2;
	// static u8g2_t u8g2c;
	gpio_num_t _mosi;
	gpio_num_t _miso;
	gpio_num_t _scl;
	gpio_num_t _dc;
	gpio_num_t _reset;
	gpio_num_t _cs;
	u8g2_esp32_hal_t u8g2_esp32_hal;
	static float _te;
	static float _temp;
	static float _clipte;
	static float _ate;
	static int   _atesec;
	static float _battery;
	static int _lastTE;
	static int _lastATE;
	static int _range;
	SetupCMD *_setup;
	static int _divisons;
	static int _pixpmd;
	static float _range_clip;
	static bool _menu;
	enum display _dtype;
	static int tick;
};


#endif /* MAIN_DOTDISPLAY_H_ */
