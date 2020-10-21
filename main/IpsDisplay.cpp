/*
 * IpsDisplay.cpp
 *
 *  Created on: Oct 7, 2019
 *      Author: iltis
 *
 */


#include "sdkconfig.h"
#include <stdio.h>
#include <string.h>
#include <Ucglib.h>
#include "IpsDisplay.h"
#include "BTSender.h"
#include "DallasRmt.h"
#include "freertos/task.h"
#include <logdef.h>


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
#define abs(x)  (x < 0.0 ? -x : x)

#define FIELD_START 85
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
int IpsDisplay::prefalt=0;
int IpsDisplay::chargealt=-1;
int IpsDisplay::btqueue=-1;
int IpsDisplay::tempalt = -2000;
int IpsDisplay::mcalt = -100;
bool IpsDisplay::s2fmodealt = false;
int IpsDisplay::s2fclipalt = 0;
int IpsDisplay::iasalt = -1;
int IpsDisplay::yposalt = 0;
int IpsDisplay::tyalt = 0;
int IpsDisplay::pyalt = 0;
int IpsDisplay::wkalt = -3;
int IpsDisplay::wkspeeds[6];
ucg_color_t IpsDisplay::wkcolor;
char IpsDisplay::wkss[6];
int IpsDisplay::wkposalt;
int IpsDisplay::wkialt;

float IpsDisplay::_range_clip = 0;
int   IpsDisplay::_divisons = 5;
float IpsDisplay::_range = 5;
int IpsDisplay::average_climb = -100;
bool IpsDisplay::wkbox = false;
int  IpsDisplay::pref_qnh = 0;


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
	int fh = ucg->getFontAscent();
	int fl = ucg->getStrWidth("123");
	if( arightside )
		ucg->drawTriangle( x+fl+4,y-(fh/2)-3,x+fl+4,y+(fh/2)+3,x+fl+4+fh/2,y );
	else
		ucg->drawTriangle( x,y-(fh/2)-3,   x,y+(fh/2)+3,   x-fh/2,y );
}

void IpsDisplay::drawLegend( bool onlyLines ) {
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
	ucg->setColor( COLOR_BLACK );
	ucg->drawBox( 0,0,240,320 );
}

void IpsDisplay::bootDisplay() {
	ESP_LOGI(FNAME,"IpsDisplay::bootDisplay()");
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
	ESP_LOGI(FNAME,"IpsDisplay::initDisplay()");
	bootDisplay();
	if( display_style.get() == DISPLAY_RETRO ) {
		initRetroDisplay();
	}
	if( display_style.get() == DISPLAY_AIRLINER ) {
		ucg->setPrintPos(0,YVAR-VARFONTH);
		ucg->setColor(0, COLOR_HEADER );
		if( UNITVAR == 0 ) // m/s
			ucg->print("   m/s ");
		if( UNITVAR == 1 ) // ft/min
			ucg->print("100 ft/m");
		if( UNITVAR == 2 ) // knots
			ucg->print("  Knots");
		ucg->setPrintPos(FIELD_START,YVAR-VARFONTH);    // 65 -52 = 13

		ucg->print("AVG Vario");
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
		ucg->setPrintPos(FIELD_START+6,YS2F-(2*fh)-8);
		ucg->setColor(0, COLOR_HEADER );
		String iasu;
		if( UNITAS == 0 ) // km/h
			iasu = "km/h";
		if( UNITAS == 1 ) // mph
			iasu = "mph";
		if( UNITAS == 2 ) // knots
			iasu = "kt";

		if( airspeed_mode.get() == MODE_IAS )
			ucg->printf("IAS %s", iasu.c_str());
		else if( airspeed_mode.get() == MODE_TAS )
			ucg->printf("TAS %s", iasu.c_str());

		ucg->setPrintPos(ASVALX,YS2F-(2*fh)-8);
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
		redrawValues();
	}
}

// Thermometer
void IpsDisplay::drawThermometer( int x, int y) {
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

float avc_old=0;

void IpsDisplay::drawAvg( float avclimb ){
	ESP_LOGI(FNAME,"drawAvg: av=%.2f", avclimb );
	int pos=130;
	int size=6;
	if( avc_old != -1000 ){
		ucg->setColor( COLOR_BLACK );
		int x=AMIDX - cos((avc_old/_range)*M_PI_2)*pos;
		int y=AMIDY - sin((avc_old/_range)*M_PI_2)*pos;
		ucg->drawTetragon( x+size, y, x,y+size, x-size,y, x,y-size );
		avc_old=avclimb;
	}
	ucg->setColor( COLOR_RED );
	int x=AMIDX - cos((avclimb/_range)*M_PI_2)*pos;
	int y=AMIDY - sin((avclimb/_range)*M_PI_2)*pos;
	ESP_LOGI(FNAME,"drawAvg: x=%d  y=%d", x,y );
	ucg->drawTetragon( x+size,y, x, y+size, x-size,y, x,y-size );
}

void IpsDisplay::redrawValues()
{
	ESP_LOGI(FNAME,"IpsDisplay::redrawValues()");
	chargealt = 101;
	tempalt = -2000;
	s2falt = -1;
	s2fdalt = -1;
	btqueue = -1;
	_te=-200;
	mcalt = -100;
	iasalt = -1;
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
	wkalt = -3;
	wkspeeds[0] = 220;
	wkspeeds[1] = flap_minus_2.get();
	wkspeeds[2] = flap_minus_1.get();
	wkspeeds[3] = flap_0.get();
	wkspeeds[4] = flap_plus_1.get();
	wkspeeds[5] = 60;
	wkbox = false;
	wkposalt = -100;
	wkialt = -3;
	tyalt = -1000;
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

float wkRelPos( float wks, float minv, float maxv ){
	// ESP_LOGI(FNAME,"wks:%f min:%f max:%f", wks, minv, maxv );
	if( wks <= maxv && wks >= minv )
		return ((wks-minv)/(maxv-minv));
	else if( wks > maxv )
		return 1;
	else if( wks < minv )
		return 0.5;
	return 0.5;
}

int IpsDisplay::getWk( float wks )
{
	for( int wk=-2; wk<=2; wk++ ){
		if( wks <= wkspeeds[wk+2] && wks >=  wkspeeds[wk+3] ) {
			return wk;
		}
	}
	if( wks < wkspeeds[5] ) {
		return 1;
	}
	else if( wks > wkspeeds[0] ){
		return -2;
	}
	else {
		return 1;
	}
}

void IpsDisplay::drawWkBar( int ypos, int xpos, float wkf ){
	ucg->setFont(ucg_font_profont22_mr );
	int	lfh = ucg->getFontAscent()+4;
	int lfw = ucg->getStrWidth( "+2" );
	int top = ypos-lfh/2;
	if( !wkbox ) {
		ucg->drawFrame(xpos-5, top-3, lfw+4, 2*lfh);
		int tri = ypos+lfh/2-3;
		ucg->drawTriangle( xpos-10, tri-5,  xpos-10,tri+5, xpos-5, tri );
		wkbox = true;
	}
	ucg->setClipRange( xpos-2, top-2, lfw, 2*lfh-2 );
	for( int wk=int(wkf-1); wk<=int(wkf+1) && wk<=2; wk++ ){
		if(wk<-2)
			continue;
		if( wk == 0 )
			sprintf( wkss,"% d", wk);
		else
			sprintf( wkss,"%+d", wk);
		int y=top+(lfh+4)*(5-(wk+2))+(int)((wkf-2)*(lfh+4));
		ucg->setPrintPos(xpos-2, y );
		ucg->setColor(COLOR_WHITE);
		ucg->printf(wkss);
		if( wk != -2 ) {
			ucg->drawHLine(xpos-5, y+3, lfw+4 );
		}
	}
	ucg->undoClipRange();
}

#define NUMPOS  5
#define MINPOS -2
#define MAXPOS  2

int wkyold=0;

void IpsDisplay::drawBigWkBar( int ypos, int xpos, float wkf ){
	ucg->setFont(ucg_font_profont22_mr );
	ucg->setFontPosCenter();
	int lfh = ucg->getFontAscent()+10;  // a bit place around number
	int lfw = ucg->getStrWidth( "+2" );
	int size = NUMPOS*lfh;
	// draw Frame around and a triangle
	if( !wkbox ) {
		for( int wk=MINPOS; wk<=MAXPOS; wk++ ){
			if( wk == 0 )
				sprintf( wkss,"% d", wk);
			else
				sprintf( wkss,"%+d", wk);
			int y= ypos + lfh*wk;  // negative WK eq. lower position
			ucg->setPrintPos(xpos+2, y);
			ucg->setColor(COLOR_WHITE);
			// print digit
			ucg->printf(wkss);
			// Frame around digit
			ucg->drawFrame(xpos-2, y-(lfh/2), lfw+6, lfh );
		}
		wkbox = true;
	}
	ucg->setClipRange( xpos-15, ypos-size, 15, 2*size );
	// now draw the numbers

	int y = ypos + (int)((wkf)*(lfh) + 0.5 );
	if( wkyold != y ) {
		ucg->setColor(COLOR_BLACK);
		ucg->drawTriangle( xpos-15,wkyold-5,  xpos-15,wkyold+5,  xpos-2,wkyold );
		ucg->setColor(COLOR_WHITE);
		ucg->drawTriangle( xpos-15,y-5,  xpos-15,y+5,  xpos,y );
		wkyold = y;
	}

	ucg->setFontPosBottom();
	ucg->undoClipRange();
}

#define DISCRAD 3
#define BOXLEN  12
#define FLAPLEN 14
#define WKSYMST DISPLAY_W-28


void IpsDisplay::drawWkSymbol( int ypos, int xpos, int wk, int wkalt ){
	ucg->setColor( COLOR_WHITE );
	ucg->drawDisc( xpos, ypos, DISCRAD, UCG_DRAW_ALL );
	ucg->drawBox( xpos, ypos-DISCRAD, BOXLEN, DISCRAD*2+1  );
	ucg->setColor( COLOR_BLACK );
	ucg->drawTriangle( xpos+DISCRAD+BOXLEN-2, ypos-DISCRAD,
			xpos+DISCRAD+BOXLEN-2, ypos+DISCRAD+1,
			xpos+DISCRAD+BOXLEN-2+FLAPLEN, ypos+wkalt*4 );
	ucg->setColor( COLOR_RED );
	ucg->drawTriangle( xpos+DISCRAD+BOXLEN-2, ypos-DISCRAD,
			xpos+DISCRAD+BOXLEN-2, ypos+DISCRAD+1,
			xpos+DISCRAD+BOXLEN-2+FLAPLEN, ypos+wk*4 );
}

void IpsDisplay::drawMC( float mc, bool large ) {
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(5,DISPLAY_H-8);
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
	ucg->drawTriangle( x+6, y+7, x+8, y-4, x+S2FSS, y+3 );
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

void IpsDisplay::drawBT() {
	int btq=BTSender::queueFull();
	if( btq != btqueue ){
		if( blue_enable.get() ) {
			ucg_int_t btx=DISPLAY_W-22;
			ucg_int_t bty=BTH/2;
			if( btq )
				ucg->setColor( COLOR_MGREY );
			else
				ucg->setColor( COLOR_BLUE );  // blue

			ucg->drawRBox( btx-BTW/2, bty-BTH/2, BTW, BTH, BTW/2-1);
			// inner symbol
			ucg->setColor( COLOR_WHITE );
			ucg->drawTriangle( btx, bty, btx+BTSIZE, bty-BTSIZE, btx, bty-2*BTSIZE );
			ucg->drawTriangle( btx, bty, btx+BTSIZE, bty+BTSIZE, btx, bty+2*BTSIZE );
			ucg->drawLine( btx, bty, btx-BTSIZE, bty-BTSIZE );
			ucg->drawLine( btx, bty, btx-BTSIZE, bty+BTSIZE );
		}
		btqueue = btq;
	}
}

void IpsDisplay::drawBat( float volt ) {
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
	ucg->drawBox( BATX-40,BATY-2, 36, 12  );  // Bat body square
	ucg->drawBox( BATX-4, BATY+1, 3, 6  );      // Bat pluspole pimple
	if ( charge > yellow )  // >25% grün
		ucg->setColor( COLOR_GREEN ); // green
	else if ( charge < yellow && charge > red )
		ucg->setColor( COLOR_YELLOW ); //  yellow
	else if ( charge < red )
		ucg->setColor( COLOR_RED ); // red
	int chgpos=(charge*32)/100;
	if(chgpos <= 4)
		chgpos = 4;
	ucg->drawBox( BATX-40+2,BATY, chgpos, 8  );  // Bat charge state
	ucg->setColor( DARK_GREY );
	ucg->drawBox( BATX-40+2+chgpos,BATY, 32-chgpos, 8 );  // Empty bat bar
	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(BATX-40,BATY-8);
	if( battery_display.get() == 0 )
		ucg->printf("%3d%%  ", charge);
	else {
		ucg->setPrintPos(BATX-50,BATY-8);
		ucg->printf("%2.1f V", volt);
	}

	if( charge < red ) {  // blank battery for blinking
		if( (tick%100) == 0 ) {
			ucg->setColor( COLOR_BLACK );
			ucg->drawBox( BATX-40,BATY-2, 40, 12  );
		}
		if( ((tick+50)%100) == 0 )  // trigger redraw
		{
			chargealt++;
		}
	}
}

void IpsDisplay::drawTemperature( int x, int y, float t ) {
	ucg->setFont(ucg_font_fur14_hf);
	ucg->setColor( COLOR_WHITE );
	ucg->setPrintPos(x,y);
	if( t != DEVICE_DISCONNECTED_C )
		ucg->printf("%-2.1f\xb0""  ", t );
	else
		ucg->printf(" ---   ");
}

void IpsDisplay::drawTetragon( float a, int x0, int y0, int l1, int l2, int w, bool del ){
	float si=sin(a);
	float co=cos(a);
	int w2=w+1;
	float xn_0 = x0-l1*co+w2*si;
	float yn_0 = y0-l1*si-w2*co;
	float xn_1 = x0-l1*co-w2*si;
	float yn_1 = y0-l1*si+w2*co;
	float xn_2 = x0-l2*co-w*si;
	float yn_2 = y0-l2*si+w*co;
	float xn_3 = x0-l2*co+w*si;
	float yn_3 = y0-l2*si-w*co;
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
	ucg->setColor(  COLOR_WHITE  );
	ucg->drawTetragon(xn_0,yn_0,xn_1,yn_1,xn_2,yn_2,xn_3,yn_3);
}


void IpsDisplay::drawAnalogScale( int val, int pos ){
	ucg->setFontPosCenter();
	ucg->setFont(ucg_font_fub14_hn);
	int x=AMIDX - cos((val/_range)*M_PI_2)*pos;
	int y=AMIDY+1 - sin((val/_range)*M_PI_2)*pos;
	ucg->setPrintPos(x-8,y);
	ucg->printf("%d", val );
	ucg->setFontPosBottom();
}

void IpsDisplay::initRetroDisplay(){
	redrawValues();
	int modulo=1;
	if( _range > 10 )
		modulo = 2;
	for( int a=-(int)_range; a<=(int)_range; a+=modulo ) {
		ESP_LOGI(FNAME,"a=%d",a);
		drawTetragon( ((float)a/_range)*M_PI_2, AMIDX, AMIDY, 120, 140, 2, false );
	}
    int r = (int)_range;
	drawAnalogScale(-r,150);
	drawAnalogScale(r,150);
	drawAnalogScale(0, 132);
	if((r%2) == 0) {
		drawAnalogScale(r/2,150);
		drawAnalogScale(-r/2,155);
	}
	else{
		drawAnalogScale((r-1)/2,150);
		drawAnalogScale((-r+1)/2,155);
	}
	// Unit's
	String units;
	ucg->setFont(ucg_font_fub11_hr);
	if     ( UNITVAR == 0 )
		units = "m/s";
	else if(  UNITVAR == 1 )
		units="ft/m";
	else if(  UNITVAR == 2 )
		units="kt ";
	ucg->setPrintPos(85,15);
	ucg->print(units.c_str());
	drawBT();
	drawMC( MC.get(), true );
	drawThermometer(  10, 30 );

}


void IpsDisplay::drawRetroDisplay( int ias, float te, float ate, float polar_sink, float altitude,
		float temp, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt ){
	if( _menu )
		return;
	tick++;
	// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f IAS:%d km/h", te, ias );
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	if( te > _range )
		te = _range;
	if( te < -_range )
		te = -_range;

	// S2F given im km/h: Unit adaption for mph and knots
	if( UNITVAR == 1 ){  // mph
		s2f = s2f*0.621371;
		s2fd = s2fd*0.621371;
	}
	else if( UNITVAR == 2 ){ // knots
		s2f = s2f*0.539957;
		s2fd = s2fd*0.539957;
	}

	float a = (te)/(_range) * (M_PI_2);
	if( int(a*100) != int(old_a*100) ) {
		drawTetragon( a, AMIDX, AMIDY, 60, 120, 3 );
		// ESP_LOGI(FNAME,"IpsDisplay::drawRetroDisplay  TE=%0.1f  x0:%d y0:%d x2:%d y2:%d", te, x0, y0, x2,y2 );
	}
	if( (int)(ate*30) != _ate && !(tick%3) ) {
		if( ate > 9.9 )
			ate = 9.9;
		if( ate < -9.9 )
			ate = -9.9;
		ucg->setPrintPos(90, AMIDY+2 );
		ucg->setFontPosCenter();
		ucg->setColor( COLOR_WHITE );
		// ucg->setFont(ucg_font_fub25_hr);
		ucg->setFont(ucg_font_fub35_hn);
		ucg->setClipRange( 90, AMIDY-30, 95, 50 );
		// ucg->drawFrame( 90, AMIDY-30, 95, 50 );
		float pte = ate;
		if( pte < 0 ) {
			pte = -pte;
			ucg->printf("-");
		}
		else
			ucg->printf(" ");
		if( UNITVAR == 0 ) {
			ucg->printf("%0.1f ", pte);
			// ESP_LOGI(FNAME,"pte %f", pte );
		}
		else if(  UNITVAR == 1 ){
			int fpm = (int((pte*196.85)+0.5)/10)*10;
			ucg->printf("%d ", fpm );  // ft/min
		}
		else if(  UNITVAR == 2 )
			ucg->printf("%0.1f ", pte*1.94384 );         // knots
		ucg->setFontPosBottom();
		ucg->undoClipRange();
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
		drawBT();
	}
	// S2F Command triangle
	if( (int)s2fd != s2fdalt && !((tick+1)%2) ) {
		// ESP_LOGI(FNAME,"S2F in");
		int start=120;
		int width=50;
		int maxs2f=55;
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
	if(!(tick%8) ) {
		int alt = (int)(altitude+0.5);
		if( alt != prefalt ) {
			ucg->setColor(  COLOR_WHITE  );
			ucg->setPrintPos(110,273);
			ucg->setFont(ucg_font_fub20_hr);
			if( UNITALT == 0 ) { //m
				ucg->printf("  %-4d m ", alt  );
			}
			if( UNITALT == 1 ){ //feet
				ucg->printf("  %-4d ft ", int((altitude*3.28084) + 0.5)  );
			}
			if( UNITALT == 2 ){ //FL
				ucg->printf("FL %-4d  ", int((altitude*0.0328084) + 0.5)  );
			}
			prefalt = alt;
		}
	}

	int chargev = (int)( volt *10 );
	if ( chargealt != chargev  ) {
		drawBat( volt );
		chargealt = chargev;
	}

	// Temperature Value
	if( (int)(temp*10) != tempalt && !(tick%12)) {
		drawTemperature( 20, 30, temp );
		tempalt=(int)(temp*10);
	}

	// WK-Indicator
	if( flap_enable.get() && !(tick%7) )
	{
		float wkspeed = ias * sqrt( 100.0/( ballast.get() +100.0) );
		int wki = getWk( wkspeed );
		float wkpos=wkRelPos( wkspeed, wkspeeds[wki+3], wkspeeds[wki+2] );
		int wk = (int)((wki - wkpos + 0.5)*10);
		ESP_LOGD(FNAME,"ias:%d wksp:%f wki:%d wk:%d wkpos%f", ias, wkspeed, wki, wk, wkpos );
		if( wkposalt != wk ) {
			ESP_LOGD(FNAME,"WK changed");
			ucg->setColor(  COLOR_WHITE  );
			drawBigWkBar( AMIDY, WKSYMST-4, (float)(wk)/10 );
			wkposalt = wk;
		}
		if( wki != wkialt ) {
			drawWkSymbol( AMIDY-82, WKSYMST-3, wki, wkialt );
			wkialt=wki;
		}
	}
	if( !(tick%11) ){
		drawS2FMode( 180, 16, s2fmode );
	}

	ESP_LOGI(FNAME,"acl:%f iacl:%d, nt:%d", acl, average_climb, !(tick%16) );
	if ( average_climb != (int)(acl*10) && !(tick%16) && acl > 0 ){
		drawAvg( acl );
		average_climb = (int)(acl*10);
	}

	xSemaphoreGive(spiMutex);
}


void IpsDisplay::drawDisplay( int ias, float te, float ate, float polar_sink, float altitude,
		float temp, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt ){
	if( _menu )
		return;

	if( display_style.get() == DISPLAY_AIRLINER )
		drawAirlinerDisplay( ias,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_alt );
	else if( display_style.get() == DISPLAY_RETRO )
		drawRetroDisplay( ias,te,ate, polar_sink, altitude, temp, volt, s2fd, s2f, acl, s2fmode, standard_alt );

}

void IpsDisplay::drawAirlinerDisplay( int ias, float te, float ate, float polar_sink, float altitude,
		float temp, float volt, float s2fd, float s2f, float acl, bool s2fmode, bool standard_alt ){
	if( _menu )
		return;
	// ESP_LOGI(FNAME,"IpsDisplay::drawDisplay  TE=%0.1f", te);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	tick++;

	// S2F given im km/h: Unit adaption for mph and knots
	if( UNITVAR == 1 ){  // mph
		s2f = s2f*0.621371;
		s2fd = s2fd*0.621371;
	}
	else if( UNITVAR == 2 ){ // knots
		s2f = s2f*0.539957;
		s2fd = s2fd*0.539957;
	}

	// WK-Indicator

	if( flap_enable.get() && !(tick%7) )
	{
		float wkspeed = ias * sqrt( 100.0/( ballast.get() +100.0) );
		int wki = getWk( wkspeed );
		float wkpos=wkRelPos( wkspeed, wkspeeds[wki+3], wkspeeds[wki+2] );
		int wk = (int)((wki - wkpos + 0.5)*10);
		if( wkposalt != wk ) {
			// ESP_LOGI(FNAME,"ias:%d wksp:%f wki:%d wk:%d wkpos%f", ias, wkspeed, wki, wk, wkpos );
			ucg->setColor(  COLOR_WHITE  );
			drawWkBar( YS2F-fh, WKSYMST+2, (float)(wk)/10 );
			wkposalt = wk;
		}
		if( wki != wkialt ) {
			drawWkSymbol( YS2F-fh-25, WKSYMST+2, wki, wkialt );
			wkialt=wki;
		}
	}

	ucg->setFont(ucg_font_fub35_hn);  // 52 height
	ucg->setColor(  COLOR_WHITE  );

	// Average Vario
	if( _ate != (int)(ate*10) && !(tick%3) ) {
		if( ate < 0 ) {
			// erase V line from +
			ucg->setColor( COLOR_BLACK );
			ucg->drawVLine( FIELD_START+PMLEN/2-1, YVARMID-PMLEN/2, PMLEN );
			ucg->drawVLine( FIELD_START+PMLEN/2, YVARMID-PMLEN/2, PMLEN );
			ucg->drawVLine( FIELD_START+PMLEN/2+1, YVARMID-PMLEN/2, PMLEN );
			// draw just minus
			ucg->setColor(  COLOR_WHITE  );
			ucg->drawHLine( FIELD_START, YVARMID-1, PMLEN );
			ucg->drawHLine( FIELD_START, YVARMID, PMLEN );
			ucg->drawHLine( FIELD_START, YVARMID+1, PMLEN );
		}
		else {
			// draw just plus
			ucg->drawHLine( FIELD_START, YVARMID-1, PMLEN );
			ucg->drawHLine( FIELD_START, YVARMID, PMLEN );
			ucg->drawHLine( FIELD_START, YVARMID+1, PMLEN );
			ucg->drawVLine( FIELD_START+PMLEN/2-1, YVARMID-PMLEN/2, PMLEN );
			ucg->drawVLine( FIELD_START+PMLEN/2, YVARMID-PMLEN/2, PMLEN );
			ucg->drawVLine( FIELD_START+PMLEN/2+1, YVARMID-PMLEN/2, PMLEN );
		}
		ucg->setPrintPos(FIELD_START+SIGNLEN,YVAR);
		float tep=ate;
		if( tep < 0 )
			tep=-ate;

		if     ( UNITVAR == 0 )
			ucg->printf("%0.1f  ", tep);
		else if(  UNITVAR == 1 ){
			int fpm = (int((tep*196.85)+0.5)/10)*10;
			if( abs(fpm) > 999 ) {
				ucg->setPrintPos(FIELD_START+SIGNLEN,YVAR-8);
				ucg->setFont(ucg_font_fub25_hr);
			}
			ucg->printf("%d   ", fpm );  // ft/min
		}
		else if(  UNITVAR == 2 )
			ucg->printf("%0.1f  ", tep*1.94384 );         // knots

		String units;
		ucg->setFont(ucg_font_fub11_hr);
		if     ( UNITVAR == 0 )
			units = "m/s";
		else if(  UNITVAR == 1 )
			units="ft/m";
		else if(  UNITVAR == 2 )
			units="kt ";
		int mslen = ucg->getStrWidth(units.c_str());
		ucg->setPrintPos(DISPLAY_W-mslen,YVAR-12);
		ucg->print(units.c_str());
		_ate = (int)(ate)*10;
	}

	// Altitude Header

	if( !(tick%24) ){
		int qnh = (int)(QNH.get() +0.5 );
		if( standard_alt )
			qnh = 1013;
		if( qnh != pref_qnh ) {
			ucg->setFont(ucg_font_fub11_tr);
			ucg->setPrintPos(FIELD_START,YALT-S2FFONTH);
			if( standard_alt ) {
				ucg->setColor(0, COLOR_BLACK );
				ucg->printf("Altitude QNH %d ", pref_qnh );
				ucg->setPrintPos(FIELD_START,YALT-S2FFONTH);
				ucg->setColor(0, COLOR_HEADER );
				ucg->printf("Altitude QNE %d ", qnh );
			}
			else {
				ucg->setColor(0, COLOR_BLACK );
				ucg->printf("Altitude QNE %d ", pref_qnh );
				ucg->setPrintPos(FIELD_START,YALT-S2FFONTH);
				ucg->setColor(0, COLOR_HEADER );
				ucg->printf("Altitude QNH %d ", qnh );
			}
			pref_qnh = qnh;
		}
	}
	// Altitude

	if(!(tick%7) ) {
		int alt = (int)(altitude+0.5);
		if( alt != prefalt ) {
			ucg->setColor(  COLOR_WHITE  );
			ucg->setPrintPos(FIELD_START,YALT);
			ucg->setFont(ucg_font_fub25_hr);
			if( UNITALT == 0 ) { //m
				ucg->printf("  %-4d m ", alt  );
			}
			if( UNITALT == 1 ){ //feet
				ucg->printf("  %-4d ft ", int((altitude*3.28084) + 0.5)  );
			}
			if( UNITALT == 2 ){ //FL
				ucg->printf("FL %-4d  ", int((altitude*0.0328084) + 0.5)  );
			}
			prefalt = alt;
		}
	}
	// MC Value
	if(  !(tick%8) ) {
		int aMC = MC.get() * 10;
		if( aMC != mcalt && !(tick%4) ) {
			drawMC( MC.get() );
			mcalt=aMC;
		}
	}

	// Temperature Value
	if( (int)(temp*10) != tempalt && !(tick%11)) {
	    drawTemperature( FIELD_START+2, DISPLAY_H, temp );
		tempalt=(int)(temp*10);
	}
	// Battery Symbol

	int chargev = (int)( volt *10 );
	if ( chargealt != chargev  ) {
		drawBat( volt );
		chargealt = chargev;
	}

	// Bluetooth Symbol

	if( !(tick%12) )
	{
		drawBT();
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


	int s2fclip = s2fd;
	if( s2fclip > MAXS2FTRI )
		s2fclip = MAXS2FTRI;
	if( s2fclip < -MAXS2FTRI )
		s2fclip = -MAXS2FTRI;

	int ty = 0;
	if( UNITVAR == 0 )  // m/s
			ty = (int)(te*_pixpmd);         // 1 unit = 1 m/s
	else if( UNITVAR == 1 )
		ty = (int)(te*_pixpmd*1.9685);  // 1 unit = 100 ft/min
	else if( UNITVAR == 2 )
		ty = (int)(te*_pixpmd*1.94384); // 1 unit = 1 kt

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

		if( ps_display.get() )
			setTeBuf(  dmid, py, COLOR_BLUE );
		if( ty > 0 ){
			setTeBuf(  dmid, ty, COLOR_GREEN );
			if( ps_display.get() )
				setTeBuf(  dmid, py, COLOR_GREEN );
		}
		else {
			if( ps_display.get() ) {
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
		// vTaskDelay(3);

	}


	// AS
	if( iasalt != ias && !(tick%2)) {
		// draw new
		int iasp=0;
		if( UNITVAR == 0 ) // km/h
			iasp = ias;
		if( UNITVAR == 1 ) // mph
			iasp = ias*0.621371;
		if( UNITVAR == 2 ) // knots
			iasp = ias*0.539957;

		ucg->setColor(  COLOR_WHITE  );
		// print speed values bar
		ucg->setFont(ucg_font_fub11_hn);
		ucg->drawFrame( FIELD_START, dmid-(MAXS2FTRI)-4, ASLEN+6, (MAXS2FTRI*2)+8 );
		ucg->setClipRange( FIELD_START, dmid-(MAXS2FTRI), ASLEN+6, (MAXS2FTRI*2) );
		for( int speed = iasp-MAXS2FTRI-(fh); speed<iasp+MAXS2FTRI+(fh); speed++ )
		{
			if( (speed%20) == 0 && (speed >= 0) ) {
				// blank old values
				ucg->setColor( COLOR_BLACK );
				if( speed == 0 )
					ucg->drawBox( FIELD_START+6,dmid+(speed-iasp)-(fh/2)-19, ASLEN-6, fh+25 );
				else
					ucg->drawBox( FIELD_START+6,dmid+(speed-iasp)-(fh/2)-9, ASLEN-6, fh+15 );
				int col = abs(((speed-iasp)*2));
				ucg->setColor(  col,col,col  );
				ucg->setPrintPos(FIELD_START+8,dmid+(speed-iasp)+(fh/2));
				ucg->printf("%3d ""-", speed);
			}
		}
		ucg->undoClipRange();
		// AS cleartext
		ucg->setFont(ucg_font_fub14_hn);
		ucg->setPrintPos(FIELD_START+8, YS2F-fh );
		ucg->setColor(  COLOR_WHITE  );
		ucg->printf("%3d ", iasp);
		iasalt = ias;
	}
	// S2F command trend triangle
	if( (int)s2fd != s2fdalt && !((tick+1)%2) ) {
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
		ucg->setPrintPos(ASVALX, YS2F-fh);
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
			ypos = dmid+s2fclip+2+fa;
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


