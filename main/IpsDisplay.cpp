/*
 * IpsDisplay.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: iltis
 *
 */

#include "IpsDisplay.h"
#include "BTSender.h"
#include "BLESender.h"
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
#include "CenterAid.h"
#include "Rotate.h"


////////////////////////////
// types

// Triangle
typedef struct Triangle {
	int16_t x_0=0, y_0=0, x_1=0, y_1=1, x_2=1, y_2=0;
} Triangle_t;

// Indicator
class PolarIndicator {
public:
	PolarIndicator();
	// API
	void setGeometry(int16_t base, int16_t tip, int16_t half_width);
	void forceRedraw() { dirty = true; }
	void setColor(ucg_color_t c) { color = c;}
	bool drawPolarIndicator( float a, bool dirty=false );
	bool drawPolarIndicatorAndBow( float a, bool dirty=false );

	// attributes
private:
	int16_t base = 0; // distance from center to base of arrow
	int16_t tip = 0; // distance from center to arrow tip
	int16_t h_width = 0; // half width of the arrow base
	int base_val_offset; // angle "* sincosScale" from base point to shoulder point of arrow, wrt display center
	ucg_color_t color;
	Triangle_t prev;
	int prev_needle_pos = 0; // -pi/2 .. pi/2 * sincosScale
	bool dirty = true;
};



// local variables
int screens_init = INIT_DISPLAY_NULL;

int   IpsDisplay::tick = 0;
bool  IpsDisplay::_menu = false;
int   IpsDisplay::_pixpmd = 10;
int   IpsDisplay::charge = 100;
int   IpsDisplay::red = 10;
int   IpsDisplay::yellow = 25;
float IpsDisplay::flt_altitude = 0;


bool IpsDisplay::netto_old = false;
int16_t IpsDisplay::char_width; // for roling altimeter
int16_t IpsDisplay::char_height;

// Average Vario data
int IpsDisplay::last_avg = -1000;
int IpsDisplay::x_start = 240;
PolarIndicator* IpsDisplay::indicator = nullptr;

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

extern xSemaphoreHandle spiMutex;
xSemaphoreHandle display_mutex=NULL;

ucg_color_t IpsDisplay::colors[TEMAX+1+TEGAP];
ucg_color_t IpsDisplay::colorsalt[TEMAX+1+TEGAP];

AdaptUGC *IpsDisplay::ucg = 0;

int IpsDisplay::_te=0;
int IpsDisplay::_ate=-1000;
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

float IpsDisplay::_range_clip = 0;
int   IpsDisplay::_divisons = 5;
float IpsDisplay::_range = 5.;
float IpsDisplay::average_climbf = 0;
int   IpsDisplay::prev_winddir = 0;
int   IpsDisplay::prev_heading = 0;
int   IpsDisplay::prev_windspeed = 0;
float IpsDisplay::pref_qnh = 0;
float IpsDisplay::old_polar_sink = 0;

temp_status_t IpsDisplay::siliconTempStatusOld = MPU_T_UNKNOWN;

const int16_t SINCOS_OVER_110 = 256; // need to be a power of 2
const float sincosScale = SINCOS_OVER_110/M_PI_2*90./110.;
static float precalc_sin[SINCOS_OVER_110];
static float precalc_cos[SINCOS_OVER_110];
static int16_t old_vario_bar_val = 0;
static int16_t old_sink_bar_val = 0;
static int16_t alt_quant = 1;

static bool wind_dirty = false;
static bool compass_dirty = false;
static bool alt_dirty = false;
static bool speed_dirty = false;

static float prev_needle = 0;

#define WKBARMID (AMIDY-15)

float polar_sink_prev = 0;
float te_prev = 0;
bool blankold = false;
bool blank = false;
bool flarm_connected=false;
typedef enum e_bow_color { BC_GREEN, BC_RED, BC_BLUE } t_bow_color;
const static ucg_color_t bowcolor[3] = { {COLOR_GREEN}, {COLOR_RED}, {COLOR_BBLUE} };
const static ucg_color_t needlecolor[3] = { {COLOR_WHITE}, {COLOR_ORANGE}, {COLOR_RED} };

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


PolarIndicator::PolarIndicator() :
					base(80),
					tip(132),
					h_width(9)
{
	color = needlecolor[1];
	base_val_offset = (int)(atan(static_cast<float>(h_width)/base)*sincosScale);
	prev.x_0 = gaugeCos(prev_needle_pos+base_val_offset, base); // top shoulder
	prev.y_0 = gaugeSin(prev_needle_pos+base_val_offset, base);
	prev.x_1 = gaugeCos(prev_needle_pos-base_val_offset, base); // lower shoulder
	prev.y_1 = gaugeSin(prev_needle_pos-base_val_offset, base);
	prev.x_2 = gaugeCos(prev_needle_pos, tip); // tip
	prev.y_2 = gaugeSin(prev_needle_pos, tip);
}

void PolarIndicator::setGeometry(int16_t base_p, int16_t tip_p, int16_t half_width_p)
{
	base = base_p;
	tip = tip_p;
	h_width = half_width_p;
	base_val_offset = (int)(atan(static_cast<float>(h_width)/base)*sincosScale);
	prev.x_0 = gaugeCos(prev_needle_pos+base_val_offset, base); // top shoulder
	prev.y_0 = gaugeSin(prev_needle_pos+base_val_offset, base);
	prev.x_1 = gaugeCos(prev_needle_pos-base_val_offset, base); // lower shoulder
	prev.y_1 = gaugeSin(prev_needle_pos-base_val_offset, base);
	prev.x_2 = gaugeCos(prev_needle_pos, tip); // tip
	prev.y_2 = gaugeSin(prev_needle_pos, tip);
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
	indicator = new PolarIndicator();
	display_mutex = xSemaphoreCreateMutex();
}

IpsDisplay::~IpsDisplay() {
	if ( indicator ) {
		delete indicator;
		indicator = nullptr;
	}
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
	// ESP_LOGI(FNAME,"display clear()");
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
	// ESP_LOGI(FNAME,"clear boot");
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
		ucg->setFont(ucg_font_fub11_tr);
		ucg->setFontPosBottom();
		ucg->setPrintPos(DISPLAY_LEFT+5,YVAR-VARFONTH+7);
		ucg->setColor( COLOR_HEADER );
		ucg->print(Units::VarioUnit());
		ucg->setPrintPos(FIELD_START,YVAR-VARFONTH+7);

		ucg->print(PROGMEM"AV Vario");
		ucg->setColor(0, COLOR_WHITE );

		// print TE scale
		drawLegend();

		ucg->drawVLine( DISPLAY_LEFT+5,      VARBARGAP , DISPLAY_H-(VARBARGAP*2) );
		ucg->drawHLine( DISPLAY_LEFT+5, VARBARGAP , bw+1 );
		ucg->drawVLine( DISPLAY_LEFT+5+bw+1, VARBARGAP, DISPLAY_H-(VARBARGAP*2) );
		ucg->drawHLine( DISPLAY_LEFT+5, DISPLAY_H-(VARBARGAP)-1, bw+1 );

		// S2F Text
		ucg->setFont(ucg_font_fub11_tr);
		fh = ucg->getFontAscent();
		ucg->setPrintPos(FIELD_START+6,YS2F-(2*fh) - 8);
		ucg->setColor(0, COLOR_HEADER );

		ucg->printf("%s %s", Units::AirspeedModeStr(), Units::AirspeedUnitStr() );

		ucg->setPrintPos(ASVALX,YS2F-(2*fh) - 8);
		ucg->print(PROGMEM" S2F");

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
		drawThermometer(  FIELD_START+5, DISPLAY_H-6 );

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
	_range = Units::Vario( range.get() );

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
	// ESP_LOGI(FNAME,"Pixel per m/s %d", _pixpmd );
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

static float avc_old=-1000;

void IpsDisplay::drawAvg( float avclimb, float delta ){
	static int yusize=6;
	static int ylsize=6;
	if( _menu )
		return;

	ESP_LOGD(FNAME,"drawAvg: av=%.2f delta=%.2f", avclimb, delta );
	int pos=145;
	int size=7;
	float a;
	if( avc_old != avclimb ){
		ucg->setColor( COLOR_BLACK );
		a = (_gauge)(avc_old);
		int x=gaugeCos(a, pos);
		int y=gaugeSin(a, pos);
		ucg->drawTetragon( x+size, y, x,y+ylsize, x-size,y, x,y-yusize );
		// refresh scale around old AVG icon
		drawScale( _range, -_range, 140, 0, avc_old*10.f );
	}
	if( delta > (mean_climb_major_change.get())/core_climb_history.get() ){
		ucg->setColor( COLOR_GREEN );
		yusize=size*2;
		ylsize=size;
	}
	else if ( delta < -(mean_climb_major_change.get())/core_climb_history.get() ){
		ucg->setColor( COLOR_RED );
		ylsize=size*2;
		yusize=size;
	}
	else if( delta > (mean_climb_major_change.get()/2.0)/core_climb_history.get() ){
		ucg->setColor( COLOR_GREEN );
		yusize=size;
		ylsize=size;
	}
	else if ( delta < -(mean_climb_major_change.get()/2.0)/core_climb_history.get() ){
		ucg->setColor( COLOR_RED );
		ylsize=size;
		yusize=size;
	}
	else{
		ucg->setColor( COLOR_WHITE );
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
	indicator->forceRedraw();
	indicator->setColor(needlecolor[needle_color.get()]);
	mcalt = -100;
	as_prev = -1;
	_ate = -2000;
	last_avg = -1000;
	x_start = 240;

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
	average_climbf = -1000.0;

	wkoptalt = -100;
	tyalt = -1000;
	polar_sink_prev = 0.1;
	if ( FLAP ) FLAP->redraw();
	netto_old = false;
	s2fmode_prev = 100;
	old_polar_sink = -100;
	old_vario_bar_val = 0;
	old_sink_bar_val = 0;
	prev_winddir = -1000;
	prev_heading = -1000;

	switch ( alt_quantization.get() ) {
	case ALT_QUANT_DISABLE:
		alt_quant = 0;
		break;
	case ALT_QUANT_5:
		alt_quant = 5;
		break;
	case ALT_QUANT_10:
		alt_quant = 10;
		break;
	case ALT_QUANT_20:
		alt_quant = 20;
		break;
	default:
		alt_quant = 2;
	}
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
	ucg->setPrintPos(5, DISPLAY_H-2);
	ucg->setColor(COLOR_WHITE);
	if( large ) {
		ucg->setFont(ucg_font_fub20_hn, false);
	} else {
		ucg->setFont(ucg_font_fub14_hn, false);
	}
	char s[32];
	std::sprintf(s, "%1.1f", Units::Vario(mc) );
	ucg->print(s);
	int16_t fl = ucg->getStrWidth(s);
	ucg->setFont(ucg_font_fub11_hr, true);
	ucg->setColor(COLOR_HEADER);
	ucg->setPrintPos(5+fl+2, DISPLAY_H-2);
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
			drawArrow(x, y+(i>0?1:-1)*24, i, i*inc < 0);
			// ESP_LOGI(FNAME,"s2fbar draw %d,%d", i, (i*inc < 0)?0:inc);
		}
	}

	s2f_level_prev = level;
}

void IpsDisplay::drawBT() {
	if( _menu )
		return;
	int btq=0;
	if( wireless == WL_BLUETOOTH )
		btq=BTSender::queueFull();
	else if( wireless == WL_BLUETOOTH_LE )
		btq=BLESender::queueFull();
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
		drawCable(DISPLAY_W-20, y+18);
	}
}

void IpsDisplay::drawConnection( int16_t x, int16_t y )
{
	if( wireless == WL_BLUETOOTH || wireless == WL_BLUETOOTH_LE )
		drawBT();
	else if( wireless != WL_DISABLE )
		drawWifi(x, y);
	else if( SetupCommon::isWired() )
		drawCable(DISPLAY_W-20, y);
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
			ucg->setColor( COLOR_DGREY );
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
	char s[32];
	if( t != DEVICE_DISCONNECTED_C ) {
		float temp_unit = Units::TemperatureUnit( t );
		sprintf(s, "   %4.1f", std::roundf(temp_unit*10.f)/10.f );
	}
	else {
		strcpy(s, "    --- ");
	}
	ucg->setColor( COLOR_WHITE );
	ucg->setPrintPos(x-ucg->getStrWidth(s),y);
	ucg->print(s);
	if( HAS_MPU_TEMP_CONTROL ){   // Color if T unit shows if MPU silicon temperature is locked, too high or too low
		switch( MPU.getSiliconTempStatus() ){
		case MPU_T_LOCKED:
			ucg->setColor( COLOR_HEADER );
			break;
		case MPU_T_LOW:
			ucg->setColor( COLOR_LBLUE );
			break;
		case MPU_T_HIGH:
			ucg->setColor( COLOR_RED );
			break;
		default:
			ucg->setColor( COLOR_HEADER );
		}
	}else{
		ucg->setColor( COLOR_HEADER );
	}
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
bool PolarIndicator::drawPolarIndicator( float a, bool dirty_p )
{
	Triangle_t n;
	if( IpsDisplay::inMenu() ) return false;
	int val = (int)(a*sincosScale); // descrete int indicator position, to compare with prev needle pos (do not round!)
	bool change = val != prev_needle_pos;
	dirty_p = dirty_p || dirty;
	if ( ! change && ! dirty_p )
		return false; // nothing painted
	dirty = false; // a one shot re-paint trigger

	alt_dirty |= a < -M_PI_2*60./90.;
	speed_dirty |= a > M_PI_2*75./90.;
	wind_dirty |= a < -M_PI_2*25./90. && a > -M_PI_2*55./90.;
	compass_dirty |= a > M_PI_2*35./90. && a < M_PI_2*75./90.;
    if( !alt_dirty ){
    	alt_dirty |= prev_needle < -M_PI_2*60./90.;
    }
    if( !speed_dirty ){
    	speed_dirty |= prev_needle > M_PI_2*75./90.;
    }
    if( !wind_dirty ){
    	wind_dirty |= prev_needle < -M_PI_2*25./90. && prev_needle > -M_PI_2*55./90.;
    }
    if( !compass_dirty ){
    	compass_dirty |= prev_needle > M_PI_2*35./90. && prev_needle < M_PI_2*75./90.;
    }

	n.x_0 = gaugeCos(val+base_val_offset, base); // top shoulder
	n.y_0 = gaugeSin(val+base_val_offset, base);
	n.x_1 = gaugeCos(val-base_val_offset, base); // lower shoulder
	n.y_1 = gaugeSin(val-base_val_offset, base);
	n.x_2 = gaugeCos(val, tip); // tip
	n.y_2 = gaugeSin(val, tip);
	// ESP_LOGI(FNAME,"IpsDisplay::drawTetragon  x0:%d y0:%d x1:%d y1:%d x2:%d y2:%d", n.x_0, n.y_0, n.x_1 ,n.y_1, n.x_2, n.y_2 );

	if ( std::abs(val-prev_needle_pos) < 9./90.*sincosScale  ) { // 6deg:=atan(7/70)

		// Clear just a smal triangle
		int16_t x_2 = gaugeCos(prev_needle_pos, base+7);
		int16_t y_2 = gaugeSin(prev_needle_pos, base+7);
		if( change ){
			IpsDisplay::ucg->setColor( COLOR_BLACK );
			IpsDisplay::ucg->drawTriangle(prev.x_0,prev.y_0, prev.x_1,prev.y_1, x_2,y_2);
		}

		// draw pointer
		IpsDisplay::ucg->setColor( color.color[0], color.color[1], color.color[2] );
		IpsDisplay::ucg->drawTriangle(n.x_0,n.y_0, n.x_1,n.y_1, n.x_2,n.y_2);

		// cleanup respecting overlap
		if( change ){  // we need to cleanup only if position has changed, otherwise a redraw at same position is enough
			IpsDisplay::ucg->setColor( COLOR_BLACK );
			// clear area to the side
			if ( val > prev_needle_pos ) {
				// up
				IpsDisplay::ucg->drawTetragon(prev.x_2,prev.y_2, prev.x_1,prev.y_1, n.x_1,n.y_1, n.x_2,n.y_2);
			} else {
				IpsDisplay::ucg->drawTetragon(prev.x_2,prev.y_2, n.x_2,n.y_2, n.x_0,n.y_0, prev.x_0,prev.y_0);
			}
		}

	}
	else {
		if( change ){
			// cleanup previous incarnation
			IpsDisplay::ucg->setColor( COLOR_BLACK );
			IpsDisplay::ucg->drawTriangle(prev.x_0,prev.y_0,prev.x_1,prev.y_1,prev.x_2,prev.y_2);
			// draw pointer
			IpsDisplay::ucg->setColor( color.color[0], color.color[1], color.color[2] );
			IpsDisplay::ucg->drawTriangle(n.x_0,n.y_0,n.x_1,n.y_1,n.x_2,n.y_2);
		}
	}
	// ESP_LOGI(FNAME,"change=%d  prev=%d  now=%d", change, prev_needle_pos, val  );
	prev = n;
	prev_needle_pos = val;
	prev_needle = a;
	return change;
}

// draw incremental bow up to indicator given in rad, pos
void IpsDisplay::drawBow( float a, int16_t &old_a_level, int16_t l1, ucg_color_t c)
{
	int16_t level = (int16_t)(a*sincosScale); // dice up into discrete steps

	if ( _menu ) return;
	// ESP_LOGI(FNAME,"drawBbow af=%f level=%d old_level=%d", a, level, old_a_level );

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
			// ESP_LOGI(FNAME, "lines a:%d end:%d label: %d  width: %d", a, end, draw_label, width );

			float val = (*_gauge)((float)a/10.);
			if( width < 3 )
				drawOneScaleLine( val, pos, end, width, COLOR_DGREY );  // darker color for small scale
			else
				drawOneScaleLine( val, pos, end, width, COLOR_WHITE );

			if ( draw_label ) { drawOneLabel(val, a/10, pos+12, offset); }
			if ( (-a/10) >= max_neg && at < max_neg ) {
				if( width < 3 )
					drawOneScaleLine( -val, pos, end, width, COLOR_DGREY );
				else
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
	int y=gaugeSin(val+to_side, pos) +5;

	ucg->setColor(COLOR_LBBLUE);
	ucg->setPrintPos(x,y);
	if ( offset != 0 ) {
		ucg->printf("%d", labl+offset );
	} else  {
		ucg->printf("%d", abs(labl) );
	}
}

static int wx0,wy0,wx1,wy1,wx3,wy3 = 0;  // initialize by zero

// draw windsock style alike arrow white and red
void IpsDisplay::drawWindArrow( float a, float speed, int type ){
	if( _menu )
		return;
	const int X=75;
	const int Y=215;
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
	// cleanup previous incarnation
	if( wx0 ){  // check if not zero (init condition, not yet drawn)
		ucg->setColor(  COLOR_BLACK  );
		Flarm::drawAirplane( wx0, wy0, false, true ); // clear small airplane symbol, need to clear anytime as it moves...
		ucg->drawTriangle(wx0,wy0,wx1,wy1,wx3,wy3);
	}
	if( s > 1 ){
		ucg->setColor( COLOR_WHITE );
		ucg->drawTriangle(xn_0,yn_0,xn_1,yn_1,xn_3,yn_3);
		ucg->setColor(  COLOR_RED  );
		ucg->drawTriangle(xn_2,yn_2,xn_1,yn_1,xn_3,yn_3);
	}
	if( wind_reference.get() != WR_NORTH ){
		ucg->setColor( COLOR_WHITE );
		Flarm::drawAirplane( xn_0, yn_0, false, true ); // draw a small airplane symbol
	}
	wx0 = xn_0;
	wy0 = yn_0;
	wx1 = xn_1;
	wy1 = yn_1;
	wx3 = xn_3;
	wy3 = yn_3;
}

void IpsDisplay::initRetroDisplay( bool ulmode ){
	if( _menu )
		return;
	bootDisplay();
	ucg->setFontPosBottom();
	redrawValues();
	initGauge(_range, log_scale.get());
	drawScale( _range, -_range, 140, 0);
	indicator->setGeometry(80, 132, 9);

	// Unit's
	if( _menu )
		return;
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(5,50);
	ucg->setColor(COLOR_HEADER);
	ucg->print( Units::VarioUnit() );
	drawConnection(DISPLAY_W-27, FLOGO+2 );
	drawSpeed(0., INNER_RIGHT_ALIGN, 75, true, true );
	drawAltitude( altitude.get(), INNER_RIGHT_ALIGN, 270, true, true );
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


void IpsDisplay::drawAvgVario( int16_t x, int16_t y, float val, bool large ){
	if( _menu )
		return;
	int ival = rint(val*10);  // integer value in steps of 10th
	if( last_avg != ival){  // only print if there a change in rounded numeric string
		char s[32];
		if( large )
			ucg->setFont(eglib_font_free_sansbold_66, false );
		else
			ucg->setFont(ucg_font_fub35_hn, false );
		ucg->setFontPosCenter();
		static const char* format[2] = {"%2.1f","%2.0f"};
		sprintf(s, format[std::abs(ival)>100], float(ival/10.) );
		int new_x_start = x - ucg->getStrWidth(s);
		if( new_x_start > x_start ){      // do we have a shorter string stating at higer x position
			ucg->setColor( COLOR_BLACK );    // yes -> so blank exact prepending area
			int fh = ucg->getFontAscent();   // height of blanking box
			ucg->drawBox( x_start, y-fh/2, new_x_start-x_start, fh );  // draw blanking box
		}
		ucg->setColor( COLOR_WHITE );
		ucg->setPrintPos(new_x_start, y + 7);
		ucg->print(s);
		last_avg = ival;
		x_start = new_x_start;
		ucg->setFontPosBottom();
	}
}


// right-aligned to value, unit optional behind
// altidude >=0 are displayed correctly with last two digits rolling accoring to their fraction to the right
static uint8_t last_quant = 0;

bool IpsDisplay::drawAltitude( float altitude, int16_t x, int16_t y, bool dirty, bool incl_unit )
{
	if( _menu ) return false;
	// ESP_LOGI(FNAME,"draw alt %f dirty:%d", altitude, dirty );
	// check on the rendered value for change

	int alt = (int)(altitude);
	int unitalt = alt_unit.get();
	if( gflags.standard_setting == true ){ // respect autotransition switch
		unitalt = ALT_UNIT_FL;
	}
	int used_quant = alt_quant;
	if( unitalt == ALT_UNIT_FL ) { // may change dynamically in case of autotransition enabled
		used_quant = 1;            // we use 1 for FL, this rolls smooth as slowly
	}
	if ( used_quant ) {
		alt = (int)(altitude*(20.0/used_quant)); // respect difference according to choosen quantisation
	}
	// The dirty criterion
	dirty = dirty || alt != alt_prev;
	if ( ! dirty ) return false;
	alt_prev = alt;
	alt = (int)roundf(altitude);

	bool ret=false;
	// ESP_LOGI(FNAME,"draw alt %f dirty:%d", altitude, dirty );

	char s[32]; // plain altimeter as a string
	ucg->setFont(ucg_font_fub25_hr, true);
	ucg->setColor( COLOR_WHITE );
	sprintf(s,"  %03d", alt); // need the string with at least three digits !!
	// FL also now also displayed fancy with low quant looks good to allow switching with no artefacts
	if( used_quant != last_quant ){ // cleanup artefacts from higher digits
		last_quant = used_quant;
		ucg->setColor( COLOR_BLACK );
		ucg->drawBox(x-2*char_width,y-char_height*1.5, 2*char_width, char_height*2 );
	}

	alt_dirty = false;
	if ( ! used_quant ) {
		// Plain plot of altitude for m and ft
		sprintf(s,"  %d", alt);
		ucg->setPrintPos(x-ucg->getStrWidth(s),y);
		ucg->print(s);
	}
	else {
		// Modifications on altitude string when quantization is set
		// for meter and feet
		int len = std::strlen(s);
		int nr_rolling_digits = (used_quant>9)? 2 : 1; // maximum two rolling last digits

		// Quantized altitude, strip and save sign
		float alt_f = std::abs(altitude); // float altitude w/o sign
		int sign = std::signbit(altitude)?-1:1; // interger sign of the altitude
		alt = (int)(alt_f); // to integer truncated altitude
		alt = ((alt+used_quant/2)/used_quant)*used_quant;
		float fraction = (alt_f+used_quant/2 - alt) / used_quant;
		int mod = (nr_rolling_digits==2)? 100 : 10; // mod = pow10(nr_rolling_digits);
		int alt_leadpart = alt/(mod*10); // left remaining part of altitude
		s[len-nr_rolling_digits] = '\0'; len -= nr_rolling_digits; // chop nr_rolling_digits digits
		static float fraction_prev = 1.;
		if (dirty || std::abs(fraction - fraction_prev) > 0.01 )
		{
			// move last used_quant digit(s)
			int base = mod/10;
			int lastdigit = alt%mod;
			int16_t m = sign * ((1.f-fraction) * char_height - char_height/2); // to pixel offest
			// ESP_LOGI(FNAME,"Last %f/%d: %f m%d .%d", altitude, alt, fraction, m, lastdigit);
			int16_t xp = x - nr_rolling_digits*char_width;
			// ucg->drawFrame(xp-1, y - char_height* 1.35 -1, char_width*nr_rolling_digits, char_height*1.8 +1); // checker box
			ucg->setClipRange(xp, y - char_height * 1.35, char_width*nr_rolling_digits-1, char_height * 1.8 ); // space to get 2 digits displayed uncut
			ucg->setPrintPos(xp, y - m - char_height);
			char tmp[32];
			sprintf(tmp, "%0*u", nr_rolling_digits, abs((lastdigit+(sign*used_quant))%mod) );
			// ESP_LOGI(FNAME,"tmp0 %s ld: %d", tmp, (lastdigit+(sign*used_quant))%mod );
			ucg->print(tmp); // one above
			ucg->setPrintPos(xp, y - m);
			sprintf(tmp, "%0*u", nr_rolling_digits, lastdigit);
			// ESP_LOGI(FNAME,"tmp1 %s ld: %d", tmp, lastdigit );
			ucg->print(tmp);
			ucg->setPrintPos(xp, y - m + char_height);
			// ESP_LOGI(FNAME,"Last %f/%d: %f m%d .%d ldc:%d mod:%d", altitude, alt, fraction, m, lastdigit, ((lastdigit+mod-(sign*used_quant))%mod), mod );
			sprintf(tmp, "%0*u", nr_rolling_digits, abs((lastdigit+mod-(sign*used_quant))%mod));
			// ESP_LOGI(FNAME,"tmp2 %s ld: %d rd:%d s:%d aq:%d las:%d ", tmp, (lastdigit-(sign*used_quant))%mod, nr_rolling_digits, sign, used_quant, lastdigit );
			ucg->print(tmp); // one below
			fraction_prev = fraction;

			// Roll leading digit independant of quant setting in 2 * (mod/10) range
			int lead_quant = 2 * base; // eg. 2 for Q=1 and Q=5
			int rollover = ((int)(alt_f)%mod)/base;
			if ( (rollover < 1 && alt_leadpart != 0) || (rollover > 8) ) { // [9.1,..,0.9]: roll-over needs clarification on leading digit
				// Re-Quantized leading altitude part
				fraction = (float)((int)((alt_f+base)*10)%(mod*10)) / (lead_quant*10);
				int16_t m = sign * fraction * char_height; // to pixel offest
				int lead_digit = ((alt+lead_quant)/mod)%10;
				// ESP_LOGI(FNAME,"Lead %f/%d: %f - %f m%d %d.", altitude, alt_leadpart, fraction, m, lead_digit);
				nr_rolling_digits++; // one less digit remains to print
				xp -= char_width; // one to the left
				// ucg->undoClipRange();
				// ucg->drawFrame(xp-1, y - char_height-1, char_width+1, char_height+1);
				ucg->setClipRange(xp, y - char_height, char_width-1, char_height-1);
				ucg->setPrintPos(xp, y + m - char_height);
				ucg->print(lead_digit); // one above
				ucg->setPrintPos(xp, y + m );
				ucg->print((lead_digit+9)%10);
				// ESP_LOGI(FNAME,"ld4: %d", (lead_digit+9)%10 );
				s[len-1] = '\0'; len--; // chop another digits
			}
			ucg->undoClipRange();
			ret=true;
		}
		ucg->setPrintPos(x - ucg->getStrWidth(s) - nr_rolling_digits*char_width , y);
		static char altpart_prev_s[32] = "";
		if (dirty || strcmp(altpart_prev_s, s) != 0 ) {
			ucg->print(s);
			// ESP_LOGI(FNAME,"s5: %s", s );
			strcpy(altpart_prev_s, s);
		}
	}
	if ( incl_unit ) {
		// unit todo needs further refinment to not draw the unit every time to just have the QNH updated
		ucg->setFont(ucg_font_fub11_hr, true);
		ucg->setColor( COLOR_HEADER );
		ucg->setPrintPos(x+5, y-3);
		const char * dmode;
		if( alt_display_mode.get() == MODE_QFE )
			dmode = "QFE";
		else if( alt_display_mode.get() == MODE_QNH )
			dmode = "QNH";
		else
			dmode = "";
		sprintf(s, "%s %s ", Units::AltitudeUnit(unitalt), dmode );  // e.g. ' m QNH'
		ucg->print(s);
		// QNH
		int16_t qnh_x = x+25;
		float qnh = QNH.get();
		if( gflags.standard_setting == true ){
			qnh = 1013;
		}
		if( qnh_unit.get() == QNH_INHG )
			sprintf(s, "%.2f ", Units::Qnh(qnh));
		else
			sprintf(s, "%d ", Units::QnhRounded(qnh));
		ucg->setPrintPos(qnh_x, y-19);
		ucg->setColor( COLOR_WHITE );
		ucg->print(s);
	}
	return ret;
}

// Accepts speed in kmh IAS/TAS, translates into configured unit
// right-aligned to value in 25 font size, no unit
void IpsDisplay::drawSmallSpeed(float v, int16_t x, int16_t y)
{
	int airspeed = (int)roundf(v);
	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub14_hr, true);
	char s[32];
	sprintf(s,"  %3d",  airspeed ); //additional blank to avoid artefacts
	ucg->setPrintPos(x-ucg->getStrWidth(s), y);
	ucg->print(s);
}

// Accepts speed in kmh IAS/TAS, translates into configured unit
// set dirty, when obscured from vario needle
// right-aligned to value, unit optional behind
// leant to display as well slip angle
// TBD: Make this place a general field capable to display other values as well
//      For now this is hijacked for slip angle tests
bool IpsDisplay::drawSpeed(float v_kmh, int16_t x, int16_t y, bool dirty, bool inc_unit)
{
	if( _menu ) return false;
	int airspeed = Units::AirspeedRounded(v_kmh);

	dirty = dirty || as_prev != airspeed;
	if ( ! dirty ) return false;
	// ESP_LOGI(FNAME,"draw airspeed %f %d", v_kmh, as_prev );

	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub25_hn, true);

	char s[32];
	if( airspeed_mode.get() != MODE_SLIP )
		sprintf(s,"  %3d",  airspeed );
	else
		sprintf(s,"  %.1f", -slipAngle );  // Slip Angle
	ucg->setPrintPos(x-ucg->getStrWidth(s), y);
	ucg->print(s);
	if ( inc_unit ) {
		ucg->setFont(ucg_font_fub11_hr);
		ucg->setColor( COLOR_HEADER );
		ucg->setPrintPos(x+5,y-3);
		if( airspeed_mode.get() != MODE_SLIP )
			ucg->print(Units::AirspeedUnitStr() );
		else
			ucg->print("deg");
		ucg->setPrintPos(x+5,y-17);
		ucg->print(Units::AirspeedModeStr());
	}
	as_prev = airspeed;
	speed_dirty = false;
	return true;
}


// ----------- Horizon Display -------------

#define WIDTH_2  (DISPLAY_W/2)        // 120
#define HEIGHT_2 (DISPLAY_H/2)        // 160
#define WIDTH_   (DISPLAY_W-1)        // 239
#define HEIGHT_  (DISPLAY_H-1)        // 319
#define AHRS_TOP (HEIGHT_2-WIDTH_2)   //  40
#define AHRS_BOT (HEIGHT_2+WIDTH_2)-1 // 279

const static ucg_color_t skycolor[4] = {{COLOR_DSKY},   {COLOR_LSKY},   {COLOR_LSKY},   {COLOR_BLACK}};
const static ucg_color_t gndcolor[4] = {{COLOR_DGROUND},{COLOR_LGROUND},{COLOR_LGROUND},{COLOR_BLACK}};
const static ucg_color_t hzncolor[4] = {{COLOR_WHITE},  {COLOR_BLACK},  {COLOR_WHITE},  {COLOR_WHITE}};
// >>> may want to change skycolor[3] to dark grey or dark blue? but all black avoids "debris".
static const ucg_color_t *sky_color;
static const ucg_color_t *gnd_color;
static const ucg_color_t *hzn_color;

void IpsDisplay::HznSetColor( const ucg_color_t *color ) {
	ucg->setColor( (color->color)[0], (color->color)[1], (color->color)[2] );
}

static int p5 = 0;
static int p10;
static int p15;
static int p20;
static int p25;
static int p30;

static float pitch_offset = 0;   // reflects the NG horizon_offset

// current and previous horizon state variables
static int hzn_x0, hzn_x1, hzn_y0, hzn_y1;
static int old_x0, old_x1, old_y0, old_y1;
static int pitchpixels, oldpitchpixels;
static bool pitch_ticks_drawn = false;
static bool bank_ticks_drawn = false;
// these are sin() & cos() of the bank angle, as integers scaled 256x
static int sin_bank = 0;
static int cos_bank = 0x100;
static int old_sin_bank = 0;
static int old_cos_bank = 0x100;

static float limited_pitch( float p ) {
	// limit to about 40 degrees
	if (p > 0.7)  return  0.7;
	else if (p < -0.7)  return -0.7;
	else  return p;
}

static float limited_bank( float b ) {
	// limit to a bit under 90 degrees
	if (b > 1.56)  return 1.56;
	else if (b < -1.56)  return -1.56;
	else  return b;
}

static float approxsin( float x ) {
	//ESP_LOGI(FNAME,"approxsin(%f)", x);
	if (x > 0) {
		if (x > 2.3562)                      // x > 135 degrees (but < 180)
			return approxsin( 3.1416-x );   // now x < 45
		else if (x > 0.7854)                // x > 45 degrees
			return approxcos( 1.5708-x );   // approxcos() more accurate
	} else {
		if (x < -2.3562)
			return approxsin( -3.1416-x );
		else if (x < -0.7854)
			return -approxcos( x+1.5708 );
	}
	return (x*(1-0.1667*x*x));
}

static float approxcos( float x ) {
	//ESP_LOGI(FNAME,"approxcos(%f)", x);
	if (x > 0) {
		if (x > 2.3562)  // 135 degrees
			return -approxcos( 3.1416-x );
		if (x > 0.7854)  // 45 degrees
			return approxsin( 1.5708-x );
	} else {
		if (x < -2.3562)  // -135 degrees
			return -approxcos( x+3.1416 );
		if (x < -0.7854)  // -45 degrees
			return approxsin( x+1.5708 );
	}
	x = 0.5*x*x;
	return (1.0 - x + 0.1667*x*x);
}

static int pitch2pixels( float p ) {
	if( pitch_offset != 0 )
		p += D2R(pitch_offset);
	// Move center of horizon up or down by approximately sin(pitch)
	// The 2.3 is a visual exaggeration factor
	float sp = approxsin( p );
	//ESP_LOGI(FNAME,"approxsin(%f) = %f", p, sp);
	return( (int)((WIDTH_2 * 2.3) * sp) );
}

// Draw a pair of lines, after checking that they are in display range
// The "which" parameter allows changing colors in-between the two lines.
// Try and draw the second line "below" the first one (towards ground).
void IpsDisplay::double_line( int x1, int y1, int x2, int y2, int which ) {
	xSemaphoreTake(spiMutex, portMAX_DELAY );
	if ( x1 >= 0 && x1 <= WIDTH_
	  && x2 >= 0 && x2 <= WIDTH_
	  && y1 >= AHRS_TOP && y1 <= AHRS_BOT
	  && y2 >= AHRS_TOP && y2 <= AHRS_BOT) {
		if( which != 1 ) {   // 0 or 2
			ucg->drawLine( x1,y1, x2,y2 );
		}
		if( which != 0 ) {   // 1 or 2
			// draw another line next to it to make it thicker
			if( abs(y2-y1) > abs(x2-x1) ) {  // more vertical than horizontal
//				if( y1 > y2 && x2 < WIDTH_ )
//					ucg->drawLine( ++x1,y1, ++x2,y2 );
//				else if( y1 < y2 && x1 > 0 )
//					ucg->drawLine( --x1,y1, --x2,y2 );
// instead let eglib do the clipping for a change
				if( y1 > y2 )
					ucg->drawLine( ++x1,y1, ++x2,y2 );
				else if( y1 < y2 )
					ucg->drawLine( --x1,y1, --x2,y2 );

			} else {
				if( y1 < AHRS_BOT && y2 < AHRS_BOT )
					ucg->drawLine( x1,++y1, x2,++y2 );
			}
		}
	}
	xSemaphoreGive(spiMutex);
}

// Draw (or erase) the pitch ticks parallel to the horizon
// Note: horizon, and ticks, move when pitch offset is changed
void IpsDisplay::pitch_tick( bool draw, bool major, int pt ) {
	int x0, y0, x1, y1, sb, cb, pp;
	if (draw) {
		x0 = hzn_x0;
		x1 = hzn_x1;
		y0 = hzn_y0;
		y1 = hzn_y1;
		sb = sin_bank;
		cb = cos_bank;
		pp = pitchpixels;
	} else {          // erase old tick
		x0 = old_x0;
		x1 = old_x1;
		y0 = old_y0;
		y1 = old_y1;
		sb = old_sin_bank;
		cb = old_cos_bank;
		pp = oldpitchpixels;
	}
	// point on horizon that is abeam the middle of the display/airplane:
//	int x = WIDTH_2  + ((pp*sb)>>8);
//	int y = HEIGHT_2 + ((pp*cb)>>8);
	// move it "up" (perpendicular to horizon) by pt pixels:
//	x -= ((pt*sb)>>8);
//	y -= ((pt*cb)>>8);
	// can combine the two calculations
	int x = WIDTH_2  + (((pp-pt)*sb)>>8);
	int y = HEIGHT_2 + (((pp-pt)*cb)>>8);
	// alternative for pitch ticks: zero on the airplane, read value on horizon:
	//int x = WIDTH_2  + ((pt*sb)>>8);
	//int y = HEIGHT_2 + ((pt*cb)>>8);
	int w = (x1-x0);    // <<< this width is variable
	int v = (y1-y0);
//	if( abs(w)+abs(v) < WIDTH_2/4 )    // horizon line very close to corner
//		return;
	w >>= (major?4:5);     // half-width of tick projected along horizontal axis
	v >>= (major?4:5);     // half-width of tick projected along vertical axis
	if (draw)
		HznSetColor( hzn_color );
	// Now that we moved the erase back to just before the draw,
	// use color based on which side of the *new* horizon we are:
	else if ((x-hzn_x0)*(hzn_y1-hzn_y0) > (y-hzn_y0)*(hzn_x1-hzn_x0))
		HznSetColor( sky_color );
	else
		HznSetColor( gnd_color );
	double_line( x-w,y-v, x+w,y+v );
//	if ((sb > 170 || sb < -170)) {
//		ESP_LOGI(FNAME,"pitch tick %d y0=%d y1=%d sb=%d cb=%d", draw, y0, y1, sb, cb );
//		ESP_LOGI(FNAME,"... pt=%d pp=%d %d,%d - %d,%d", pt, pp, x-w,y-v, x+w,y+v );
//	}
}

bool IpsDisplay::pitch_ticks( bool draw ) {
	bool drawn = false;
	int pp;
	if (draw)
		pp = pitchpixels;
	else
		pp = oldpitchpixels;
	if ( pp > 15 ) {              //  3-degree threshold
		// draw ticks above the horizon
		drawn = true;
		pitch_tick( draw, false,   p5 );   //  5 deg minor tick
	if ( pp > p5 ) {
		pitch_tick( draw, true,   p10 );   // 10 deg major tick
	if ( pp > p10 ) {
		pitch_tick( draw, false,  p15 );   // 15 deg minor tick
	if ( pp > p15 ) {
		pitch_tick( draw, true,   p20 );   // 20 deg major tick
	if ( pp > p20 ) {
		pitch_tick( draw, false,  p25 );   // 25 deg minor tick
	if ( pp > p25 )
		pitch_tick( draw, true,   p30 );   // 30 deg major tick
	}}}}} else
	if ( pp < -15 ) {
		// draw ticks below the horizon
		drawn = true;
		pitch_tick( draw, false,  -p5 );   //  5 deg minor tick
	if ( pp < -p5 ) {
		pitch_tick( draw, true,  -p10 );   // 10 deg major tick
	if ( pp < -p10 ) {
		pitch_tick( draw, false, -p15 );   // 15 deg minor tick
	if ( pp < -p15 ) {
		pitch_tick( draw, true,  -p20 );   // 20 deg major tick
	if ( pp < -p20 ) {
		pitch_tick( draw, false, -p25 );   // 15 deg minor tick
	if ( pp < -p25 ) 
		pitch_tick( draw, true,  -p30 );   // 20 deg major tick
	}}}}}
	return drawn;
}

// Stages of erasing and drawing bank ticks:
//   0 - early - erase old ticks that cross new horizon
//   1 - late - erase old ticks that do not cross new horizon
//   2 - draw new ticks that do not cross new horizon
//   3 - replaces 0,1,2 sometimes - erase all old ticks, do not draw new

bool IpsDisplay::bank_tick( int stage, int x1, int y1, int x2, int y2 ) {
	//ESP_LOGI(FNAME,"bank_tick( stage=%d, y=%d )", stage, y1 );
	bool side1, side2;
	// extend past tick ends to be sure double-line does not cross
	int xx1, yy1, xx2, yy2;
	if (x1 < x2) {
		xx1 = x1 - 1;
		xx2 = x2 + 1;
	} else {
		xx1 = x1 + 1;
		xx2 = x2 - 1;
	}
	if (y1 < y2) {
		yy1 = y1 - 1;
		yy2 = y2 + 1;
	} else {
		yy1 = y1 + 1;
		yy2 = y2 - 1;
	}
	if (stage != 2) {    // erasing
		// determine whether tick crosses *old* horizon line
		side1 = ((xx1-old_x0)*(old_y1-old_y0) > (yy1-old_y0)*(old_x1-old_x0));
		side2 = ((xx2-old_x0)*(old_y1-old_y0) > (yy2-old_y0)*(old_x1-old_x0));
		if (side1 != side2) {   // ends of tick are on opposite sides of old horizon
			//ESP_LOGI(FNAME,"... old rejected");
			return false;    // old tick was not drawn, no need to erase
		}
		if( stage == 0 || stage == 3 ) {    // early erasing, color based on old horizon
			if( side1 )
				HznSetColor( sky_color );
			else
				HznSetColor( gnd_color );
		}
	} else {   // stage == 2, drawing
		HznSetColor( hzn_color );
	}
	// determine whether tick crosses *new* horizon line
	side1 = ((xx1-hzn_x0)*(hzn_y1-hzn_y0) > (yy1-hzn_y0)*(hzn_x1-hzn_x0));
	side2 = ((xx2-hzn_x0)*(hzn_y1-hzn_y0) > (yy2-hzn_y0)*(hzn_x1-hzn_x0));
	if (side1 != side2) {    // ends of tick are on opposite sides of new horizon
		if (stage == 1) {     // should have been erased in stage 0
			//ESP_LOGI(FNAME,"... presumed done earlier");
			return false;
		} else if (stage == 2) {
			//ESP_LOGI(FNAME,"... new rejected");
			return false;      // do not draw
		}
	} else {                    // tick does not cross new horizon
		if (stage == 0) {
			//ESP_LOGI(FNAME,"... postponed");
			return false;      // erase it later in stage 1
		}
		// if stage == 3 (early & will not draw new) then do not postpone
	}
	if ( stage == 1 ) {     // late erasing, color based on new horizon position
		if( side1 )
			HznSetColor( sky_color );
		else
			HznSetColor( gnd_color );
	}
	double_line( x1, y1, x2, y2 );
	return true;
}

bool IpsDisplay::ticks_bank( int stage, int bank ) {
	int sinbb, cosbb, sbt;
	bool major;
	if (bank == 15) {
		sinbb = 66;    // 256*sin(15deg)
		cosbb = 247;   // 256*cos(15deg)
		sbt = 33;      // threshold sin(current bank) for drawing this tick
		major = false;
	} else if (bank == 30) {
		sinbb = 128;   // 256*sin(30deg)
		cosbb = 222;   // 256*cos(30deg)
		sbt = 96;
		major = true;
	} else if (bank == 45) {
		sinbb = 181;   // 256*sin(45deg)
		cosbb = 181;   // 256*cos(45deg)
		sbt = 156;
		major = false;
	} else {   // 60
		sinbb = 219;   // 256*sin(60deg)
		cosbb = 116;   // 256*cos(60deg)
		sbt = 202;
		major = true;
	}
	int sb, cb;
	if( stage == 2 ) {      // drawing new
		sb = sin_bank;
		cb = cos_bank;
	} else {                // erasing old
		sb = old_sin_bank;
		cb = old_cos_bank;
	}
	int abs_sb = abs(sb);
	if ( abs_sb <= sbt ) {   // do not show ticks until current bank > threshold
		return false;
	}
	int m = WIDTH_2;        // draw the bank ticks around the airplane in the center
	int n = HEIGHT_2;
	int h = WIDTH_2;
	// use trig formulas for sin & cos of (bb-b), arranged for integer math
	int w4 = ((h * ((cb*cosbb + abs_sb*sinbb)>>8))>>8);   // horizontal size of tick (>0)
	int z4 = ((h * ((cb*sinbb - abs_sb*cosbb)>>8))>>8);   // vertical offset of tick
	int w3, z3;
	if ( major ) {
		// ticks from 3/4-radius to full radius of circle
		w3 = w4 - (w4>>2);
		z3 = z4 - (z4>>2);
	} else {
		// ticks from 7/8-radius to full radius of circle
		w3 = w4 - (w4>>3);
		z3 = z4 - (z4>>3);
	}
	// Only draw ticks in the 2 relevant quadrants out of 4
	bool drawn1 = false;
	bool drawn2 = false;
	if( sb > 0 ) {
		drawn1 = bank_tick( stage, m-w3,n-z3, m-w4,n-z4 );
		drawn2 = bank_tick( stage, m+w3,n+z3, m+w4,n+z4 );
	}
	if( sb < 0 ) {
		drawn1 = bank_tick( stage, m-w3,n+z3, m-w4,n+z4 );
		drawn2 = bank_tick( stage, m+w3,n-z3, m+w4,n-z4 );
	}
	return (drawn1 || drawn2);
}

bool IpsDisplay::bank_ticks( bool drawn, bool draw, bool early, int bank ) {
	if (early) {
		if (drawn) {
			if ( !draw ) {
				(void) ticks_bank( 3, bank );    // erase all old early
				drawn = false;                   // skip the late stages
			} else {
				(void) ticks_bank( 0, bank );    // erase some old
			}
		}
	} else {
		if (drawn) {
			(void) ticks_bank( 1, bank );        // erase rest of old
			drawn = false;
		}
		if (draw)
			drawn = ticks_bank( 2, bank );       // draw new
	}
	return drawn;
}

// ticks for 15, 30, 45 and 60 degrees bank
bool IpsDisplay::banks_ticks( bool drawn, bool draw, bool early ) {
	if ( !drawn && !draw )
		return false;
	bool drawn15 = bank_ticks( drawn, draw, early, 15 );
	bool drawn30 = bank_ticks( drawn, draw, early, 30 );
	bool drawn45 = bank_ticks( drawn, draw, early, 45 );
	bool drawn60 = bank_ticks( drawn, draw, early, 60 );
	//ESP_LOGI(FNAME,"banks_ticks() drawn: %d %d %d %d", drawn15, drawn30, drawn45, drawn60 );
	return (drawn15 || drawn30 || drawn45 || drawn60);   // at least one tick was drawn
	// note that higher-bank tick may be drawn while lower skipped due to crossing horizon
}

// draw a simple "airplane" icon, scaled to use 3/4 of the display width
void IpsDisplay::airplane_icon( bool draw ) {
	//ESP_LOGI(FNAME,"draw airplane icon...");
	// The following variables are used to mark the unsafe zone, where other objects
	//   may have been painted over when the horizon was drawn.
	int k0 = ((old_y0 < hzn_y0)? old_y0 : hzn_y0);
	int k1 = ((old_y1 < hzn_y1)? old_y1 : hzn_y1);
	int g0 = ((old_y0 > hzn_y0)? old_y0 : hzn_y0);
	int g1 = ((old_y1 > hzn_y1)? old_y1 : hzn_y1);
	int kmid = (k0+k1)>>1;
	int gmid = (g0+g1)>>1;
	int m = WIDTH_2;
	int n = HEIGHT_2;
	int w, z, y;
	if( horizon_largeicon.get() ) {
		w = DISPLAY_W/4 + DISPLAY_W/8;   // half wingspan
		z = w>>2;           // half-width of horizontal tail
		y = n - 3 - z;      // Y-position of top of vertical tail
	} else {
		w = DISPLAY_W/4;
		z = 0;              // no horizontal tail
		y = n - 3 - DISPLAY_W/10;
	}
	// skip redraw if likely no overlap with redrawn sky & ground parts
	// overlaps if k above bottom (largest y) & g below top (smallest y)
	HznSetColor( hzn_color );
	xSemaphoreTake(spiMutex, portMAX_DELAY );
	if ( draw || ((k0 < n+3 || kmid < n+3) && (g0 > n-3 || gmid > n-3)) )
		ucg->drawTetragon( m-w,n+3, m-w,n-3, m-10,n-3, m-10,n+3 );  // left wing
	if ( draw || ((k1 < n+3 || kmid < n+3) && (g1 > n-3 || gmid > n-3)) )
		ucg->drawTetragon( m+10,n+3, m+10,n-3, m+w,n-3, m+w,n+3 );  // right wing
	if ( draw || pitch_ticks_drawn || ( kmid < n && gmid+3 > y ))
		ucg->drawTetragon( m-3,n-9, m-3,y, m+3,y, m+3,n-9 );        // v tail
	if ( draw || pitch_ticks_drawn || (z && (k0<y || k1<y) && (g0>y-6 || g1>y-6)))
		ucg->drawTetragon( m-z,y, m-z,y-6, m+z,y-6, m+z,y );        // h tail
	if ( draw || ( kmid < n+15 && gmid >= n-15 ) || pitch_ticks_drawn) {
		ucg->drawCircle( m, n,  9, UCG_DRAW_ALL );
		ucg->drawCircle( m, n, 10, UCG_DRAW_ALL );                  // fuselage
	}
	xSemaphoreGive(spiMutex);
}

bool calc_horizon( int sb, int cb, int& x0, int& y0, int& x1, int& y1 ) {
	// prepare to draw new horizon line
	//ESP_LOGI(FNAME,"prepare to draw new horizon");
	int m = WIDTH_2;
	int n = WIDTH_2;  // this function lives in a square world
	if ( cb <= 0 )   cb = 256;
	// Vertical position of horizon at center of width after offsetting for pitch:
	// Divide by cos(bank) since "pitch" is defined as perpendicular to ground
	int pc = (pitchpixels<<8) / cb;
	int y = n + pc;
	// move ends of horizon up or down by tan(bank)
	int h = (WIDTH_2 * sb) / cb;
	// ensure some sky and some ground even with offset
	bool wrongpitch = false;
	//ESP_LOGI(FNAME,"calc_horizon() y=%d h=%d", y, h );
	int abs_h = abs(h);
	if (y-abs_h > (WIDTH_-15)) {     // y > WIDTH_-15 + abs(h)
		y = (WIDTH_-15) + abs_h;
		wrongpitch = true;
	} else
	if (y+abs_h < 15) {              // y < 15 - abs(h)
		y = 15 - abs_h;
		wrongpitch = true;
	}
	y0 = y + h;      // left end of horizon line, lower if h>0 i.e. bank>0
	y1 = y - h;      // right end of horizon line
	x0 = 0;
	x1 = WIDTH_;
	//ESP_LOGI(FNAME,"calc_horizon() y0=%d y1=%d", y0, y1 );
	// In steep bank y0,y1 may be outside the display square Y range,
	// Compute where horizon touches top and/or bottom edges of square instead
	// if (h=0) h=m;  - in any case don't crash on a division-by-0
	if (y0 < 0) {
		x0 = m - (m*(m+pc))/(h?-h:m);   // h<0
		//ESP_LOGI(FNAME,"top-left y0=%d -> x0=%d, pc=%d h=%d", y0, x0, pc, h );
		y0 = 0;
	} else if (y0 > WIDTH_) {
		x0 = m - (m*(m-pc))/(h?h:m);    // h>0
		//ESP_LOGI(FNAME,"bot-left y0=%d -> x0=%d, pc=%d h=%d", y0, x0, pc, h );
		y0 = WIDTH_;
	}
	if (y1 < 0) {
		x1 = m + (m*(m+pc))/(h?h:m);    // h>0
		//ESP_LOGI(FNAME,"top-right y1=%d -> x1=%d, pc=%d h=%d", y1, x1, pc, h );
		y1 = 0;
	} else if (y1 > WIDTH_) {
		x1 = m + (m*(m-pc))/(h?-h:m);   // h<0
		//ESP_LOGI(FNAME,"bot-right y1=%d -> x1=%d, pc=%d h=%d", y1, x1, pc, h );
		y1 = WIDTH_;
	}
	// ensure still some sky and some ground (corner can shrink to 0)
	if (x0 > WIDTH_ - 25)
		x0 = WIDTH_ - 25;
	else
	if (x1 < 25)
		x1 = 25;
	return wrongpitch;
}

// erase the thin line between ground and sky
//  - need elaborate procedure to avoid leaving "debris"
void IpsDisplay::erase_horizon_line( bool up0, bool up1 ) {
	if (up0 && up1) {             // the whole line moved up
		HznSetColor( gnd_color );
		double_line( old_x0,old_y0, old_x1,old_y1 );
	} else if (!up0 && !up1) {    // the whole line moved down
		HznSetColor( sky_color );
		double_line( old_x0,old_y0, old_x1,old_y1 );
	} else {
		// Old and new horizon lines crossed:
		// use different colors for the two parts of the line.
		// Try and compute the crossing point (xc,yc).
		// This simplified method is not exact when one end of the line
		// is on a side and the other is on a top or bottom - it is never
		// exact anyway: small changes in x and/or y and integer math.
		int sum = abs(hzn_y0-old_y0) + abs(hzn_x0-old_x0);
		int ratio = (sum << 8);
		sum += abs(hzn_y1-old_y1) + abs(hzn_x1-old_x1);
		// sum should always be > 0 since something moved, but guard anyway:
		ratio /= (sum?sum:256);
		int xc, yc;
		// Calling drawLine() for part of the line misses some pixels.
		// Instead, paint the whole line twice, once in each color,
		// but use clip range to limit where the paint sticks.
		int xdiff = abs(old_x1-old_x0);
		int ydiff = abs(old_y1-old_y0);
		if (xdiff > ydiff) {         // line more horizontal than vertical
			xc = old_x0 + (((old_x1-old_x0)*ratio) >> 8);
			ucg->setClipRange( 0, AHRS_TOP, xc, DISPLAY_W );              // left part
			if (up0)
				HznSetColor( gnd_color );
			else
				HznSetColor( sky_color );
		} else {             // more vertical
			yc = old_y0 + (((old_y1-old_y0)*ratio) >> 8);
			ucg->setClipRange( 0, AHRS_TOP, DISPLAY_W, yc - AHRS_TOP );   // top part
			if ( (old_y0 > old_y1)? up1 : up0 )
				HznSetColor( gnd_color );
			else
				HznSetColor( sky_color );
		}
		double_line( old_x0,old_y0, old_x1,old_y1 );
		if (xdiff > ydiff) {
			ucg->setClipRange( xc, AHRS_TOP, DISPLAY_W - xc, DISPLAY_W ); // right part
			if (up1)
				HznSetColor( gnd_color );
			else
				HznSetColor( sky_color );
		} else {
			ucg->setClipRange( 0, yc, DISPLAY_W, (AHRS_BOT+1) - yc );     // bottom part
			if ( (old_y0 > old_y1)? up0 : up1)
				HznSetColor( gnd_color );
			else
				HznSetColor( sky_color );
		}
		double_line( old_x0,old_y0, old_x1,old_y1 );
		ucg->undoClipRange();
	}
}

void IpsDisplay::drawHorizon( float p, float b, float yaw ){   // ( pitch, roll, yaw )

	tick++;
//	if ( (tick&0x07) != 0 )    // redraw horizon only every 160 ms
	if ( (tick&0x03) != 0 )    // potentially redraw horizon every 80 ms
		return;

	static int horizon_done = 0;

	if( !(screens_init & INIT_DISPLAY_HORIZON) ){
		clear();
		horizon_done = 0;
		screens_init |= INIT_DISPLAY_HORIZON;
		int hc = horizon_colors.get();
		sky_color = &skycolor[hc];
		gnd_color = &gndcolor[hc];
		hzn_color = &hzncolor[hc];
		return;    // yield for now, will draw sky & ground on the next call
	}

	if (horizon_done == 0) {

		// paint AHRS square, half dark sky and half dark ground:
		xSemaphoreTake(spiMutex, portMAX_DELAY );
		HznSetColor( sky_color );
		ucg->drawTetragon( 0,AHRS_TOP, 0,HEIGHT_2, WIDTH_,HEIGHT_2, WIDTH_,AHRS_TOP );
		xSemaphoreGive(spiMutex);
		vTaskDelay(2);
		xSemaphoreTake(spiMutex, portMAX_DELAY );
		HznSetColor( gnd_color );
		ucg->drawTetragon( 0,AHRS_BOT, 0,HEIGHT_2, WIDTH_,HEIGHT_2, WIDTH_,AHRS_BOT );
		xSemaphoreGive(spiMutex);
		// if dark theme, add thin brighter line at horizon:
		if (horizon_colors.get() == WHITE_ON_DARK || horizon_colors.get() == WHITE_ON_BLACK) {
			HznSetColor( hzn_color );
			double_line( 0,HEIGHT_2, WIDTH_,HEIGHT_2 );
		}
		//vTaskDelay(2);
		if (p5 == 0) {
			pitch_offset = 0;
			p5  = pitch2pixels( D2R(5) );
			p10 = pitch2pixels( D2R(10) );
			p15 = pitch2pixels( D2R(15) );
			p20 = pitch2pixels( D2R(20) );
			p25 = pitch2pixels( D2R(25) );
			p30 = pitch2pixels( D2R(30) );
		}
		hzn_x0 = 0;
		hzn_x1 = WIDTH_;
		hzn_y0 = HEIGHT_2;
		hzn_y1 = HEIGHT_2;
		old_x0 = 0;
		old_x1 = WIDTH_;
		old_y0 = HEIGHT_2;
		old_y1 = HEIGHT_2;
		pitchpixels = 0;
		oldpitchpixels = 0;
		horizon_done = 1;
		//ESP_LOGI(FNAME,"horizon_done = 1");

		airplane_icon( true );

		if( !gflags.ahrsKeyValid ) {
			ucg->setFont(ucg_font_fub14_hn, true);
			ucg->setPrintPos(30,310);
			ucg->setColor( COLOR_BRED );
			ucg->print( " AHRS disabled " );
			return;
		}

		// yield for now, draw real horizon etc on next call
		return;
	}

	if( !gflags.ahrsKeyValid )    // static demo does not change
		return;

	// periodically clean up and print some numbers

	if ( (tick&0x1F) == 0 ) {

		// completely redraw the airplane icon periodically
		airplane_icon( true );

		// display heading (or course), if possible
		//ESP_LOGI(FNAME,"about to draw heading");
		static int heading_old = -1;
		int heading = 999;
		int headingtype = 0;
		if( compass_enable.get() != CS_DISABLE ) {
			heading = static_cast<int>(rintf(mag_hdt.get()));
			headingtype = 1;
		} else if( Flarm::gpsStatus() ) {
			heading = static_cast<int>(rintf(Flarm::getGndCourse()));
			headingtype = 2;
		}
		ucg->setColor( COLOR_WHITE );
		if( heading != heading_old ){
			ucg->setFont(ucg_font_fub20_hr, true);
			ucg->setPrintPos(60,318);
			if( headingtype == 0 ) {
				ucg->print( "      ---°  " );
			} else {
				if( heading <= 0 )
					heading += 360;
				else if( heading > 360 )
					heading -= 360;
				if (headingtype == 1)
					ucg->printf( " hdg  %03d°  ", heading );
				else
					ucg->printf( " crs  %03d°  ", heading );
				heading_old = heading;
			}
		}

		// also print pitch and bank as numbers at top
		ucg->setFont(ucg_font_fub14_hn, true);
//		ucg->setColor( COLOR_WHITE );
		ucg->setPrintPos(10,38);
		ucg->printf( "b %2.0f  ", 57.296 * b );
		ucg->setPrintPos(170,38);
		ucg->printf( "p %4.1f  ", 57.296 * p );

		// show a warning about pitch offset being applied
//		ucg->setFont(ucg_font_fub14_hn, true);
		ucg->setColor( COLOR_BRED );
		ucg->setPrintPos(215,315);
		if (pitch_offset > 0)
			ucg->print( "^" );
		else if (pitch_offset < 0)
			ucg->print( "v" );
		else  // == 0
			ucg->print( "  " );

		return;     // skip drawing horizon & ticks until next call
	}

	// force redraw horizon if pitch offset has been changed
	if ( horizon_offset.get() != pitch_offset ) {
		pitch_offset = horizon_offset.get();
		horizon_done = 1;
	}

	// if < 1 pixel change, wait until further change before processing
	// (about 1/2 degree bank or 1/4 degree pitch)
	static float old_p = 0;
	static float old_b = 0;
	float b_ = limited_bank(b);
	float p_ = limited_pitch(p);
	if ( horizon_done == 2 ) {
		if ( fabs(old_p-p_) < 0.004 && fabs(old_b-b_) < 0.009 )
			return;
	}
	horizon_done = 2;
	old_p = p_;
	old_b = b_;
	bool drawticks = ( p_ == p );       // limited_pitch() did not reduce it

	// prepare to draw new horizon line
	//ESP_LOGI(FNAME,"prepare to draw new horizon");
	cos_bank = (int)(256 * approxcos(b_));
	//ESP_LOGI(FNAME,"approxcos(%f) = %f", b_, (float)cos_bank/256.0);
	sin_bank = (int)(256 * approxsin(b_));
	//ESP_LOGI(FNAME,"approxsin(%f) = %f", b_, (float)sin_bank/256.0);
	pitchpixels = pitch2pixels(p_);     // includes the offset
	int x0, y0, x1, y1;
	if ( b_ > 1.13 ) {
		// over about 65 degrees bank:
		// use calc_horizon in a world rotated 90 degrees:
		//ESP_LOGI(FNAME,"calling calc_horizon() 1");
		calc_horizon( -cos_bank, sin_bank, x0, y0, x1, y1 );
		hzn_x0 = y0;
		hzn_y0 = AHRS_BOT - x0;
		hzn_x1 = y1;
		hzn_y1 = AHRS_BOT - x1;
		drawticks = false;
	} else if ( b_ < -1.13 ) {
		//ESP_LOGI(FNAME,"calling calc_horizon() 2");
		calc_horizon( cos_bank, -sin_bank, x0, y0, x1, y1 );
		hzn_x0 = WIDTH_ - y0;
		hzn_y0 = AHRS_TOP + x0;
		hzn_x1 = WIDTH_ - y1;
		hzn_y1 = AHRS_TOP + x1;
		drawticks = false;
	} else {
		// under about 65 degrees, use the straightforward method:
		//ESP_LOGI(FNAME,"calling calc_horizon() 3");
		if( calc_horizon( sin_bank, cos_bank, x0, y0, x1, y1 ) )
			drawticks = false;
		//ESP_LOGI(FNAME,"... calc_horizon() returned");
		hzn_x0 = x0;
		hzn_y0 = AHRS_TOP + y0;
		hzn_x1 = x1;
		hzn_y1 = AHRS_TOP + y1;
	}

	// redraw only if change will be visible in pixel resolution
	if ( hzn_x0==old_x0 && hzn_x1==old_x1 && hzn_y0==old_y0 && hzn_y1==old_y1 )
		return;

	// find out in which direction each end of the horizon line moved
	// also set up yy0,yy1 for correct choice of top or bottom
	//   (only used if x0 != old_x0 or x1 != old_x1)
	int yy0 = AHRS_TOP;
	bool up0 = (hzn_y0 < old_y0);
	if (hzn_y0==AHRS_TOP || old_y0==AHRS_TOP) {   // may have passed the corner
		up0 = (up0 || hzn_x0 > old_x0);
		//ESP_LOGI(FNAME,"horiz passed top left corner, up0=%d", up0);
	} else
	if (hzn_y0==AHRS_BOT || old_y0==AHRS_BOT) {
		yy0 = AHRS_BOT;
		up0 = (up0 || hzn_x0 < old_x0);
		//ESP_LOGI(FNAME,"horiz passed bot left corner, up1=%d", up0);
	}
	int yy1 = AHRS_TOP;
	bool up1 = (hzn_y1 < old_y1);
	if (hzn_y1==AHRS_TOP || old_y1==AHRS_TOP) {
		up1 = (up1 || hzn_x1 < old_x1);
		//ESP_LOGI(FNAME,"horiz passed top right corner, up1=%d", up1);
	} else
	if (hzn_y1==AHRS_BOT || old_y1==AHRS_BOT) {
		yy1 = AHRS_BOT;
		up1 = (up1 || hzn_x1 > old_x1);
		//ESP_LOGI(FNAME,"horiz passed bot right corner, up1=%d", up1);
	}
	int x2, y2;
	if (up0 == up1) {
		x2 = old_x1;
		y2 = old_y1;
	} else {           // old and new horizon lines crossed
		x2 = hzn_x1;
		y2 = hzn_y1;
	}

	// first erase the thin line between ground and sky (dark themes only)
	if (horizon_colors.get() == WHITE_ON_DARK || horizon_colors.get() == WHITE_ON_BLACK) {
		erase_horizon_line( up0, up1 );
	}

	// erase now any existing bank tick marks that cross the new horizon
	// (if drawticks==false will erase all bank ticks now & return false)
	bank_ticks_drawn = banks_ticks( bank_ticks_drawn, drawticks, true );

	// Finally, redraw sky and ground for the new horizon position.
	// Paint only narrow triangles as needed to cover the change.
	// This algorithm may paint up to about double the actual changed area
	// when the lines cross, but is simpler than computing the crossing point
	// Re-arranged to always draw sky first, ground second.
	// The number of triangles actually drawn is never more than 4,
	// and only 2 if horizon hasn't crossed corners of display.
	//ESP_LOGI(FNAME,"about to draw triangles");

	xSemaphoreTake(spiMutex, portMAX_DELAY );
	HznSetColor( sky_color );
	if ( !up0 ) {
		if ( hzn_y0 != old_y0 )
			ucg->drawTriangle( x2,y2, 0,hzn_y0, 0,old_y0 );
		if ( hzn_x0 != old_x0 )
			ucg->drawTriangle( x2,y2, hzn_x0,yy0, old_x0,yy0 );
	}
	if ( !up1 ) {
		if ( hzn_y1 != old_y1 )
			ucg->drawTriangle( hzn_x0,hzn_y0, WIDTH_,hzn_y1, WIDTH_,old_y1 );
		if ( hzn_x1 != old_x1 )
			ucg->drawTriangle( hzn_x0,hzn_y0, hzn_x1,yy1, old_x1,yy1 );
	}
	HznSetColor( gnd_color );
	if ( up0 ) {
		if ( hzn_y0 != old_y0 )
			ucg->drawTriangle( x2,y2, 0,hzn_y0, 0,old_y0 );
		if ( hzn_x0 != old_x0 )
			ucg->drawTriangle( x2,y2, hzn_x0,yy0, old_x0,yy0 );
	}
	if ( up1 ) {
		if ( hzn_y1 != old_y1 )
			ucg->drawTriangle( hzn_x0,hzn_y0, WIDTH_,hzn_y1, WIDTH_,old_y1 );
		if ( hzn_x1 != old_x1 )
			ucg->drawTriangle( hzn_x0,hzn_y0, hzn_x1,yy1, old_x1,yy1 );
	}
	xSemaphoreGive(spiMutex);

	// late-erase bank ticks that do not cross the new horizon, and draw
	// new ticks immediately after erasing old ones, to minimize flicker:
	bank_ticks_drawn = banks_ticks( bank_ticks_drawn, drawticks, false );

	if ( pitch_ticks_drawn ) {
		// erase old pitch tickmarks - they never cross the horizon
		pitch_ticks( false );
		pitch_ticks_drawn = false;
	}
	if ( drawticks )
		pitch_ticks_drawn = pitch_ticks( true );

	// add thin more obvious line at horizon (dark themes only)
	if (horizon_colors.get() == WHITE_ON_DARK || horizon_colors.get() == WHITE_ON_BLACK) {
		HznSetColor( hzn_color );
		double_line( hzn_x0,hzn_y0, hzn_x1,hzn_y1 );
	}

	// redraw parts of airplane icon, only as needed
	airplane_icon( false );

	// store current values for future reference
	old_x0 = hzn_x0;
	old_x1 = hzn_x1;
	old_y0 = hzn_y0;
	old_y1 = hzn_y1;
	old_sin_bank = sin_bank;
	old_cos_bank = cos_bank;
	oldpitchpixels = pitchpixels;
}

// ----------- end of Horizon Display -------------


//////////////////////////////////////////////
// The load display

static float old_gmax = 100;
static float old_gmin = -100;
static float old_ias_max = -1;

void IpsDisplay::drawLoadDisplayTexts(){
	ucg->setFont(ucg_font_fub11_hr, true);
	ucg->setPrintPos(130,70);
	ucg->setColor(  COLOR_HEADER_LIGHT  );
	ucg->print( PROGMEM"MAX POS G" );
	ucg->setPrintPos(130,205);
	ucg->print( PROGMEM"MAX NEG G" );
	ucg->setPrintPos(130,260);
	ucg->printf( PROGMEM"MAX IAS %s", Units::AirspeedUnitStr() );
}

void IpsDisplay::initLoadDisplay(){
	if( _menu )
		return;
	ESP_LOGI(FNAME,"initLoadDisplay()");
	xSemaphoreTake(spiMutex,portMAX_DELAY);
	ucg->setColor( COLOR_HEADER );
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(20,20);
	ucg->print( PROGMEM"G-Force" );
	drawLoadDisplayTexts();
	int max_gscale = (int)( gload_pos_limit.get() )+1;
	if( -gload_neg_limit.get() >= max_gscale )
		max_gscale = (int)( -gload_neg_limit.get()  )+1;

	for( float a=gload_pos_limit.get()-1; a<max_gscale; a+=0.05 ) {
		drawOneScaleLine( ((float)a/max_gscale)*M_PI_2, 140, 150, 2, COLOR_RED );
	}
	for( float a=gload_neg_limit.get()-1; a>-max_gscale; a-=0.05 ) {
		drawOneScaleLine( ((float)a/max_gscale)*M_PI_2, 140, 150, 2, COLOR_RED );
	}
	for( float a=gload_pos_limit_low.get()-1; a<gload_pos_limit.get()-1; a+=0.05 ) {
		drawOneScaleLine( ((float)a/max_gscale)*M_PI_2, 140, 150, 2, COLOR_ORANGE );
	}
	for( float a=gload_neg_limit_low.get()-1; a>gload_neg_limit.get()-1; a-=0.05 ) {
		drawOneScaleLine( ((float)a/max_gscale)*M_PI_2, 140, 150, 2, COLOR_ORANGE );
	}

	initGauge(max_gscale, false); // no logarithmic gauge for g-load
	drawScale( max_gscale, -max_gscale, 140, 1 );
	indicator->setGeometry(70, 129, 7);
	old_gmax = 100;
	old_gmin = -100;
	old_ias_max = -1;
	xSemaphoreGive(spiMutex);
	ESP_LOGI(FNAME,"initLoadDisplay end");
}

void IpsDisplay::drawLoadDisplay( float loadFactor ){
	// ESP_LOGI(FNAME,"drawLoadDisplay %1.1f tick: %d", loadFactor, tick );
	if( _menu )
		return;
	tick++;

	if( !(screens_init & INIT_DISPLAY_GLOAD) ){
		clear();
		initLoadDisplay();
		screens_init |= INIT_DISPLAY_GLOAD;
	}
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	// draw G pointer
	float a = (*_gauge)(loadFactor-1.);
	indicator->drawPolarIndicator( a );
	// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );

	// G load digital
	if( (int)(loadFactor*30) != _ate && !(tick%3) ) {
		drawAvgVario( AMIDX+38, AMIDY, loadFactor, true );
		_ate = (int)(loadFactor*30);
	}
	// Min/Max values
	if( old_gmax != gload_pos_max.get() || !(tick%10)) {
		if( gload_pos_max.get() < gload_pos_limit.get() )
			ucg->setColor(  COLOR_WHITE  );
		else
			ucg->setColor(  COLOR_RED  );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->setPrintPos(120,105);
		ucg->printf("  %+1.2f   ", gload_pos_max.get() );
		old_gmax = gload_pos_max.get();
	}
	if( old_gmin != gload_neg_max.get() || !(tick%10)){
		if( gload_neg_max.get() > gload_neg_limit.get() )
			ucg->setColor(  COLOR_WHITE  );
		else
			ucg->setColor(  COLOR_RED  );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->setPrintPos(125,235);
		ucg->printf("  %+1.2f   ", gload_neg_max.get() );
		old_gmin = gload_neg_max.get();
	}
	if( old_ias_max != airspeed_max.get() || !(tick%10)){
		if( airspeed_max.get() < v_max.get() )
			ucg->setColor(  COLOR_WHITE  );
		else
			ucg->setColor(  COLOR_RED  );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->setPrintPos(125,295);
		ucg->printf("  %3d   ", Units::AirspeedRounded( airspeed_max.get() ) );
		old_ias_max = airspeed_max.get();
	}

	if( !(tick%10)){
		drawLoadDisplayTexts();
	}
	xSemaphoreGive(spiMutex);
}


float IpsDisplay::getHeading(){
	float heading = 0;
	if( (wind_reference.get() & WR_HEADING) )  // wind relative to airplane, first choice compass, second is GPS true course
	{
		heading = mag_hdt.get();
		if( (heading < 0) && Flarm::gpsStatus() )            // fall back to GPS course
			heading = Flarm::getGndCourse();
	}
	else if( (wind_reference.get() & WR_GPS_COURSE) ){
		if( Flarm::gpsStatus() ){
			heading = Flarm::getGndCourse();
		}
	}
	return heading;
}

// Compass or Wind Display
bool IpsDisplay::drawCompass(int16_t x, int16_t y, bool _dirty, bool compass_dirty) {
	bool ret=false;
	if( _menu )
		return ret;
	// ESP_LOGI(FNAME, "drawCompass: %d ", _dirty );
	bool wind_ok = false;
	if( (wind_display.get() & WD_DIGITS) || (wind_display.get() & WD_ARROW) ){
		int winddir=0;
		float wind=0;
		int ageStraight, ageCircling;
		char type = '/';
		if( wind_enable.get() == WA_STRAIGHT ){  // check what kind of wind is available from calculator
			wind_ok = theWind.getWind( &winddir, &wind, &ageStraight );
			type = '|';
		}
		else if( wind_enable.get() == WA_CIRCLING ){
			wind_ok = CircleWind::getWind( &winddir, &wind, &ageCircling );
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
				wind_ok = true;
			}
			else if( okc && ageCircling <= ageStraight )
			{
				wind = wc;
				winddir = wdc;
				type = '/';
				wind_ok = true;
			}
			// ESP_LOGI(FNAME, "SWIND dir=%d, SSPEED=%f ageC=%d ageS=%d okc:=%d oks=%d ok:=%d", wds, ws, ageCircling, ageStraight, okc, oks, ok  );
		}ucg->setPrintPos(85,104);
		// ESP_LOGI(FNAME, "WIND dir %d, speed %f, ok=%d", winddir, wind, ok );
		// Windspeed and Direction digital
		int windspeed = (int)( Units::Airspeed(wind)+0.5 );
		if( prev_winddir != winddir || prev_windspeed != windspeed || compass_dirty ){
			ucg->setPrintPos(85,104);
			ucg->setColor(  COLOR_WHITE  );
			// ucg->setFont(ucg_font_fub20_hr);
			ucg->setFont(ucg_font_fub17_hf, true);
			char s[32];
			if( wind_display.get() & WD_DIGITS ){
				if( wind_ok ){
					sprintf(s,"%3d°%c%2d", winddir, type, windspeed );
					if( windspeed < 10 )
						ucg->printf("%s    ", s);
					else if( windspeed < 100 )
						ucg->printf("%s   ", s);
					else
						ucg->printf("%s  ", s);
					compass_dirty = false;
					ret = true;
				}
			}
		}
		float heading = getHeading();
		// Wind arrow
		if( (prev_winddir != winddir) || (prev_windspeed != windspeed) || _dirty || (int)heading != (int)prev_heading ){
			// ESP_LOGI(FNAME, "draw WIND arrow");
			prev_winddir = winddir;  // absolute windir related to geographic north
			prev_heading = heading;  // two things to consider here, heading and wind direction
			if( wind_display.get() & WD_ARROW  ){ // draw wind arror
				float dir = Vector::angleDiffDeg( winddir, heading );
				drawWindArrow( dir, windspeed, 0 );
				// ESP_LOGI(FNAME, "gap = false");
				ret = true;
			}
			prev_windspeed = windspeed;
			wind_dirty = false;
		}
	}
	// Compass
	if( (wind_display.get() & WD_COMPASS) || ((wind_display.get() & WD_DIGITS) && !wind_ok) ){
		int heading = static_cast<int>(rintf(mag_hdt.get()));
		if( heading >= 360 )
			heading -= 360;
		// ESP_LOGI(FNAME, "heading %d", heading );
		if( prev_heading != heading || compass_dirty ){
			char s[32];
			if( heading < 0 )
				sprintf(s,"%s", "   ---" );
			else
				sprintf(s," %4d°  ", heading );
			ucg->setColor( COLOR_WHITE );
			ucg->setFont(ucg_font_fub20_hr, true);
			ucg->setPrintPos(110, 104);
			ucg->print(s);
			prev_heading = heading;
			compass_dirty = false;
			ret = true;
		}
	}
	return ret;
}

void IpsDisplay::drawNetto( int16_t x, int16_t y, bool netto ) {
	ESP_LOGI(FNAME,"drawNetto x:%d y:%d netto:%d", x,y,netto );
	if( netto )
		ucg->setColor( COLOR_WHITE );
	else
		ucg->setColor( COLOR_BLACK );
	const char *s = "net";
	if( netto_mode.get() == NETTO_RELATIVE )
		s = "s-net";
	ucg->setFont(ucg_font_fub11_hr, true);
	ucg->setPrintPos(x-ucg->getStrWidth(s),y);
	ucg->print(s);
}

bool PolarIndicator::drawPolarIndicatorAndBow(float a, bool dirty){
	if( drawPolarIndicator(a, false) ) {
		// Draw colored bow
		float bar_val = (a>0.) ? a : 0.;
		// draw green/red vario bar
		IpsDisplay::drawBow(bar_val, old_vario_bar_val, 134, bowcolor[BC_GREEN] );
		return true;
	}
	return false;
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
		drawNetto( 126, DISPLAY_H/2-33, netto );
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
	int unit = alt_unit.get();
	if( gflags.standard_setting == true ){
		unit = ALT_UNIT_FL;
	}
	flt_altitude += (altitude_m - flt_altitude) *0.1; // a bit lowpass make sense, any jitter would mess up tape display
	float altitude = Units::Altitude( flt_altitude, unit );

	// TE vario pointer position in rad
	float needle_pos = (*_gauge)(te);
	// Check overlap on inner figures

	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
	// average Climb
	if( ((int)(ate*30) != _ate) && !(tick%10) ) {
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

	// S2F Command triangle
	if( (((int)s2fd != s2fdalt && !((tick+1)%2) ) || !((tick+3)%30)) && !ulmode ) {
		// static float s=0; // check the bar code
		// s2fd = sin(s) * 42.;
		// s+=0.04;
		drawS2FBar(AMIDX, AMIDY,(int)s2fd);
	}

	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
	bool needle_prio = (drawing_prio.get() == DP_NEEDLE);
	bool bg_prio = (drawing_prio.get() == DP_BACKGROUND);
	if( !(tick%2) && bg_prio ){  // draw needle first when background has prio
		indicator->drawPolarIndicatorAndBow(needle_pos, false);
	}
	// Airspeed (NEEDLE overlap)
	if( !(tick%6) ) {
		if( bg_prio ){
			drawSpeed( airspeed_kmh, INNER_RIGHT_ALIGN, 75, speed_dirty );
		}else {
			if( drawSpeed( airspeed_kmh, INNER_RIGHT_ALIGN, 75, (speed_dirty && !(tick%10)) ) ){
				indicator->drawPolarIndicatorAndBow(needle_pos, false);
			}
		}
	}
	// Altitude (NEEDLE overlap)
	if( !(tick%2) ) {
		// { // Enable those line, comment previous condition, for a drawAltimeter simulation
		// static float alt = 0, rad = 0.0; int min_aq = std::max(alt_quant, (int16_t)1);
		// altitude = alt + sin(rad) * (5*min_aq+2); rad += 0.003*min_aq;
		if( bg_prio ){
			drawAltitude( altitude, INNER_RIGHT_ALIGN, 270, alt_dirty );
		}else{  // needle prio
			if( drawAltitude( altitude, INNER_RIGHT_ALIGN, 270, (alt_dirty && !(tick%10)) ) ){
				indicator->drawPolarIndicatorAndBow(needle_pos, true);
			}
		}
	}
	// Compass  (NEEDLE overlap)
	if( !(tick%2) ){
		if( bg_prio )
			drawCompass(INNER_RIGHT_ALIGN, 105, wind_dirty, compass_dirty );
		else{
			if( drawCompass(INNER_RIGHT_ALIGN, 105, wind_dirty && !(tick%10), compass_dirty && !(tick%10) ) ){
				indicator->drawPolarIndicatorAndBow(needle_pos, true);
			}
		}
	}

	// Center Aid around grafic wind
	if( centeraid && !(tick % 4)   ){
		centeraid->drawCenterAid();
	}

	// Vario Needle in Front mode drawn as last
	if( !(tick%2) && needle_prio ){
		indicator->drawPolarIndicatorAndBow(needle_pos, false);
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
	temp_status_t mputemp = MPU.getSiliconTempStatus();
	if( (((int)(temp*10) != tempalt) || (mputemp != siliconTempStatusOld)) && !(tick%12)) {
		ucg->setClipRange(ulmode?15:5,1,120,100); // avoid overwriting thermometer
		drawTemperature( ulmode?65:55, 25, temp );
		ucg->undoClipRange();
		tempalt=(int)(temp*10);
		siliconTempStatusOld = mputemp;
	}

	// WK-Indicator
	if( FLAP && !(tick%7) )
	{
		float wkspeed = Units::ActualWingloadCorrection(ias.get());
		int wki;
		float wkopt = FLAP->getOptimum( wkspeed, wki );
		int wk = (int)((wki - wkopt + 0.5)*10);
		// ESP_LOGI(FNAME,"as:%d wksp:%f wki:%d wk:%d wkpos:%f", airspeed_kmh, wkspeed, wki, wk, wkpos );
		// ESP_LOGI(FNAME,"WK changed WKE=%d WKS=%f", wk, wksensor );
		ucg->setColor(  COLOR_WHITE  );
		FLAP->drawBigBar( (float)(wk)/10, wksensor );
		wkoptalt = wk;

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
	// ESP_LOGI(FNAME,"acl:%f nt:%d", acl, average_climbf, !(tick%9) );
	if( acl != average_climbf && !(tick%9) && acl > 0 ){
		drawAvg( acl, acl-average_climbf );
		average_climbf = acl;
	}
	// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );
	xSemaphoreGive(spiMutex);
}

void IpsDisplay::drawDisplay( int airspeed, float te, float ate, float polar_sink, float altitude,
		float temp, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_setting, float wksensor ){
	if( _menu )
		return;

	if ( alt_display_mode.get() == MODE_QFE ) {
		altitude -= elevation.get();
	}
	xSemaphoreTake(display_mutex,portMAX_DELAY);
	if( display_style.get() == DISPLAY_AIRLINER )
		drawAirlinerDisplay( airspeed,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor );
	else if( display_style.get() == DISPLAY_RETRO )
		drawRetroDisplay( airspeed,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor, false );
	else if( display_style.get() == DISPLAY_UL )
		drawRetroDisplay( airspeed,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor, true );
	xSemaphoreGive(display_mutex);

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
	if( netto != netto_old ){
		drawNetto( DISPLAY_W-37, 20, netto );
		netto_old = netto;
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
	int unit = alt_unit.get();
	if( gflags.standard_setting == true ){
		unit = ALT_UNIT_FL;
	}
	flt_altitude += (altitude_m - flt_altitude) *0.1; // a bit lowpass make sense, any jitter would mess up tape display readability
	float altitude = Units::Altitude( flt_altitude, unit );

	vTaskDelay(3);
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}

	// WK-Indicator
	if( FLAP && !(tick%7) )
	{
		float wkspeed = Units::ActualWingloadCorrection(airspeed_kmh); // no units conversion in here, tbd: move sub to other place
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
		drawAvgVario( FIELD_START+88, YVAR-23, ate );
		ucg->undoClipRange();
		_ate = (int)(ate)*10;
	}

	// Altitude
	if(!(tick%2) ) {
		drawAltitude( altitude, FIELD_START+80, YALT-12, !(tick%40), true );  // small alignment for larger tape
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
		ucg->setClipRange(FIELD_START+10, 1,500,500); // avoid overwriting thermometer
		drawTemperature( FIELD_START+65, DISPLAY_H-5, temp );
		ucg->undoClipRange();
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

	if ( average_climbf !=  acl && !(tick%9) && acl > 0 ){
		drawAvgSymbol(  (acl*_pixpmd)/10, COLOR_BLACK );
		drawLegend( true );
		average_climbf = acl;
		drawAvgSymbol(  (acl*_pixpmd)/10, COLOR_RED );
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
		drawSmallSpeed(airspeed, FIELD_START+35, YS2F-fh+3);
		as_prev = airspeed;
	}
	// S2F command trend triangle
	if( _menu ){
		xSemaphoreGive(spiMutex);
		return;
	}
	if( ((int)s2fd != s2fdalt) || (s2falt != (int)(s2f+0.5)) || !(tick%21) ) {
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
