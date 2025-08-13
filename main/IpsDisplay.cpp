/*
 * IpsDisplay.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: iltis
 *
 */

#include "IpsDisplay.h"

#include "screen/element/PolarGauge.h"
#include "screen/element/WindCircle.h"
#include "screen/element/McCready.h"
#include "screen/element/Battery.h"
#include "screen/element/Altimeter.h"

#include "math/Trigenometry.h"
#include "comm/DeviceMgr.h"
#include "BLESender.h"
#include "OneWireESP32.h"
#include "sensor.h"
#include "Units.h"
#include "Flap.h"
#include "Flarm.h"
#include "Compass.h"
#include "wind/StraightWind.h"
#include "wind/CircleWind.h"
#include "comm/WifiApSta.h"
#include "comm/BTspp.h"
#include "comm/CanBus.h"
#include "protocol/AliveMonitor.h"
#include "setup/SetupNG.h"
#include "CenterAid.h"
#include "Rotate.h"
#include "AdaptUGC.h"
#include "logdef.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <cmath>
#include <cstdio>
#include <cstring>


////////////////////////////
// types


static const char* AirspeedModeStr();

// local variables
int screens_init = INIT_DISPLAY_NULL;

int   IpsDisplay::tick = 0;
int   IpsDisplay::_pixpmd = 10;

bool IpsDisplay::netto_old = false;

// Average Vario data
int IpsDisplay::last_avg = -1000;
int IpsDisplay::x_start = 240;
PolarGauge* IpsDisplay::gauge = nullptr;
WindCircle* IpsDisplay::polWind = nullptr;
McCready*   IpsDisplay::MCgauge = nullptr;
Battery*    IpsDisplay::BATgauge = nullptr;
Altimeter*	IpsDisplay::ALTgauge = nullptr;

int16_t DISPLAY_H;
int16_t DISPLAY_W;

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
#define BATY (DISPLAY_H-12)
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
static int AMIDY;
static int AMIDX;
static int AVGOFFX;
static int SPEEDYPOS;

static int16_t INNER_RIGHT_ALIGN = 170;
static int16_t LOAD_MPG_POS = 0;
static int16_t LOAD_MNG_POS = 0;
static int16_t LOAD_MIAS_POS = 0;
static int fh;

SemaphoreHandle_t display_mutex=NULL;

ucg_color_t IpsDisplay::colors[320+1];
ucg_color_t IpsDisplay::colorsalt[320+1];

AdaptUGC *IpsDisplay::ucg = 0;

int IpsDisplay::_te=0;
int IpsDisplay::_ate=-1000;
int IpsDisplay::s2falt=-1;
int IpsDisplay::s2fdalt=0;
int IpsDisplay::s2f_level_prev=0;
int IpsDisplay::s2fmode_prev=100;
bool IpsDisplay::wireless_alive = false;
int IpsDisplay::tempalt = -2000;
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

// static float prev_needle = 0;

temp_status_t IpsDisplay::siliconTempStatusOld = MPU_T_UNKNOWN;

// constexpr float sincosScale = 180.f/My_PIf*2.f; // rad -> deg/2 a 0.5deg resolution discrete scale
static int16_t old_vario_bar_val = 0;
static int16_t old_sink_bar_val = 0;

static bool bottom_dirty = false;


#define WKBARMID (AMIDY-15)

float polar_sink_prev = 0;
float te_prev = 0;
bool flarm_connected=false;
typedef enum e_bow_color { BC_GREEN, BC_BLUE, BC_RED, BC_ORANGE } t_bow_color;
const static ucg_color_t bowcolor[4] = { {COLOR_GREEN}, {COLOR_BBLUE}, {COLOR_RED}, {COLOR_ORANGE} };


static void initRefs()
{
	AVGOFFX = -4;
	SPEEDYPOS = 106;
	INNER_RIGHT_ALIGN = DISPLAY_W - 44;
	LOAD_MPG_POS = DISPLAY_H*0.25;
	LOAD_MNG_POS = DISPLAY_H*0.64;
	LOAD_MIAS_POS = DISPLAY_H*0.81;
	if ( display_orientation.get() == DISPLAY_NINETY ) {
		INNER_RIGHT_ALIGN = DISPLAY_W - 70;
		AMIDX = DISPLAY_W/2 - 43;
		AVGOFFX = 36;
		SPEEDYPOS = 80;
		LOAD_MNG_POS = DISPLAY_H*0.53;
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
	DISPLAY_W = ucg->getDisplayWidth();
	DISPLAY_H = ucg->getDisplayHeight();
	display_mutex = xSemaphoreCreateMutex();
}

IpsDisplay::~IpsDisplay() {
	if ( gauge ) {
		delete gauge;
		gauge = nullptr;
	}
	if ( polWind ) {
		delete polWind;
		polWind = nullptr;
	}
	if ( MCgauge ) {
		delete MCgauge;
		MCgauge = nullptr;
	}
}


void IpsDisplay::drawArrowBox( int x, int y, bool arightside ){
	int fh = ucg->getFontAscent();
	int fl = ucg->getStrWidth("123");
	if( arightside )
		ucg->drawTriangle( x+fl+4,y-(fh/2)-3,x+fl+4,y+(fh/2)+3,x+fl+4+fh/2,y );
	else
		ucg->drawTriangle( x,y-(fh/2)-3,   x,y+(fh/2)+3,   x-fh/2,y );
}

void IpsDisplay::drawLegend( bool onlyLines )
{
	ucg->setColor(COLOR_WHITE);
	ucg->drawVLine( DISPLAY_LEFT+5,      VARBARGAP , DISPLAY_H-(VARBARGAP*2) );
	ucg->drawHLine( DISPLAY_LEFT+5, VARBARGAP , bw+1 );
	ucg->drawVLine( DISPLAY_LEFT+5+bw+1, VARBARGAP, DISPLAY_H-(VARBARGAP*2) );
	ucg->drawHLine( DISPLAY_LEFT+5, DISPLAY_H-(VARBARGAP)-1, bw+1 );

	int hc=0;
	if( onlyLines == false ){
		ucg->setFont(ucg_font_9x15B_mf, true);
		hc = ucg->getFontAscent()/2;
	}
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

void IpsDisplay::writeText( int line, const char *text )
{
	ucg->setFont(ucg_font_ncenR14_hr, true );
	ucg->setPrintPos( 1, 26*line );
	ucg->setColor(COLOR_WHITE);
	ucg->printf("%s", text);
}

void IpsDisplay::writeText( int line, std::string &text ){
	ucg->setFont(ucg_font_ncenR14_hr, true );
	ucg->setPrintPos( 1, 26*line );
	ucg->setColor(COLOR_WHITE);
	ucg->printf("%s",text.c_str());
}


void IpsDisplay::clear(){
	// ESP_LOGI(FNAME,"display clear()");
	ucg->setColor( COLOR_BLACK );
	ucg->drawBox( 0,0,DISPLAY_W,DISPLAY_H );
	screens_init = INIT_DISPLAY_NULL;
}

void IpsDisplay::bootDisplay() {
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

	// grab screen layout
	AMIDX = (DISPLAY_W/2 + 30);
	AMIDY = (DISPLAY_H)/2;
	if ( display_orientation.get() == DISPLAY_NINETY ) {
		AMIDX = DISPLAY_W/2 - 43;
	}
}

void IpsDisplay::initDisplay() {
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
	// Create common elements
	
	if ( ! gauge ) { // shared with 
		int16_t scale_geometry = ( display_orientation.get() == DISPLAY_NINETY ) ? 120 : 90;
		gauge = new PolarGauge(AMIDX, AMIDY, scale_geometry, DISPLAY_H/2-20);
	}
	gauge->setRange(Units::Vario(scale_range.get()), 0.f, log_scale.get());
	gauge->setColor(needle_color.get());
	if ( ! MCgauge ) {
		MCgauge = new McCready(1, DISPLAY_H+2);
	}
	if ( ! BATgauge ) {
		BATgauge = new Battery(BATX, BATY);
	}
	if ( vario_lower_gauge.get() && ! ALTgauge ) {
		ALTgauge = new Altimeter(INNER_RIGHT_ALIGN, 0.8*DISPLAY_H);
	}

	if( display_style.get() != DISPLAY_AIRLINER ) {
		initRetroDisplay( display_style.get() == DISPLAY_UL );
	}
	else { // Airliner
		clear();
		ucg->setFont(ucg_font_fub11_tr);
		ucg->setFontPosBottom();
		ucg->setPrintPos(DISPLAY_LEFT+5,YVAR-VARFONTH+7);
		ucg->setColor( COLOR_HEADER );
		ucg->print(Units::VarioUnit());
		ucg->setPrintPos(FIELD_START,YVAR-VARFONTH+7);

		ucg->print("AV Vario");
		ucg->setColor(COLOR_WHITE );

		// small MC
		MCgauge->setLarge(false);
		ALTgauge->setRef(FIELD_START+80, YALT-12);
		

		// draw TE scale
		drawLegend();

		// S2F Text
		ucg->setFont(ucg_font_fub11_tr);
		fh = ucg->getFontAscent();
		ucg->setPrintPos(FIELD_START+6,YS2F-(2*fh) - 8);
		ucg->setColor(COLOR_HEADER );

		ucg->printf("%s %s", AirspeedModeStr(), Units::AirspeedUnitStr() );

		ucg->setPrintPos(ASVALX,YS2F-(2*fh) - 8);
		ucg->print(" S2F");

		ucg->setColor(COLOR_WHITE );
		// AS Box
		int fl = ucg->getStrWidth(" 200- ");
		ASLEN = fl+3;
		S2FST = ASLEN+16;
		ucg->drawFrame( FIELD_START, dmid-(MAXS2FTRI)-4, ASLEN+6, (MAXS2FTRI*2)+8 );

		// S2F Zero
		// ucg->drawTriangle( FIELD_START, dmid+5, FIELD_START, dmid-5, FIELD_START+5, dmid);
		ucg->drawTriangle( FIELD_START+ASLEN-1, dmid, FIELD_START+ASLEN+5, dmid-6, FIELD_START+ASLEN+5, dmid+6);

		// Thermometer
		drawThermometer(  FIELD_START+5, DISPLAY_H-5 );

		if ( FLAP ) {
			FLAP->setBarPosition( WKSYMST+2, YS2F-fh );
			FLAP->setSymbolPosition( WKSYMST+2, YS2F-fh-25 );
		}
		bottom_dirty = false;
	}

	redrawValues();
}

// Thermometer
void IpsDisplay::drawThermometer( int x, int y) {
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
	_range = Units::Vario( scale_range.get() );

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
	int size = 6;
	ucg->setColor( r,g,b );
	ucg->drawTetragon( x+size-1,dmid-y, x,dmid-y+size, x-size,dmid-y, x,dmid-y-size );
}

// static float avc_old=-1000;

void IpsDisplay::drawAvg( float avclimb, float delta ){
	// static int yusize=6;
	// static int ylsize=6;

	// ESP_LOGD(FNAME,"drawAvg: av=%.2f delta=%.2f", avclimb, delta );
	// int pos=145;
	// int size=7;
	// float a;
	// if( avc_old != avclimb ){
	// 	ucg->setColor( COLOR_BLACK );
	// 	a = (_gauge)(avc_old);
	// 	int x=gaugeCosCentered(a, pos);
	// 	int y=gaugeSinCentered(a, pos);
	// 	ucg->drawTetragon( x+size, y, x,y+ylsize, x-size,y, x,y-yusize );
	// 	// refresh scale around old AVG icon
	// 	drawScale( _range, -_range, DISPLAY_H/2-20, avc_old*10.f );
	// }
	// if( delta > (mean_climb_major_change.get())/core_climb_history.get() ){
	// 	ucg->setColor( COLOR_GREEN );
	// 	yusize=size*2;
	// 	ylsize=size;
	// }
	// else if ( delta < -(mean_climb_major_change.get())/core_climb_history.get() ){
	// 	ucg->setColor( COLOR_RED );
	// 	ylsize=size*2;
	// 	yusize=size;
	// }
	// else if( delta > (mean_climb_major_change.get()/2.0)/core_climb_history.get() ){
	// 	ucg->setColor( COLOR_GREEN );
	// 	yusize=size;
	// 	ylsize=size;
	// }
	// else if ( delta < -(mean_climb_major_change.get()/2.0)/core_climb_history.get() ){
	// 	ucg->setColor( COLOR_RED );
	// 	ylsize=size;
	// 	yusize=size;
	// }
	// else{
	// 	ucg->setColor( COLOR_WHITE );
	// 	ylsize=size;
	// 	yusize=size;
	// }

	// if( avclimb > _range ) // clip values off weeds
	// 	avclimb = _range;
	// a = (_gauge)(avclimb);
	// int x=gaugeCosCentered(a, pos);
	// int y=gaugeSinCentered(a, pos);
	// ESP_LOGD(FNAME,"drawAvg: x=%d  y=%d", x,y );
	// ucg->drawTetragon( x+size,y, x, y+ylsize, x-size,y, x,y-yusize );
	// avc_old=avclimb;
}

void IpsDisplay::redrawValues()
{
	// ESP_LOGI(FNAME,"IpsDisplay::redrawValues()");
	tempalt = -2000;
	s2falt = -1;
	s2fdalt = -1;
	s2f_level_prev = 0;
	wireless_alive = false;
	_te=-200;
	if ( MCgauge ) {
		MCgauge->forceRedraw();
	}
	BATgauge->forceRedraw();
	if ( ALTgauge ) {
		// ALTgauge->drawUnit();
		ALTgauge->forceRedraw();
	}
	as_prev = -1;
	_ate = -2000;
	last_avg = -1000;
	x_start = 240;

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
}

void IpsDisplay::drawTeBuf(){
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


#define S2FSS 10
#define S2FTS 5

void IpsDisplay::drawCircling( int x, int y, bool draw ){
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
	if( draw )
		ucg->setColor( COLOR_WHITE );
	else
		ucg->setColor( COLOR_BLACK );
	ucg->drawTetragon(x-S2FSS,y-5, x-S2FSS,y-1, x+S2FSS,y+5, x+S2FSS,y+1 );
	ucg->drawTriangle( x+S2FSS-10, y+7, x+S2FSS-8, y-4, x+S2FSS, y+3 );
}

void IpsDisplay::drawS2FMode( int x, int y, bool cruise ){
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
	bool bta=true;
	if( DEVMAN->isIntf(BT_SPP) && BTspp )
		bta = BTspp->isConnected();
	else if( DEVMAN->isIntf(BT_LE) )
		bta=BLESender::queueFull() ? false : true;
	if( bta != wireless_alive || flarm_alive.get() > ALIVE_NONE ) {
		int16_t btx=DISPLAY_W-18;
		int16_t bty=(BTH/2) + 6;
		if( ! bta )
			ucg->setColor( COLOR_MGREY );
		else
			ucg->setColor( COLOR_BLUE );  // blue

		ucg->drawRBox( btx-BTW/2, bty-BTH/2, BTW, BTH, BTW/2-1);
		// inner symbol
		if( flarm_alive.get() == ALIVE_OK )
			ucg->setColor( COLOR_GREEN );
		else
			ucg->setColor( COLOR_WHITE );
		ucg->drawTriangle( btx, bty, btx+BTSIZE, bty-BTSIZE, btx, bty-2*BTSIZE );
		ucg->drawTriangle( btx, bty, btx+BTSIZE, bty+BTSIZE, btx, bty+2*BTSIZE );
		ucg->drawLine( btx, bty, btx-BTSIZE, bty-BTSIZE );
		ucg->drawLine( btx, bty, btx-BTSIZE, bty+BTSIZE );

		wireless_alive = bta;
		flarm_connected = flarm_alive.get();
	}
	if( SetupCommon::isWired() ) {
		drawCable(DISPLAY_W-20, BTH + 22);
	}
}

void IpsDisplay::drawCable(int16_t x, int16_t y)
{
	const int16_t CANH = 8;
	const int16_t CANW = 14;

	int connectedXCV = xcv_alive.get();
	int connectedMag = mags_alive.get();

	(connectedXCV == ALIVE_OK)? ucg->setColor(COLOR_LBLUE) : ucg->setColor(COLOR_MGREY);
	// lower horizontal line
	if (connectedMag) {
		ucg->setColor(COLOR_GREEN);
	}
	ucg->drawLine( x-CANW/2, y+CANH/2, x+3, y+CANH/2 );
	ucg->drawLine( x-CANW/2, y+CANH/2-1, x+3, y+CANH/2-1 );
	ucg->drawDisc( x-CANW/2, y+CANH/2, 2, UCG_DRAW_ALL);
	(connectedMag == ALIVE_OK)? ucg->setColor(COLOR_LBLUE) : ucg->setColor(COLOR_MGREY);
	// Z diagonal line
	if (flarm_alive.get() == ALIVE_OK) { ucg->setColor(COLOR_GREEN); }
	ucg->drawLine( x+2, y+CANH/2, x-4, y-CANH/2 );
	ucg->drawLine( x+3, y+CANH/2-1, x-3, y-CANH/2-1 );
	// upper horizontal line
	(connectedXCV == ALIVE_OK)? ucg->setColor(COLOR_LBLUE) : ucg->setColor(COLOR_MGREY);
	ucg->drawLine( x-3, y-CANH/2, x+CANW/2, y-CANH/2 );
	ucg->drawLine( x-3, y-CANH/2-1, x+CANW/2, y-CANH/2-1 );
	ucg->drawDisc( x+CANW/2, y-CANH/2, 2, UCG_DRAW_ALL);
}

void IpsDisplay::drawWifi( int x, int y ) {
	if( !DEVMAN->isIntf(WIFI_APSTA) ) {
		return;
	}
	bool wla = WIFI->isAlive();
	if( wla != wireless_alive || flarm_alive.get() > ALIVE_NONE ){
		ESP_LOGI(FNAME,"IpsDisplay::drawWifi %d %d %d", x,y,wla);
		if( ! wla ) {
			ucg->setColor(COLOR_MGREY);
		} else {
			ucg->setColor( COLOR_BLUE );
		}
		ucg->drawCircle( x, y, 9, UCG_DRAW_UPPER_RIGHT);
		ucg->drawCircle( x, y, 10, UCG_DRAW_UPPER_RIGHT);
		ucg->drawCircle( x, y, 16, UCG_DRAW_UPPER_RIGHT);
		ucg->drawCircle( x, y, 17, UCG_DRAW_UPPER_RIGHT);
		if( flarm_alive.get() == ALIVE_OK ) {
			ucg->setColor( COLOR_GREEN );
		}
		ucg->drawDisc( x, y, 3, UCG_DRAW_ALL );
		flarm_connected = flarm_alive.get();
		wireless_alive = wla;
	}
	if( SetupCommon::isWired() ) {
		drawCable(DISPLAY_W-20, y+18);
	}
}

void IpsDisplay::drawConnection( int16_t x, int16_t y )
{
	if ( DEVMAN->isIntf(BT_SPP) || DEVMAN->isIntf(BT_LE) ) {
		drawBT();
	}
	else if( DEVMAN->isIntf(WIFI_APSTA) ) {
		drawWifi(x, y);
	}
	else if( SetupCommon::isWired() ) {
		drawCable(DISPLAY_W-18, y);
	}
}


// accept temperature in deg C and display in configured unit
// right-aligned value to x, incl. unit right of x
void IpsDisplay::drawTemperature( int x, int y, float t ) {
	ucg->setFont(ucg_font_fub14_hn, false);
	char s[32];
	if( t != DEVICE_DISCONNECTED_C ) {
		float temp_unit = Units::TemperatureUnit( t );
		sprintf(s, "%.1f ", std::roundf(temp_unit*10.f)/10.f );
	}
	else {
		strcpy(s, "---");
	}
	// ESP_LOGI(FNAME,"drawTemperature: %d,%d %s", x, y, s);
	ucg->setColor( COLOR_WHITE );
	ucg->setPrintPos(x,y-3);
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
	ucg->setFont(ucg_font_fub11_hn, false);
	ucg->setPrintPos(x+ucg->getStrWidth(s)+2,y-3);
	ucg->printf("%s ", Units::TemperatureUnitStr(temperature_unit.get()));
}


static int wx0,wy0,wx1,wy1,wx3,wy3 = 0;  // initialize by zero

// draw windsock style alike arrow white and red
void IpsDisplay::drawWindArrow( float a, float speed, int type ){
	const int X=75;
	const int Y=215;
	float si=fast_sin_deg(a);
	float co=fast_cos_deg(a);
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

void IpsDisplay::setBottomDirty()
{
	bottom_dirty = true;
	ESP_LOGI(FNAME,"bottom dirty");
}

void IpsDisplay::initRetroDisplay( bool ulmode ){
	clear();
	ucg->setFontPosBottom();
	_range = Units::Vario( scale_range.get() );

	gauge->drawScale();
	gauge->forceRedraw();

	initRefs();
	if ( ! polWind ) {
		polWind = new WindCircle(AMIDX+AVGOFFX-37, AMIDY);
	}
	if ( ulmode ) {
		if ( MCgauge ) {
			delete MCgauge;
			MCgauge = nullptr;
		}
	}
	else {
		MCgauge->setLarge(true);
	}
	if ( vario_lower_gauge.get() ) {
		ALTgauge->setRef(INNER_RIGHT_ALIGN, 0.8*DISPLAY_H);
	}
	else {
		delete ALTgauge;
		ALTgauge = nullptr;
	}
	redrawValues();

	// Unit's
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(2,50);
	ucg->setColor(COLOR_HEADER);
	ucg->print( Units::VarioUnit() );
	if ( vario_upper_gauge.get() ) {
		drawTopGauge(0, INNER_RIGHT_ALIGN, SPEEDYPOS, true );
	}

	if ( FLAP ) {
		FLAP->setBarPosition( WKSYMST-4, WKBARMID);
		FLAP->setSymbolPosition( WKSYMST-3, WKBARMID-27*(abs(FLAP->getNegMax()))-18 );
	}
	if( theCenteraid ){
		theCenteraid->setGeometry(AMIDX+AVGOFFX-38, AMIDY, 47);
	}
}

void IpsDisplay::drawWarning( const char *warn, bool push ){
	ESP_LOGI(FNAME,"drawWarning");
	clear();
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
}


void IpsDisplay::drawAvgVario( int16_t x, int16_t y, float val, bool large )
{
	int ival = rint(val*10);  // integer value in steps of 10th
	if( last_avg != ival){  // only print if there a change in rounded numeric string
		char s[32];
		// if( large ) {
		// 	ucg->setFont(eglib_font_free_sansbold_66, false );
		// } else {
		ucg->setFont(ucg_font_fub35_hn, false );
		// }
		ucg->setFontPosCenter();
		static const char* format[2] = {"%2.1f","%2.0f"};
		sprintf(s, format[std::abs(ival)>100], float(abs(ival)/10.) );
		int new_x_start = x - ucg->getStrWidth(s);
		if( new_x_start > x_start ){         // do we have a shorter string starting at higer x position
			ucg->setColor( COLOR_BLACK );    // yes -> so blank exact prepending area
			int fh = ucg->getFontAscent();   // height of blanking box
			ucg->drawBox( x_start, y-fh/2, new_x_start-x_start, fh );  // draw blanking box
		}
		if (ival<0) {
			ucg->setColor( COLOR_BBLUE );
		} else {
			ucg->setColor( COLOR_WHITE );
		}
		ucg->setPrintPos(new_x_start, y + 8);
		ucg->print(s);
		last_avg = ival;
		x_start = new_x_start;
		ucg->setFontPosBottom();
	}
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

const char* AirspeedModeStr()
{
	if (airspeed_mode.get() == MODE_IAS)
	{
		return "IAS";
	}
	else if (airspeed_mode.get() == MODE_TAS)
	{
		return "TAS";
	}
	else if (airspeed_mode.get() == MODE_CAS)
	{
		return "CAS";
	}
	else
	{
		return "-";
	}
}

// Accepts speed in kmh IAS/TAS, translates into configured unit
// set dirty, when obscured from vario needle
// right-aligned to value
bool IpsDisplay::drawTopGauge(int val, int16_t x, int16_t y, bool inc_unit)
{
	switch ( vario_upper_gauge.get() ) {
	case GAUGE_S2F:
		val = Units::Airspeed(s2f_ideal.get());
		break;
	case NETTO_VARIO:
		val = static_cast<int>(std::round(slipAngle*-10.f)); // todo better home for the variable
		break;
	case GAUGE_SLIP:
		val = static_cast<int>(std::round(slipAngle*-10.f)); // todo better home for the variable
		break;
	case GAUGE_HEADING:
		val = static_cast<int>(std::round(getHeading()));
		break;
	default:
		break;
	}
	

	if ( as_prev == val ) return false;
	// ESP_LOGI(FNAME,"draw val %d %d", val, as_prev );

	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub25_hn, true);

	char s[32];
	if ( vario_upper_gauge.get() != GAUGE_SLIP ) {
		sprintf(s,"  %3d", val);
	}
	else {
		sprintf(s,"  %2d.%01d", val/10, std::abs(val)%10 );  // Slip Angle
	}
	ucg->setPrintPos(x-ucg->getStrWidth(s), y);
	ucg->print(s);
	if ( inc_unit ) {
		ucg->setFont(ucg_font_fub11_hr);
		ucg->setColor( COLOR_HEADER );
		ucg->setPrintPos(x+5,y-3);
		if( vario_upper_gauge.get() == GAUGE_SPEED ||vario_upper_gauge.get() == GAUGE_S2F ) {
			ucg->print(Units::AirspeedUnitStr() );
			ucg->setPrintPos(x+5,y-17);
			if ( vario_upper_gauge.get() == GAUGE_SPEED ) { 
				ucg->print(AirspeedModeStr());
			}
			else {
				ucg->print("S2F");
			}
		}
		else {
			ucg->print("deg");
		ucg->setPrintPos(x+5,y-17);
			if ( vario_upper_gauge.get() == GAUGE_SLIP ) { 
				ucg->print("SLIP");
			}
			else {
				ucg->print("HDG");
			}
		}
	}
	as_prev = val;
	return true;
}

//////////////////////////////////////////////
// The load display

static float old_gmax = 100;
static float old_gmin = -100;
static float old_ias_max = -1;

void IpsDisplay::drawLoadDisplayTexts(){
	ucg->setFont(ucg_font_fub11_hr, true);
	ucg->setPrintPos(INNER_RIGHT_ALIGN-40, LOAD_MPG_POS);
	ucg->setColor(  COLOR_HEADER_LIGHT  );
	ucg->print( "MAX POS G" );
	ucg->setPrintPos(INNER_RIGHT_ALIGN-40, LOAD_MNG_POS);
	ucg->print( "MAX NEG G" );
	ucg->setPrintPos(INNER_RIGHT_ALIGN-40, LOAD_MIAS_POS);
	ucg->printf( "MAX IAS %s", Units::AirspeedUnitStr() );
}

void IpsDisplay::initLoadDisplay(){
	clear();
	ESP_LOGI(FNAME,"initLoadDisplay()");
	ucg->setColor( COLOR_HEADER );
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(20,20);
	ucg->print("G-Force");
	drawLoadDisplayTexts();
	int max_gscale = gload_pos_limit.get() + 2;
	int min_gscale = gload_neg_limit.get() - 2;
	if ( ! gauge ) { // shared with 
		int16_t scale_geometry = ( display_orientation.get() == DISPLAY_NINETY ) ? 120 : 90;
		gauge = new PolarGauge(AMIDX, AMIDY, scale_geometry, DISPLAY_H/2-20);
	}
	gauge->setRange(max_gscale, 1.f, false);
	gauge->setColor(needle_color.get());
	// put the scale colored section into the background
	gauge->colorRange(gload_pos_limit_low.get(), gload_pos_limit.get(), PolarGauge::ORANGE);
	gauge->colorRange(gload_pos_limit.get(), max_gscale, PolarGauge::RED);
	gauge->colorRange(gload_neg_limit_low.get(), gload_neg_limit.get(), PolarGauge::ORANGE);
	gauge->colorRange(gload_neg_limit.get(), min_gscale, PolarGauge::RED);
	gauge->drawScale();
	gauge->forceRedraw();
	old_gmax = 100;
	old_gmin = -100;
	old_ias_max = -1;
	bottom_dirty = false;
	ESP_LOGI(FNAME,"initLoadDisplay end");
}

static Point P1o;
static Point P2o;
static Point P3o;
static Point P4o;
static Point P5o;
static Point P6o;

static float oroll=0;
static int heading_old = -1;

void IpsDisplay::drawHorizon( float pitch, float roll, float yaw ){
	// ESP_LOGI(FNAME,"drawHorizon P: %1.1f R: %1.1f Y: %1.1f", rad2deg(pitch), rad2deg(roll), rad2deg(yaw) );
	tick++;
	if( !(screens_init & INIT_DISPLAY_HORIZON) ){
		clear();
		P1o.y = 0;
		ucg->setColor( COLOR_WHITE );
		ucg->drawTriangle( 1,   150, 20,  160, 1,   170 ); // Triangles l/r
		ucg->drawTriangle( 240, 150, 220, 160, 240, 170 );
		for( int i=-80; i<=80; i+=20 ){  // 10° scale
			ucg->drawHLine( 1,160+i, 20 );
			ucg->drawHLine( 220,160+i, 20 );
		}
		for( int i=-70; i<=70; i+=20 ){  // 5° scale
			ucg->drawHLine( 10,160+i, 10 );
			ucg->drawHLine( 220,160+i, 10 );
		}
		screens_init |= INIT_DISPLAY_HORIZON;
	}
	Point P1( -100, -60 );
	Point P2(  340, -60 );
	Point P3(  340, 160 );
	Point P4( -100, 160 );
	Point P5( -100, 380 );
	Point P6(  340, 380 );
	Point Center( 120, 160 );
	Point P1r = P1.rotate( Center, -roll );
	Point P2r = P2.rotate( Center, -roll );
	Point P3r = P3.rotate( Center, -roll );
	Point P4r = P4.rotate( Center, -roll );
	Point P5r = P5.rotate( Center, -roll );
	Point P6r = P6.rotate( Center, -roll );
	int p = -rint(rad2deg( pitch )*2);  // 1 deg := 1 pixel
	P1r.moveVertical(p);
	P2r.moveVertical(p);
	P3r.moveVertical(p);
	P4r.moveVertical(p);
	P5r.moveVertical(p);
	P6r.moveVertical(p);
	int heading = 0;

	// ESP_LOGI(FNAME,"P1:%d/%d P2:%d/%d P3:%d/%d P4:%d/%d roll:%f d:%d ", P1r.x, P1r.y+p, P2r.x, P2r.y+p, P3r.x, P3r.y+p, P4r.x , P4r.y+p, rad2deg(roll), p  );
	if( P1r.y != P1o.y || P1r.x != P1o.x ){
		// ESP_LOGI(FNAME,"drawHorizon P: %1.1f R: %1.1f Y: %1.1f", rad2deg(pitch), rad2deg(roll), rad2deg(yaw) );
		xSemaphoreTake(display_mutex,portMAX_DELAY );
		ucg->setClipRange( 20, 60, 200, 200 );
		ucg->setColor( COLOR_LBLUE );
		ucg->drawTetragon( P1r.x, P1r.y, P2r.x, P2r.y, P3r.x, P3r.y, P4r.x , P4r.y );
		ucg->setColor( COLOR_BROWN );
		ucg->drawTetragon( P4r.x, P4r.y, P3r.x, P3r.y, P6r.x, P6r.y, P5r.x , P5r.y );
		// Flarm::drawAirplane( 120, 160, true, false );  would be nice hence flickering
		P1o = P1r;
		P2o = P2r;
		P3o = P3r;
		P4o = P4r;
		P5o = P5r;
		P6o = P6r;
		oroll = roll;
		ucg->undoClipRange();
		xSemaphoreGive(display_mutex);
	}
	if( theCompass ){
		heading = static_cast<int>(rintf(mag_hdt.get()));
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->setPrintPos(70,310);
		if( heading >= 360 )
			heading -= 360;
		//			ESP_LOGI(FNAME,"compass enable, heading: %d", heading );
		if( heading > 0  && heading != heading_old){
			ucg->setColor( COLOR_WHITE );
			ucg->printf("   %d°   ", heading );
			heading_old = heading;
		}
	}
}


void IpsDisplay::drawLoadDisplay( float loadFactor ){
	// ESP_LOGI(FNAME,"drawLoadDisplay %1.1f tick: %d", loadFactor, tick );
	tick++;

	if( !(screens_init & INIT_DISPLAY_GLOAD) ){
		initLoadDisplay();
		screens_init |= INIT_DISPLAY_GLOAD;
	}
	// draw G pointer
	gauge->drawIndicator( loadFactor );

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
		ucg->setPrintPos(INNER_RIGHT_ALIGN-40, LOAD_MPG_POS+30);
		ucg->printf("  %+1.2f   ", gload_pos_max.get() );
		old_gmax = gload_pos_max.get();
	}
	if( old_gmin != gload_neg_max.get() || !(tick%10)){
		if( gload_neg_max.get() > gload_neg_limit.get() )
			ucg->setColor(  COLOR_WHITE  );
		else
			ucg->setColor(  COLOR_RED  );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->setPrintPos(INNER_RIGHT_ALIGN-40, LOAD_MNG_POS+30);
		ucg->printf("  %+1.2f   ", gload_neg_max.get() );
		old_gmin = gload_neg_max.get();
	}
	if( old_ias_max != airspeed_max.get() || !(tick%10)){
		if( airspeed_max.get() < v_max.get() )
			ucg->setColor(  COLOR_WHITE  );
		else
			ucg->setColor(  COLOR_RED  );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->setPrintPos(INNER_RIGHT_ALIGN-40, LOAD_MIAS_POS+30);
		ucg->printf("  %3d   ", Units::AirspeedRounded( airspeed_max.get() ) );
		old_ias_max = airspeed_max.get();
	}

	if( !(tick%10)){
		drawLoadDisplayTexts();
	}
	if ( bottom_dirty ) {
		bottom_dirty = false;
		initLoadDisplay();
	}
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
	// ESP_LOGI(FNAME, "drawCompass: %d ", _dirty );
	bool wind_ok = false;
	if( (wind_display.get() & WD_DIGITS) || (wind_display.get() & WD_ARROW) ){
		int winddir=0;
		float wind=0;
		int ageStraight, ageCircling;
		char type = '/';
		if( straightWind ) {
			// check what kind of wind is available from calculator
			wind_ok = straightWind->getWind( &winddir, &wind, &ageStraight );
			type = '|';
		}
		else if( circleWind ){
			wind_ok = circleWind->getWind( &winddir, &wind, &ageCircling );
		}
		else if( wind_enable.get() == WA_BOTH ){  // dynamically change type depending on younger calculation
			int wds=0, wdc;
			float ws=0, wc;
			bool oks=false, okc=false;
			if ( straightWind ) {
				oks = straightWind->getWind( &wds, &ws, &ageStraight );
			}
			if ( circleWind ) {
				okc = circleWind->getWind( &wdc, &wc, &ageCircling);
			}
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
			ESP_LOGI(FNAME, "SWIND dir=%d, SSPEED=%f ageC=%d ageS=%d okc:=%d oks=%d ok:=%d", wds, ws, ageCircling, ageStraight, okc, oks, wind_ok  );
		}

		ucg->setPrintPos(85,104);
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

void IpsDisplay::drawRetroDisplay( int airspeed_kmh, float te_ms, float ate_ms, float polar_sink_ms, float altitude_m,
		float temp, float volt, float s2fd_ms, float s2f_ms, float acl_ms, bool s2fmode, bool standard_setting, float wksensor ){
	// ESP_LOGI(FNAME,"drawRetroDisplay polar_sink: %f AVario: %f m/s", polar_sink_ms, ate_ms );
	if( !(screens_init & INIT_DISPLAY_RETRO) ){
		initDisplay();
		screens_init |= INIT_DISPLAY_RETRO;
	}
	tick++;
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
		drawNetto( DISPLAY_W-38, 50, netto );
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
	// float s2fd = Units::Airspeed( s2fd_ms );
	// int airspeed =  (int)(Units::Airspeed( airspeed_kmh ) + 0.5);

	// TE vario pointer position in rad
	// float needle_pos = (*_gauge)(te);
	// Check overlap on inner figures

	// average Climb
	if( ((int)(ate*10) != _ate) || !(tick%10) ) {
		drawAvgVario( AMIDX+AVGOFFX, AMIDY+4, ate );
		_ate = (int)(ate*10);
	}

	// S2F Command triangle
	// if( (((int)s2fd != s2fdalt && !((tick+1)%2) ) || !((tick+3)%30)) && !ulmode ) {
		// static float s=0; // check the bar code
		// s2fd = sin(s) * 42.;
		// s+=0.04;
	// 	drawS2FBar(AMIDX, AMIDY,(int)s2fd);
	// }

	// MC val
	if( MCgauge && !(tick%8) ) {
		MCgauge->draw(MC.get());
	}

	// Bluetooth etc
	if( !(tick%12) )
	{
		drawConnection(DISPLAY_W-25, FLOGO );
	}

	// Airspeed
	if( vario_upper_gauge.get() && !(tick%5) ) {
		drawTopGauge( airspeed_kmh, INNER_RIGHT_ALIGN, SPEEDYPOS );
	}
	// Altitude
	if( ALTgauge && !(tick%4) ) {
		ALTgauge->draw(altitude_m);
	}

	// Wind & center aid
	if( !(tick%2) ){
		if ( theCenteraid && ! s2fmode ) {
			theCenteraid->drawCenterAid();
		}
		else {
			static int swdir=-1, iwdir=-1;
			static int sw=0, iw=0;
			// static float d=0; // check the wind
			// float d = (rand()%180) / M_PI_2;
			// swdir += abs(sin(d)) + 2;
			// sw = int((sw+d))%120;

			// int ageStraight;
			// int ageCircling;
			// if ( wind_enable.get() & WA_BOTH ) {
			// 	if ( straightWind && ! straightWind->getWind(&iwdir, &iw, &ageStraight) ) {
			// 		iwdir = -1;
			// 	}

			// 	if ( circleWind && ! circleWind->getWind(&swdir, &iw, &ageCircling) ) {
			// 		iwdir = -1;
			// 	}
			// }
		// else{
				iwdir = extwind_inst_dir.get();
				iw = extwind_inst_speed.get();
				swdir = extwind_sptc_dir.get();
				sw = extwind_sptc_speed.get();
		// 	}
			polWind->draw(swdir, sw, iwdir, iw);
		}
	}

	// Vario indicator
	if( !(tick%2) ){
		gauge->draw(te);
	}
	// ESP_LOGI(FNAME,"polar-sink:%f Old:%f int:%d old:%d", polar_sink, old_polar_sink, int( polar_sink*100.), int( old_polar_sink*100. ) );
	if( ps_display.get() && !(tick%3) ){
		gauge->drawPolarSink(polar_sink);
		// if( int( polar_sink*100.) != int( old_polar_sink*100. ) ){
		// 	drawBow( (*_gauge)(polar_sink), old_sink_bar_val, DISPLAY_H/2-22, bowcolor[BC_BLUE], 3 );
		// 	old_polar_sink = polar_sink;
		// }
	}

	// Battery
	if ( !(tick%40) ) {
		BATgauge->draw(volt);
	}

	// Temperature Value
	temp_status_t mputemp = MPU.getSiliconTempStatus();
	if( (((int)(temp*10) != tempalt) || (mputemp != siliconTempStatusOld)) && !(tick%12)) {
		drawTemperature( 4, 30, temp );
		tempalt=(int)(temp*10);
		siliconTempStatusOld = mputemp;
	}

	// WK-Indicator
	if( FLAP && !(tick%3) )
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

		if ( FLAP->getNegMax() > -3 ) {
			FLAP->drawWingSymbol( wki, wksensor);
		}
	}

	// Cruise mode or circling
	if( ((int)s2fmode != s2fmode_prev) ) { // && !ulmode ){
		drawS2FMode( DISPLAY_W-50, 18, s2fmode );
		s2fmode_prev = (int)s2fmode;
	}
	// Medium Climb Indicator
	// ESP_LOGI(FNAME,"acl:%f nt:%d", acl, average_climbf, !(tick%9) );
	if( acl != average_climbf && !(tick%9) && acl > 0 ){
		drawAvg( acl, acl-average_climbf );
		average_climbf = acl;
	}
	if ( bottom_dirty ) {
		ESP_LOGI(FNAME,"redraw sale around %f", -_range+2);
		bottom_dirty = false;
		gauge->drawScale(gauge->getMRange()+2);
		if ( MCgauge ) {
			MCgauge->forceRedraw();
			MCgauge->draw(MC.get());
		}
		BATgauge->draw(volt);
	}
	// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );
}

void IpsDisplay::drawDisplay( int airspeed, float te, float ate, float polar_sink, float altitude,
		float temp, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_setting, float wksensor )
{
	xSemaphoreTake(display_mutex,portMAX_DELAY);
	if( display_style.get() == DISPLAY_AIRLINER ) {
		drawAirlinerDisplay( airspeed,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor );
	} else {
		drawRetroDisplay( airspeed,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_setting, wksensor );
	}
	xSemaphoreGive(display_mutex);
}

void IpsDisplay::drawAirlinerDisplay( int airspeed_kmh, float te_ms, float ate_ms, float polar_sink_ms, float altitude_m,
		float temp, float volt, float s2fd_ms, float s2f_ms, float acl_ms, bool s2fmode, bool standard_setting, float wksensor ){
	if( !(screens_init & INIT_DISPLAY_AIRLINER) ){
		initDisplay();
		screens_init |= INIT_DISPLAY_AIRLINER;
	}
	// ESP_LOGI(FNAME,"IpsDisplay::drawDisplay  TE=%0.1f", te);
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
	if( ALTgauge && !(tick%4) ) {
		ALTgauge->draw(altitude_m);
	}
	// MC Value
	if( MCgauge && !(tick%8) ) {
		MCgauge->draw(MC.get());
	}

	// Temperature ValueAirliner
	if( (int)(temp*10) != tempalt && !(tick%11)) {
		drawTemperature( FIELD_START+18, DISPLAY_H+3, temp );
		tempalt=(int)(temp*10);
	}

	// Battery Symbol
	if ( !(tick%40) ) {
		BATgauge->draw(volt);
	}

	// Bluetooth Symbol etc
	if( !(tick%12) ){
		drawConnection(DISPLAY_W-25, FLOGO);
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
		ucg->undoClipRange();
		ucg->setClipRange( flx+FLOGO/4+3, fly-FLOGO, FLOGO, FLOGO );
		ucg->drawCircle( flx, fly, FLOGO/2 + (FLOGO/4)-2, UCG_DRAW_UPPER_RIGHT);
		ucg->undoClipRange();
		ucg->setClipRange( flx+FLOGO/4+5, fly-FLOGO, FLOGO, FLOGO );
		ucg->drawCircle( flx, fly, FLOGO/2 + (FLOGO/2)-2, UCG_DRAW_UPPER_RIGHT);
		ucg->drawCircle( flx, fly, FLOGO/2 + (FLOGO/2)-3, UCG_DRAW_UPPER_RIGHT);
		ucg->undoClipRange();
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
	}
	// AS
	if( as_prev != airspeed_kmh && !(tick%2)) {
		// draw new
		ucg->setColor(  COLOR_WHITE  );
		// print speed values bar
		ucg->setFont(ucg_font_fub11_hn, true);
		ucg->setClipRange( FIELD_START, dmid-(MAXS2FTRI), ASLEN, (MAXS2FTRI*2) );
		for( int speed = airspeed_kmh-MAXS2FTRI-(fh)-20; speed<airspeed_kmh+MAXS2FTRI+(fh); speed++ )
		{
			if( ((speed%20) == 0 && (speed >= 0)) || speed == -20 ) {
				// blank old values
				ucg->setColor( COLOR_BLACK );
				ucg->drawBox( FIELD_START+6,dmid+(speed-airspeed_kmh)-(fh/2)-9, ASLEN-6, fh+15 );
				int col = 0;
				if ( display_variant.get() == DISPLAY_WHITE_ON_BLACK ) {
					col = abs(((speed-airspeed_kmh)*2));
				}
				else {
					col = abs(255 - abs(((speed-airspeed_kmh)*2)));
				}
				if( speed >= 0 ){
					ucg->setColor(  col,col,col  );
					ucg->setPrintPos(FIELD_START+8,dmid+(speed-airspeed_kmh)+(fh/2));
					ucg->printf("%3d ""- ", speed);
				}
			}
		}
		ucg->undoClipRange();
		// AS cleartext
		drawSmallSpeed(airspeed_kmh, FIELD_START+35, YS2F-fh+3);
		as_prev = airspeed_kmh;
	}
	// S2F command trend triangle
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

	if ( bottom_dirty ) {
		bottom_dirty = false;
		drawLegend();
	}
}
