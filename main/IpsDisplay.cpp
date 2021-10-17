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
#include "canbus.h"

int screens_init = INIT_DISPLAY_NULL;

int   IpsDisplay::tick = 0;
bool  IpsDisplay::_menu = false;
int   IpsDisplay::_pixpmd = 10;
int   IpsDisplay::charge = 100;
int   IpsDisplay::red = 10;
int   IpsDisplay::yellow = 25;

float IpsDisplay::old_a = 0;

bool IpsDisplay::netto_old = false;
ucg_int_t IpsDisplay::char_width;

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
#define FIELD_START_UL 170
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

#define BATX (DISPLAY_W-10)
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
#define AMIDY DISPLAY_H/2
#define AMIDX (DISPLAY_W/2 + 30)
static int fh;

extern xSemaphoreHandle spiMutex; // todo needs a better concept here

#define PMLEN 24

ucg_color_t IpsDisplay::colors[TEMAX+1+TEGAP];
ucg_color_t IpsDisplay::colorsalt[TEMAX+1+TEGAP];


Ucglib_ILI9341_18x240x320_HWSPI *IpsDisplay::ucg = 0;

int IpsDisplay::_te=0;
int IpsDisplay::_ate=0;
int IpsDisplay::s2falt=-1;
int IpsDisplay::s2fdalt=0;
int IpsDisplay::s2fmode_prev=100;
int IpsDisplay::alt_prev=0;
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


// Flap definitions
#define WKSYMST DISPLAY_W-28
ucg_color_t IpsDisplay::wkcolor;
int IpsDisplay::wkoptalt;
int IpsDisplay::wksensoralt;

float IpsDisplay::_range_clip = 0;
int   IpsDisplay::_divisons = 5;
float IpsDisplay::_range = 5;
int   IpsDisplay::average_climb = -100;
float IpsDisplay::average_climbf = 0;
int   IpsDisplay::prev_heading = 0;
float IpsDisplay::pref_qnh = 0;

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
	ESP_LOGI(FNAME,"display clear()");
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
		g_col_header_r=179;
		g_col_header_g=171;
		g_col_header_b=164;
		g_col_header_light_r=94;
		g_col_header_light_g=87;
		g_col_header_light_b=0;
	}
	else {
		g_col_background = 0;
		g_col_highlight = 255;
		g_col_header_r=179;
		g_col_header_g=171;
		g_col_header_b=164;
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
		ucg->setColor( COLOR_HEADER );
        ucg->print(Units::VarioUnit());
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
		drawThermometer(  FIELD_START+10, DISPLAY_H-6 );

	}

	// Fancy altimeter
	ucg->setFont(ucg_font_fub20_hr);
	char_width = ucg->getStrWidth("2");

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

void IpsDisplay::drawAvg( float avclimb, float delta ){
	static float avc_old=-1000;
	static int yusize=7;
	static int ylsize=7;
	if( _menu )
		return;

	ESP_LOGD(FNAME,"drawAvg: av=%.2f delta=%.2f", avclimb, delta );
	int pos=145;
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
	alt_prev = -1;
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

	wkoptalt = -100;
	wksensoralt = -1;
	tyalt = -1000;
	polar_sink_prev = 0.1;
	if ( FLAP ) FLAP->redraw();
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
	ucg->setPrintPos(5, DISPLAY_H-5);
	ucg->setColor(COLOR_WHITE);
	if( large ) {
		ucg->setFont(ucg_font_fub20_hn);
    } else {
		ucg->setFont(ucg_font_fub14_hn);
    }
    char s[10];
	std::sprintf(s, "%1.1f", mc );
    ucg->printf(s);
    ucg_int_t fl = ucg->getStrWidth(s);
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setColor(COLOR_HEADER);
	ucg->setPrintPos(5+fl, DISPLAY_H-6);
	ucg->printf(" MC");
}

#define S2FSS 10
#define S2FTS 5

void IpsDisplay::drawCircling( int x, int y, bool draw ){
	if( _menu )
		return;
	if( draw )
		ucg->setColor( COLOR_HEADER );
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
		ucg->setColor( COLOR_HEADER );
	else
		ucg->setColor( COLOR_BLACK );
	ucg->drawTetragon(x-S2FSS,y-5, x-S2FSS,y-1, x+S2FSS,y+5, x+S2FSS,y+1 );
	ucg->drawTriangle( x+S2FSS-10, y+7, x+S2FSS-8, y-4, x+S2FSS, y+3 );
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

void IpsDisplay::drawArrow(int x, int y, int level, bool del)
{
	const int width=40;
	const int step=8;
	const int gap=2;
	int height=step;

    if ( level == 0 ) return;

	if( del ) {
		ucg->setColor( COLOR_BLACK );
	}
	else {
		if ( std::abs(level) == 4 ) {
			height=3;
			ucg->setColor( COLOR_ORANGE );
		}
		else {
			if( level < 0 ) {
				ucg->setColor( COLOR_GREEN );
			} else {
				ucg->setColor( COLOR_BLUE );
			}
		}
	}
	int l = level-1;
    if ( level < 0 ) {
        height = -height;
        l = level+1;
    }
	// ucg->drawTetragon(x,y+level*(step+gap), x+width,y+level*gap, x,y+level*(step+gap)+height, x-width, y+level*gap);
	ucg->drawTriangle(x,y+l*(step+gap), x,y+l*(step+gap)+height, x-width, y+l*gap);
	ucg->drawTriangle(x,y+l*(step+gap), x+width,y+l*gap, x,y+l*(step+gap)+height);
}

// speed to fly delta in kmh, s2fd > 0 means speed up
void IpsDisplay::drawS2FBar(int x, int y, int s2fd)
{
	static int level_old = 0;
	int level = s2fd/10;

	// draw max. three bars plus a yellow top
	if ( level > 4 ) { level = 4; }
	else if ( level < -4 ) { level = -4; }

	if ( level == level_old ) {
		return;
	}

	int inc = (level-level_old > 0) ? 1 : -1;
	int i = level_old + ((level_old==0 || level_old*inc>0) ? inc : 0);
    do {
		if ( i != 0 ) {
			drawArrow(x, y-2+(i>0?1:-1)*22, i, (i*inc < 0));
        	ESP_LOGI(FNAME,"s2fbar draw %d,%d", i, (i*inc < 0));
		}
		if ( i == level ) break;
		i+=inc;
	}
    while ( i != level );
    level_old = level;
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
	if( SetupCommon::isWired() ) {
		drawCable(DISPLAY_W-30, BTH + 27);
	}
}

void IpsDisplay::drawCable(int x, int y)
{
    CAN->connectedXCV() ? ucg->setColor( COLOR_LBLUE ) : ucg->setColor(COLOR_MGREY);
    ucg->setFont(ucg_font_fub11_hr);
    ucg->setPrintPos(x-8,y);
    if( CAN->connectedMagSens() ) {
        ucg->setColor( COLOR_GREEN );
    }
    ucg->printf("C");
    CAN->connectedMagSens() ? ucg->setColor( COLOR_LBLUE ) : ucg->setColor(COLOR_MGREY);
    if( Flarm::connected() ) {
        ucg->setColor( COLOR_GREEN );
    }
    ucg->printf("A");
    CAN->connectedXCV() ? ucg->setColor( COLOR_LBLUE ) : ucg->setColor(COLOR_MGREY);
    ucg->printf("N");
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
	if( _menu  )
		return;
	if( !SetupCommon::haveWLAN() )
		return;
	int btq=1;
	// ESP_LOGI(FNAME,"wireless %d", wireless );
	if( wireless == WL_WLAN_CLIENT ){
		if( WifiClient::isConnected(8884) )
			btq=0;
	}
	else if( wireless == WL_WLAN_STANDALONE || wireless == WL_WLAN_MASTER  )
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
	if( SetupCommon::isWired() ) {
		drawCable(x-6, y+22);
	}
}

void IpsDisplay::drawConnection( int x, int y )
{
	if( wireless == WL_BLUETOOTH )
		drawBT();
	else if( wireless != WL_DISABLE )
		drawWifi(x, y);
	else if( SetupCommon::isWired() )
		drawCable(x, y);
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
			ucg->setColor( COLOR_HEADER );
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
			ucg->setFont(ucg_font_fub11_hr);
			ucg->setPrintPos(x-42,y-6);
		}
		ucg->setColor( COLOR_HEADER );
		if( battery_display.get() == BAT_PERCENTAGE )
			ucg->printf("%3d%%  ", charge);
		else if ( battery_display.get() == BAT_VOLTAGE ) {
			ucg->setPrintPos(x-50,y-8);
			ucg->printf("%2.1f V", volt);
		}
		else if ( battery_display.get() == BAT_VOLTAGE_BIG ) {
			ucg->setPrintPos(x-50,y+11);
			ucg->setFont(ucg_font_fub14_hr);
			ucg->printf("%2.1fV", volt);
		}

	}
}

void IpsDisplay::drawTemperature( int x, int y, float t ) {
	if( _menu )
		return;
	ucg->setFont(ucg_font_fur14_hf);
	ucg->setPrintPos(x,y);
	if( t != DEVICE_DISCONNECTED_C ) {
		ucg->setColor( COLOR_WHITE );
		ucg->printf("%-2.1f", std::roundf(t*10.f)/10.f );
		ucg->setColor( COLOR_HEADER );
		ucg->printf("\xb0""C  ");
	}
	else {
		ucg->setColor( COLOR_HEADER );
		ucg->printf(" -- \xb0""C  ");
    }
}

// val, center x, y, start radius, end radius, width, r,g,b, clenaup
void IpsDisplay::drawPolarIndicator( float a, int x0, int y0, int l1, int l2, int w, int r, int g, int b, bool del )
{
	static ucg_int_t x_0 = 0;
	static ucg_int_t y_0 = 0;
	static ucg_int_t x_1 = 0;
	static ucg_int_t y_1 = 0;
	static ucg_int_t x_2 = 0;
	static ucg_int_t y_2 = 0;
	static ucg_int_t x_3 = 0;
	static ucg_int_t y_3 = 0;
	static ucg_int_t x_tip = 0;
	static ucg_int_t y_tip = 0;

	if( _menu ) return;

	ucg_int_t w0 = del?w+2:w;
	float si=sin(a);
	float co=cos(a);
	ucg_int_t xn_0 = x0-l1*co+w0*si;
	ucg_int_t yn_0 = y0-l1*si-w0*co;
	ucg_int_t xn_1 = x0-l1*co-w0*si;
	ucg_int_t yn_1 = y0-l1*si+w0*co;
	ucg_int_t xn_2 = x0-l2*co-w*si;
	ucg_int_t yn_2 = y0-l2*si+w*co;
	ucg_int_t xn_3 = x0-l2*co+w*si;
	ucg_int_t yn_3 = y0-l2*si-w*co;
	ucg_int_t x_ntip = x0-(l2+2)*co;
	ucg_int_t y_ntip = y0-(l2+2)*si;
	// ESP_LOGI(FNAME,"IpsDisplay::drawTetragon  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d x3:%d y3:%d", (int)xn_0, (int)yn_0, (int)xn_1 ,(int)yn_1, (int)xn_2, (int)yn_2, (int)xn_3 ,(int)yn_3 );
	if( del ) {
		// cleanup previous incarnation
		ucg->setColor(  COLOR_BLACK  );
		ucg->drawTetragon(x_0,y_0,x_1,y_1,x_2,y_2,x_3,y_3);
		ucg->drawDisc(x_tip, y_tip, 2*w, UCG_DRAW_ALL);
		x_0 = xn_0;
		y_0 = yn_0;
		x_1 = xn_1;
		y_1 = yn_1;
		x_2 = xn_2;
		y_2 = yn_2;
		x_3 = xn_3;
		y_3 = yn_3;
		x_tip = x_ntip;
		y_tip = y_ntip;
		old_a = a;
	}
	ucg->setColor( r,g,b  );
	ucg->drawTetragon(xn_0,yn_0,xn_1,yn_1,xn_2,yn_2,xn_3,yn_3);
	if ( del ) ucg->drawDisc(x_ntip, y_ntip, 2*w, UCG_DRAW_ALL);
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
		int end=150;
		int r = (int)max_pos;
		if( a==0 || abs(a)==r ){
			width=2;
			end=155;
		}
		if((r%2) == 0) {
			if(abs(a)==r/2){  // half scale big line
				width = 2;
				end=155;
			}
		}
		else{
			if( abs(a) == (r-1)/2 ){  // half scale minus one for even ranges big line
				width = 2;
				end=155;
			}
		}
		if( modulo < 1 ){
			if( fmod(a,1) == 0 ){  // every integer big line
				width = 2;
				end=155;
			}
		}
		drawPolarIndicator( ((float)a/max_pos)*M_PI_2, AMIDX, AMIDY, 140, end, width, COLOR_WHITE, false );
	}
}

// Draw scale numbers for positive or negative value
void IpsDisplay::annotateScale( int val, int pos, float range, int offset ){
	if( _menu )
		return;
	ucg->setFontPosCenter();
	ucg->setFont(ucg_font_fub14_hn);
	const float to_side = 1.07;
	int x=AMIDX - cos((val*to_side/range)*M_PI_2)*pos;
	int y=AMIDY+1 - sin((val*to_side/range)*M_PI_2)*pos;
	if( val > 0 )
		ucg->setPrintPos(x-17,y);
	else
		ucg->setPrintPos(x-10,y);
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
	annotateScale(-r,150, _range );
	annotateScale(r,150, _range);
	// drawAnalogScale(0, 132);
	if((r%2) == 0) {
		annotateScale(r/2,155, _range);
		annotateScale(-r/2,155, _range);
	}
	else{
		annotateScale((r-1)/2,155, _range);
		annotateScale((-r+1)/2,155, _range);
	}
	// Unit's
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(85,15);
	ucg->print( Units::VarioUnit() );
	drawConnection(DISPLAY_W-27, FLOGO+2 );
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
	annotateScale(-r,150, _range );
	annotateScale(r,150, _range);
	// drawAnalogScale(0, 132);
	if((r%2) == 0) {
		annotateScale(r/2,155, _range);
		annotateScale(-r/2,155, _range);
	}
	else{
		annotateScale((r-1)/2,155, _range);
		annotateScale((-r+1)/2,155, _range);
	}
	// Unit's
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(5,50);
    ucg->setColor(COLOR_HEADER);
	ucg->print( Units::VarioUnit() );
	drawConnection(DISPLAY_W-27, FLOGO+2 );
	drawMC( MC.get(), true );
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
// right-aligned to value, unit optional behind
void IpsDisplay::drawAltitude( float altitude, ucg_int_t x, ucg_int_t y, bool incl_unit )
{

	int alt = (int)(altitude*10.); // redered value
	if ( alt_unit.get() == ALT_UNIT_FL ) { alt /= 10; }

	// check on the rendered value for change
	if ( alt == alt_prev ) return;

	char s[15];
	ucg->setFont(ucg_font_fub20_hr);
	ucg->setColor( COLOR_WHITE );
	sprintf(s,"%5d", alt);
	int fl=ucg->getStrWidth(s);
	if ( alt_unit.get() == ALT_UNIT_FL ) {
		ucg->setPrintPos(x-fl,y);
        ucg->printf(s);
	}
    else {
		int len = std::strlen(s);
		int16_t fraction = len>0?s[len-1]-'0':0;

		alt /= 10; // chop fraction and last actual digit
		char ldigit = '0' + (alt%10);
		alt /= 10; // chop last actual digit
		// snapy second last digit
		if ( (ldigit=='0') && fraction<3 ) { alt -= 1; }
		if ( (ldigit=='9') && fraction>7 ) { alt += 1; }
		sprintf(s,"%5d8", alt);
		fl=ucg->getStrWidth(s);
		s[std::strlen(s)-1] = '\0'; // len(s) > 0 ensured!
		ucg->setPrintPos(x - fl, y);
		static int altpart_prev = 0;
		if (altpart_prev != alt) {
			ucg->printf(s);
			altpart_prev = alt;
		}
		// ESP_LOGI(FNAME,"Alti %d, fr%d - %c", alt, fraction, ldigit);

		static int fraction_prev = -1;
		if (fraction != fraction_prev)
		{
			// move last digit
			int16_t char_height = ucg->getFontAscent() - ucg->getFontDescent();
			int16_t m = (fraction / 10.f) * char_height; // to pixel offest
			ucg->setClipRange(x - char_width, y - char_height * 1.3,
							 char_width, char_height * 1.5);
			// ucg->drawFrame(x - char_width, y - char_height * 1.3,
			// 				 char_width, char_height * 1.5);
			ucg->setPrintPos(x - char_width, y - m);
			ucg->print(ldigit);
			ucg->setPrintPos(x - char_width, y - m - char_height);
			ucg->print((ldigit == '0') ? '9' : (char)(ldigit - 1)); // one above
			ucg->setPrintPos(x - char_width, y - m + char_height);
			ucg->print((ldigit == '9') ? '0' : (char)(ldigit + 1)); // one below
			ucg->undoClipRange();
			fraction_prev = fraction;
		}
	}
	if ( incl_unit ) {
		ucg->setFont(ucg_font_fub11_hr);
		ucg->setColor( COLOR_HEADER );
		ucg->setPrintPos(x, y-3);
		ucg->printf(" %s ", Units::AltitudeUnit() );
	}
	alt_prev = alt;
}
// right-aligned to value, unit optional behind
void IpsDisplay::drawSpeed(int airspeed, ucg_int_t x, ucg_int_t y, bool inc_unit)
{
	// ESP_LOGI(FNAME,"draw airspeed %d %d", airspeed, as_prev );
	ucg->setColor( COLOR_WHITE );
	if ( inc_unit ) { // todo shortcut
		ucg->setFont(ucg_font_fub20_hr);
	} else {
		ucg->setFont(ucg_font_fub14_hn);
	}
	char s[10];
	sprintf(s," %3d",  airspeed );
	int fl=ucg->getStrWidth(s);
	ucg->setPrintPos(x-fl,y);
	ucg->printf(s);
	if ( inc_unit ) {
		ucg->setFont(ucg_font_fub11_hr);
		ucg->setPrintPos(x,y-3);
		ucg->setColor( COLOR_HEADER );
		ucg->printf(" %s ", Units::AirspeedUnit() );
	}
	as_prev = airspeed;
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

	annotateScale(-max_gscale,150,max_gscale, 1 );
	annotateScale(max_gscale,150,max_gscale, 1 );
	// drawAnalogScale(0, 132);
	if((max_gscale%2) == 0) {
		annotateScale(max_gscale/2,155,max_gscale, 1);
		annotateScale(-max_gscale/2,155,max_gscale, 1);
	}
	else{
		annotateScale((max_gscale-1)/2,155,max_gscale, 1);
		annotateScale((-max_gscale+1)/2,155,max_gscale, 1);
	}
	for( float a=gload_pos_limit.get()-1; a<max_gscale; a+=0.05 ) {
		drawPolarIndicator( ((float)a/max_gscale)*M_PI_2, AMIDX, AMIDY, 120, 130, 2, COLOR_RED, false );
	}
	for( float a=gload_neg_limit.get()-1; a>(-max_gscale); a-=0.05 ) {
		drawPolarIndicator( ((float)a/max_gscale)*M_PI_2, AMIDX, AMIDY, 120, 130, 2, COLOR_RED, false );
	}

	ESP_LOGI(FNAME,"initLoadDisplay end");
}

float old_gmax = 100;
float old_gmin = -100;

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
		drawPolarIndicator( a, AMIDX, AMIDY, 60, 120, 3, COLOR_WHITE );
		// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );
	}
	// G load digital
	if( (int)(loadFactor*30) != _ate && !(tick%3) ) {
		drawAvgVario( 90, AMIDY+2, loadFactor );
		_ate = (int)(loadFactor*30);
	}
	// Min/Max values
	if( old_gmax != gload_pos_max.get() ){
		if( gload_pos_max.get() < gload_pos_limit.get() )
			ucg->setColor(  COLOR_WHITE  );
		else
			ucg->setColor(  COLOR_RED  );
		ucg->setFont(ucg_font_fub20_hr);
		ucg->setPrintPos(120,105);
		ucg->printf("%+1.2f   ", gload_pos_max.get() );
		old_gmax = gload_pos_max.get();
	}
	if( old_gmin != gload_neg_max.get() ){
		if( gload_neg_max.get() > gload_neg_limit.get() )
			ucg->setColor(  COLOR_WHITE  );
		else
			ucg->setColor(  COLOR_RED  );
		ucg->setFont(ucg_font_fub20_hr);
		ucg->setPrintPos(125,245);
		ucg->printf("%+1.2f   ", gload_neg_max.get() );
		old_gmin = gload_neg_max.get();
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
	// uncomment for scroll test
	// static int scy=0;
	// scy+=10;
	// ucg->scrollLines( scy%320 );
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
			ucg->setPrintPos(70,15);
			if( netto )
				ucg->setColor( COLOR_HEADER );
			else
				ucg->setColor( COLOR_BLACK );
			if( netto_mode.get() == NETTO_NORMAL )
				ucg->print( "net  " );
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
		drawPolarIndicator( a, AMIDX, AMIDY, 75, 130, 2, COLOR_ORANGE );
		// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );
	}
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
	// draw green bar
	if( !(tick%5) ){
		if( (int)(te*10) != (int)(te_prev*10) ) {
			float step= (M_PI_2/150) * _range;
			if( te > te_prev && te > 0 ){  // draw green what's missing
				for( float a=te_prev; a<te && a<_range; a+=step ) {
					if( a >= step*2 ) // don't overwrite the '0'
						drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 135, 140, 2, COLOR_GREEN, false );
				}
			}
			else{   // delete what's too much
				ESP_LOGD(FNAME,"delete te:%0.2f prev:%0.2f", te, te_prev );
				for( float a=te_prev+step; a>=te && a >= step; a-=step ) {
					ESP_LOGD(FNAME,"delete %0.2f", a );
					drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 134, 141, 2, COLOR_BLACK, false );
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
								drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 135, 140, 2, COLOR_RED, false );
							else
								drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 135, 140, 2, COLOR_BLUE, false );
						}
					}
				}
				else{   // delete what's too much
					for( float a=polar_sink_prev-step; a<=val && a <= -step; a+=step ) {
						ESP_LOGD(FNAME,"black a=%f",a);
						drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 134, 141, 2, COLOR_BLACK, false );
					}
				}
				polar_sink_prev = val + step;
			}
		}
	}

	// average Climb
	if( (int)(ate*30) != _ate && !(tick%3) ) {
		drawAvgVario( AMIDX - 50, AMIDY+2, ate );
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
		drawConnection(DISPLAY_W-27, FLOGO+2 );
	}

	// S2F Command triangle
	if( ((int)s2fd != s2fdalt && !((tick+1)%2) ) || !((tick+3)%30) ) {
		drawS2FBar(AMIDX, AMIDY,(int)s2fd);
	}

	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}

	// Altitude & Airspeed
	if( !(tick%8) ) {
		drawAltitude( altitude, 180, 250 );
		if( as_prev != airspeed || !(tick%64) ) {
			drawSpeed(airspeed, 180, 282);
		}
	}

	// Battery
	int chargev = (int)( volt *10 );
	if( volt < bat_red_volt.get() ){
		if( !(tick%40) )
			blank = true;
		else if( !((tick+10)%20) )
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
		drawTemperature( 5, 25, temp );
		tempalt=(int)(temp*10);
	}

	// WK-Indicator
	if( FLAP && !(tick%7) )
	{
		float wkspeed = Units::ActualWingloadCorrection(airspeed_kmh);
		int wki;
		float wkopt = FLAP->getOptimum( wkspeed, wki );
		int wk = (int)((wki - wkopt + 0.5)*10);
		// ESP_LOGI(FNAME,"as:%d wksp:%f wki:%d wk:%d wkpos:%f", airspeed_kmh, wkspeed, wki, wk, wkpos );
		// ESP_LOGI(FNAME,"WK changed WKE=%d WKS=%f", wk, wksensor );
		ucg->setColor(  COLOR_WHITE  );
		FLAP->drawBigBar( WKBARMID, WKSYMST-4, (float)(wk)/10, wksensor );
		wkoptalt = wk;
		wksensoralt = (int)(wksensor*10);

		// FLAP->drawWingSymbol( WKBARMID-27*(abs(flap_neg_max.get()))-18, WKSYMST-3, wki, wksensor);
	}

	// Cruise mode or circling
	if( (int)s2fmode != s2fmode_prev ){
		drawS2FMode( 17, 272, s2fmode );
		s2fmode_prev = (int)s2fmode;
	}

	// Medium Climb Indicator
	// ESP_LOGI(FNAME,"acl:%f iacl:%d, nt:%d", acl, average_climb, !(tick%16) );
	if ( average_climb != (int)(acl*10) && !(tick%16) && acl > 0 ){
		drawAvg( acl, acl-average_climbf );
		average_climb = (int)(acl*10);
		average_climbf = acl;
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
		drawPolarIndicator( a, AMIDX, AMIDY, 60, 120, 3, COLOR_RED );
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
						drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 125, 2, COLOR_GREEN, false );
				}
			}
			else{   // delete what's too much
				ESP_LOGD(FNAME,"delete te:%0.2f prev:%0.2f", te, te_prev );
				for( float a=te_prev+step; a>=te && a >= step; a-=step ) {
					ESP_LOGD(FNAME,"delete %0.2f", a );
					drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 119, 126, 2, COLOR_BLACK, false );
				}
			}
			te_prev = te+step;
		}
	}
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
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
								drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 125, 2, COLOR_RED, false );
							else
								drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 125, 2, COLOR_BLUE, false );
						}
					}
				}
				else{   // delete what's too much
					for( float a=polar_sink_prev-step; a<=val && a <= -step; a+=step ) {
						ESP_LOGD(FNAME,"black a=%f",a);
						drawPolarIndicator( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 119, 126, 2, COLOR_BLACK, false );
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
		drawConnection(DISPLAY_W-27, FLOGO+2 );
	}

	// Altitude Header
	if( !(tick%24) ){
		float qnh = QNH.get();
		// ESP_LOGI(FNAME,"standard_setting:%d",standard_setting );
		if( standard_setting )
			qnh = Units::Qnh( 1013.25 );
		// redraw just in case the vario pointer was there
		if( qnh != pref_qnh ) {
			ucg->setFont(ucg_font_fub11_tr);
			char unit[4];
			if( standard_setting )
				sprintf( unit, "QNE" );
			else
				sprintf( unit, "QNH" );
			ucg->setPrintPos(FIELD_START_UL-50,(YALT-S2FFONTH-10));
			ucg->setColor(0, COLOR_HEADER );
			ucg->printf("%s %.2f %s   ", unit, qnh, Units::QnhUnit( qnh_unit.get() ) );
			pref_qnh = qnh;
		}
	}
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
	// Altitude
	if(!(tick%8) ) {
		drawAltitude( altitude, FIELD_START_UL, YALT-4 );
	}

	// Battery
	int chargev = (int)( volt *10 );
	if( volt < bat_red_volt.get() ){
		if( !(tick%40) )
			blank = true;
		else if( !((tick+10)%20) )
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
		drawTemperature( 20, 30, temp );
		tempalt=(int)(temp*10);
	}

	// WK-Indicator
	if( FLAP && !(tick%7) )
	{
		float wkspeed = Units::ActualWingloadCorrection(airspeed_kmh);
		int wki;
		float wkopt=FLAP->getOptimum( wkspeed, wki );
		int wk = (int)((wki - wkopt + 0.5)*10);
		// ESP_LOGI(FNAME,"as:%d wksp:%f wki:%d wk:%d", airspeed, wkspeed, wki, wk  );
		// ESP_LOGI(FNAME,"WK changed WKE=%d WKS=%f", wk, wksensor );
		ucg->setColor(  COLOR_WHITE  );
		FLAP->drawBigBar( WKBARMID, WKSYMST-4, (float)(wk)/10, wksensor);
		wkoptalt = wk;
		wksensoralt = (int)(wksensor*10);

		FLAP->drawWingSymbol( WKBARMID-27*(abs(flap_neg_max.get()))-18, WKSYMST-3, wki, wksensor);
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
			drawSpeed(airspeed, FIELD_START_UL, 73);
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
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}

	// WK-Indicator
	if( FLAP && !(tick%7) )
	{
		float wkspeed = Units::ActualWingloadCorrection(airspeed_kmh);
		int wki;
		float wkopt=FLAP->getOptimum( wkspeed, wki );
		int wk = (int)((wki - wkopt + 0.5)*10);
		// ESP_LOGI(FNAME,"ias:%d wksp:%f wki:%d wk:%d wkpos%f", airspeed, wkspeed, wki, wk, wkpos );
		ucg->setColor(  COLOR_WHITE  );
		FLAP->drawSmallBar( YS2F-fh, WKSYMST+2, (float)(wk)/10 );
		wkoptalt = wk;

		FLAP->drawWingSymbol( YS2F-fh-25, WKSYMST+2, wki, wksensor);
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
		float qnh = QNH.get();
		// ESP_LOGI(FNAME,"standard_setting:%d",standard_setting );
		if( standard_setting )
			qnh = Units::Qnh( 1013.25 );
		if( qnh != pref_qnh ) {
			ucg->setFont(ucg_font_fub11_tr);
			ucg->setPrintPos(FIELD_START,YALT-S2FFONTH);
			char unit[4];
			if( standard_setting )
				sprintf( unit, "QNE" );
			else
				sprintf( unit, "QNH" );
			ucg->setPrintPos(FIELD_START,(YALT-S2FFONTH));
			ucg->setColor(0, COLOR_HEADER );
			ucg->printf("%s %.2f %s  ", unit, qnh,  Units::QnhUnit( qnh_unit.get() ) );
			pref_qnh = qnh;
		}
	}

	// Altitude
	if(!(tick%8) ) {
		drawAltitude( altitude, FIELD_START+80, YALT+6 );
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
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
	// Temperature Value
	if( (int)(temp*10) != tempalt && !(tick%11)) {
		drawTemperature( FIELD_START+20, DISPLAY_H-6, temp );
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
		drawBat( volt, BATX, BATY, blank );
		chargealt = chargev;
		blankold = blank;
	}

	// Bluetooth Symbol

	if( !(tick%12) ){
		drawConnection(DISPLAY_W-27, FLOGO);
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
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
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
		if( _menu ){
			xSemaphoreGive(spiMutex);
			return;
		}
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
		drawSpeed(airspeed, FIELD_START+35, YS2F-fh+3, false);
		as_prev = airspeed;
	}
	// S2F command trend triangle
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
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
		drawSpeed((int)(s2f+0.5), ASVALX+30, YS2F-fh+3, false);

		// draw S2F Delta
		drawSpeed((int)(s2fd+0.5), ASVALX+30, DISPLAY_H/2+fh+3, false);
		drawS2FBar(ASVALX+20, DISPLAY_H/2, s2fd);

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
