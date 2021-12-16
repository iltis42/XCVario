/*
 * IpsDisplay.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: iltis
 *
 */

#include "IpsDisplay.h"
#include "BTSender.h"
#include "DallasRmt.h"
#include "WifiClient.h"
#include "WifiApp.h"
#include "sensor.h"
#include "Units.h"
#include "Flap.h"
#include "Flarm.h"
#include "Compass.h"
#include "CircleWind.h"
#include "canbus.h"
#include "Blackboard.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <AdaptUGC.h>
#include <logdef.h>

#include "sdkconfig.h"
#include <cmath>
#include <cstdio>
#include <cstring>

int screens_init = INIT_DISPLAY_NULL;

int   IpsDisplay::tick = 0;
bool  IpsDisplay::_menu = false;
int   IpsDisplay::_pixpmd = 10;
int   IpsDisplay::charge = 100;
int   IpsDisplay::red = 10;
int   IpsDisplay::yellow = 25;

bool IpsDisplay::netto_old = false;
int16_t IpsDisplay::char_width; // for roling altimeter
int16_t IpsDisplay::char_height;

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

#define HEADFONTH 14
#define VARFONTH  54 // fub35_hn
#define YVAR HEADFONTH+VARFONTH
#define YVARMID (YVAR - (VARFONTH/2))

#define S2FFONTH 35 //31
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
static const int16_t INNER_RIGHT_ALIGN = 170;
static int fh;

extern xSemaphoreHandle spiMutex; // todo needs a better concept here

#define PMLEN 24

ucg_color_t IpsDisplay::colors[TEMAX+1+TEGAP];
ucg_color_t IpsDisplay::colorsalt[TEMAX+1+TEGAP];

AdaptUGC *IpsDisplay::ucg = 0;

int IpsDisplay::_te=0;
int IpsDisplay::_ate=0;
int IpsDisplay::s2falt=-1;
int IpsDisplay::s2fdalt=0;
int IpsDisplay::s2f_level_prev=0;
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
float IpsDisplay::_range = 5.;
int   IpsDisplay::average_climb = -100;
float IpsDisplay::average_climbf = 0;
int   IpsDisplay::prev_heading = 0;
int   IpsDisplay::prev_windspeed = 0;
float IpsDisplay::pref_qnh = 0;
float IpsDisplay::old_polar_sink = 0;

const int16_t SINCOS_OVER_110 = 256; // need to be a power of 2
const float sincosScale = SINCOS_OVER_110/M_PI_2*90./110.;
static float precalc_sin[SINCOS_OVER_110];
static float precalc_cos[SINCOS_OVER_110];
static int16_t old_vario_bar_val = 0;
static int16_t old_sink_bar_val = 0;


#define WKBARMID (AMIDY-15)

float polar_sink_prev = 0;
float te_prev = 0;
bool blankold = false;
bool blank = false;
bool flarm_connected=false;
typedef enum e_bow_color { BC_GREEN, BC_RED, BC_BLUE } t_bow_color;
const static ucg_color_t bowcolor[3] = { {COLOR_GREEN}, {COLOR_RED}, {COLOR_BLUE} };

////////////////////////////
// trigenometric helpers for gauge painters

// calculate a gauge indicator position in rad (-pi/2 .. pi/2) for a value
static float _scale_k = M_PI_2 / 5.;
static float logGaugeIdx(const float val)
{
	return log2f(std::abs(val)+1.f) * _scale_k * (std::signbit(val)?-1.:1.);
}
static float linGaugeIdx(const float val)
{
	return val * _scale_k;
}
static float (*_gauge)(float) = &linGaugeIdx;
static int   needle_pos_old = 0; // -pi/2 .. pi/2 * sincosScale

// get sin/cos position from gauge index in rad
static inline int precal_scaled_idx(const float val) { return (int)(abs(val)*sincosScale)&(SINCOS_OVER_110-1); }
static inline int precal_idx(const int ival) { return abs(ival)&(SINCOS_OVER_110-1); }

static inline int gaugeSin(const float val, const int len)
{
	return AMIDY - precalc_sin[precal_scaled_idx(val)] * len * (std::signbit(val)?-1:1);
}
static inline int gaugeCos(const float val, const int len)
{
	return AMIDX - precalc_cos[precal_scaled_idx(val)] * len;
}
static inline int gaugeSin(const int val, const int len)
{
	return AMIDY - precalc_sin[precal_idx(val)] * len * (std::signbit(val)?-1:1);
}
static inline int gaugeCos(const int val, const int len)
{
	return AMIDX - precalc_cos[precal_idx(val)] * len;
}
static inline int sinIncr(const int val, const int len)
{
	return precalc_sin[precal_idx(val)] * len * (std::signbit(val)?-1:1);
}
static inline int cosIncr(const int val, const int len)
{
	return precalc_cos[precal_idx(val)] * len;
}
// Or just simply the plain trigenometric function rad(-110) < idx < rad(110)
static inline float mySin(const float val)
{
	return precalc_sin[precal_scaled_idx(val)] * (std::signbit(val)?-1:1);
}
static inline float myCos(const float val)
{
	return precalc_cos[precal_scaled_idx(val)];
}
static inline float mySin(const int ival)
{
	return precalc_sin[precal_idx(ival)] * (std::signbit(ival)?-1:1);
}
static inline float myCos(const int ival)
{
	return precalc_cos[precal_idx(ival)];
}
static void initGauge(const float max, const bool log)
{
	if ( log ) {
		_scale_k = M_PI_2 / log2f(max+1.);
		_gauge = &logGaugeIdx;
	} else {
		_scale_k = M_PI_2 / max;
		_gauge = &linGaugeIdx;
	}
	static bool initialized = false;
	if ( initialized ) return;

	for ( int i=0; i<SINCOS_OVER_110; i++ ) {
		precalc_sin[i] = sin(i/sincosScale);
		precalc_cos[i] = cos(i/sincosScale);
	}
	initialized = true;
}
// inverse to xxGaugeIdx. Get the value for an indicator position
static float gaugeValueFromIdx(const float rad)
{
	if ( _gauge == &logGaugeIdx ) {
		return (pow(2., std::abs(rad))-1.f) / _scale_k * (std::signbit(rad)?-1.:1.);
	} else {
		return rad / _scale_k;
	}
}

////////////////////////////
// IpsDisplay implementation
IpsDisplay::IpsDisplay( AdaptUGC *aucg ) {
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
		ucg->setFont(ucg_font_9x15B_mf, true);
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
// also used by OTA screen with updteing sekonds...

void IpsDisplay::writeText( int line, String text ){
	ucg->setFont(ucg_font_ncenR14_hr, true );
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
		initRetroDisplay( false );
	}
	if( display_style.get() == DISPLAY_UL ) {
		initRetroDisplay( true );
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
		ucg->drawHLine( DISPLAY_LEFT+5, DISPLAY_H-(VARBARGAP)-1, bw+1 );

		// Sollfahrt Text
		ucg->setFont(ucg_font_fub11_tr);
		fh = ucg->getFontAscent();
		ucg->setPrintPos(FIELD_START+6,YS2F-(2*fh) - 8);
		ucg->setColor(0, COLOR_HEADER );

		ucg->printf("%s %s", Units::AirspeedModeStr(), Units::AirspeedUnitStr() );

		ucg->setPrintPos(ASVALX,YS2F-(2*fh) - 8);
		ucg->print(" S2F");

		ucg->setColor(0, COLOR_WHITE );
		// AS Box
		int fl = ucg->getStrWidth(" 200- ");
		ASLEN = fl+3;
		S2FST = ASLEN+16;
		ucg->drawFrame( FIELD_START, dmid-(MAXS2FTRI)-4, ASLEN+6, (MAXS2FTRI*2)+8 );

		// S2F Zero
		// ucg->drawTriangle( FIELD_START, dmid+5, FIELD_START, dmid-5, FIELD_START+5, dmid);
		ucg->drawTriangle( FIELD_START+ASLEN-1, dmid, FIELD_START+ASLEN+5, dmid-6, FIELD_START+ASLEN+5, dmid+6);

		// Thermometer
		drawThermometer(  FIELD_START+10, DISPLAY_H-6 );

		if ( FLAP ) {
			FLAP->setBarPosition( WKSYMST+2, YS2F-fh );
			FLAP->setSymbolPosition( WKSYMST+2, YS2F-fh-25 );
		}
	}

	// Fancy altimeter
	ucg->setFont(ucg_font_fub25_hr, true);
	char_width = ucg->getStrWidth("2");
	char_height = ucg->getFontAscent() - ucg->getFontDescent() - 4;

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
	ucg->begin();
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
	static int yusize=6;
	static int ylsize=6;
	if( _menu )
		return;

	ESP_LOGD(FNAME,"drawAvg: av=%.2f delta=%.2f", avclimb, delta );
	int pos=145;
	int size=7;
	float a;
	if( avc_old != -1000 ){
		ucg->setColor( COLOR_BLACK );
		a = (_gauge)(avc_old);
		int x=gaugeCos(a, pos);
		int y=gaugeSin(a, pos);
		ucg->drawTetragon( x+size, y, x,y+ylsize, x-size,y, x,y-yusize );
		// refresh scale around old AVG icon
		drawScale( _range, -_range, 140, 0, avc_old*10.f );
	}
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
	a = (_gauge)(avclimb);
	int x=gaugeCos(a, pos);
	int y=gaugeSin(a, pos);
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
	s2f_level_prev = 0;
	btqueue = -1;
	_te=-200;
	needle_pos_old=-1000;
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
	old_polar_sink = -100;
	old_vario_bar_val = 0;
	old_sink_bar_val = 0;
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
		ucg->setFont(ucg_font_fub20_hn, false);
	} else {
		ucg->setFont(ucg_font_fub14_hn, false);
	}
	char s[10];
	std::sprintf(s, "%1.1f", Units::Vario(mc) );
	ucg->print(s);
	int16_t fl = ucg->getStrWidth(s);
	ucg->setFont(ucg_font_fub11_hr, true);
	ucg->setColor(COLOR_HEADER);
	ucg->setPrintPos(5+fl+2, DISPLAY_H-4);
	ucg->print("MC");
}

#define S2FSS 10
#define S2FTS 5

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


void IpsDisplay::setArrowColor( bool upper ){
	if( s2f_arrow_color.get() == AC_BLUE_BLUE )
		ucg->setColor( COLOR_BBLUE );
	if( s2f_arrow_color.get() == AC_WHITE_WHITE )
		ucg->setColor( COLOR_WHITE );
	if( s2f_arrow_color.get() == AC_GREEN_RED ){
		if( upper )
			ucg->setColor( COLOR_GREEN );
		else
			ucg->setColor( COLOR_RED );
	}
}

void IpsDisplay::drawArrow(int16_t x, int16_t y, int16_t level, bool del)
{
	const int width=40;
	const int step=8;
	const int gap=2;
	int height=step;

	if ( level == 0 ) return;

	int init = 1;
	if ( std::abs(level) == 4 ) {
		init = 2;
		if ( del )
			setArrowColor( level < 0 );
		else{
			if( s2f_arrow_color.get() == AC_GREEN_RED  )
				ucg->setColor( COLOR_WHITE );
			else
				ucg->setColor( COLOR_RED );
		}
	}
	else if ( del ) {
		ucg->setColor( COLOR_BLACK );
	}
	else {
		setArrowColor( level < 0 );
	}

	int l = level-init;
	if ( level < 0 ) {
		height = -height;
		l = level+init;
	}
	ucg->drawTriangle(x,y+l*(step+gap), x,y+l*(step+gap)+height, x-width, y+l*gap);
	ucg->drawTriangle(x,y+l*(step+gap), x+width,y+l*gap, x,y+l*(step+gap)+height);
}

// speed to fly delta given in kmh, s2fd > 0 means speed up
// bars dice up 10 kmh steps
void IpsDisplay::drawS2FBar(int16_t x, int16_t y, int s2fd)
{
	if( _menu ) return;

	int level = s2fd/10; // dice up into 10 kmh steps

	// draw max. three bars, then change color of the last one to red
	if ( level > 4 ) { level = 4; }
	else if ( level < -4 ) { level = -4; }

	// ESP_LOGI(FNAME,"s2fbar %d %d", s2fd, level);
	if ( level == s2f_level_prev ) {
		return;
	}

	int inc = (level-s2f_level_prev > 0) ? 1 : -1;
	for ( int i = s2f_level_prev + ((s2f_level_prev==0 || s2f_level_prev*inc>0) ? inc : 0);
			i != level+((i*inc < 0)?0:inc); i+=inc ) {
		if ( i != 0 ) {
			drawArrow(x, y-2+(i>0?1:-1)*22, i, i*inc < 0);
			// ESP_LOGI(FNAME,"s2fbar draw %d,%d", i, (i*inc < 0)?0:inc);
		}
	}

	s2f_level_prev = level;
}

void IpsDisplay::drawBT() {
	if( _menu )
		return;
	int btq=BTSender::queueFull();
	if( btq != btqueue || Flarm::connected() != flarm_connected ){
		int16_t btx=DISPLAY_W-20;
		int16_t bty=(BTH/2) + 8;
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
		drawCable(DISPLAY_W-20, BTH + 22);
	}
}

void IpsDisplay::drawCable(int16_t x, int16_t y)
{
	const int16_t CANH = 8;
	const int16_t CANW = 14;
	CAN->connectedXCV() ? ucg->setColor(COLOR_LBLUE) : ucg->setColor(COLOR_MGREY);
	// ucg->setFont(ucg_font_fub11_hr);
	// ucg->setPrintPos(x - 8, y);
	if (CAN->connectedMagSens()) {
		ucg->setColor(COLOR_GREEN);
	}
	ucg->drawLine( x-CANW/2, y+CANH/2, x+3, y+CANH/2 );
	ucg->drawLine( x-CANW/2, y+CANH/2-1, x+3, y+CANH/2-1 );
	ucg->drawDisc( x-CANW/2, y+CANH/2, 2, UCG_DRAW_ALL);
	// ucg->print("c");
	CAN->connectedMagSens() ? ucg->setColor(COLOR_LBLUE) : ucg->setColor(COLOR_MGREY);
	if (Flarm::connected()) {
		ucg->setColor(COLOR_GREEN);
	}
	ucg->drawLine( x+2, y+CANH/2, x-4, y-CANH/2 );
	ucg->drawLine( x+3, y+CANH/2-1, x-3, y-CANH/2-1 );
	// ucg->print("a");
	CAN->connectedXCV() ? ucg->setColor(COLOR_LBLUE) : ucg->setColor(COLOR_MGREY);
	ucg->drawLine( x-3, y-CANH/2, x+CANW/2, y-CANH/2 );
	ucg->drawLine( x-3, y-CANH/2-1, x+CANW/2, y-CANH/2-1 );
	ucg->drawDisc( x+CANW/2, y-CANH/2, 2, UCG_DRAW_ALL);
	// ucg->print("n");
}

void IpsDisplay::drawFlarm( int x, int y, bool flarm ) {
	if( _menu )
		return;
	int16_t flx=x;
	int16_t fly=y;
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
		drawCable(x, y+18);
	}
}

void IpsDisplay::drawConnection( int16_t x, int16_t y )
{
	if( wireless == WL_BLUETOOTH )
		drawBT();
	else if( wireless != WL_DISABLE )
		drawWifi(x, y);
	else if( SetupCommon::isWired() )
		drawCable(x+10, y);
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
			ucg->drawBox( x-55,y-12, 65, 22  );
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
			ucg->setFont(ucg_font_fub11_hr, true);
			ucg->setPrintPos(x-42,y-6);
		}
		ucg->setColor( COLOR_WHITE );
		if( battery_display.get() == BAT_PERCENTAGE ) {
			ucg->printf("%3d", charge);
			ucg->setColor( COLOR_HEADER );
			ucg->print("% ");
		}
		else if ( battery_display.get() == BAT_VOLTAGE ) {
			// ucg->setPrintPos(x-40,y-8);
			ucg->printf("%2.1f", volt);
			ucg->setColor( COLOR_HEADER );
			ucg->print("V ");
		}
		else if ( battery_display.get() == BAT_VOLTAGE_BIG ) {
			ucg->setPrintPos(x-50,y+11);
			ucg->setFont(ucg_font_fub14_hr, true);
			ucg->printf("%2.1f", volt);
			ucg->setColor( COLOR_HEADER );
			ucg->print("V ");
		}

	}
}

// accept temperature in deg C and display in configured unit
// right-aligned value to x, incl. unit right of x
void IpsDisplay::drawTemperature( int x, int y, float t ) {
	if( _menu )
		return;
	ucg->setFont(ucg_font_fur14_hf, true);
	char s[10];
	if( t != DEVICE_DISCONNECTED_C ) {
		float temp_unit = Units::TemperatureUnit( t );
		sprintf(s, " %4.1f", std::roundf(temp_unit*10.f)/10.f );
	}
	else {
		strcpy(s, " - ");
	}
	ucg->setColor( COLOR_WHITE );
	ucg->setPrintPos(x-ucg->getStrWidth(s),y);
	ucg->print(s);
	ucg->setColor( COLOR_HEADER );
	ucg->setPrintPos(x+3,y);
	ucg->print(Units::TemperatureUnitStr(temperature_unit.get()));
}

// val, center x, y, start radius, end radius, width, r,g,b
void IpsDisplay::drawOneScaleLine( float a, int16_t l1, int16_t l2, int16_t w, uint8_t r, uint8_t g, uint8_t b)
{
	if( _menu ) return;

	float si=sin(a);
	float co=cos(a);
	int16_t w0 = w/2;
	w = w - w0; // total width := w + w0
	int16_t xn_0 = AMIDX-l1*co+w0*si;
	int16_t yn_0 = AMIDY-l1*si-w0*co;
	int16_t xn_1 = AMIDX-l1*co-w*si;
	int16_t yn_1 = AMIDY-l1*si+w*co;
	int16_t xn_2 = AMIDX-l2*co-w*si;
	int16_t yn_2 = AMIDY-l2*si+w*co;
	int16_t xn_3 = AMIDX-l2*co+w0*si;
	int16_t yn_3 = AMIDY-l2*si-w0*co;
	// ESP_LOGI(FNAME,"IpsDisplay::drawTetragon  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d x3:%d y3:%d", (int)xn_0, (int)yn_0, (int)xn_1 ,(int)yn_1, (int)xn_2, (int)yn_2, (int)xn_3 ,(int)yn_3 );
	ucg->setColor( r,g,b  );
	ucg->drawTetragon(xn_0,yn_0,xn_1,yn_1,xn_2,yn_2,xn_3,yn_3);
}

// -pi/2 < val < pi/2, center x, y, start radius, end radius, width, r,g,b
// return status on updated a changed pointer position
bool IpsDisplay::drawPolarIndicator( float a, int16_t l1, int16_t l2, int16_t w, ucg_color_t color, bool dirty )
{
	typedef struct Triangle {
		int16_t x_0=0, y_0=0, x_1=0, y_1=1, x_2=1, y_2=0;
	} Triangle_t;
	static Triangle_t o;
	Triangle_t n;
	if( _menu ) return false;

	int val = (int)(a*sincosScale + 0.5 ); // descrete indicator position
	bool change = val != needle_pos_old;
	if ( ! change && ! dirty ) return false; // nothing painted

	float si=mySin(val);
	float co=myCos(val);
	float psi=gaugeSin(val,l1);
	float pco=gaugeCos(val,l1);

	n.x_0 = pco+w*si; // top shoulder
	n.y_0 = psi-w*co;
	n.x_1 = pco-w*si; // lower shoulder
	n.y_1 = psi+w*co;
	n.x_2 = gaugeCos(val, l2); // tip
	n.y_2 = gaugeSin(val, l2);
	// ESP_LOGI(FNAME,"IpsDisplay::drawTetragon  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d x3:%d y3:%d", (int)xn_0, (int)yn_0, (int)xn_1 ,(int)yn_1, (int)xn_2, (int)yn_2, (int)xn_3 ,(int)yn_3 );
	ucg->setColor( COLOR_BLACK );
	ucg->drawTriangle(o.x_0,o.y_0,o.x_1,o.y_1,o.x_2,o.y_2);
	ucg->setColor( color.color[0], color.color[1], color.color[2] );
	ucg->drawTriangle(n.x_0,n.y_0, n.x_1,n.y_1, n.x_2,n.y_2);
	o = n;
	needle_pos_old = val;
	return change;
}


// draw incremental bow up to indicator given in rad, pos
void IpsDisplay::drawBow( float a, int16_t &old_a_level, int16_t l1, ucg_color_t c)
{
	int16_t level = (int16_t)(a*sincosScale); // dice up into discrete steps

	if ( _menu ) return;

	if ( level == old_a_level ) {
		return;
	}

	// potentially clean first
	if ( std::abs(level) < std::abs(old_a_level)
			|| level*old_a_level < 0 ) {
		ucg->setColor(COLOR_BLACK);
	}
	else {
		ucg->setColor(c.color[0], c.color[1], c.color[2]); // green-up, red-down
	}
	// ESP_LOGI(FNAME,"bow lev %d", level);

	int inc = (level-old_a_level > 0) ? 1 : -1;
	for ( int i = old_a_level + ((old_a_level==0 || old_a_level*inc>0) ? inc : 0);
			i != level+((i*inc < 0)?0:inc); i+=inc ) {
		if ( i != 0 ) {
			int x = gaugeCos(i, l1);
			int y = gaugeSin(i, l1);
			int xe = x - cosIncr(i, 5);
			int ye = y - sinIncr(i, 5);
			// ESP_LOGI(FNAME,"drawLine x1:%d y1:%d x2:%d y2:%d", x,y,xe,ye );
			ucg->drawLine(x, y, xe, ye);
			int d = std::signbit(i)?-1:1;
			ucg->drawLine(x, y+d, xe, ye+d);
		}
		else ucg->setColor(c.color[0], c.color[1], c.color[2]);
	}
	old_a_level = level;
}

// +/- range, radius to AMID [pixel], opt. small area refresh at [scale*10]
void IpsDisplay::drawScale( int16_t max_pos, int16_t max_neg, int16_t pos, int16_t offset, int16_t at ) {
	if( _menu ) return;

	int modulo = 10;
	if( max_pos > 10 ) {
		modulo = 20;
	} else if( max_pos < 6 ) {
		modulo = 5;
	}

	// for larger ranges put at least on extra labl in the middle of the scale
	int16_t mid_lpos = (int)(gaugeValueFromIdx(0.5*M_PI_2)+.5) * 10;
	mid_lpos /= modulo;
	mid_lpos *= modulo; // round down to the next modulo hit
	ucg->setFontPosCenter();
	// ucg->setFontMode(UCG_FONT_MODE_TRANSPARENT);  is default
	ucg->setFont(ucg_font_fub14_hn);

	// calc pixel dist for interval 0.5-1
	int16_t dist = (int)(((*_gauge)(1.) - (*_gauge)(0.5)) * pos); // in pixel
	// ESP_LOGI(FNAME, "lines go m%d %d %d", modulo, dist, mid_lpos);
	int16_t start=max_pos*10, stop=0;
	if ( at != -1000 ) {
		start = at+4;
		stop = at-4;
		if ( start <= 10 ) {
			modulo = (dist>24)?1:(dist>16)?2:(dist>8)?5:10;
		}
	}
	bool draw_label = start == max_pos*10;
	for( int a=start; a>=stop; a-- ) {
		int width=0;
		int end=pos+5;

		if ( a == 10 ) {
			draw_label = true;
			modulo = (dist>24)?1:(dist>16)?2:(dist>8)?5:10;
		}

		if ( ! (a%modulo) ) {
			// any line
			width = 1;

			if ( ! (a%5) ) {
				// .5 lines
				width = 2;
				end = pos+10;
			}

			if ( ! (a%10) ) {
				// every integer big line
				if ( (modulo < 11)
						|| (a == start || a == mid_lpos) ) {
					width = 3;
					end = pos+15;
				}
				draw_label = a!=0 && (draw_label || max_pos<5 || a==mid_lpos);
			}
			// ESP_LOGI(FNAME, "lines a %d %d %d", a, end, draw_label);

			float val = (*_gauge)((float)a/10.);
			drawOneScaleLine( val, pos, end, width, COLOR_WHITE );
			if ( draw_label ) { drawOneLabel(val, a/10, pos+12, offset); }
			if ( (-a/10) >= max_neg && at < max_neg ) {
				drawOneScaleLine( -val, pos, end, width, COLOR_WHITE );
				if ( draw_label ) { drawOneLabel(-val, a/10, pos+12, -offset); }
			}
			draw_label = false;
		}
	}
	// ucg->setFontMode(UCG_FONT_MODE_SOLID);
	ucg->setFontPosBottom();
}

// Draw scale label numbers for -pi/2 to pi/2 w/o sign
void IpsDisplay::drawOneLabel( float val, int16_t labl, int16_t pos, int16_t offset ) {
	// ESP_LOGI( FNAME,"drawOneLabel val %.2f, label %d", val, labl );
	if( _menu )
		return;
	float to_side = 0.05;
	float incr = (M_PI_2-std::abs(val)) * 2; // increase pos towards 0
	pos += (int)incr -3;
	if( val > 0 ){
		to_side += incr/(M_PI_2*80);
	}
	else{
		to_side = -to_side;
		to_side -= incr/(M_PI_2*80);
	}
	// ESP_LOGI( FNAME,"drawOneLabel val:%.2f label:%d  toside:%.2f inc:%.2f", val, labl, to_side, incr );
	int x=gaugeCos(val+to_side, pos);
	int y=gaugeSin(val+to_side, pos) +3;

	ucg->setColor(COLOR_BBLUE);
	ucg->setPrintPos(x,y);
	if ( offset != 0 ) {
		ucg->printf("%d", labl+offset );
	} else  {
		ucg->printf("%d", abs(labl) );
	}
}


// draw windsock style alike arrow white and red
void IpsDisplay::drawWindArrow( float a, float speed, int type ){
	static int wx0,wy0,wx1,wy1,wx3,wy3;
	static bool del_wind=false;

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

void IpsDisplay::initRetroDisplay( bool ulmode ){
	if( _menu )
		return;
	bootDisplay();
	ucg->setFontPosBottom();
	redrawValues();
	initGauge(_range, log_scale.get());
	drawScale( _range, -_range, 140, 0);

	// Unit's
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(5,50);
	ucg->setColor(COLOR_HEADER);
	ucg->print( Units::VarioUnit() );
	drawConnection(DISPLAY_W-27, FLOGO+2 );
	drawSpeed(0., INNER_RIGHT_ALIGN, 75, true, true );
	drawAltitude( 0., INNER_RIGHT_ALIGN, 270, true, true );
	if( !ulmode )
		drawMC( MC.get(), true );
	if ( FLAP ) {
		FLAP->setBarPosition( WKSYMST-4, WKBARMID);
		FLAP->setSymbolPosition( WKSYMST-3, WKBARMID-27*(abs(flap_neg_max.get()))-18 );
	}
	if (ulmode){
// Thermometer
		drawThermometer(10, 25 );
	}
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
	ucg->print(warn);
	ucg->setFontPosBottom();
	xSemaphoreGive(spiMutex);
}


void IpsDisplay::drawAvgVario( int16_t x, int16_t y, float ate ){
	if( _menu )
		return;
	char s[15];
	ucg->setFont(ucg_font_fub35_hn, false );
	ucg->setFontPosCenter();
	static const char* format[2] = {"   %2.1f", "   %2.0f"};
	ucg->setColor( COLOR_WHITE );
	sprintf(s, format[std::abs(ate)>10], round(ate*10.)/10.); // Avoid "-" sign because of not sown mantissa
	ucg->setPrintPos(x - ucg->getStrWidth(s), y + 3);
	ucg->print(s);
	ucg->setFontPosBottom();
}

// right-aligned to value, unit optional behind
// altidude >=0 are displayed correctly with last two digits rolling accoring to their fraction to the right
bool IpsDisplay::drawAltitude( float altitude, int16_t x, int16_t y, bool dirty, bool incl_unit )
{
	if( _menu ) return false;

	int alt = (int)(altitude); // rendered value

	// check on the rendered value for change
	dirty = dirty || alt != alt_prev;
	if ( ! dirty ) return false;
	alt_prev = alt;

	char s[20];
	ucg->setFont(ucg_font_fub25_hr, true);
	ucg->setColor( COLOR_WHITE );
	sprintf(s,"  %03d", alt); // need the string with at least three digits !!
	if ( alt_unit.get() == ALT_UNIT_FL ) {
		ucg->setPrintPos(x-ucg->getStrWidth(s),y);
		ucg->print(s);
	}
	else if ( ! alt_quant.get() ) {
		sprintf(s,"  %d", alt);
		ucg->setPrintPos(x-ucg->getStrWidth(s),y);
		ucg->print(s);
	}
	else {
		int16_t len = std::strlen(s);
		int16_t quant = 10;
		if ( alt_unit.get() == ALT_UNIT_FT ) {
			quant = 20;
		}
		alt = ((alt+quant/2)/quant)*quant;
		int16_t fraction = (altitude+quant/2 - alt) * 100/quant; // %  0..100

		int16_t lastdigit = (alt%100)/10;
		s[len-2] = '\0'; len-=2; // chop 2 digits
		int16_t nr_rolling = 2;

		static int16_t fraction_prev = -1;
		if (dirty || fraction != fraction_prev)
		{
			// move last digit
			int16_t m = (fraction * char_height/ 100) - char_height/2; // to pixel offest
			int16_t q = quant/10; // here either 1 (for m) or 2 (for ft)
			int16_t xp = x - 2*char_width;
			char tmp[3] = {'0', '0', 0};
			ucg->setClipRange(xp, y - char_height * 1.25, char_width*2, char_height * 1.45);
			ucg->setPrintPos(xp, y - m - char_height);
			tmp[0] = (lastdigit+10-q)%10 + '0';
			ucg->print(tmp); // one above
			ucg->setPrintPos(xp, y - m);
			tmp[0] = lastdigit%10 + '0';
			ucg->print(tmp);
			ucg->setPrintPos(xp, y - m + char_height);
			tmp[0] = (lastdigit+q)%10 + '0';
			ucg->print(tmp); // one below
			ucg->undoClipRange();

			fraction_prev = fraction;

			if ( m < 0 && lastdigit < q ) { // hdigit in disharmonie with digit of altitude to display
				int16_t hdigit = (alt%1000)/100;
				// ESP_LOGI(FNAME,"Alti %f: %d - %dm%d %d.%d", altitude, fraction, alt, m, hdigit, lastdigit);
				nr_rolling++;
				xp -= char_width; // one to the left
				// ucg->drawFrame(xp, y - char_height, char_width, char_height);
				ucg->setClipRange(xp, y - char_height, char_width, char_height-1);  // to be crosschecked by hjr
				ucg->setPrintPos(xp, y - m);
				ucg->print(hdigit);
				ucg->setPrintPos(xp, y - m - char_height); // one above
				ucg->print((hdigit+10-q)%10);
				ucg->undoClipRange();
				s[len-1] = '\0'; len--; // chop another digits
			}
		}
		ucg->setPrintPos(x - ucg->getStrWidth(s) - nr_rolling*char_width , y);
		static int altpart_prev = 0;
		alt/=100;
		if (dirty || altpart_prev != alt) {
			ucg->print(s);
			altpart_prev = alt;
		}
	}
	if ( incl_unit ) {
		// unit todo needs further refinment to not draw the unit every time to just have the QNH updated
		ucg->setFont(ucg_font_fub11_hr, true);
		ucg->setColor( COLOR_HEADER );
		ucg->setPrintPos(x+5, y-3);
		sprintf(s, "%s QNH", Units::AltitudeUnit()); // todo and QFE?
		ucg->print(s);
		// QNH
		int16_t qnh_x = x+5+ucg->getStrWidth(s);
		sprintf(s, "%d", Units::QnhRounded(QNH.get()));
		ucg->setPrintPos(qnh_x - ucg->getStrWidth(s), y-17);
		ucg->setColor( COLOR_WHITE );
		ucg->print(s);
	}

	return true;
}


// Accepts speed in kmh IAS/TAS, translates into configured unit
// right-aligned to value in 25 font size, no unit
void IpsDisplay::drawSmallSpeed(float v_kmh, int16_t x, int16_t y)
{
	int airspeed = Units::AirspeedRounded(v_kmh);
	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub14_hr, true);
	char s[10];
	sprintf(s,"  %3d",  airspeed ); //additional blank to avoid artefacts
	ucg->setPrintPos(x-ucg->getStrWidth(s), y);
	ucg->print(s);
}



// Accepts speed in kmh IAS/TAS, translates into configured unit
// set dirty, when obscured from vario needle
// right-aligned to value, unit optional behind
bool IpsDisplay::drawSpeed(float v_kmh, int16_t x, int16_t y, bool dirty, bool inc_unit)
{
	if( _menu ) return false;

	int airspeed = Units::AirspeedRounded(v_kmh);

	dirty = dirty || as_prev != airspeed;
	if ( ! dirty ) return false;
	// ESP_LOGI(FNAME,"draw airspeed %f %d", v_kmh, as_prev );

	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub25_hn, true);

	char s[10];
	sprintf(s,"  %3d",  airspeed );
	ucg->setPrintPos(x-ucg->getStrWidth(s), y);
	ucg->print(s);
	if ( inc_unit ) {
		ucg->setFont(ucg_font_fub11_hr);
		ucg->setColor( COLOR_HEADER );
		ucg->setPrintPos(x+5,y-3);
		ucg->print(Units::AirspeedUnitStr() );
		ucg->setPrintPos(x+5,y-17);
		ucg->print(Units::AirspeedModeStr());
	}
	as_prev = airspeed;
	return true;
}

//////////////////////////////////////////////
// The load display
void IpsDisplay::initLoadDisplay(){
	if( _menu )
		return;
	ESP_LOGI(FNAME,"initLoadDisplay()");
	ucg->setColor( COLOR_HEADER );
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(40,15);
	ucg->print( "G-Force" );
	ucg->setPrintPos(130,70);
	ucg->setColor(  COLOR_HEADER_LIGHT  );
	ucg->print( "MAX POS" );
	ucg->setPrintPos(130,210);
	ucg->print( "MAX NEG" );
	int max_gscale = (int)( gload_pos_limit.get() )+1;
	if( -gload_neg_limit.get() >= max_gscale )
		max_gscale = (int)( -gload_neg_limit.get()  )+1;

	for( float a=gload_pos_limit.get()-1; a<max_gscale; a+=0.05 ) {
		drawOneScaleLine( ((float)a/max_gscale)*M_PI_2, 140, 150, 2, COLOR_RED );
	}
	for( float a=gload_neg_limit.get()-1; a>-max_gscale; a-=0.05 ) {
		drawOneScaleLine( ((float)a/max_gscale)*M_PI_2, 140, 150, 2, COLOR_RED );
	}

	initGauge(max_gscale, false); // no logarithmic gauge for g-load
	drawScale( max_gscale, -max_gscale, 140, 1 );

	ESP_LOGI(FNAME,"initLoadDisplay end");
}


void IpsDisplay::drawLoadDisplay( float loadFactor ){
	static float old_gmax = 100;
	static float old_gmin = -100;

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
	float a = (*_gauge)(loadFactor-1.);
        ucg_color_t needlecolor[3] = { {COLOR_WHITE}, {COLOR_ORANGE}, {COLOR_RED} };
	drawPolarIndicator( a, 70, 129, 7, needlecolor[0] );
	// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );

	// G load digital
	if( (int)(loadFactor*30) != _ate && !(tick%3) ) {
		drawAvgVario( AMIDX+38, AMIDY, loadFactor );
		_ate = (int)(loadFactor*30);
	}
	// Min/Max values
	if( old_gmax != gload_pos_max.get() ){
		if( gload_pos_max.get() < gload_pos_limit.get() )
			ucg->setColor(  COLOR_WHITE  );
		else
			ucg->setColor(  COLOR_RED  );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->setPrintPos(120,105);
		ucg->printf("%+1.2f   ", gload_pos_max.get() );
		old_gmax = gload_pos_max.get();
	}
	if( old_gmin != gload_neg_max.get() ){
		if( gload_neg_max.get() > gload_neg_limit.get() )
			ucg->setColor(  COLOR_WHITE  );
		else
			ucg->setColor(  COLOR_RED  );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->setPrintPos(125,245);
		ucg->printf("%+1.2f   ", gload_neg_max.get() );
		old_gmin = gload_neg_max.get();
	}

	xSemaphoreGive(spiMutex);
}




// Compass or Wind Display
void IpsDisplay::drawCompass(int16_t x, int16_t y, bool wind_dirty, bool compass_dirty) {
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
		int windspeed = (int)( Units::Airspeed(wind)+0.5 );
		if( prev_heading != winddir || prev_windspeed != windspeed || compass_dirty ){
			ucg->setPrintPos(85,104);
			ucg->setColor(  COLOR_WHITE  );
			// ucg->setFont(ucg_font_fub20_hr);
			ucg->setFont(ucg_font_fub17_hf, true);
			char s[12];
			if( wind_display.get() & WD_DIGITS ){
				if( ok )
					sprintf(s,"%3d°%c%2d", winddir, type, windspeed );
				else
					sprintf(s,"%s", "    --/--" );
				if( windspeed < 10 )
					ucg->printf("%s    ", s);
				else if( windspeed < 100 )
					ucg->printf("%s   ", s);
				else
					ucg->printf("%s  ", s);
			}
		}
		if( prev_heading != winddir || prev_windspeed != windspeed || wind_dirty ){
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
			prev_windspeed = windspeed;
		}
	}
	else if( wind_display.get() & WD_COMPASS ){
		int heading = static_cast<int>(rintf(mag_hdt.get()));
		if( heading >= 360 )
			heading -= 360;
		// ESP_LOGI(FNAME, "heading %d, valid %d", heading, Compass::headingValid() );
		if( prev_heading != heading || compass_dirty ){
			char s[20];
			if( heading < 0 )
				sprintf(s,"%s", "  ---" );
			else
				sprintf(s," %4d", heading );
			ucg->setColor( COLOR_WHITE );
			ucg->setFont(ucg_font_fub20_hr, true);
			ucg->setPrintPos(x-ucg->getStrWidth(s), y);
			ucg->print(s);
			ucg->setColor( COLOR_HEADER );
			ucg->setPrintPos(x+5, y);
			ucg->print("° ");
			prev_heading = heading;
		}
	}
}

void IpsDisplay::drawRetroDisplay( int airspeed_kmh, float te_ms, float ate_ms, float polar_sink_ms, float altitude_m,
		float temp, float volt, float s2fd_ms, float s2f_ms, float acl_ms, bool s2fmode, bool standard_setting, float wksensor, bool ulmode ){
	// ESP_LOGI(FNAME,"drawRetroDisplay polar_sink: %f", polar_sink_ms );
	if( _menu )
		return;
	if( !(screens_init & INIT_DISPLAY_RETRO) ){
		initDisplay();
		screens_init |= INIT_DISPLAY_RETRO;
	}
	tick++;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	// ESP_LOGI(FNAME,"drawRetroDisplay  TE=%0.1f IAS:%d km/h  WK=%d", te, airspeed, wksensor  );
	static bool alt_dirty = false; // looped status
	static bool speed_dirty = false;
	static bool compass_dirty = false;
	static bool wind_dirty = false;

	bool netto=false;
	if( vario_mode.get() == VARIO_NETTO || (s2fmode && ( vario_mode.get() == CRUISE_NETTO )) ){
		// todo this calulation belongs to the blackboard that keeps hosted values consitent
		if( netto_mode.get() == NETTO_NORMAL ){
			te_ms = te_ms - polar_sink_ms;
			ate_ms = ate_ms - polar_sink_ms;
		}
		else if( netto_mode.get() == NETTO_RELATIVE ){  // Super Netto, considering circling sink
			te_ms = te_ms - polar_sink_ms + Speed2Fly.circlingSink( ias.get() );
			ate_ms = ate_ms - polar_sink_ms + Speed2Fly.circlingSink( ias.get() );
		}
		netto=true;
	}
	// indicate vario mode
	if( netto != netto_old ) {
		if( netto )
			ucg->setColor( COLOR_WHITE );
		else
			ucg->setColor( COLOR_BLACK );
		char s[10];
		if( netto_mode.get() == NETTO_NORMAL )
			sprintf(s, "net");
		else
			sprintf(s, "s-net");
		ucg->setFont(ucg_font_fub11_hr, true);
		ucg->setPrintPos(120-ucg->getStrWidth(s), DISPLAY_H/2-30);
		ucg->print(s);
		netto_old = netto;
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
	if( polar_sink < -_range )
		polar_sink = -_range;
	//  float s2f = Units::Airspeed( s2f_ms );   not used for now
	float s2fd = Units::Airspeed( s2fd_ms );
	// int airspeed =  (int)(Units::Airspeed( airspeed_kmh ) + 0.5);
	float altitude = Units::Altitude( altitude_m );

	// TE vario pointer position in rad
	float needle_pos = (*_gauge)(te);
	bool needle_dirty = false;
	// Check overlap on inner figures
	bool alt_overlap = needle_pos < -M_PI_2*60./90.;
	bool speed_overlap = needle_pos > M_PI_2*75./90.;
	bool wind_overlap = needle_pos < -M_PI_2*25./90. && needle_pos > -M_PI_2*55./90.;
	bool compass_overlap = needle_pos > M_PI_2*35./90. && needle_pos < M_PI_2*75./90.;
	// ESP_LOGI(FNAME,"NP: %f wd: %d", R2D(needle_pos), wind_dirty );

	static bool alt_overlap_old = false;
	static bool speed_overlap_old = false;
	static bool wind_overlap_old = false;
	static bool compass_overlap_old = false;
	// ESP_LOGI(FNAME,"alt %d speed %d", alt_overlap, speed_overlap );

	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
	// average Climb
	if( (int)(ate*30) != _ate && !(tick%3) ) {
		drawAvgVario( AMIDX + 38, AMIDY, ate );
		_ate = (int)(ate*30);
	}

	// S2F Command triangle
	if( (((int)s2fd != s2fdalt && !((tick+1)%2) ) || !((tick+3)%30) )  && !ulmode )  {
		drawS2FBar(AMIDX, AMIDY,(int)s2fd);
	}

	// MC val
	if(  !(tick%8) && !ulmode ) {
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

	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}

	// Altitude & Airspeed
	if( !(tick%8) || speed_dirty ) {
		if ( drawSpeed( airspeed_kmh, INNER_RIGHT_ALIGN, 75, speed_dirty ) ) {
			if( speed_overlap ){
				needle_dirty = true;
			}
		}
		speed_dirty = false;
	}
	if( !(tick%11) || alt_dirty ) {
		if ( drawAltitude( altitude, INNER_RIGHT_ALIGN, 270, alt_dirty ) ) {
			if( alt_overlap ){
				needle_dirty = true;
			}
		}
		alt_dirty = false;
	}

	// Compass
	if( !(tick%8) || wind_dirty || compass_dirty){
		drawCompass(INNER_RIGHT_ALIGN, 105, wind_dirty, compass_dirty );
		if( compass_overlap )
			needle_dirty = true;
		if( wind_overlap )
			needle_dirty = true;
		compass_dirty = false;
		wind_dirty = false;
	}

	// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );
	ucg_color_t needlecolor[3] = { {COLOR_WHITE}, {COLOR_ORANGE}, {COLOR_RED} };
	if( drawPolarIndicator(needle_pos, 80, 132, 9, needlecolor[needle_color.get()], needle_dirty) ) {
		alt_dirty = alt_overlap_old;
		alt_overlap_old = alt_overlap;

		speed_dirty = speed_overlap_old;
		speed_overlap_old = speed_overlap;

		wind_dirty = wind_overlap_old;
		wind_overlap_old = wind_overlap;

		compass_dirty = compass_overlap_old;
		compass_overlap_old = compass_overlap;

		// Draw colored bow
		float bar_val = (needle_pos>0.) ? needle_pos : 0.;
		// draw green/red vario bar
		drawBow(bar_val, old_vario_bar_val, 134, bowcolor[BC_GREEN] );
	}

	// ESP_LOGI(FNAME,"polar-sink:%f Old:%f int:%d old:%d", polar_sink, old_polar_sink, int( polar_sink*100.), int( old_polar_sink*100. ) );
	if( ps_display.get() && !(tick%3) ){
		if( int( polar_sink*100.) != int( old_polar_sink*100. ) ){
			drawBow(  (*_gauge)(polar_sink), old_sink_bar_val, 134, bowcolor[BC_BLUE] );
			old_polar_sink = polar_sink;
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
		drawTemperature( ulmode?60:50, 25, temp );
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
		FLAP->drawBigBar( (float)(wk)/10, wksensor );
		wkoptalt = wk;
		wksensoralt = (int)(wksensor*10);

		if ( flap_neg_max.get() > -3 ) {
			FLAP->drawWingSymbol( wki, wksensor);
		}
	}

	// Cruise mode or circling
	if( ((int)s2fmode != s2fmode_prev) && !ulmode ){
		drawS2FMode( 190, 18, s2fmode );
		s2fmode_prev = (int)s2fmode;
	}

	// Medium Climb Indicator
	// ESP_LOGI(FNAME,"acl:%f iacl:%d, nt:%d", acl, average_climb, !(tick%16) );
	if ( average_climb != (int)(acl*10) && !(tick%16) && acl > 0 ){
		drawAvg( acl, acl-average_climbf );
		average_climb = (int)(acl*10);
		average_climbf = acl;
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

	if( display_style.get() == DISPLAY_AIRLINER )
		drawAirlinerDisplay( airspeed,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor );
	else if( display_style.get() == DISPLAY_RETRO )
		drawRetroDisplay( airspeed,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor, false );
	else if( display_style.get() == DISPLAY_UL )
		drawRetroDisplay( airspeed,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor, true );

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
			te_ms = te_ms - polar_sink_ms + Speed2Fly.circlingSink( ias.get() );
			ate_ms = ate_ms - polar_sink_ms + Speed2Fly.circlingSink( ias.get() );
		}
		netto=true;
	}
	if( !(tick%20) ){
		if( netto != netto_old ){
			ucg->setFont(ucg_font_fub11_hr, true);
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
	int airspeed =  Units::AirspeedRounded( airspeed_kmh );
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
		// ESP_LOGI(FNAME,"ias:%d wksp:%f wki:%d wk:%d wkpos%f", airspeed_kmh, wkspeed, wki, wk, wkpos );
		ucg->setColor(  COLOR_WHITE  );
		FLAP->drawSmallBar( (float)(wk)/10 );
		wkoptalt = wk;

		FLAP->drawWingSymbol( wki, wksensor);
	}
	ucg->setFont(ucg_font_fub35_hn, true);  // 52 height
	ucg->setColor(  COLOR_WHITE  );

	// Average Vario
	if( _ate != (int)(ate*10) && !(tick%3) ) {
		// draw numeric value
        // set coarse clipbox to avoid overwriting Vario Skale and pointer
        ucg->setClipRange(DISPLAY_LEFT+bw+22,1,120,100);
		drawAvgVario( FIELD_START+88, YVAR-20, ate );
        ucg->undoClipRange();

		_ate = (int)(ate)*10;
	}

	// Altitude Header
	if( !(tick%24) ){
		float qnh = Units::Qnh( QNH.get() );
		// ESP_LOGI(FNAME,"standard_setting:%d",standard_setting );
		if( standard_setting )
			qnh = Units::Qnh( 1013.25 );
		if( qnh != pref_qnh ) {
			ucg->setFont(ucg_font_fub11_tr, false);
			char unit[4];
			if( standard_setting )
				strcpy( unit, "QNE" );
			else
				strcpy( unit, "QNH" );
			ucg->setPrintPos(FIELD_START,YALT-S2FFONTH-15);
			ucg->setColor(0, COLOR_HEADER );
			ucg->printf("%s %.2f %s  ", unit, qnh,  Units::QnhUnit( qnh_unit.get() ) );
			pref_qnh = qnh;
		}
	}

	// Altitude
	if(!(tick%8) ) {
		drawAltitude( altitude, FIELD_START+80, YALT-6, false, false );
	}
	// MC Value
	if(  !(tick%8) ) {
		int aMC = MC.get() * 10;
		if( aMC != mcalt && !(tick%4) ) {
			drawMC( MC.get(), false );
			mcalt=aMC;
		}
	}
	vTaskDelay(3);
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
	// Temperature ValueAirliner
	if( (int)(temp*10) != tempalt && !(tick%11)) {
		drawTemperature( FIELD_START+65, DISPLAY_H-5, temp );
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
		int16_t flx=DISPLAY_W-58;
		int16_t fly=FLOGO-7;
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
		ucg->setFont(ucg_font_fub11_hn, true);
		ucg->setClipRange( FIELD_START, dmid-(MAXS2FTRI), ASLEN, (MAXS2FTRI*2) );
		for( int speed = airspeed-MAXS2FTRI-(fh)-20; speed<airspeed+MAXS2FTRI+(fh); speed++ )
		{
			if( ((speed%20) == 0 && (speed >= 0)) || speed == -20 ) {
				// blank old values
                ucg->setColor( COLOR_BLACK );
				ucg->drawBox( FIELD_START+6,dmid+(speed-airspeed)-(fh/2)-9, ASLEN-6, fh+15 );
				int col = 0;
                if ( display_variant.get() == DISPLAY_WHITE_ON_BLACK ) {
					col = abs(((speed-airspeed)*2));
				}
				else {
					col = abs(255 - abs(((speed-airspeed)*2)));
				}
                if( speed >= 0 ){
                	ucg->setColor(  col,col,col  );
                	ucg->setPrintPos(FIELD_START+8,dmid+(speed-airspeed)+(fh/2));
                	ucg->printf("%3d ""- ", speed);
                }
			}
		}
		ucg->undoClipRange();
		// AS cleartext
		drawSmallSpeed(airspeed_kmh, FIELD_START+35, YS2F-fh+3);
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
		drawSmallSpeed(s2f, ASVALX+30, YS2F-fh+3);

		// draw S2F Delta
		drawSmallSpeed(s2fd, ASVALX+30, DISPLAY_H/2+fh+7);
		drawS2FBar(ASVALX+20, DISPLAY_H/2 + 10, s2fd);

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
