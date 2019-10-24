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

int   IpsDisplay::tick = 0;
bool  IpsDisplay::_menu = false;
int   IpsDisplay::_pixpmd = 10;

#define DISPLAY_H 320
#define DISPLAY_W 240

// u8g2_t IpsDisplay::u8g2c;

const int   dmid = 160;   // display middle
const int   bwide = 64;   // total width of bargraph
const int   smfh  = 12;   // small font heigth
const int   hbw   = 12;   // horizontal bar width for unit of bargraph
const int   bw    = 32;   // bar width
const int   trisize = 100; // triangle size quality up/down

#define DISPLAY_LEFT 20

#define TRISIZE 15
#define abs(x)  (x < 0.0 ? -x : x)

#define FIELD_START 80
#define SIGNLEN 24+4
#define GAP 6

#define HEADFONTH 16
#define VARFONTH  42  // fub35_hn
#define YVAR HEADFONTH+VARFONTH
#define YVARMID (YVAR - (VARFONTH/2))

#define S2FFONTH 31
#define YS2F YVAR+S2FFONTH+HEADFONTH+GAP

#define VARBARGAP (HEADFONTH+(HEADFONTH/2))
#define MAXS2FTRI 50

#define YALT (YS2F+S2FFONTH+HEADFONTH+GAP+2*MAXS2FTRI +10 )

#define LOWBAT  11.6    // 20%  -> 0%
#define FULLBAT 12.8    // 100%

#define BTSIZE  5
#define BTW    15
#define BTH    24


Ucglib_ILI9341_18x240x320_HWSPI *IpsDisplay::ucg = 0;

IpsDisplay::IpsDisplay( Ucglib_ILI9341_18x240x320_HWSPI *aucg ) {
    ucg = aucg;
	_setup = 0;
	_dtype = ILI9341;
	_divisons = 5;
	_range_clip = 0;
	_range = 5;
	_clipte = 5;
	tick = 0;
	_dc = GPIO_NUM_MAX;
	_reset = GPIO_NUM_MAX;
	_cs = GPIO_NUM_MAX;
}

IpsDisplay::~IpsDisplay() {
}

float IpsDisplay::_range_clip = 0;
int   IpsDisplay::_divisons = 5;
Setup *IpsDisplay::_setup = 0;
float IpsDisplay::_range = 5;

// draw all that does not need refresh when values change
void IpsDisplay::initDisplay() {
	setup();
	ucg->setColor(255, 255, 255);
	ucg->drawBox( 0,0,240,320 );

	ucg->setColor(1, 255, 255, 255);
	ucg->setColor(0, 0, 0, 0);
	ucg->setFont(ucg_font_fub11_tr);
	ucg->setPrintPos(0,YVAR-VARFONTH);
	ucg->print("Var m/s");
	ucg->setPrintPos(FIELD_START,YVAR-VARFONTH);    // 65 -52 = 13
	ucg->print("Average Vario");

	// print TE scale
	ucg->setFont(ucg_font_9x15B_mf);
	for( int i=_divisons; i >=-_divisons; i-- )
	{
		float legend = ((float)i*_range)/_divisons;  // only print the integers
		int hc = ucg->getFontAscent()/2;
		int y = (int)(dmid - int(legend*_pixpmd));

		if( abs( legend  - int( legend )) < 0.1 ) {
			ucg->setPrintPos(0, y+hc  );
			ucg->printf("%+d",(int)legend );
		}
		ucg->drawHLine( DISPLAY_LEFT, y , 4 );
	}
	ucg->drawVLine( DISPLAY_LEFT+5,      VARBARGAP , DISPLAY_H-(VARBARGAP*2) );
	ucg->drawHLine( DISPLAY_LEFT+5, VARBARGAP , bw+1 );
	ucg->drawVLine( DISPLAY_LEFT+5+bw+1, VARBARGAP, DISPLAY_H-(VARBARGAP*2) );
	ucg->drawHLine( DISPLAY_LEFT+5, DISPLAY_H-(VARBARGAP), bw+1 );

	// Sollfahrt Text
	ucg->setFont(ucg_font_fub11_tr);
	ucg->setPrintPos(FIELD_START,YS2F-S2FFONTH);
	ucg->print("Speed To Fly");
	ucg->setFont(ucg_font_fub11_hr);
	int mslen = ucg->getStrWidth("km/h");
	ucg->setPrintPos(DISPLAY_W-mslen,YS2F);
	ucg->print("km/h");

	// Altitude
	ucg->setFont(ucg_font_fub11_tr);
	ucg->setPrintPos(FIELD_START,YALT-S2FFONTH);
	ucg->printf("Altitude QNH %d", (int)(_setup->get()->_QNH +0.5 ) );

	mslen = ucg->getStrWidth("m");
	ucg->setPrintPos(DISPLAY_W-mslen,YALT);
	ucg->print("m");

	ucg->drawDisc( FIELD_START, DISPLAY_H-4,  4, UCG_DRAW_ALL );
	ucg->setColor(0, 255, 255);
	ucg->drawDisc( FIELD_START, DISPLAY_H-4,  2, UCG_DRAW_ALL );
	ucg->setColor(0, 0, 0);
	ucg->drawVLine( FIELD_START-1, DISPLAY_H-20, 14 );
	ucg->setColor(0, 255, 255);
	ucg->drawVLine( FIELD_START,  DISPLAY_H-20, 14 );
	ucg->setColor(0, 0, 0);
	ucg->drawVLine( FIELD_START+1, DISPLAY_H-20, 14 );
	redrawValues();
}

void IpsDisplay::begin( Setup* asetup ) {
	// printf("IpsDisplay::begin\n");
	ucg->begin(UCG_FONT_MODE_SOLID);
	_setup = asetup;
	setup();
}

void IpsDisplay::setup()
{
	_range = _setup->get()->_range;

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
	printf("Pixel per m/s %d", _pixpmd );
	_range_clip = _range;
}


int _te=0;
int yalt;
int s2falt=0;
int s2fdalt=0;
int prefalt=0;
int tevlalt=0;
int chargealt=-1;
int btqueue=-1;
int tempalt = -2000;
bool s2fmodealt = false;

extern xSemaphoreHandle spiMutex;

#define PMLEN 24

void IpsDisplay::redrawValues()
{
	chargealt = 101;
	tempalt = -2000;
	s2falt = -1;
	s2fdalt = -1;
	btqueue = -1;
	_te=-200;
}

void IpsDisplay::drawDisplay( float te, float ate, float altitude, float temp, float volt, float s2fd, float s2f, float acl, bool s2fmode ){
	if( _menu )
			return;
	// printf("IpsDisplay::drawDisplay\n");
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setFont(ucg_font_fub35_hn);  // 52 height
	ucg->setColor( 0,0,0 );
	// ucg->setColor(1, 127, 127, 127);
	if( int(_te*10) != (int)(te*10) ) {
		if( te < 0 ) {
			// erase V line from +
			ucg->setColor( 255,255,255 );
			ucg->drawVLine( FIELD_START+PMLEN/2-1, YVARMID-PMLEN/2, PMLEN );
			ucg->drawVLine( FIELD_START+PMLEN/2, YVARMID-PMLEN/2, PMLEN );
			ucg->drawVLine( FIELD_START+PMLEN/2+1, YVARMID-PMLEN/2, PMLEN );
			// draw just minus
			ucg->setColor( 0,0,0 );
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
		char tev[10];
		sprintf( tev, "%0.1f  ", abs(te) );
		ucg->setPrintPos(FIELD_START+SIGNLEN,YVAR);
		ucg->print(tev);
		ucg->setFont(ucg_font_fub11_hr);
		int mslen = ucg->getStrWidth(" m/s");
		ucg->setPrintPos(DISPLAY_W-mslen,YVAR);
		ucg->print(" m/s");
		_te = te;
	}

	float _clipte = te;
	if ( te > _range_clip )
		_clipte = _range_clip;
	if ( te < -_range_clip )
		_clipte = -_range_clip;

	int y = int(_clipte*_pixpmd+0.5);

	/*   Zeiger Idee erstmal auf Eis, klappt so einfach nicht
	if( y != yalt ) {
	// Zeiger clear
		ucg->setColor( 255,255,255 );
		for( int i=-2; i<=2; i++ )
			ucg->drawLine( 30+abs(i), dmid-yalt+i, 90, dmid+i );
		// Zeiger draw
		ucg->setColor( 0, 0, 0 );
		for( int i=-2; i<=2; i++ )
			ucg->drawLine( 30+abs(i), dmid-y+i, 90, dmid+i );
	}
	*/

	// S2F
	int _s2f = (int)(s2f+0.5);
	if( _s2f != s2falt ) {
		ucg->setPrintPos(FIELD_START,YS2F);
		ucg->setFont(ucg_font_fub25_hn);
		ucg->printf("   %3d ", (int)(s2f+0.5)  );
		s2falt = _s2f;

	}

	// Altitude
	int alt = (int)(altitude+0.5);
	if( alt != prefalt ) {
		ucg->setPrintPos(FIELD_START,YALT);
		ucg->setFont(ucg_font_fub25_hn);
		ucg->printf("  %4d ", (int)(alt+0.5)  );
		prefalt = alt;
	}

    // Temperature Value
	if( (int)temp*10 != tempalt ) {
		ucg->setFont(ucg_font_fur14_hf);
		ucg->setPrintPos(FIELD_START+12,DISPLAY_H);
		char T[10];
		sprintf( T, "%d\xb0", (int)temp );
		int tlen = ucg->getStrWidth( T );
		ucg->printf(T);
		tempalt=(int)temp*10;
	}

	// Battery Symbol
	int charge = (int)(( volt - LOWBAT )*100)/( FULLBAT - LOWBAT );
	if(charge < 0)
		charge = 0;
	if( charge > 100 )
		charge = 100;
	if ( chargealt != charge ) {
		ucg->drawBox( DISPLAY_W-40,DISPLAY_H-12, 36, 12  );
		ucg->drawBox( DISPLAY_W-4,DISPLAY_H-9, 3, 6  );
		if ( charge > 16 )
			ucg->setColor( 255, 30, 255 ); // green
		else if ( charge < 16 && charge > 8 )
			ucg->setColor( 0, 0, 255 ); //  yellow
		else if ( charge < 4 )
			ucg->setColor( 0, 255, 255 ); // red
		int chgpos=(charge*36)/100;
		if(chgpos <= 4)
			chgpos = 4;
		ucg->drawBox( DISPLAY_W-40+2,DISPLAY_H-10, chgpos, 8  );
		ucg->setColor( 230, 230, 230 );
		ucg->drawBox( DISPLAY_W-40+2+chgpos,DISPLAY_H-10, 32-chgpos, 8  );
		ucg->setColor( 0, 0, 0 );
		ucg->setPrintPos(DISPLAY_W-80,DISPLAY_H);
		ucg->printf("%d%%", charge);
		chargealt = charge;
	}

	// Bluetooth Symbol
	int btq=BTSender::queueFull();
	if( btq != btqueue )
	{
		if( _setup->get()->_blue_enable ) {
			ucg_int_t btx=DISPLAY_W-100;
			ucg_int_t bty=DISPLAY_H-BTH/2;
			if( btq )
				ucg->setColor( 180, 180, 180 );  // dark grey
			else
				ucg->setColor( 255, 255, 0 );  // blue

			ucg->drawRBox( btx-BTW/2, bty-BTH/2, BTW, BTH, BTW/2-1);

			// inner symbol
			ucg->setColor( 0, 0, 0 );
			ucg->drawTriangle( btx, bty, btx+BTSIZE, bty-BTSIZE, btx, bty-2*BTSIZE );
			ucg->drawTriangle( btx, bty, btx+BTSIZE, bty+BTSIZE, btx, bty+2*BTSIZE );
			ucg->drawLine( btx, bty, btx-BTSIZE, bty-BTSIZE );
			ucg->drawLine( btx, bty, btx-BTSIZE, bty+BTSIZE );
		}
		btqueue = btq;
	}

 	if( y != yalt ) {
		// if TE value has changed
		int y = int(_clipte*_pixpmd+0.5);
		// we draw here the positive TE value bar
		if ( y > 0 ) {
		  ucg->setColor( 127,0,255 );
		  if( y > yalt ) {
			ucg->drawBox( DISPLAY_LEFT+6, dmid-y, bw, abs(y-yalt) );
		  }
		  if( y < yalt  )
		  {
			ucg->setColor( 255,255,255 );
			ucg->drawBox( DISPLAY_LEFT+6, dmid-yalt, bw, abs(yalt-y) );
			ucg->setColor( 0,0,0 );
		  }
		  if( yalt < 0 )
		  {
			ucg->setColor( 255,255,255 );
			ucg->drawBox( DISPLAY_LEFT+6, dmid, bw, abs(yalt) );
			ucg->setColor( 0,0,0 );
		  }
		}
		// and now the negative TE value bar
		else  // y < 0
		{   // we have a bigger negative value so fill the delta
			ucg->setColor( 0, 255, 255 );
			if( y < yalt ) {
			   ucg->drawBox( DISPLAY_LEFT+6, dmid-yalt, bw, abs(yalt-y) );
			}
			if( y > yalt )
			{  // a smaller negative value, blank the delta
				ucg->setColor( 255,255,255 );
				ucg->drawBox( DISPLAY_LEFT+6, dmid-y, bw, abs(y-yalt) );
				ucg->setColor( 0,0,0 );
			}
			if( yalt > 0 )
			{  // blank the overshoot across zero
				ucg->setColor( 255,255,255 );
				ucg->drawBox( DISPLAY_LEFT+6, dmid-yalt, bw, yalt );
				ucg->setColor( 0,0,0 );
			}
		}
		// Small triangle pointing to actual vario value
		if( !s2fmode ){
			// First blank the old one
			ucg->setColor( 255,255,255 );
			ucg->drawTriangle( DISPLAY_LEFT+4+bw+3,         dmid-yalt,
							   DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-yalt+TRISIZE,
							   DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-yalt-TRISIZE );
			ucg->setColor( 0,0,0 );
			ucg->drawTriangle( DISPLAY_LEFT+4+bw+3,         dmid-y,
							   DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-y+TRISIZE,
							   DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-y-TRISIZE );
		}
	    yalt = y;
	}
 	if( s2fmode !=  s2fmodealt ){
 		ucg->setColor( 255,255,255 );
 		// clear whole area
 		ucg->drawBox( DISPLAY_LEFT+4+bw+3, dmid-MAXS2FTRI, TRISIZE, 2*MAXS2FTRI  );
 		s2fmodealt = s2fmode;
 	}

 	// S2F command trend triangle
 	if( (int)s2fd != s2fdalt ) {
        // Clip S2F delta value
 		int s2fclip = s2fd;
		if( s2fclip > MAXS2FTRI )
			s2fclip = MAXS2FTRI;
		if( s2fclip < -MAXS2FTRI )
			s2fclip = -MAXS2FTRI;
		int s2fclipalt = s2fdalt;
		if( s2fclipalt > MAXS2FTRI )
				s2fclipalt = MAXS2FTRI;
			if( s2fclipalt < -MAXS2FTRI )
				s2fclipalt = -MAXS2FTRI;
        // Arrow pointing there
		if( s2fmode ){
			// erase old
			ucg->setColor( 255,255,255 );
			ucg->drawTriangle( DISPLAY_LEFT+4+bw+3+TRISIZE,  dmid+s2fclipalt,
							   DISPLAY_LEFT+4+bw+3, dmid+s2fclipalt+TRISIZE,
							   DISPLAY_LEFT+4+bw+3, dmid+s2fclipalt-TRISIZE );
			ucg->setColor( 0,0,0 );
			// Draw a new one at current position
			ucg->drawTriangle( DISPLAY_LEFT+4+bw+3+TRISIZE, dmid+(int)s2fclip,
							   DISPLAY_LEFT+4+bw+3, dmid+(int)s2fclip+TRISIZE,
							   DISPLAY_LEFT+4+bw+3, dmid+(int)s2fclip-TRISIZE );
		}

 		ucg->setFont(ucg_font_fub25_hn);
 		int s2fds = ucg->getFontAscent();
 		if( s2fd > 0 ) {
 			if( s2fdalt < 0 ) {
 				ucg->setColor( 255,255,255 );
 				ucg->drawBox( FIELD_START+20, dmid, 100, 39 );
 			}
 			ucg->setPrintPos( FIELD_START+20, dmid-4 );
 		}
 		else {
 			if( s2fdalt > 0 ) {
 				ucg->setColor( 255,255,255 );
 				ucg->drawBox( FIELD_START+20, dmid-40, 100, 39 );
 			}
 			ucg->setPrintPos( FIELD_START+20, dmid+s2fds+2);
 		}
 		ucg->setColor( 0,0,0 );
 		ucg->printf("%+d  ", (int)(s2fd + 0.5));
 		ucg->setClipRange( FIELD_START+20, dmid-MAXS2FTRI, trisize, MAXS2FTRI*2 );
 		bool clear = false;
 		if( s2fd > 0 ) {
 			if ( (int)s2fd < s2fdalt || (int)s2fdalt < 0 )
 				clear = true;
 		}
 		else {
 			if ( (int)s2fd > s2fdalt || (int)s2fdalt > 0  )
 		 		clear = true;
 		}
 		if( clear ) {
			ucg->setColor( 255,255,255 );
			ucg->drawTriangle( FIELD_START+20, dmid,
							   FIELD_START+20+(trisize/2), dmid+(int)s2fd,
							   FIELD_START+20+(trisize/2), dmid+(int)s2fdalt );
			ucg->drawTriangle( FIELD_START+20+trisize, dmid,
							   FIELD_START+20+(trisize/2), dmid+(int)s2fd,
							   FIELD_START+20+(trisize/2), dmid+(int)s2fdalt );
 		}
 		else{
 			if( s2fd < 0 )
 				ucg->setColor( 127,0,255 );
 			else
 				ucg->setColor( 0,255,255 );
			ucg->drawTriangle( FIELD_START+20, dmid,
							   FIELD_START+20+trisize, dmid,
							   FIELD_START+20+(trisize/2), dmid+(int)s2fd );
 		}
 		ucg->undoClipRange();

 		s2fdalt = (int)s2fd;
 	}
 	ucg->setColor( 0,0,0 );
 	ucg->drawHLine( DISPLAY_LEFT+6, dmid, 200 );
 	xSemaphoreGive(spiMutex);
}


