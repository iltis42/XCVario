/*
 * DotDisplay.cpp
 *
 *  Created on: Jan 25, 2018
 *      Author: iltis
 *
 */

#include "DotDisplay.h"

#include <u8g2.h>
#include "u8g2_esp32_hal.h"
#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>

u8g2_t DotDisplay::u8g2;
int DotDisplay::tick = 0;
// u8g2_t DotDisplay::u8g2c;

const float max = 5.0;  // max m/s
const int   pixpm = 12; // pixel per meter
const int   dmid = 63;  // display middle
const int   bwide = 32; // total width of bargraph
const int   smfh  = 7;  // small font heigth
const int   hbw   = 6;  // horizontal bar width for unit of bargraph
const int   bw    = 22; // bar width
const int   trisize = 28; // triangle size quality up/down
#define abs(x)  (x < 0.0 ? -x : x)

DotDisplay::DotDisplay( gpio_num_t mosi,
		gpio_num_t miso,
		gpio_num_t scl,
		gpio_num_t dc,
		gpio_num_t reset,
		gpio_num_t cs ) {
	_mosi = mosi;
	_miso = miso;
	_scl = scl;
	_dc = dc;
	_reset = reset;
	_cs = cs;
	_te = 0;
	_menu = 0;
	_clipte = 0;
	_setup = 0;
	// _dtype = ST7565;
	_dtype = UC1701;
	// _dtype = SSD1306;
	tick = 0;
}

DotDisplay::~DotDisplay() {
}


void DotDisplay::begin( SetupCMD* asetup ) {
	u8g2_esp32_hal = U8G2_ESP32_HAL_DEFAULT;
	u8g2_esp32_hal.clk   = _scl;   // SCLK_bme280;
	u8g2_esp32_hal.mosi  = _mosi;  // MOSI_bme280;
	u8g2_esp32_hal.miso  = _miso;
	u8g2_esp32_hal.cs    = _cs;    // GPIO_NUM_13;
	u8g2_esp32_hal.dc    = _dc;    // GPIO_NUM_15;
	u8g2_esp32_hal.reset = _reset; // GPIO_NUM_5;
	u8g2_esp32_hal_init(u8g2_esp32_hal);

	// a structure which will contain all the data for one display
	switch( _dtype ) {

	// U8G2_UC1701_EA_DOGS102_1_4W_SW_SPI

	case UC1701:    // China LCS
		            // u8g2_Setup_uc1701_ea_dogs102_f(
					u8g2_Setup_uc1701_mini12864_f(
					&u8g2,
					U8G2_R0,
					u8g2_esp32_spi_byte_cb,
					u8g2_esp32_gpio_and_delay_cb);
		break;

	case ST7565:  // Reichelt LCD
		u8g2_Setup_st7565_ea_dogm128_f(
				&u8g2,
				U8G2_R0,
				u8g2_esp32_spi_byte_cb,
				u8g2_esp32_gpio_and_delay_cb);
		break;

	case SSD1306:   //  OLED
		u8g2_Setup_ssd1306_128x64_noname_f(
				&u8g2,
				U8G2_R0,
				u8g2_esp32_spi_byte_cb,
				u8g2_esp32_gpio_and_delay_cb);  // init u8g2 structure
		break;
	default:
		printf("Error, unknown display type %d", _dtype );
	}

	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
	u8g2_SetPowerSave(&u8g2, 0); // deactivate power save
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFontDirection(&u8g2,1);
	u8g2_SetDrawColor(&u8g2, 1);
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetFont(&u8g2,  u8g2_font_helvB08_tf );
	u8g2_SendBuffer(&u8g2);
	_setup = asetup;
	setup();
	// u8g2_enableUTF8Print();
	// drawConstant();
}

void DotDisplay::setup()
{
	_range = _setup->get()->_range;
	if( (int)_range %2 == 0 )
		_divisons = 4;
	else
		_divisons = 5;
	_pixpmd = (int)(60.0/_range);
	_range_clip = _range*1.2;
}


int   DotDisplay::_lastTE = 0;
int   DotDisplay::_lastATE = 0;
float   DotDisplay::_te = 0;
float   DotDisplay::_clipte = 0;
float   DotDisplay::_ate = 0;
int     DotDisplay::_atesec = 5;
float   DotDisplay::_battery = 0;
float   DotDisplay::_temp = 0.0;
int     DotDisplay::_range;
float   DotDisplay::_range_clip;
int     DotDisplay::_divisons = 5;
int     DotDisplay::_pixpmd = 12;

void DotDisplay::setData( float te, float ate, float tealt, float temp, float volt, float s2fd, float s2f ) {
	if( _menu )
		return;
	// printf("DotDisplay setTE( %f %f )\n", te, ate);
	_te = te;
	_clipte = te;
	if ( te > _range_clip )
		_clipte = _range_clip;
	if ( te < -_range_clip )
		_clipte = -_range_clip;

	_ate = ate;
	_temp = temp;
	_battery = volt;
	drawDisplay( _te, _ate, tealt, temp, s2fd, s2f );
}


int tebak=0;

void DotDisplay::drawDisplay( float te, float ate, float tealt, float temp, float s2fd, float s2f ){
	int ite = (int)(te*50);
	if( tebak == ite )
		return;
	tebak = ite;
	u8g2_ClearBuffer(&u8g2);
	u8g2_SetDrawColor( &u8g2, 1);
	u8g2_DrawVLine(&u8g2, dmid, bw, 6);
	u8g2_DrawVLine(&u8g2, dmid+(5*pixpm), bw, 3);
	u8g2_DrawVLine(&u8g2, dmid-(5*pixpm), bw, 3);

	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr );
	// print scale
	char buf[10];
	for( int i=-_divisons; i <=_divisons; i++ )
	{
		float legend = ((float)i/(float)_divisons)*_range;  // only print the integers
		if( abs( legend  - int( legend )) < 0.01 )
			sprintf( buf,"%+d",(int)legend);
		else
			sprintf( buf,"-");
		u8g2_DrawStr(&u8g2, dmid-(smfh/2)+(i*pixpm), 5,buf);
	}
	// u8g2_SetFont(&u8g2,  u8g2_font_helvB08_tf );
	// u8g2_DrawStr(&u8g2, dmid-(smfh/2)+(max*pixpm)-1, bwide+4,"m/s");

	// average TE value headline, seconds

	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr );
	sprintf( buf,"Var m/s");
	u8g2_DrawStr(&u8g2, 128-8,28,buf);

	u8g2_SetFont(&u8g2, u8g2_font_helvB12_tn );
	sprintf( buf,"%0.1f", ate);
	u8g2_SetFontMode(&u8g2,0);
	int x=32;
	if( ate > 0 )
		x=37;
	u8g2_DrawStr(&u8g2, 128-23,x,buf);

	// S2F
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr );
	sprintf( buf,"S2F kmh");
	u8g2_DrawStr(&u8g2, 128-30,26,buf);

	u8g2_SetFont(&u8g2, u8g2_font_helvB08_tr );
	sprintf( buf,"%d", (int)(s2f+0.5) );
	u8g2_DrawStr(&u8g2, 128-39,32,buf);

	// TE alt
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr );
	sprintf( buf,"Alti m");
	u8g2_DrawStr(&u8g2, 28,26,buf);
	u8g2_SetFont(&u8g2, u8g2_font_helvB08_tr );
	sprintf( buf,"%d", (int)tealt );
	u8g2_DrawStr(&u8g2, 18,32,buf);

	tick++;
	if( tick < 50 ) {
	// Temperature headline, val
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr );
	sprintf( buf,"Temp C");
	u8g2_DrawStr(&u8g2, 10,26,buf);

	u8g2_SetFont(&u8g2, u8g2_font_helvB08_tf );
	sprintf( buf,"%0.1f\xb0", temp );
	u8g2_DrawStr(&u8g2, 1,32,buf);
	}
	else if( tick < 100 ){
	// Battery headline, val
	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr );
	sprintf( buf,"Battery");
	u8g2_DrawStr(&u8g2, 10,26,buf);
	sprintf( buf,"%0.1f V",_battery);
	u8g2_DrawStr(&u8g2, 1,32,buf);

	}
	else
		tick = 0;

	u8g2_SetFont(&u8g2, u8g2_font_5x7_tr );
	u8g2_SetDrawColor( &u8g2, 2);
	// Now we draw the current TE value bar

	if( _clipte > 0 )
		u8g2_DrawBox(&u8g2, dmid, 1, (int)(_clipte*_pixpmd+0.5), bw-1 );
	else
		u8g2_DrawBox(&u8g2, dmid+(int)(_clipte*_pixpmd+0.5), 1, -(int)(_clipte*_pixpmd+0.5), bw-1 );
	// u8g2_SetDrawColor( &u8g2, 1);
	u8g2_SetDrawColor( &u8g2, 2);
	// u8g2_DrawFrame( &u8g2, 0,0,127,bw );
	u8g2_DrawHLine( &u8g2, 0, 0, 127 );
	u8g2_DrawHLine( &u8g2, 0, bw, 127 );
	u8g2_DrawTriangle(&u8g2, dmid+(int)(te*_pixpmd+0.5),bw,
			dmid+(int)(te*_pixpmd+0.5)+8,bw+8,
			dmid+(int)(te*_pixpmd+0.5)-8,bw+8 );
	int tri_head;

	// S2F command trend triangle
	int tri_len = int((trisize/2)* s2fd/40.0 );
	if( tri_len > trisize )
		tri_len = trisize;
    tri_head = dmid - tri_len;

    u8g2_DrawTriangle(&u8g2, dmid,bw+8,
			dmid,bw+8+trisize,
			tri_head,bw+8+trisize/2 );

	u8g2_SendBuffer(&u8g2);
}

void DotDisplay::test( void * arg ) {
	bool forward=true;
	while( 1 )
	{
		if( forward ){
			_te += 0.2;
			if( _te > max )
				forward = false;
		}
		else{
			_te -= 0.2;
			if( _te < -max )
				forward = true;
		}
		_ate = (_te - _ate )*(1.0/_atesec) +_ate;
		drawDisplay( _te, _ate, 300.0, 20.0, 0.0, 120.0 );
		printf("display test: %f avg: %f %d\n", _te, _ate, forward );
		vTaskDelay(400 / portTICK_PERIOD_MS);
	}
}

