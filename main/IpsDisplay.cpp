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


#define DISPLAY_H 320
#define DISPLAY_W 240


// u8g2_t IpsDisplay::u8g2c;

const int   dmid = 160;   // display middle
const int   bwide = 64;   // total width of bargraph
const int   smfh  = 12;   // small font heigth
const int   hbw   = 12;   // horizontal bar width for unit of bargraph
const int   bw    = 32;   // bar width
const int   S2F_TRISIZE = 60; // triangle size quality up/down

#define DISPLAY_LEFT 25

#define TRISIZE 15
#define abs(x)  (x < 0.0 ? -x : x)

#define FIELD_START 85
#define SIGNLEN 24+4
#define GAP 12

#define HEADFONTH 16
#define VARFONTH  42  // fub35_hn
#define YVAR HEADFONTH+VARFONTH
#define YVARMID (YVAR - (VARFONTH/2))

#define S2FFONTH 31
#define YS2F YVAR+S2FFONTH+HEADFONTH+GAP-8


#define VARBARGAP (HEADFONTH+(HEADFONTH/2)+2)
#define MAXS2FTRI 43
#define MAXTEBAR ((DISPLAY_H-(VARBARGAP*2))/2)

#define YALT (YS2F+S2FFONTH+HEADFONTH+GAP+2*MAXS2FTRI +25 )

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
	ucg->drawDisc( FIELD_START+10, DISPLAY_H-4,  4, UCG_DRAW_ALL ); // white disk
	ucg->setColor(COLOR_RED);
	ucg->drawDisc( FIELD_START+10, DISPLAY_H-4,  2, UCG_DRAW_ALL );  // red disk
	ucg->setColor(COLOR_WHITE);
	ucg->drawVLine( FIELD_START-1+10, DISPLAY_H-20, 14 );
	ucg->setColor(COLOR_RED);
	ucg->drawVLine( FIELD_START+10,  DISPLAY_H-20, 14 );  // red color
	ucg->setColor(COLOR_WHITE);
	ucg->drawPixel( FIELD_START+10,  DISPLAY_H-21 );  // upper point
	ucg->drawVLine( FIELD_START+1+10, DISPLAY_H-20, 14 );
	redrawValues();
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


void IpsDisplay::drawAvgSymbol( int y, int r, int g, int b ) {
	int size = 6;
	ucg->setColor( r,g,b );
	ucg->drawTetragon( DISPLAY_LEFT+size-1, dmid-y,
			DISPLAY_LEFT,        dmid-y+size,
			DISPLAY_LEFT-size,   dmid-y,
			DISPLAY_LEFT,        dmid-y-size
	);
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

int IpsDisplay::getWk( int wks )
{
	for( int wk=-2; wk<=2; wk++ ){
		if( wks <= wkspeeds[wk+2] && wks >=  wkspeeds[wk+3] )
			return wk;
	}
	if( wks < wkspeeds[5] )
		return 1;
	else if( wks > wkspeeds[0] )
		return -2;
	else
		return 1;
}

void IpsDisplay::drawWkBar( int ypos, float wkf ){
	ucg->setFont(ucg_font_profont22_mr );
	int lfh = ucg->getFontAscent()+4;
	int lfw = ucg->getStrWidth( "+2" );
	int top = ypos-lfh/2;
	if( !wkbox ) {
		ucg->drawFrame(DISPLAY_W-lfw-5, top-3, lfw+4, 2*lfh);
		int tri = ypos+lfh/2-3;
		ucg->drawTriangle( DISPLAY_W-lfw-10, tri-5,  DISPLAY_W-lfw-10,tri+5, DISPLAY_W-lfw-5, tri );
		wkbox = true;
	}
	ucg->setClipRange( DISPLAY_W-lfw-2, top-2, lfw, 2*lfh-2 );
	for( int wk=int(wkf-1); wk<=int(wkf+1) && wk<=2; wk++ ){
		if(wk<-2)
			continue;
		if( wk == 0 )
			sprintf( wkss,"% d", wk);
		else
			sprintf( wkss,"%+d", wk);
		int y=top+(lfh+4)*(5-(wk+2))+(int)((wkf-2)*(lfh+4));
		ucg->setPrintPos(DISPLAY_W-lfw-2, y );
		if( wk == 0 )
			ucg->setColor(COLOR_WHITE);
		else
			ucg->setColor(COLOR_WHITE);
		ucg->printf(wkss);
		if( wk != -2 ) {
			ucg->setColor(COLOR_WHITE);
			ucg->drawHLine(DISPLAY_W-lfw-5, y+3, lfw+4 );
		}
	}
	ucg->undoClipRange();
}

#define DISCRAD 3
#define BOXLEN  12
#define FLAPLEN 14
#define WKSYMST DISPLAY_W-28

void IpsDisplay::drawWkSymbol( int ypos, int wk, int wkalt ){
	ucg->setColor( COLOR_WHITE );
	ucg->drawDisc( WKSYMST, ypos, DISCRAD, UCG_DRAW_ALL );
	ucg->drawBox( WKSYMST, ypos-DISCRAD, BOXLEN, DISCRAD*2+1  );
	ucg->setColor( COLOR_BLACK );
	ucg->drawTriangle( WKSYMST+DISCRAD+BOXLEN-2, ypos-DISCRAD,
			WKSYMST+DISCRAD+BOXLEN-2, ypos+DISCRAD+1,
			WKSYMST+DISCRAD+BOXLEN-2+FLAPLEN, ypos+wkalt*4 );
	ucg->setColor( COLOR_RED );
	ucg->drawTriangle( WKSYMST+DISCRAD+BOXLEN-2, ypos-DISCRAD,
			WKSYMST+DISCRAD+BOXLEN-2, ypos+DISCRAD+1,
			WKSYMST+DISCRAD+BOXLEN-2+FLAPLEN, ypos+wk*4 );
}


void IpsDisplay::drawDisplay( int ias, float te, float ate, float polar_sink, float altitude,
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
			drawWkBar( YS2F-fh, (float)(wk)/10 );
			wkposalt = wk;
		}
		if( wki != wkialt ) {
			drawWkSymbol( YS2F-fh-25, wki, wkialt );
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
			ucg->setFont(ucg_font_fub11_hr);
			ucg->setPrintPos(5,DISPLAY_H-8);
			ucg->setColor(COLOR_HEADER);
			ucg->printf("MC:");
			ucg->setPrintPos(5+ucg->getStrWidth("MC:"),DISPLAY_H-4);
			ucg->setColor(COLOR_WHITE);
			ucg->setFont(ucg_font_fur14_hf);
			ucg->printf("%1.1f", MC.get() );
			mcalt=aMC;
		}
	}

	// Temperature Value
	if( (int)(temp*10) != tempalt && !(tick%11)) {
		ucg->setFont(ucg_font_fur14_hf);
		ucg->setPrintPos(FIELD_START+20,DISPLAY_H);
		if( temp != DEVICE_DISCONNECTED_C )
			ucg->printf("%-2.1f\xb0""  ", temp );
		else
			ucg->printf(" ---   ");
		tempalt=(int)(temp*10);
	}
	// Battery Symbol

	int chargev = (int)( volt *10 );
	if ( chargealt != chargev  ) {
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
		chargealt = chargev;
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

	// Bluetooth Symbol

	if( !(tick%12) )
	{
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

	if ( average_climb !=  (int)(acl*10) && !(tick%10) ){
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


