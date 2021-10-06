/*
 * IpsDisplay.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: iltis
 *
 */

#include <cmath>
#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <Ucglib.h>
#include "IpsDisplay.h"
#include "BTSender.h"
#include "DallasRmt.h"
#include "freertos/task.h"
#include <logdef.h>
#include "WifiClient.h"
#include "WifiApp.h"
#include "sensor.h"
#include "Units.h"
#include "Flap.h"
#include "Flarm.h"
#include "Compass.h"
#include "CircleWind.h"

int screens_init = INIT_DISPLAY_NULL;

int   IpsDisplay::tick = 0;
bool  IpsDisplay::_menu = false;
int   IpsDisplay::_pixpmd = 10;
int   IpsDisplay::charge = 100;
int   IpsDisplay::red = 10;
int   IpsDisplay::yellow = 25;

float IpsDisplay::old_a = 0;
int IpsDisplay::x_0 = 0;
int IpsDisplay::y_0 = 0;
int IpsDisplay::x_1 = 0;
int IpsDisplay::y_1 = 0;
int IpsDisplay::x_2 = 0;
int IpsDisplay::y_2 = 0;
int IpsDisplay::x_3 = 0;
int IpsDisplay::y_3 = 0;

bool IpsDisplay::netto_old = false;

#define DISPLAY_H 320
#define DISPLAY_W 240

// u8g2_t IpsDisplay::u8g2c;

const int   dmid = 160;   // display middle
const int   bwide = 64;   // total width of bargraph
const int   smfh  = 12;   // small font heigth
const int   hbw   = 12;   // horizontal bar width for unit of bargraph
const int   bw    = 32;   // bar width
const int   S2F_TRISIZE = 60; // triangle size quality up/down

#define TRISIZE 15

#define FIELD_START 85
#define FIELD_START_UL_AS 185
#define SIGNLEN 24+4
#define GAP 12

#define HEADFONTH 16
#define VARFONTH  45  // fub35_hn
#define YVAR HEADFONTH+VARFONTH
#define YVARMID (YVAR - (VARFONTH/2))

#define S2FFONTH 31
#define YS2F YVAR+S2FFONTH+HEADFONTH+GAP-8


#define VARBARGAP (HEADFONTH+(HEADFONTH/2)+2)
#define MAXS2FTRI 43
#define MAXTEBAR ((DISPLAY_H-(VARBARGAP*2))/2)

#define YALT (YS2F+S2FFONTH+HEADFONTH+GAP+2*MAXS2FTRI +22 )

#define BATX (DISPLAY_W-15)
#define BATY (DISPLAY_H-15)
#define LOWBAT  11.6    // 20%  -> 0%
#define FULLBAT 12.8    // 100%

#define BTSIZE  5
#define BTW    15
#define BTH    24
#define ASVALX 163

#define FLOGO  24

int S2FST = 45;

#define UNITVAR (vario_unit.get())
#define UNITAS (ias_unit.get())
#define UNITALT (alt_unit.get())


int ASLEN = 0;
#define AMIDY 160
#define AMIDX 140
static int fh;

extern xSemaphoreHandle spiMutex;

#define PMLEN 24

ucg_color_t IpsDisplay::colors[TEMAX+1+TEGAP];
ucg_color_t IpsDisplay::colorsalt[TEMAX+1+TEGAP];


Ucglib_ILI9341_18x240x320_HWSPI *IpsDisplay::ucg = 0;

int IpsDisplay::_te=0;
int IpsDisplay::_ate=0;
int IpsDisplay::s2falt=-1;
int IpsDisplay::s2fdalt=0;
int IpsDisplay::s2fmode_prev=100;
int IpsDisplay::prefalt=0;
int IpsDisplay::chargealt=-1;
int IpsDisplay::btqueue=-1;
int IpsDisplay::tempalt = -2000;
int IpsDisplay::mcalt = -100;
bool IpsDisplay::s2fmodealt = false;
int IpsDisplay::s2fclipalt = 0;
int IpsDisplay::as_prev = -1;
int IpsDisplay::yposalt = 0;
int IpsDisplay::tyalt = 0;
int IpsDisplay::pyalt = 0;
int IpsDisplay::wkalt = -3;


// Flap definitions
#define WKSYMST DISPLAY_W-28
ucg_color_t IpsDisplay::wkcolor;
int IpsDisplay::wkoptalt;
int IpsDisplay::wksensoralt;
int IpsDisplay::wkialt;

float IpsDisplay::_range_clip = 0;
int   IpsDisplay::_divisons = 5;
float IpsDisplay::_range = 5;
int   IpsDisplay::average_climb = -100;
float IpsDisplay::average_climbf = 0;
int   IpsDisplay::prev_heading = 0;
int   IpsDisplay::pref_qnh = 0;

#define WKBARMID (AMIDY-15)

float polar_sink_prev = 0;
float te_prev = 0;
bool blankold = false;
bool blank = false;
bool flarm_connected=false;

IpsDisplay::IpsDisplay( Ucglib_ILI9341_18x240x320_HWSPI *aucg ) {
	ucg = aucg;
	_dtype = ILI9341;
	_divisons = 5;
	_range_clip = 0;
	_range = 5;
	tick = 0;
	_dc = GPIO_NUM_MAX;
	_reset = GPIO_NUM_MAX;
	_cs = GPIO_NUM_MAX;
}

IpsDisplay::~IpsDisplay() {
}


void IpsDisplay::drawArrowBox( int x, int y, bool arightside ){
	if( _menu )
		return;
	int fh = ucg->getFontAscent();
	int fl = ucg->getStrWidth("123");
	if( arightside )
		ucg->drawTriangle( x+fl+4,y-(fh/2)-3,x+fl+4,y+(fh/2)+3,x+fl+4+fh/2,y );
	else
		ucg->drawTriangle( x,y-(fh/2)-3,   x,y+(fh/2)+3,   x-fh/2,y );
}

void IpsDisplay::drawLegend( bool onlyLines ) {
	if( _menu )
		return;
	int hc=0;
	if( onlyLines == false ){
		ucg->setFont(ucg_font_9x15B_mf);
		hc = ucg->getFontAscent()/2;
	}
	ucg->setColor(COLOR_WHITE);
	for( int i=_divisons; i >=-_divisons; i-- )
	{
		float legend = ((float)i*_range)/_divisons;  // only print the integers
		int y = (int)(dmid - int(legend*_pixpmd));
		if( onlyLines == false ){
			if( abs( legend  - int( legend )) < 0.1 ) {
				ucg->setPrintPos(0, y+hc  );
				ucg->printf("%+d",(int)legend );
			}
		}
		ucg->drawHLine( DISPLAY_LEFT, y , 4 );
	}
}
// draw all that does not need refresh when values change

void IpsDisplay::writeText( int line, String text ){
	ucg->setFont(ucg_font_ncenR14_hr);
	ucg->setPrintPos( 1, 26*line );
	ucg->setColor(COLOR_WHITE);
	ucg->printf("%s",text.c_str());
}


void IpsDisplay::clear(){
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setColor( COLOR_BLACK );
	ucg->drawBox( 0,0,240,320 );
	screens_init = INIT_DISPLAY_NULL;
	xSemaphoreGive(spiMutex);
	redrawValues();
}

void IpsDisplay::bootDisplay() {
	if( _menu )
		return;
	// ESP_LOGI(FNAME,"IpsDisplay::bootDisplay()");
	setup();
	if( display_type.get() == ST7789_2INCH_12P )
		ucg->setRedBlueTwist( true );
	if( display_type.get() == ILI9341_TFT_18P )
		ucg->invertDisplay( true );
	clear();
	if( display_orientation.get() == 1 )
		ucg->setRotate180();

	ucg->setColor(1, COLOR_BLACK );
	ucg->setColor(0, COLOR_WHITE );
	ucg->setFont(ucg_font_fub11_tr);
}

void IpsDisplay::initDisplay() {
	if( _menu )
		return;
	// ESP_LOGI(FNAME,"IpsDisplay::initDisplay()");
	// set global color variables according to selected display_variant
	if ( display_variant.get() == DISPLAY_WHITE_ON_BLACK ) {
		g_col_background = 255;
		g_col_highlight = 0;
		g_col_header_r=154;
		g_col_header_g=147;
		g_col_header_b=0;
		g_col_header_light_r=94;
		g_col_header_light_g=87;
		g_col_header_light_b=0;
	}
	else {
		g_col_background = 0;
		g_col_highlight = 255;
		g_col_header_r=101;
		g_col_header_g=108;
		g_col_header_b=255;
		g_col_header_light_r=161;
		g_col_header_light_g=168;
		g_col_header_light_b=255;
	}
	if( display_style.get() == DISPLAY_RETRO ) {
		initRetroDisplay();
	}
	if( display_style.get() == DISPLAY_UL ) {
		initULDisplay();
		// AS Box
		int fl = 45; // ucg->getStrWidth("200-");

		ASLEN = fl;
		S2FST = ASLEN+16;

		// S2F Zero
		// ucg->drawTriangle( FIELD_START, dmid+5, FIELD_START, dmid-5, FIELD_START+5, dmid);
		ucg->drawTriangle( FIELD_START+ASLEN-1, dmid, FIELD_START+ASLEN+5, dmid-6, FIELD_START+ASLEN+5, dmid+6);

	}
	if( display_style.get() == DISPLAY_AIRLINER ) {
		bootDisplay();
		ucg->setFontPosBottom();
		ucg->setPrintPos(20,YVAR-VARFONTH+7);
		ucg->setColor(0, COLOR_HEADER );
		if( UNITVAR == 0 ) // m/s
			ucg->print("  m/s");
		if( UNITVAR == 1 ) // ft/min
			ucg->print("cft/m");
		if( UNITVAR == 2 ) // knots
			ucg->print("knots");
		ucg->setPrintPos(FIELD_START,YVAR-VARFONTH);    // 65 -52 = 13

		ucg->print("AV Vario");
		ucg->setColor(0, COLOR_WHITE );

		// print TE scale
		drawLegend();

		ucg->drawVLine( DISPLAY_LEFT+5,      VARBARGAP , DISPLAY_H-(VARBARGAP*2) );
		ucg->drawHLine( DISPLAY_LEFT+5, VARBARGAP , bw+1 );
		ucg->drawVLine( DISPLAY_LEFT+5+bw+1, VARBARGAP, DISPLAY_H-(VARBARGAP*2) );
		ucg->drawHLine( DISPLAY_LEFT+5, DISPLAY_H-(VARBARGAP), bw+1 );

		// Sollfahrt Text
		ucg->setFont(ucg_font_fub11_tr);
		fh = ucg->getFontAscent();
		ucg->setPrintPos(FIELD_START+6,YS2F-(2*fh)-11);
		ucg->setColor(0, COLOR_HEADER );

		if( airspeed_mode.get() == MODE_IAS )
			ucg->printf("IAS %s", Units::AirspeedUnit() );
		else if( airspeed_mode.get() == MODE_TAS )
			ucg->printf("TAS %s", Units::AirspeedUnit() );

		ucg->setPrintPos(ASVALX,YS2F-(2*fh)-11);
		ucg->print(" S2F");

		ucg->setColor(0, COLOR_WHITE );
		// AS Box
		int fl = 45; // ucg->getStrWidth("200-");

		ASLEN = fl;
		S2FST = ASLEN+16;

		// S2F Zero
		// ucg->drawTriangle( FIELD_START, dmid+5, FIELD_START, dmid-5, FIELD_START+5, dmid);
		ucg->drawTriangle( FIELD_START+ASLEN-1, dmid, FIELD_START+ASLEN+5, dmid-6, FIELD_START+ASLEN+5, dmid+6);

		// Thermometer
		drawThermometer(  FIELD_START+10, DISPLAY_H-4 );
	}
	redrawValues();
}

// Thermometer
void IpsDisplay::drawThermometer( int x, int y) {
	if( _menu )
		return;
	ucg->setColor(COLOR_WHITE);
	ucg->drawDisc( x, y,  4, UCG_DRAW_ALL ); // white disk
	ucg->setColor(COLOR_RED);
	ucg->drawDisc( x, y,  2, UCG_DRAW_ALL );  // red disk
	ucg->setColor(COLOR_WHITE);
	ucg->drawVLine( x-1, y-16, 14 );
	ucg->setColor(COLOR_RED);
	ucg->drawVLine( x,  y-16, 14 );  // red color
	ucg->setColor(COLOR_WHITE);
	ucg->drawPixel( x,  y-17 );  // upper point
	ucg->drawVLine( x+1, y-16, 14 );
}

void IpsDisplay::begin() {
	ESP_LOGI(FNAME,"IpsDisplay::begin");
	ucg->begin(UCG_FONT_MODE_SOLID);
	setup();
}

void IpsDisplay::setup()
{
	ESP_LOGI(FNAME,"IpsDisplay::setup");
	_range = range.get();

	if( (int)_range <= 5 )
		_divisons = (int)_range*2;
	else if( (int)_range == 6 )
		_divisons = 6;
	else if( (int)_range == 7 )
		_divisons = 7;
	else if( (int)_range == 8 )
		_divisons = 4;
	else if( (int)_range == 9 )
		_divisons = 3;
	else if( (int)_range == 10 )
		_divisons = 5;
	else if( (int)_range == 12 )
		_divisons = 6;
	else if( (int)_range == 13 )
		_divisons = 5;
	else if( (int)_range == 14 )
		_divisons = 7;
	else if( (int)_range == 15 )
		_divisons = 3;
	else
		_divisons = 5;

	_pixpmd = (int)((  (DISPLAY_H-(2*VARBARGAP) )/2) /_range);
	ESP_LOGI(FNAME,"Pixel per m/s %d", _pixpmd );
	_range_clip = _range;
}

void IpsDisplay::drawGaugeTriangle( int y, int r, int g, int b, bool s2f ) {
	if( _menu )
		return;
	ucg->setColor( r,g,b );
	if( s2f )
		ucg->drawTriangle( DISPLAY_LEFT+4+bw+3+TRISIZE,  dmid+y,
				DISPLAY_LEFT+4+bw+3, dmid+y+TRISIZE,
				DISPLAY_LEFT+4+bw+3, dmid+y-TRISIZE );
	else
		ucg->drawTriangle( DISPLAY_LEFT+4+bw+3,         dmid-y,
				DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-y+TRISIZE,
				DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-y-TRISIZE );
}

void IpsDisplay::drawAvgSymbol( int y, int r, int g, int b, int x ) {
	if( _menu )
		return;
	int size = 6;
	ucg->setColor( r,g,b );
	ucg->drawTetragon( x+size-1,dmid-y, x,dmid-y+size, x-size,dmid-y, x,dmid-y-size );
}

float avc_old=-1000;
int yusize=7;
int ylsize=7;

void IpsDisplay::drawAvg( float avclimb, float delta ){
	if( _menu )
		return;
	ESP_LOGD(FNAME,"drawAvg: av=%.2f delta=%.2f", avclimb, delta );
	int pos=130;
	int size=7;
	if( avc_old != -1000 ){
		ucg->setColor( COLOR_BLACK );
		int x=AMIDX - cos((avc_old/_range)*M_PI_2)*pos;
		int y=AMIDY - sin((avc_old/_range)*M_PI_2)*pos;
		ucg->drawTetragon( x+size, y, x,y+ylsize, x-size,y, x,y-yusize );
	}
	drawScaleLines( false, _range, -_range );
	// drawAnalogScale(0, 132);
	if( delta > 0 )
		ucg->setColor( COLOR_GREEN );
	else
		ucg->setColor( COLOR_RED );

	if( delta > 0.2 ){
		yusize=size*2;
		ylsize=size;
	}
	else if ( delta < -0.2 ){
		ylsize=size*2;
		yusize=size;
	}
	else{
		ylsize=size;
		yusize=size;
	}

	if( avclimb > _range ) // clip values off weeds
		avclimb = _range;
	int x=AMIDX - cos((avclimb/_range)*M_PI_2)*pos;
	int y=AMIDY - sin((avclimb/_range)*M_PI_2)*pos;
	ESP_LOGD(FNAME,"drawAvg: x=%d  y=%d", x,y );
	ucg->drawTetragon( x+size,y, x, y+ylsize, x-size,y, x,y-yusize );
	avc_old=avclimb;
}

void IpsDisplay::redrawValues()
{
	if( _menu )
		return;
	// ESP_LOGI(FNAME,"IpsDisplay::redrawValues()");
	chargealt = 101;
	tempalt = -2000;
	s2falt = -1;
	s2fdalt = -1;
	btqueue = -1;
	_te=-200;
	old_a=-1000;
	mcalt = -100;
	as_prev = -1;
	_ate = -200;
	prefalt = -1;
	pref_qnh = -1;
	tyalt = 0;
	for( int l=TEMIN-1; l<=TEMAX; l++){
		colors[l].color[0] = 0;
		colors[l].color[1] = 0;
		colors[l].color[2] = 0;
		colorsalt[l].color[0] = 0;
		colorsalt[l].color[1] = 0;
		colorsalt[l].color[2] = 0;
	}
	average_climb = -1000;

	wkalt = -4;
	wkoptalt = -100;
	wksensoralt = -1;
	wkialt = -3;
	tyalt = -1000;
	polar_sink_prev = 0.1;
	Flap::redraw();
	netto_old = false;
	s2fmode_prev = 100;
}

void IpsDisplay::drawTeBuf(){
	if( _menu )
		return;
	for( int l=TEMIN+1; l<TEMAX; l++){
		if( colorsalt[l].color[0] != colors[l].color[0]  || colorsalt[l].color[1] != colors[l].color[1] || colorsalt[l].color[2] != colors[l].color[2])
		{
			ucg->setColor( colors[l].color[0], colors[l].color[1], colors[l].color[2] );
			ucg->drawHLine( DISPLAY_LEFT+6, l, bw );
			colorsalt[l] = colors[l];
		}
	}
}

void IpsDisplay::setTeBuf( int y1, int h, int r, int g, int b ){
	// if( h == 0 )
	// 	return;
	// clip values for max TE bar
	y1 = dmid+(dmid-y1);
	if( y1-h >= TEMAX )
		h = -(TEMAX-y1);
	if( y1-h < TEMIN )
		h = TEMIN+y1;

	if( h>=0 ) {
		while( h >=0  ) {
			if( y1-h < TEMAX+1+TEGAP )
			{
				colors[y1-h].color[0] = r;
				colors[y1-h].color[1] = g;
				colors[y1-h].color[2] = b;
			}
			h--;
		}
	}
	else {
		while( h < 0  ) {
			if( y1-h < TEMAX+1+TEGAP )
			{
				colors[y1-h].color[0] = r;
				colors[y1-h].color[1] = g;
				colors[y1-h].color[2] = b;
			}
			h++;
		}
	}
}

void IpsDisplay::drawMC( float mc, bool large ) {
	if( _menu )
		return;
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(5,DISPLAY_H-6);
	ucg->setColor(COLOR_HEADER);
	ucg->printf("MC:");
	ucg->setPrintPos(5+ucg->getStrWidth("MC:"),DISPLAY_H-4);
	ucg->setColor(COLOR_WHITE);
	if( large )
		ucg->setFont(ucg_font_fub20_hn);
	else
		ucg->setFont(ucg_font_fub14_hn);
	ucg->printf("%1.1f", mc );
}


#define S2FSS 16
#define S2FTS 6

void IpsDisplay::drawCircling( int x, int y, bool draw ){
	if( _menu )
		return;
	if( draw )
		ucg->setColor( COLOR_WHITE );
	else
		ucg->setColor( COLOR_BLACK );
	ucg->drawCircle( x, y, S2FSS,   UCG_DRAW_ALL );
	ucg->drawCircle( x, y, S2FSS-1, UCG_DRAW_ALL );
	int tm=x-S2FSS+1;
	ucg->drawTriangle( tm-S2FTS, y+2, tm+S2FTS, y+2, tm, y+2-2*S2FTS );
}

void IpsDisplay::drawCruise( int x, int y, bool draw ){
	if( _menu )
		return;
	if( draw )
		ucg->setColor( COLOR_WHITE );
	else
		ucg->setColor( COLOR_BLACK );
	ucg->drawTetragon(x-S2FSS,y-5, x-S2FSS,y-1, x+S2FSS,y+5, x+S2FSS,y+1 );
	ucg->drawTriangle( x+6, y+7, x+8, y-4, x+S2FSS, y+3 );
}

void IpsDisplay::drawS2FMode( int x, int y, bool cruise ){
	if( _menu )
		return;
	if( cruise ) {
		drawCircling(x,y,false);
		drawCruise(x,y,true);
	}
	else
	{
		drawCruise(x,y,false);
		drawCircling(x,y,true);
	}
}

void IpsDisplay::drawBT() {
	if( _menu )
		return;
	int btq=BTSender::queueFull();
	if( btq != btqueue || Flarm::connected() != flarm_connected ){
		ucg_int_t btx=DISPLAY_W-22;
		ucg_int_t bty=(BTH/2) + 8;
		if( btq )
			ucg->setColor( COLOR_MGREY );
		else
			ucg->setColor( COLOR_BLUE );  // blue

		ucg->drawRBox( btx-BTW/2, bty-BTH/2, BTW, BTH, BTW/2-1);
		// inner symbol
		if( Flarm::connected() )
			ucg->setColor( COLOR_GREEN );
		else
			ucg->setColor( COLOR_WHITE );
		ucg->drawTriangle( btx, bty, btx+BTSIZE, bty-BTSIZE, btx, bty-2*BTSIZE );
		ucg->drawTriangle( btx, bty, btx+BTSIZE, bty+BTSIZE, btx, bty+2*BTSIZE );
		ucg->drawLine( btx, bty, btx-BTSIZE, bty-BTSIZE );
		ucg->drawLine( btx, bty, btx-BTSIZE, bty+BTSIZE );
		btqueue = btq;
		flarm_connected = Flarm::connected();
	}
}

void IpsDisplay::drawFlarm( int x, int y, bool flarm ) {
	if( _menu )
		return;
	ucg_int_t flx=x;
	ucg_int_t fly=y;
	if( flarm )
		ucg->setColor(COLOR_RED);
	else
		ucg->setColor( COLOR_MGREY );
	ucg->setClipRange( flx, fly-FLOGO, FLOGO, FLOGO );
	ucg->drawTriangle( flx+1, fly, flx+1+(FLOGO/2), fly, flx+1+(FLOGO/4), fly-(FLOGO/2)+2 );
	ucg->setClipRange( flx+FLOGO/4+3, fly-FLOGO, FLOGO, FLOGO );
	ucg->drawCircle( flx, fly, FLOGO/2 + (FLOGO/4)-2, UCG_DRAW_UPPER_RIGHT);
	ucg->setClipRange( flx+FLOGO/4+5, fly-FLOGO, FLOGO, FLOGO );
	ucg->drawCircle( flx, fly, FLOGO/2 + (FLOGO/2)-2, UCG_DRAW_UPPER_RIGHT);
	ucg->drawCircle( flx, fly, FLOGO/2 + (FLOGO/2)-3, UCG_DRAW_UPPER_RIGHT);
	ucg->undoClipRange();
}

void IpsDisplay::drawWifi( int x, int y ) {
	if( _menu )
		return;
	int btq=1;
	// ESP_LOGI(FNAME,"wireless %d", wireless );
	if( wireless == WL_WLAN_CLIENT ){
		if( WifiClient::isConnected(8884) )
			btq=0;
	}
	else if( wireless == WL_WLAN || wireless == WL_WLAN_MASTER )
		btq=WifiApp::queueFull();
	else
		return;
	if( btq != btqueue || Flarm::connected() != flarm_connected ){
		ESP_LOGD(FNAME,"IpsDisplay::drawWifi %d %d %d", x,y,btq);
		if( btq )
			ucg->setColor(COLOR_MGREY);
		else
			ucg->setColor( COLOR_BLUE );
		ucg->drawCircle( x, y, 9, UCG_DRAW_UPPER_RIGHT);
		ucg->drawCircle( x, y, 10, UCG_DRAW_UPPER_RIGHT);
		ucg->drawCircle( x, y, 16, UCG_DRAW_UPPER_RIGHT);
		ucg->drawCircle( x, y, 17, UCG_DRAW_UPPER_RIGHT);
		if( Flarm::connected() )
			ucg->setColor( COLOR_GREEN );
		ucg->drawDisc( x, y, 3, UCG_DRAW_ALL );
		flarm_connected = Flarm::connected();
		btqueue = btq;
	}
}

void IpsDisplay::drawBat( float volt, int x, int y, bool blank ) {
	if( _menu )
		return;
	if( blank ) {  // blank battery for blinking
		// ESP_LOGI(FNAME,"blank bat");
		if( battery_display.get() != BAT_VOLTAGE_BIG ){
			ucg->setColor( COLOR_BLACK );
			ucg->drawBox( x-40,y-2, 40, 12  );
		}else{
			ucg->setColor( COLOR_BLACK );
			ucg->drawBox( x-60,y-16, 70, 22  );
		}
	}
	else
	{
		charge = (int)(( volt -  bat_low_volt.get() )*100)/( bat_full_volt.get() - bat_low_volt.get() );
		if(charge < 0)
			charge = 0;
		if( charge > 100 )
			charge = 100;
		if( (tick%100) == 0 )  // check setup changes all 10 sec
		{
			yellow =  (int)(( bat_yellow_volt.get() - bat_low_volt.get() )*100)/( bat_full_volt.get() - bat_low_volt.get() );
			red = (int)(( bat_red_volt.get() - bat_low_volt.get() )*100)/( bat_full_volt.get() - bat_low_volt.get() );
		}
		ucg->setColor( COLOR_WHITE );
		if ( battery_display.get() != BAT_VOLTAGE_BIG ){
			ucg->drawBox( x-40,y-2, 36, 12  );  // Bat body square
			ucg->drawBox( x-4, y+1, 3, 6  );      // Bat pluspole pimple
			if ( charge > yellow )  // >25% grün
				ucg->setColor( COLOR_GREEN ); // green
			else if ( charge < yellow && charge > red )
				ucg->setColor( COLOR_YELLOW ); //  yellow
			else if ( charge < red )
				ucg->setColor( COLOR_RED ); // red
			else
				ucg->setColor( COLOR_RED ); // red
			int chgpos=(charge*32)/100;
			if(chgpos <= 4)
				chgpos = 4;
			ucg->drawBox( x-40+2,y, chgpos, 8  );  // Bat charge state
			ucg->setColor( DARK_GREY );
			ucg->drawBox( x-40+2+chgpos,y, 32-chgpos, 8 );  // Empty bat bar
			ucg->setColor( COLOR_WHITE );
			ucg->setFont(ucg_font_fub11_hr);
			ucg->setPrintPos(x-40,y-7);
		}
		if( battery_display.get() == BAT_PERCENTAGE )
			ucg->printf("%3d%%  ", charge);
		else if ( battery_display.get() == BAT_VOLTAGE ) {
			ucg->setPrintPos(x-50,y-8);
			ucg->printf("%2.1f V", volt);
		}
		else if ( battery_display.get() == BAT_VOLTAGE_BIG ) {
			ucg->setPrintPos(x-60,y+8);
			ucg->setFont(ucg_font_fub14_hr);
			ucg->printf("%2.1fV", volt);
		}

	}
}

void IpsDisplay::drawTemperature( int x, int y, float t ) {
	if( _menu )
		return;
	ucg->setFont(ucg_font_fur14_hf);
	ucg->setColor( COLOR_WHITE );
	ucg->setPrintPos(x,y);
	if( t != DEVICE_DISCONNECTED_C )
		ucg->printf("%-2.1f\xb0""  ", std::roundf(t*10.f)/10.f );
	else
		ucg->printf(" ---   ");
}

void IpsDisplay::drawTetragon( float a, int x0, int y0, int l1, int l2, int w, int r, int g, int b, bool del ){
	if( _menu )
		return;
	float si=sin(a);
	float co=cos(a);
	int w2=w;
	int xn_0 = x0-l1*co+w2*si;
	int yn_0 = y0-l1*si-w2*co;
	int xn_1 = x0-l1*co-w2*si;
	int yn_1 = y0-l1*si+w2*co;
	int xn_2 = x0-l2*co-w2*si;
	int yn_2 = y0-l2*si+w2*co;
	int xn_3 = x0-l2*co+w2*si;
	int yn_3 = y0-l2*si-w2*co;
	// ESP_LOGI(FNAME,"IpsDisplay::drawTetragon  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d x3:%d y3:%d", (int)xn_0, (int)yn_0, (int)xn_1 ,(int)yn_1, (int)xn_2, (int)yn_2, (int)xn_3 ,(int)yn_3 );
	if( del ) {  // cleanup previous incarnation
		ucg->setColor(  COLOR_BLACK  );
		ucg->drawTetragon(x_0,y_0,x_1,y_1,x_2,y_2,x_3,y_3);
		x_0 = xn_0;
		y_0 = yn_0;
		x_1 = xn_1;
		y_1 = yn_1;
		x_2 = xn_2;
		y_2 = yn_2;
		x_3 = xn_3;
		y_3 = yn_3;
		old_a = a;
	}
	ucg->setColor( r,g,b  );
	ucg->drawTetragon(xn_0,yn_0,xn_1,yn_1,xn_2,yn_2,xn_3,yn_3);
}

void IpsDisplay::drawScaleLines( bool full, float max_pos, float max_neg ){
	if( _menu )
		return;
	float modulo=1;
	if( max_pos > 10 )
		modulo = 2;
	if( max_pos < 5 )
		modulo = 0.5;
	int lower = 0;
	if( full )
		lower = (int)max_neg;
	for( float a=lower; a<=(int)max_pos; a+=modulo ) {
		int width=1;
		int end=135;
		int r = (int)max_pos;
		if( a==0 || abs(a)==r ){
			width=2;
			end=140;
		}
		if((r%2) == 0) {
			if(abs(a)==r/2){  // half scale big line
				width = 2;
				end=140;
			}
		}
		else{
			if( abs(a) == (r-1)/2 ){  // half scale minus one for even ranges big line
				width = 2;
				end=140;
			}
		}
		if( modulo < 1 ){
			if( fmod(a,1) == 0 ){  // every integer big line
				width = 2;
				end=140;
			}
		}
		drawTetragon( ((float)a/max_pos)*M_PI_2, AMIDX, AMIDY, 125, end, width, COLOR_WHITE, false );
	}
}

// Draw scale numbers for positive or negative value
void IpsDisplay::drawAnalogScale( int val, int pos, float range, int offset ){
	if( _menu )
		return;
	ucg->setFontPosCenter();
	ucg->setFont(ucg_font_fub14_hn);
	int x=AMIDX - cos((val/range)*M_PI_2)*pos;
	int y=AMIDY+1 - sin((val/range)*M_PI_2)*pos;
	if( val > 0 )
		ucg->setPrintPos(x-15,y);
	else
		ucg->setPrintPos(x-8,y);
	ucg->printf("%+d", val+offset );
	ucg->setFontPosBottom();
}


static int wx0,wy0,wx1,wy1,wx2,wy2,wx3,wy3;
static bool del_wind=false;


// draw windsock style alike arrow white and red
void IpsDisplay::drawWindArrow( float a, float speed, int type ){
	if( _menu )
		return;
	const int X=80;
	const int Y=220;
	float si=sin(D2R(a));
	float co=cos(D2R(a));
	const int b=9; // width of the arrow
	int s=speed*0.6;
	int s2=s;
	if( s>30 )
		s2=30;   // maximum space we got on the display
	if( s<10 )
		s2=10;    // minimum size, otherwise arrow is not readable

	int xn_0 = rint(X-s2*si);    // tip
	int yn_0 = rint(Y+s2*co);

	int xn_1 = rint(X+s2*si - b*co);  // left back
	int yn_1 = rint(Y-s2*co - b*si);

	int xn_3 = rint(X+s2*si + b*co);  // right back
	int yn_3 = rint(Y-s2*co + b*si);

	int xn_2 = rint(X+(s2*si*0.2));  // tip of second smaller arrow in red
	int yn_2 = rint(Y-(s2*co*0.2));

	// ESP_LOGI(FNAME,"IpsDisplay::drawWindArrow  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d x3:%d y3:%d", (int)xn_0, (int)yn_0, (int)xn_1 ,(int)yn_1, (int)xn_2, (int)yn_2, (int)xn_3 ,(int)yn_3 );
	if( del_wind ) {  // cleanup previous incarnation
		ucg->setColor(  COLOR_BLACK  );
		ucg->drawTriangle(wx0,wy0,wx1,wy1,wx3,wy3);
		if( wind_reference.get() != WR_NORTH )
			Flarm::drawAirplane( wx0, wy0, false, true ); // clear small airplane symbol
		wx0 = xn_0;
		wy0 = yn_0;
		wx1 = xn_1;
		wy1 = yn_1;
		wx2 = xn_2;
		wy2 = yn_2;
		wx3 = xn_3;
		wy3 = yn_3;
	}
	ucg->setColor( COLOR_WHITE );
	if( wind_reference.get() != WR_NORTH )
		Flarm::drawAirplane( xn_0, yn_0, false, true ); // draw a small airplane symbol
	if( s > 5 ){
		ucg->drawTriangle(xn_0,yn_0,xn_1,yn_1,xn_3,yn_3);
		ucg->setColor(  COLOR_RED  );
		ucg->drawTriangle(xn_2,yn_2,xn_1,yn_1,xn_3,yn_3);
	}
	del_wind = true;
}

void IpsDisplay::initULDisplay(){
	if( _menu )
		return;
	bootDisplay();
	ucg->setFontPosBottom();
	redrawValues();
	drawScaleLines( true, _range, -_range );
	int r = (int)_range;
	drawAnalogScale(-r,150, _range );
	drawAnalogScale(r,150, _range);
	// drawAnalogScale(0, 132);
	if((r%2) == 0) {
		drawAnalogScale(r/2,150, _range);
		drawAnalogScale(-r/2,155, _range);
	}
	else{
		drawAnalogScale((r-1)/2,150, _range);
		drawAnalogScale((-r+1)/2,155, _range);
	}
	// Unit's
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(85,15);
	ucg->print( Units::VarioUnit() );
	if( wireless == WL_BLUETOOTH )
		drawBT();
	if( wireless == WL_WLAN  ||  wireless == WL_WLAN_CLIENT )
		drawWifi(DISPLAY_W-27, FLOGO+2 );
	drawThermometer(  10, 30 );
}


void IpsDisplay::initRetroDisplay(){
	if( _menu )
		return;
	bootDisplay();
	ucg->setFontPosBottom();
	redrawValues();
	drawScaleLines( true, _range, -_range );
	int r = (int)_range;
	drawAnalogScale(-r,150, _range );
	drawAnalogScale(r,150, _range);
	// drawAnalogScale(0, 132);
	if((r%2) == 0) {
		drawAnalogScale(r/2,150, _range);
		drawAnalogScale(-r/2,155, _range);
	}
	else{
		drawAnalogScale((r-1)/2,150, _range);
		drawAnalogScale((-r+1)/2,155, _range);
	}
	// Unit's
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(85,15);
	ucg->print( Units::VarioUnit() );
	if( wireless == WL_BLUETOOTH )
		drawBT();
	if( wireless == WL_WLAN  ||  wireless == WL_WLAN_CLIENT )
		drawWifi(DISPLAY_W-27, FLOGO+2 );
	drawMC( MC.get(), true );
	drawThermometer(  10, 30 );
}

void IpsDisplay::drawWarning( const char *warn, bool push ){
	ESP_LOGI(FNAME,"drawWarning");
	clear();
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setColor( COLOR_RED );
	if( push ){
		ucg->drawTriangle(  60, 220, 180, 220, 120, 262 );
	}
	ucg->setPrintPos(10, AMIDY );
	ucg->setFontPosCenter();
	ucg->setColor( COLOR_RED );
	ucg->setFont(ucg_font_fub35_hr);
	ucg->printf(warn);
	ucg->setFontPosBottom();
	xSemaphoreGive(spiMutex);
}

void IpsDisplay::drawAvgVario( int x, int y, float ate ){
	if( _menu )
		return;
	ucg->setPrintPos(x, y );
	ucg->setFontPosCenter();
	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub35_hn);
	ucg->setClipRange( x, y-30, 95, 50 );
	ucg->setFont(ucg_font_fub35_hn);
	if( ate > 0 ){
		if ( ate < 10 )
			ucg->printf(" %2.1f   ", ate );
		else
			ucg->printf(" %2.0f   ", ate);
	}
	else{
		if ( ate > -10 )
			ucg->printf("%2.1f   ", ate );
		else
			ucg->printf("%2.0f   ", ate);
	}
	ucg->setFontPosBottom();
	ucg->undoClipRange();
}

void IpsDisplay::drawAltitude( float altitude, int x, int y ){
	if( _menu )
		return;
	int alt = (int)(altitude);
	if( alt != prefalt || !(tick%40) ) {
		ucg->setColor(  COLOR_WHITE  );
		ucg->setPrintPos(x,y);
		ucg->setFont(ucg_font_fub25_hr);
		if( UNITALT == 0 ) { //m
			ucg->printf("%-4d %s   ", alt, Units::AltitudeUnit() );
		}
		if( UNITALT == 1 ){ //feet
			ucg->printf("%-5d %s   ", (alt/10)*10, Units::AltitudeUnit() );
		}
		if( UNITALT == 2 ){ //FL
			ucg->printf("%s %-4d   ", Units::AltitudeUnit(), alt  );
		}
		prefalt = alt;
	}
}

int max_gscale = 0;

void IpsDisplay::initLoadDisplay(){
	if( _menu )
		return;
	ESP_LOGI(FNAME,"initLoadDisplay()");
	ucg->setColor(  COLOR_WHITE  );
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(40,15);
	ucg->print( "G-Force" );
	ucg->setPrintPos(130,70);
	ucg->setColor(  COLOR_HEADER_LIGHT  );
	ucg->print( "MAX POS" );
	ucg->setPrintPos(130,210);
	ucg->print( "MAX NEG" );
	max_gscale = (int)( gload_pos_limit.get() )+1;
	if( -gload_neg_limit.get() >= max_gscale )
		max_gscale = (int)( -gload_neg_limit.get()  )+1;
	drawScaleLines( true, max_gscale, -max_gscale );

	drawAnalogScale(-max_gscale,150,max_gscale, 1 );
	drawAnalogScale(max_gscale,150,max_gscale, 1 );
	// drawAnalogScale(0, 132);
	if((max_gscale%2) == 0) {
		drawAnalogScale(max_gscale/2,150,max_gscale, 1);
		drawAnalogScale(-max_gscale/2,155,max_gscale, 1);
	}
	else{
		drawAnalogScale((max_gscale-1)/2,150,max_gscale, 1);
		drawAnalogScale((-max_gscale+1)/2,155,max_gscale, 1);
	}
	for( float a=gload_pos_limit.get()-1; a<max_gscale; a+=0.05 ) {
		drawTetragon( ((float)a/max_gscale)*M_PI_2, AMIDX, AMIDY, 120, 130, 2, COLOR_RED, false );
	}
	for( float a=gload_neg_limit.get()-1; a>(-max_gscale); a-=0.05 ) {
		drawTetragon( ((float)a/max_gscale)*M_PI_2, AMIDX, AMIDY, 120, 130, 2, COLOR_RED, false );
	}

	ESP_LOGI(FNAME,"initLoadDisplay end");
}

float old_gmax = 0;
float old_gmin = 0;

void IpsDisplay::drawLoadDisplay( float loadFactor ){
	// ESP_LOGI(FNAME,"drawLoadDisplay %1.1f", loadFactor );
	if( _menu )
		return;
	tick++;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	if( !(screens_init & INIT_DISPLAY_GLOAD) ){
		initLoadDisplay();
		screens_init |= INIT_DISPLAY_GLOAD;
	}
	// draw G pointer
	float a = (loadFactor-1)/max_gscale * (M_PI_2);
	if( int(a*100) != int(old_a*100) ) {
		drawTetragon( a, AMIDX, AMIDY, 60, 120, 3, COLOR_WHITE );
		// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );
	}
	// G load digital
	if( (int)(loadFactor*30) != _ate && !(tick%3) ) {
		drawAvgVario( 90, AMIDY+2, loadFactor );
		_ate = (int)(loadFactor*30);
	}
	// Min/Max values
	if( old_gmax != gload_pos_max.get() ){
		ucg->setFont(ucg_font_fub20_hr);
		ucg->setPrintPos(120,105);
		ucg->printf("%+1.2f", gload_pos_max.get() );
	}
	if( old_gmin != gload_neg_max.get() ){
		ucg->setFont(ucg_font_fub20_hr);
		ucg->setPrintPos(115,245);
		ucg->printf("%+1.2f", gload_neg_max.get() );
	}

	xSemaphoreGive(spiMutex);
}


// Compass or Wind Display
void IpsDisplay::drawCompass(){
	if( _menu )
		return;
	// ESP_LOGI(FNAME, "drawCompass: %d ", wind_display.get() );
	if( (wind_display.get() & WD_DIGITS) || (wind_display.get() & WD_ARROW) ){
		int winddir=0;
		float wind=0;
		bool ok=false;
		int ageStraight, ageCircling;
		char type = '/';
		if( wind_enable.get() == WA_STRAIGHT ){  // check what kind of wind is available from calculator
			ok = theWind.getWind( &winddir, &wind, &ageStraight );
			type = '|';
		}
		else if( wind_enable.get() == WA_CIRCLING ){
			ok = CircleWind::getWind( &winddir, &wind, &ageCircling );
		}
		else if( wind_enable.get() == WA_BOTH ){  // dynamically change type depending on younger calculation
			int wds, wdc;
			float ws, wc;
			bool oks, okc;
			oks = theWind.getWind( &wds, &ws, &ageStraight );
			okc = CircleWind::getWind( &wdc, &wc, &ageCircling);
			if( oks && ageStraight <= ageCircling ){
				wind = ws;
				winddir = wds;
				type = '|';
				ok = true;
			}
			else if( okc && ageCircling <= ageStraight )
			{
				wind = wc;
				winddir = wdc;
				type = '/';
				ok = true;
			}
			// ESP_LOGI(FNAME, "SWIND dir=%d, SSPEED=%f ageC=%d ageS=%d okc:=%d oks=%d ok:=%d", wds, ws, ageCircling, ageStraight, okc, oks, ok  );
		}
		// ESP_LOGI(FNAME, "WIND dir %d, speed %f, ok=%d", winddir, wind, ok );
		if( prev_heading != winddir || !(tick%16) ){
			ucg->setPrintPos(85,104);
			ucg->setColor(  COLOR_WHITE  );
			// ucg->setFont(ucg_font_fub20_hr);
			ucg->setFont(ucg_font_fub17_hf);
			char s[12];
			int windspeed = (int)( Units::Airspeed(wind)+0.5 );
			if( wind_display.get() & WD_DIGITS ){
				if( ok )
					sprintf(s,"%3d\xb0%c%2d", winddir, type, windspeed );
				else
					sprintf(s,"%s", "    --/--" );
				if( windspeed < 10 )
					ucg->printf("%s    ", s);
				else if( windspeed < 100 )
					ucg->printf("%s   ", s);
				else
					ucg->printf("%s  ", s);
			}
			prev_heading = winddir;
			if( wind_display.get() & WD_ARROW  ){
				float dir=winddir;  // absolute wind related to geographic north
				if( (wind_reference.get() & WR_HEADING) )  // wind relative to airplane, first choice compass, second is GPS true course
				{
					float heading = mag_hdt.get();
					if( (heading < 0) && Flarm::gpsStatus() )            // fall back to GPS course
						heading = Flarm::getGndCourse();
					dir = Vector::angleDiffDeg( winddir, heading );
				}
				else if( (wind_reference.get() & WR_GPS_COURSE) ){
					if( Flarm::gpsStatus() ){
						float heading = Flarm::getGndCourse();
						dir = Vector::angleDiffDeg( winddir, heading );
					}
				}
				drawWindArrow( dir, windspeed, 0 );
			}
		}
	}
	else if( wind_display.get() & WD_COMPASS ){
		int heading = static_cast<int>(rintf(mag_hdt.get()));
		if( heading >= 360 )
			heading -= 360;
		// ESP_LOGI(FNAME, "heading %d, valid %d", heading, Compass::headingValid() );
		if( prev_heading != heading || !(tick%16) ){
			ucg->setPrintPos(105,104);
			ucg->setColor(  COLOR_WHITE  );
			ucg->setFont(ucg_font_fub20_hr);
			char s[12];
			if( heading < 0 )
				sprintf(s,"%s", "  ---" );
			else
				sprintf(s,"%3d", heading );

			if( heading < 10 )
				ucg->printf("%s    ", s);
			else if( heading < 100 )
				ucg->printf("%s   ", s);
			else
				ucg->printf("%s  ", s);
			ucg->setFont(ucg_font_fub20_hf);
			ucg->setPrintPos(120+ucg->getStrWidth(s),105);
			ucg->printf("\xb0 ");
			prev_heading = heading;
		}
	}
}
// Compass or Wind Display for ULStyle
void IpsDisplay::drawULCompass(){
	if( _menu )
		return;
	// ESP_LOGI(FNAME, "drawULCompass: %d ", wind_display.get() );
	if( (wind_display.get() & WD_DIGITS) || (wind_display.get() & WD_ARROW) ){
		int winddir=0;
		float wind=0;
		bool ok=false;
		int ageStraight, ageCircling;
		char type = '/';
		if( wind_enable.get() == WA_STRAIGHT ){  // check what kind of wind is available from calculator
			ok = theWind.getWind( &winddir, &wind, &ageStraight );
			type = '|';
		}
		else if( wind_enable.get() == WA_CIRCLING ){
			ok = CircleWind::getWind( &winddir, &wind, &ageCircling );
		}
		else if( wind_enable.get() == WA_BOTH ){  // dynamically change type depening on younger calculation
			int wds, wdc;
			float ws, wc;
			bool oks, okc;
			oks = theWind.getWind( &wds, &ws, &ageStraight );
			okc = CircleWind::getWind( &wdc, &wc, &ageCircling);
			if( oks && ageStraight < ageCircling ){
				wind = ws;
				winddir = wds;
				type = '|';
				ok = true;
			}
			else if( okc && ageCircling < ageStraight )
			{
				wind = wc;
				winddir = wdc;
				type = '/';
				ok = true;
			}
		}
		// ESP_LOGI(FNAME, "WIND dir %d, speed %f, ok=%d", winddir, wind, ok );
		if( prev_heading != winddir || !(tick%16) ){
			ucg->setPrintPos(85,104);
			ucg->setColor(  COLOR_WHITE  );
			// ucg->setFont(ucg_font_fub20_hr);
			ucg->setFont(ucg_font_fub17_hf);
			char s[12];
			int windspeed = (int)( Units::Airspeed(wind)+0.5 );
			if( wind_display.get() & WD_DIGITS ){
				if( ok )
					sprintf(s,"%3d\xb0%c%2d", winddir, type, windspeed );
				else
					sprintf(s,"%s", "    --/--" );
				if( windspeed < 10 )
					ucg->printf("%s    ", s);
				else if( windspeed < 100 )
					ucg->printf("%s   ", s);
				else
					ucg->printf("%s  ", s);
			}
			prev_heading = winddir;
			if( wind_display.get() & WD_ARROW  ){
				float dir=winddir;  // absolute wind related to geographic north
				if( (wind_reference.get() & WR_HEADING) )  // wind relative to airplane, first choice compass, second is GPS true course
				{
					float heading = mag_hdt.get();
					if( (heading < 0) && Flarm::gpsStatus() )            // fall back to GPS course
						heading = Flarm::getGndCourse();
					dir = Vector::angleDiffDeg( winddir, heading );
				}
				else if( (wind_reference.get() & WR_GPS_COURSE) ){
					if( Flarm::gpsStatus() ){
						float heading = Flarm::getGndCourse();
						dir = Vector::angleDiffDeg( winddir, heading );
					}
				}
				drawWindArrow( dir, windspeed, 0 );
			}
		}
	}
	//	else if( wind_display.get() & WD_COMPASS ){
	//	allways draw compass if possible and enabled
	if( compass_enable.get() && compass_calibrated.get() ){
		int heading = static_cast<int>(rintf(mag_hdt.get()));
		if( heading >= 360 )
			heading -= 360;
		// ESP_LOGI(FNAME, "heading %d, valid %d", heading, Compass::headingValid() );
		if( prev_heading != heading || !(tick%16) ){
			ucg->setPrintPos(113,220);
			ucg->setColor(  COLOR_WHITE  );
			ucg->setFont(ucg_font_fub20_hf);
			char s[14];
			if( heading > 0 )
				sprintf(s,"%3d\xb0", heading );
			else
				sprintf(s,"%s", "  ---" );

			if( heading < 10 )
				ucg->printf("%s    ", s);
			else if( heading < 100 )
				ucg->printf("%s   ", s);
			else
				ucg->printf("%s  ", s);

			prev_heading = heading;
		}
	}
}


void IpsDisplay::drawRetroDisplay( int airspeed_kmh, float te_ms, float ate_ms, float polar_sink_ms, float altitude_m,
		float temp, float volt, float s2fd_ms, float s2f_ms, float acl_ms, bool s2fmode, bool standard_setting, float wksensor ){
	if( _menu )
		return;
	if( !(screens_init & INIT_DISPLAY_RETRO) ){
		initDisplay();
		screens_init |= INIT_DISPLAY_RETRO;
	}
	tick++;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	// ESP_LOGI(FNAME,"drawRetroDisplay  TE=%0.1f IAS:%d km/h  WK=%d", te, airspeed, wksensor  );

	bool netto=false;
	if( vario_mode.get() == VARIO_NETTO || (s2fmode && ( vario_mode.get() == CRUISE_NETTO )) ){
		if( netto_mode.get() == NETTO_NORMAL ){
			te_ms = te_ms - polar_sink_ms;
			ate_ms = ate_ms - polar_sink_ms;
		}
		else if( netto_mode.get() == NETTO_RELATIVE ){  // Super Netto, considering circling sink
			te_ms = te_ms - polar_sink_ms + Speed2Fly.circlingSink( airspeed_kmh );
			ate_ms = ate_ms - polar_sink_ms + Speed2Fly.circlingSink( airspeed_kmh );
		}
		netto=true;
	}
	if( !(tick%20) ){
		if( netto != netto_old ){
			ucg->setFont(ucg_font_fub11_hr);
			ucg->setPrintPos(40,15);
			if( netto )
				ucg->setColor( COLOR_WHITE );
			else
				ucg->setColor( COLOR_BLACK );
			if( netto_mode.get() == NETTO_NORMAL )
				ucg->print( "  net" );
			else
				ucg->print( "s-net" );
			netto_old = netto;
		}
	}

	// Unit adaption for mph and knots
	float te = Units::Vario( te_ms );
	float ate = Units::Vario( ate_ms );
	float acl = Units::Vario( acl_ms );
	if( te > _range )
		te = _range;
	if( te < -_range )
		te = -_range;
	float polar_sink = Units::Vario( polar_sink_ms );
	//  float s2f = Units::Airspeed( s2f_ms );   not used for now
	float s2fd = Units::Airspeed( s2fd_ms );
	int airspeed =  (int)(Units::Airspeed( airspeed_kmh ) + 0.5);
	float altitude = Units::Altitude( altitude_m );

	// draw TE pointer
	float a = (te)/(_range) * (M_PI_2);
	if( int(a*100) != int(old_a*100) ) {
		drawTetragon( a, AMIDX, AMIDY, 60, 120, 3, COLOR_WHITE );
		// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );
		// Climb bar

	}
	// draw green bar
	if( !(tick%5) ){
		if( (int)(te*10) != (int)(te_prev*10) ) {
			float step= (M_PI_2/150) * _range;
			if( te > te_prev && te > 0 ){  // draw green what's missing
				for( float a=te_prev; a<te && a<_range; a+=step ) {
					if( a >= step*2 ) // don't overwrite the '0'
						drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 125, 2, COLOR_GREEN, false );
				}
			}
			else{   // delete what's too much
				ESP_LOGD(FNAME,"delete te:%0.2f prev:%0.2f", te, te_prev );
				for( float a=te_prev+step; a>=te && a >= step; a-=step ) {
					ESP_LOGD(FNAME,"delete %0.2f", a );
					drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 119, 126, 2, COLOR_BLACK, false );
				}
			}
			te_prev = te+step;
		}
	}
	// Polar Sink or Netto Sink
	if( !(tick%5) ){
		if( netto || ps_display.get() ){
			float val = polar_sink;
			if( netto )
				val = te;
			if( (int)(val*10) != (int)(polar_sink_prev*10) ) {  // tbd: rename polar_sink_prev
				float step= (M_PI_2/150) * _range;
				if( val < polar_sink_prev && val < 0 ){  // draw what's missing
					for( float a=polar_sink_prev; a>val && a>-_range; a-=step ) {
						ESP_LOGD(FNAME,"blue a=%f",a);
						if( a <= -step*2 ){ // don't overwrite the '0'
							if( netto )
								drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 125, 2, COLOR_RED, false );
							else
								drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 125, 2, COLOR_BLUE, false );
						}
					}
				}
				else{   // delete what's too much
					for( float a=polar_sink_prev-step; a<=val && a <= -step; a+=step ) {
						ESP_LOGD(FNAME,"black a=%f",a);
						drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 119, 126, 2, COLOR_BLACK, false );
					}
				}
				polar_sink_prev = val + step;
			}
		}
	}
	// vTaskDelay(3);
	// average Climb
	if( (int)(ate*30) != _ate && !(tick%3) ) {
		drawAvgVario( 90, AMIDY+2, ate );
		_ate = (int)(ate*30);
	}
	// MC val
	if(  !(tick%8) ) {
		int aMC = MC.get() * 10;
		if( aMC != mcalt && !(tick%4) ) {
			drawMC( MC.get(), true );
			mcalt=aMC;
		}
	}
	// Bluetooth
	if( !(tick%12) )
	{
		if( wireless == WL_BLUETOOTH )
			drawBT();
		if( wireless == WL_WLAN ||  wireless == WL_WLAN_CLIENT  ||  wireless == WL_WLAN_MASTER )
			drawWifi(DISPLAY_W-27, FLOGO+2 );
	}

	// S2F Command triangle
	if( ((int)s2fd != s2fdalt && !((tick+1)%2) ) || !(tick+3%30) ) {
		// ESP_LOGI(FNAME,"S2F in");
		int start=120;
		int width=50;
		int maxs2f=55;
		if( compass_enable.get() && compass_enable.get() )
			maxs2f=35;
		ucg->setClipRange( start, dmid-maxs2f-25, width, (maxs2f*2)+1+25 );
		bool clear = false;
		int dmo = dmid+25;
		if( s2fd > 0 ) {
			if ( (int)s2fd < s2fdalt || (int)s2fdalt < 0 ){
				clear = true;
			}
		}
		else {
			if ( (int)s2fd > s2fdalt || (int)s2fdalt > 0  ) {
				clear = true;
			}
		}
		if( int(s2fd) < 0  && (int)s2fdalt < 0 )
			dmo = dmid-25;

		if( dmo < dmid )
			ucg->setClipRange( start, dmid-25-maxs2f, width, (maxs2f)+1 );
		else
			ucg->setClipRange( start, dmid+25, width, (maxs2f)+1 );
		// clear old triangle for S2F
		if( clear ) {
			ucg->setColor( COLOR_BLACK );
			ucg->drawTriangle(  start, dmo,
					start+(width/2), dmo+(int)s2fd,
					start+(width/2), dmo+(int)s2fdalt );
			ucg->drawTriangle( 	start+width, dmo,
					start+(width/2), dmo+(int)s2fd,
					start+(width/2), dmo+(int)s2fdalt );
		}
		// draw new S2F command triangle
		if( s2fd < 0 )
			ucg->setColor( LIGHT_GREEN );
		else
			ucg->setColor( COLOR_RED );
		// ESP_LOGI(FNAME,"S2F %d-%d %d-%d %d-%d", start, dmid, start+width, dmid, start+(width/2), dmid+(int)s2fd );
		ucg->drawTriangle(  start, dmo,
				start+width, dmo,
				start+(width/2), dmo+(int)s2fd );

		ucg->undoClipRange();
		if( s2fd > 0 && s2fdalt < 0 ){
			ucg->setColor( COLOR_BLACK );
			ucg->drawBox( start, dmid-25-maxs2f, width, (maxs2f)+1 );
		}
		else if( s2fd < 0 && s2fdalt > 0 ){
			ucg->setColor( COLOR_BLACK );
			ucg->drawBox( start, dmid+25, width, (maxs2f)+1 );
		}
		// every 10 km/h one line
		if( s2fd > 0 ){
			ucg->setColor( COLOR_BLACK );
			for( int i=0; i<s2fd && i<maxs2f; i+=10 ) {
				ucg->drawHLine( start, dmid+25+i, width );
				ucg->drawHLine( start, dmid+25+i+1, width );
			}
		}else{
			ucg->setColor( COLOR_BLACK );
			for( int i=0; i>s2fd && i>-maxs2f; i-=10 ) {
				ucg->drawHLine( start, dmid-25+i, width );
				ucg->drawHLine( start, dmid-25+i-1, width );
			}
		}
		s2fdalt=(int)s2fd;
	}

	// Altitude
	if(!(tick%8) ) {
		drawAltitude( altitude, 110,282 );
	}

	// Battery
	int chargev = (int)( volt *10 );
	if( volt < bat_red_volt.get() ){
		if( !(tick%40) )
			blank = true;
		else if( !((tick+20)%40) )
			blank = false;
	}
	else
		blank = false;
	if ( chargealt != chargev || blank != blankold  ) {
		drawBat( volt, BATX, BATY, blank );
		chargealt = chargev;
		blankold = blank;
	}

	// Temperature Value
	if( (int)(temp*10) != tempalt && !(tick%12)) {
		drawTemperature( 20, 38, temp );
		tempalt=(int)(temp*10);
	}

	// WK-Indicator
	if( flap_enable.get() && !(tick%7) )
	{
		float wkspeed = airspeed * sqrt( 100.0/( ballast.get() +100.0) );
		int wki;
		float wkopt = Flap::getOptimum( wkspeed, wki );
		int wk = (int)((wki - wkopt + 0.5)*10);
		// ESP_LOGI(FNAME,"as:%d wksp:%f wki:%d wk:%d wkpos:%f", airspeed, wkspeed, wki, wk, wkpos );
        // ESP_LOGI(FNAME,"WK changed WKE=%d WKS=%f", wk, wksensor );
        ucg->setColor(  COLOR_WHITE  );
        Flap::drawBigBar( WKBARMID, WKSYMST-4, (float)(wk)/10, wksensor );
        wkoptalt = wk;
        wksensoralt = (int)(wksensor*10);

		if( wki != wkialt ) {
			Flap::drawWingSymbol( WKBARMID-(27*(abs(flap_neg_max.get())+1) ), WKSYMST-3, wki, wkialt ,wksensor);
			wkialt=wki;
		}
	}

	// Cruise mode or circling
	if( (int)s2fmode != s2fmode_prev ){
		drawS2FMode( 180, 20, s2fmode );
		s2fmode_prev = (int)s2fmode;
	}

	// Medium Climb Indicator
	// ESP_LOGI(FNAME,"acl:%f iacl:%d, nt:%d", acl, average_climb, !(tick%16) );
	if ( average_climb != (int)(acl*10) && !(tick%16) && acl > 0 ){
		drawAvg( acl, acl-average_climbf );
		average_climb = (int)(acl*10);
		average_climbf = acl;
	}
	// Airspeed
	if( !(tick%7) ){
		if( as_prev != airspeed || !(tick%70) ) {
			// ESP_LOGI(FNAME,"draw airspeed %d %d", airspeed, as_prev );
			ucg->setColor(  COLOR_WHITE  );
			ucg->setPrintPos(113,73);
			ucg->setFont(ucg_font_fub20_hr);
			char s[10];
			sprintf(s,"%3d",  airspeed );
			int fl=ucg->getStrWidth(s);
			ucg->printf("%s  ", s);
			ucg->setPrintPos(113+fl,70);
			ucg->setFont(ucg_font_fub11_hr);
			ucg->printf(" %s  ", Units::AirspeedUnit() );
			as_prev = airspeed;
		}
	}
	// Compass
	if( !(tick%8) ){
		drawCompass();
	}
	xSemaphoreGive(spiMutex);
}

void IpsDisplay::drawULDisplay( int airspeed_kmh, float te_ms, float ate_ms, float polar_sink_ms, float altitude_m,
		float temp, float volt, float s2fd_ms, float s2f_ms, float acl_ms, bool s2fmode, bool standard_setting, float wksensor ){
	if( _menu )
		return;
	if( !(screens_init & INIT_DISPLAY_UL) ){
		initDisplay();
		screens_init |= INIT_DISPLAY_UL;
	}
	tick++;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	// ESP_LOGI(FNAME,"drawULDisplay  TE=%0.1f IAS:%d km/h  WK=%d", te, airspeed, wksensor  );

	bool netto=false;
	if( vario_mode.get() == VARIO_NETTO || (s2fmode && ( vario_mode.get() == CRUISE_NETTO )) ){
		if( netto_mode.get() == NETTO_NORMAL ){
			te_ms = te_ms - polar_sink_ms;
			ate_ms = ate_ms - polar_sink_ms;
		}
		else if( netto_mode.get() == NETTO_RELATIVE ){  // Super Netto, considering circling sink
			te_ms = te_ms - polar_sink_ms + Speed2Fly.circlingSink( airspeed_kmh );
			ate_ms = ate_ms - polar_sink_ms + Speed2Fly.circlingSink( airspeed_kmh );
		}
		netto=true;
	}
	if( !(tick%20) ){
		if( netto != netto_old ){
			ucg->setFont(ucg_font_fub11_hr);
			ucg->setPrintPos(40,15);
			if( netto )
				ucg->setColor( COLOR_WHITE );
			else
				ucg->setColor( COLOR_BLACK );
			if( netto_mode.get() == NETTO_NORMAL )
				ucg->print( "  net" );
			else
				ucg->print( "s-net" );
			netto_old = netto;
		}
	}

	// Unit adaption for mph and knots
	float te = Units::Vario( te_ms );
	float ate = Units::Vario( ate_ms );
	float acl = Units::Vario( acl_ms );
	if( te > _range )
		te = _range;
	if( te < -_range )
		te = -_range;
	float polar_sink = Units::Vario( polar_sink_ms );
	int airspeed =  (int)(Units::Airspeed( airspeed_kmh ) + 0.5);
	float altitude = Units::Altitude( altitude_m );

	// draw TE pointer
	float a = (te)/(_range) * (M_PI_2);
	if( int(a*100) != int(old_a*100) ) {
		drawTetragon( a, AMIDX, AMIDY, 60, 120, 3, COLOR_RED );
		// ESP_LOGI(FNAME,"IpsDisplay::drawULDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );
		// Climb bar

	}
	// draw green bar
	if( !(tick%3) ){
		if( (int)(te*10) != (int)(te_prev*10) ) {
			float step= (M_PI_2/150) * _range;
			if( te > te_prev && te > 0 ){  // draw green what's missing
				for( float a=te_prev; a<te && a<_range; a+=step ) {
					if( a >= step*2 ) // don't overwrite the '0'
						drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 125, 2, COLOR_GREEN, false );
				}
			}
			else{   // delete what's too much
				ESP_LOGD(FNAME,"delete te:%0.2f prev:%0.2f", te, te_prev );
				for( float a=te_prev+step; a>=te && a >= step; a-=step ) {
					ESP_LOGD(FNAME,"delete %0.2f", a );
					drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 119, 126, 2, COLOR_BLACK, false );
				}
			}
			te_prev = te+step;
		}
	}
	// Polar Sink or Netto Sink
	if( !(tick%5) ){
		if( netto || ps_display.get() ){
			float val = polar_sink;
			if( netto )
				val = te;
			if( (int)(val*10) != (int)(polar_sink_prev*10) ) {  // tbd: rename polar_sink_prev
				float step= (M_PI_2/150) * _range;
				if( val < polar_sink_prev && val < 0 ){  // draw what's missing
					for( float a=polar_sink_prev; a>val && a>-_range; a-=step ) {
						ESP_LOGD(FNAME,"blue a=%f",a);
						if( a <= -step*2 ){ // don't overwrite the '0'
							if( netto )
								drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 125, 2, COLOR_RED, false );
							else
								drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 125, 2, COLOR_BLUE, false );
						}
					}
				}
				else{   // delete what's too much
					for( float a=polar_sink_prev-step; a<=val && a <= -step; a+=step ) {
						ESP_LOGD(FNAME,"black a=%f",a);
						drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 119, 126, 2, COLOR_BLACK, false );
					}
				}
				polar_sink_prev = val + step;
			}
		}
	}
	// average Climb
	if( (int)(ate*30) != _ate && !(tick%3) ) {
		drawAvgVario( 90, AMIDY+2, ate );
		_ate = (int)(ate*30);
	}
	// Bluetooth
	if( !(tick%12) )
	{
		if( wireless == WL_BLUETOOTH )
			drawBT();
		if( wireless == WL_WLAN ||  wireless == WL_WLAN_CLIENT )
			drawWifi(DISPLAY_W-27, FLOGO+2 );
	}

	// Altitude Header
	if( !(tick%24) ){
		int qnh = (int)(QNH.get() +0.5 );
		// ESP_LOGI(FNAME,"standard_setting:%d",standard_setting );
		if( standard_setting )
			qnh = 1013;
		// redraw just in case the vario pointer was there
		ucg->setFont(ucg_font_fub11_tr);
		ucg->setPrintPos(FIELD_START,YALT-S2FFONTH-10);
		char unit[4];
		if( standard_setting )
			sprintf( unit, "QNE" );
		else
			sprintf( unit, "QNH" );
		ucg->setPrintPos(FIELD_START,(YALT-S2FFONTH-10));
		ucg->setColor(0, COLOR_HEADER );
		ucg->printf("Altitude %s %d ", unit, qnh );
		pref_qnh = qnh;
	}

	// Altitude
	if(!(tick%8) ) {
		drawAltitude( altitude, 113,YALT-4 );
	}

	// Battery
	int chargev = (int)( volt *10 );
	if( volt < bat_red_volt.get() ){
		if( !(tick%40) )
			blank = true;
		else if( !((tick+20)%40) )
			blank = false;
	}
	else
		blank = false;
	if ( chargealt != chargev || blank != blankold  ) {
		drawBat( volt, BATX, BATY, blank );
		chargealt = chargev;
		blankold = blank;
	}

	// Temperature Value
	if( (int)(temp*10) != tempalt && !(tick%12)) {
		drawTemperature( 20, 38, temp );
		tempalt=(int)(temp*10);
	}

	// WK-Indicator
	if( flap_enable.get() && !(tick%7) )
	{
		float wkspeed = airspeed * sqrt( 100.0/( ballast.get() +100.0) );
		int wki;
		float wkopt=Flap::getOptimum( wkspeed, wki );
		int wk = (int)((wki - wkopt + 0.5)*10);
		// ESP_LOGI(FNAME,"as:%d wksp:%f wki:%d wk:%d", airspeed, wkspeed, wki, wk  );
        // ESP_LOGI(FNAME,"WK changed WKE=%d WKS=%f", wk, wksensor );
        ucg->setColor(  COLOR_WHITE  );
        Flap::drawBigBar( WKBARMID, WKSYMST-4, (float)(wk)/10, wksensor);
        wkoptalt = wk;
        wksensoralt = (int)(wksensor*10);

		if( wki != wkialt ) {
			Flap::drawWingSymbol( WKBARMID-(27*(abs(flap_neg_max.get())+1) ), WKSYMST-3, wki, wkialt ,wksensor);
			wkialt=wki;
		}
	}

	// Medium Climb Indicator
	// ESP_LOGI(FNAME,"acl:%f iacl:%d, nt:%d", acl, average_climb, !(tick%16) );
	if ( average_climb != (int)(acl*10) && !(tick%16) && acl > 0 ){
		drawAvg( acl, acl-average_climbf );
		average_climb = (int)(acl*10);
		average_climbf = acl;
	}
	// Airspeed
	if( !(tick%7) ){
		if( as_prev != airspeed || !(tick%49) ) {
			ucg->setColor(  COLOR_WHITE  );
			ucg->setPrintPos(113,73);
			ucg->setFont(ucg_font_fub25_hr);
			char s[10];
			sprintf(s,"%3d",  airspeed );
			int fl=ucg->getStrWidth(s);
			ucg->printf("%s  ", s);
			ucg->setPrintPos(113+fl,70);
			ucg->setFont(ucg_font_fub20_hr);
			ucg->printf(" %s  ", Units::AirspeedUnit() );
			as_prev = airspeed;

		}
	}
	// Compass
	if( !(tick%8) ){
		drawULCompass();
	}
	xSemaphoreGive(spiMutex);
}


void IpsDisplay::drawDisplay( int airspeed, float te, float ate, float polar_sink, float altitude,
		float temp, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_setting, float wksensor ){
	if( _menu )
		return;

	if ( alt_display_mode.get() == MODE_QFE ) {
		altitude -= elevation.get();
	}
	float temp_unit = Units::TemperatureUnit( temp );

	if( display_style.get() == DISPLAY_AIRLINER )
		drawAirlinerDisplay( airspeed,te,ate, polar_sink, altitude, temp_unit, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor );
	else if( display_style.get() == DISPLAY_RETRO )
		drawRetroDisplay( airspeed,te,ate, polar_sink, altitude, temp_unit, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor );
	else if( display_style.get() == DISPLAY_UL )
		drawULDisplay( airspeed,te,ate, polar_sink, altitude, temp_unit, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor );

}

void IpsDisplay::drawAirlinerDisplay( int airspeed_kmh, float te_ms, float ate_ms, float polar_sink_ms, float altitude_m,
		float temp, float volt, float s2fd_ms, float s2f_ms, float acl_ms, bool s2fmode, bool standard_setting, float wksensor ){
	if( _menu )
		return;
	if( !(screens_init & INIT_DISPLAY_AIRLINER) ){
		initDisplay();
		screens_init |= INIT_DISPLAY_AIRLINER;
	}
	// ESP_LOGI(FNAME,"IpsDisplay::drawDisplay  TE=%0.1f", te);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	tick++;

	// S2F given im km/h: Unit adaption for mph and knots
	bool netto=false;
	if( vario_mode.get() == VARIO_NETTO || (s2fmode && ( vario_mode.get() == CRUISE_NETTO )) ){
		if( netto_mode.get() == NETTO_NORMAL ){
			te_ms = te_ms - polar_sink_ms;
			ate_ms = ate_ms - polar_sink_ms;
		}
		else if( netto_mode.get() == NETTO_RELATIVE ){  // Super Netto, considering circling sink
			te_ms = te_ms - polar_sink_ms + Speed2Fly.circlingSink( airspeed_kmh );
			ate_ms = ate_ms - polar_sink_ms + Speed2Fly.circlingSink( airspeed_kmh );
		}
		netto=true;
	}
	if( !(tick%20) ){
		if( netto != netto_old ){
			ucg->setFont(ucg_font_fub11_hr);
			ucg->setPrintPos(165,15);
			if( netto )
				ucg->setColor( COLOR_WHITE );
			else
				ucg->setColor( COLOR_BLACK );
			if( netto_mode.get() == NETTO_NORMAL )
				ucg->print( "  net" );
			else
				ucg->print( "s-net" );
			netto_old = netto;
		}
	}

	float te = Units::Vario( te_ms );
	float ate = Units::Vario( ate_ms );
	float acl = Units::Vario( acl_ms );
	if( te > _range )
		te = _range;
	if( te < -_range )
		te = -_range;
	float polar_sink = Units::Vario( polar_sink_ms );
	float s2f = Units::Airspeed( s2f_ms );
	float s2fd = Units::Airspeed( s2fd_ms );
	int airspeed =  (int)(Units::Airspeed( airspeed_kmh ) + 0.5);
	float altitude = Units::Altitude( altitude_m );

	vTaskDelay(3);
	if( _menu )
		return;

	// WK-Indicator
	if( flap_enable.get() && !(tick%7) )
	{
		float wkspeed = airspeed * sqrt( 100.0/( ballast.get() +100.0) );
		int wki;
		float wkopt=Flap::getOptimum( wkspeed, wki );
		int wk = (int)((wki - wkopt + 0.5)*10);
        // ESP_LOGI(FNAME,"ias:%d wksp:%f wki:%d wk:%d wkpos%f", airspeed, wkspeed, wki, wk, wkpos );
        ucg->setColor(  COLOR_WHITE  );
        Flap::drawSmallBar( YS2F-fh, WKSYMST+2, (float)(wk)/10 );
        wkoptalt = wk;

		if( wki != wkialt ) {
			Flap::drawWingSymbol( YS2F-fh-25, WKSYMST+2, wki, wkialt ,wksensor);
			wkialt=wki;
		}
	}



	ucg->setFont(ucg_font_fub35_hn);  // 52 height
	ucg->setColor(  COLOR_WHITE  );

	// Average Vario
	if( _ate != (int)(ate*10) && !(tick%3) ) {
		// draw numeric value
		drawAvgVario( FIELD_START, YVAR-20, ate );
		// draw Unit
		ucg->setFont(ucg_font_fub11_hr);
		int mslen = ucg->getStrWidth( Units::VarioUnit() );
		ucg->setPrintPos(DISPLAY_W-mslen,YVAR-10);
		ucg->print( Units::VarioUnit() );

		_ate = (int)(ate)*10;
	}

	// Altitude Header
	if( !(tick%24) ){
		int qnh = (int)(QNH.get() +0.5 );
		// ESP_LOGI(FNAME,"standard_setting:%d",standard_setting );
		if( standard_setting )
			qnh = 1013;
		if( qnh != pref_qnh ) {
			ucg->setFont(ucg_font_fub11_tr);
			ucg->setPrintPos(FIELD_START,YALT-S2FFONTH);
			char unit[4];
			if( standard_setting )
				sprintf( unit, "QNE" );
			else
				sprintf( unit, "QNH" );
			ucg->setColor(0, COLOR_BLACK );
			ucg->printf("Altitude %s %d ", unit, pref_qnh );
			ucg->setPrintPos(FIELD_START,(YALT-S2FFONTH));
			ucg->setColor(0, COLOR_HEADER );
			ucg->printf("Altitude %s %d ", unit, qnh );
			pref_qnh = qnh;
		}
	}

	// Altitude
	if(!(tick%8) ) {
		drawAltitude( altitude, FIELD_START,YALT+6 );
	}
	// MC Value
	if(  !(tick%8) ) {
		int aMC = MC.get() * 10;
		if( aMC != mcalt && !(tick%4) ) {
			drawMC( MC.get() );
			mcalt=aMC;
		}
	}
	vTaskDelay(3);
	if( _menu )
		return;
	// Temperature Value
	if( (int)(temp*10) != tempalt && !(tick%11)) {
		drawTemperature( FIELD_START+30, DISPLAY_H, temp );
		tempalt=(int)(temp*10);
	}
	// Battery Symbol

	int chargev = (int)( volt *10 );
	if( volt < bat_red_volt.get() ){
		if( !(tick%40) )
			blank = true;
		else if( !((tick+20)%40) )
			blank = false;
	}
	else
		blank = false;
	if ( chargealt != chargev || blank != blankold ) {
		drawBat( volt, BATX, BATY+3, blank );
		chargealt = chargev;
		blankold = blank;
	}

	// Bluetooth Symbol

	if( !(tick%12) ){
		if( wireless == WL_BLUETOOTH )
			drawBT();
		if( wireless == WL_WLAN || wireless == WL_WLAN_CLIENT )
			drawWifi(DISPLAY_W-25, FLOGO);
	}

	bool flarm=false;
	if( flarm ){
		ucg_int_t flx=DISPLAY_W-58;
		ucg_int_t fly=FLOGO-7;
		if( flarm )
			ucg->setColor(COLOR_RED);
		else
			ucg->setColor( COLOR_MGREY );
		ucg->setClipRange( flx, fly-FLOGO, FLOGO, FLOGO );
		ucg->drawTriangle( flx+1, fly, flx+1+(FLOGO/2), fly, flx+1+(FLOGO/4), fly-(FLOGO/2)+2 );
		ucg->setClipRange( flx+FLOGO/4+3, fly-FLOGO, FLOGO, FLOGO );
		ucg->drawCircle( flx, fly, FLOGO/2 + (FLOGO/4)-2, UCG_DRAW_UPPER_RIGHT);
		ucg->setClipRange( flx+FLOGO/4+5, fly-FLOGO, FLOGO, FLOGO );
		ucg->drawCircle( flx, fly, FLOGO/2 + (FLOGO/2)-2, UCG_DRAW_UPPER_RIGHT);
		ucg->drawCircle( flx, fly, FLOGO/2 + (FLOGO/2)-3, UCG_DRAW_UPPER_RIGHT);
		ucg->undoClipRange();
	}
	if( _menu )
		return;
	int s2fclip = s2fd;
	if( s2fclip > MAXS2FTRI )
		s2fclip = MAXS2FTRI;
	if( s2fclip < -MAXS2FTRI )
		s2fclip = -MAXS2FTRI;

	int ty = (int)(te*_pixpmd);

	int py = (int)(polar_sink*_pixpmd);
	// Gauge Triangle
	if( s2fmode !=  s2fmodealt ){
		drawGaugeTriangle( tyalt, COLOR_BLACK );
		drawGaugeTriangle( s2fclipalt, COLOR_BLACK, true );
		drawGaugeTriangle( ty, COLOR_BLACK );
		drawGaugeTriangle( s2fclip, COLOR_BLACK, true );
		s2fmodealt = s2fmode;
	}

	if ( average_climb !=  (int)(acl*10) && !(tick%10) && acl > 0 ){
		drawAvgSymbol(  (average_climb*_pixpmd)/10, COLOR_BLACK );
		drawLegend( true );
		average_climb = (int)(acl*10);
		drawAvgSymbol(  (average_climb*_pixpmd)/10, COLOR_RED );
	}

	// TE Stuff
	if( ty != tyalt || py != pyalt )
	{
		// setTeBuf(  dmid, _range*_pixpmd+1, COLOR_BLACK );
		// setTeBuf(  dmid, -(_range*_pixpmd+1), COLOR_BLACK );
		setTeBuf(  dmid, MAXTEBAR, COLOR_BLACK );
		setTeBuf(  dmid, -MAXTEBAR, COLOR_BLACK );

		if( ps_display.get() && !netto )
			setTeBuf(  dmid, py, COLOR_BLUE );
		if( ty > 0 ){
			setTeBuf(  dmid, ty, COLOR_GREEN );
			if( ps_display.get() && !netto )
				setTeBuf(  dmid, py, COLOR_GREEN );
		}
		else {
			if( ps_display.get() && !netto ) {
				if( ty > py ){
					setTeBuf(  dmid, ty, COLOR_BLUE );
					setTeBuf(  dmid+ty, py-ty, COLOR_GREEN );
				}
				else
				{
					setTeBuf(  dmid, py, COLOR_BLUE );
					setTeBuf(  dmid+py, ty-py, COLOR_RED );
				}
			}else
				setTeBuf(  dmid, ty, COLOR_RED );
		}
		drawTeBuf();

		// Small triangle pointing to actual vario value
		if( !s2fmode ){
			// First blank the old one
			drawGaugeTriangle( tyalt, COLOR_BLACK );
			drawGaugeTriangle( ty, COLOR_WHITE );
		}
		tyalt = ty;
		pyalt = py;
		vTaskDelay(3);
		if( _menu )
			return;

	}
	// AS
	if( as_prev != airspeed && !(tick%2)) {
		// draw new
		ucg->setColor(  COLOR_WHITE  );
		// print speed values bar
		ucg->setFont(ucg_font_fub11_hn);
		ucg->drawFrame( FIELD_START, dmid-(MAXS2FTRI)-4, ASLEN+6, (MAXS2FTRI*2)+8 );
		ucg->setClipRange( FIELD_START, dmid-(MAXS2FTRI), ASLEN+6, (MAXS2FTRI*2) );
		for( int speed = airspeed-MAXS2FTRI-(fh); speed<airspeed+MAXS2FTRI+(fh); speed++ )
		{
			if( (speed%20) == 0 && (speed >= 0) ) {
				// blank old values
				ucg->setColor( COLOR_BLACK );
				int col = 0;
				if( speed == 0 )
					ucg->drawBox( FIELD_START+6,dmid+(speed-airspeed)-(fh/2)-19, ASLEN-6, fh+25 );
				else
					ucg->drawBox( FIELD_START+6,dmid+(speed-airspeed)-(fh/2)-9, ASLEN-6, fh+15 );
				if ( display_variant.get() == DISPLAY_WHITE_ON_BLACK ) {
					col = abs(((speed-airspeed)*2));
				}
				else {
					col = abs(255 - abs(((speed-airspeed)*2)));
				}
				ucg->setColor(  col,col,col  );
				ucg->setPrintPos(FIELD_START+8,dmid+(speed-airspeed)+(fh/2));
				ucg->printf("%3d ""-", speed);
			}
		}
		ucg->undoClipRange();
		// AS cleartext
		ucg->setFont(ucg_font_fub14_hn);
		ucg->setPrintPos(FIELD_START+8, YS2F-fh-3 );
		ucg->setColor(  COLOR_WHITE  );
		ucg->printf("%3d ", airspeed );
		as_prev = airspeed;
	}
	// S2F command trend triangle
	if( _menu )
		return;
	if( ((int)s2fd != s2fdalt && !((tick+1)%2)) || !(tick%21) ) {
		// Arrow pointing there
		if( s2fmode ){
			// erase old
			drawGaugeTriangle( s2fclipalt, COLOR_BLACK, true );
			// Draw a new one at current position
			drawGaugeTriangle( s2fclip, COLOR_WHITE, true );
			ucg->setColor(  COLOR_WHITE  );
		}
		// S2F value
		ucg->setColor(  COLOR_WHITE  );
		ucg->setFont(ucg_font_fub14_hn);
		int fa=ucg->getFontAscent();
		int fl=ucg->getStrWidth("100");
		ucg->setPrintPos(ASVALX, YS2F-fh-3);
		ucg->printf("%3d  ", (int)(s2falt+0.5)  );
		// draw S2F Delta
		// erase old
		ucg->setColor(  COLOR_BLACK  );
		char s[10];
		sprintf(s,"%+3d  ",(int)(s2fdalt+0.5));
		fl=ucg->getStrWidth(s);
		ucg->setPrintPos( FIELD_START+S2FST+(S2F_TRISIZE/2)-fl/2-5,yposalt );
		ucg->printf(s);
		int ypos;
		if( s2fd < 0 )
			ypos = dmid+s2fclip-2;  // slower, up
		else
			ypos = dmid+s2fclip+12+fa;
		// new S2F Delta val
		if( abs(s2fd) > 10 ) {
			ucg->setColor(  COLOR_WHITE  );
			sprintf(s," %+3d  ",(int)(s2fd+0.5));
			fl=ucg->getStrWidth(s);
			ucg->setPrintPos( FIELD_START+S2FST+(S2F_TRISIZE/2)-fl/2,ypos );
			ucg->printf(s);
		}
		yposalt = ypos;
		ucg->setClipRange( FIELD_START+S2FST, dmid-MAXS2FTRI, S2F_TRISIZE, (MAXS2FTRI*2)+1 );
		bool clear = false;
		if( s2fd > 0 ) {
			if ( (int)s2fd < s2fdalt || (int)s2fdalt < 0 )
				clear = true;
		}
		else {
			if ( (int)s2fd > s2fdalt || (int)s2fdalt > 0  )
				clear = true;
		}
		// clear old triangle for S2F
		if( clear ) {
			ucg->setColor( COLOR_BLACK );
			ucg->drawTriangle( FIELD_START+S2FST, dmid,
					FIELD_START+S2FST+(S2F_TRISIZE/2), dmid+(int)s2fd,
					FIELD_START+S2FST+(S2F_TRISIZE/2), dmid+(int)s2fdalt );
			ucg->drawTriangle( FIELD_START+S2FST+S2F_TRISIZE, dmid,
					FIELD_START+S2FST+(S2F_TRISIZE/2), dmid+(int)s2fd,
					FIELD_START+S2FST+(S2F_TRISIZE/2), dmid+(int)s2fdalt );
		}
		// draw new S2F command triangle
		if( s2fd < 0 )
			ucg->setColor( LIGHT_GREEN );
		else
			ucg->setColor( COLOR_RED );
		ucg->drawTriangle( FIELD_START+S2FST, dmid,
				FIELD_START+S2FST+S2F_TRISIZE, dmid,
				FIELD_START+S2FST+(S2F_TRISIZE/2), dmid+(int)s2fd );

		ucg->undoClipRange();
		// green bar for optimum speed within tacho
		ucg->setClipRange( FIELD_START, dmid-(MAXS2FTRI), ASLEN+6, (MAXS2FTRI*2) );
		ucg->setColor( COLOR_BLACK );
		ucg->drawBox( FIELD_START+1,dmid+s2fdalt-16, 6, 32 );
		ucg->setColor( COLOR_GREEN );
		ucg->drawBox( FIELD_START+1,dmid+s2fd-15, 6, 30 );
		ucg->undoClipRange();
		s2fdalt = (int)s2fd;
		s2falt = (int)(s2f+0.5);
		s2fclipalt = s2fdalt;
		if( s2fclipalt > MAXS2FTRI )
			s2fclipalt = MAXS2FTRI;
		if( s2fclipalt < -MAXS2FTRI )
			s2fclipalt = -MAXS2FTRI;
	}
	ucg->setColor(  COLOR_WHITE  );
	ucg->drawHLine( DISPLAY_LEFT+6, dmid, bw );
	xSemaphoreGive(spiMutex);
}
