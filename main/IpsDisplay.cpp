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
#include "freertos/task.h"

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
const int   S2F_TRISIZE = 70; // triangle size quality up/down

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
#define YS2F YVAR+S2FFONTH+HEADFONTH+GAP-6


#define VARBARGAP (HEADFONTH+(HEADFONTH/2)+2)
#define MAXS2FTRI 43

#define YALT (YS2F+S2FFONTH+HEADFONTH+GAP+2*MAXS2FTRI +25 )

#define LOWBAT  11.6    // 20%  -> 0%
#define FULLBAT 12.8    // 100%

#define BTSIZE  5
#define BTW    15
#define BTH    24

int S2FST = 45;
int IASLEN = 0;
static int fh;


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

void IpsDisplay::drawArrowBox( int x, int y, bool arightside ){
	int fh = ucg->getFontAscent();
	int fl = ucg->getStrWidth("123");
	// S2FST = fl+fh/2+4;
	// ucg->drawFrame( x,y-(fh/2)-3,fl+4, fh+6 );
	if( arightside )
		ucg->drawTriangle( x+fl+4,y-(fh/2)-3,x+fl+4,y+(fh/2)+3,x+fl+4+fh/2,y );
	else
		ucg->drawTriangle( x,y-(fh/2)-3,   x,y+(fh/2)+3,   x-fh/2,y );
}

// draw all that does not need refresh when values change
void IpsDisplay::initDisplay() {
	printf("IpsDisplay::initDisplay()\n");
	setup();
	ucg->setColor( COLOR_BLACK );
	ucg->drawBox( 0,0,240,320 );

	ucg->setColor(1, COLOR_BLACK );
	ucg->setColor(0, COLOR_WHITE );
	ucg->setFont(ucg_font_fub11_tr);
	ucg->setPrintPos(0,YVAR-VARFONTH);
	ucg->setColor(0, COLOR_HEADER );
	ucg->print("Var m/s");
	ucg->setPrintPos(FIELD_START,YVAR-VARFONTH);    // 65 -52 = 13

	ucg->print("Average Vario");
	ucg->setColor(0, COLOR_WHITE );

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
	fh = ucg->getFontAscent();
	ucg->setPrintPos(FIELD_START+6,YS2F-(2*fh)-8);
	ucg->setColor(0, COLOR_HEADER );
	ucg->print("IAS    km/h");
	ucg->setPrintPos((DISPLAY_W - ucg->getStrWidth("S2F")-4),YS2F-(2*fh)-8);
	ucg->print("S2F");

	ucg->setColor(0, COLOR_WHITE );
	int mslen = ucg->getStrWidth("km/h");

	// IAS Box
	int fl = 45; // ucg->getStrWidth("200-");

	IASLEN = fl;
	S2FST = IASLEN+16;

	// drawArrowBox( FIELD_START, dmid );

    // S2F Zero
	ucg->drawTriangle( FIELD_START, dmid+5, FIELD_START, dmid-5, FIELD_START+5, dmid);
	ucg->drawTriangle( FIELD_START+IASLEN, dmid, FIELD_START+IASLEN+5, dmid-5, FIELD_START+IASLEN+5, dmid+5);
/*
	ucg->setFont(ucg_font_9x15B_mf);
	ucg->setFontPosCenter();
	ucg->setPrintPos(FIELD_START,dmid+20);
	ucg->print("+20 -");
	ucg->setPrintPos(FIELD_START,dmid+40);
	ucg->print("+40 -");
	ucg->setPrintPos(FIELD_START,dmid-20);
	ucg->print("-20 -");
	ucg->setPrintPos(FIELD_START,dmid-40);
	ucg->print("-40 -");
	ucg->setFontPosBaseline();
*/
	// Altitude
	ucg->setFont(ucg_font_fub11_tr);
	ucg->setPrintPos(FIELD_START,YALT-S2FFONTH);
	ucg->setColor(0, COLOR_HEADER );
	ucg->printf("Altitude QNH %d", (int)(_setup->get()->_QNH +0.5 ) );
	ucg->setColor(0, COLOR_WHITE );
	mslen = ucg->getStrWidth("m");
	ucg->setPrintPos(DISPLAY_W-mslen,YALT);
	ucg->print("m");

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

void IpsDisplay::begin( Setup* asetup ) {
	printf("IpsDisplay::begin\n");
	ucg->begin(UCG_FONT_MODE_SOLID);
	_setup = asetup;
	setup();
}

void IpsDisplay::setup()
{
	printf("IpsDisplay::setup\n");
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
	printf("Pixel per m/s %d\n", _pixpmd );
	_range_clip = _range;
}


int _te=0;
int yalt;
int s2falt=-1;
int s2fdalt=0;
int prefalt=0;
int tevlalt=0;
int chargealt=-1;
int btqueue=-1;
int tempalt = -2000;
int mcalt = -100;
bool s2fmodealt = false;
int s2fclipalt = 0;
int iasalt = -1;
int yposalt = 0;

extern xSemaphoreHandle spiMutex;

#define PMLEN 24

void IpsDisplay::redrawValues()
{
	printf("IpsDisplay::redrawValues()\n");
	chargealt = 101;
	tempalt = -2000;
	s2falt = -1;
	s2fdalt = -1;
	btqueue = -1;
	_te=-200;
	mcalt = -100;
	iasalt = -1;
	prefalt = -1;
}


void IpsDisplay::drawDisplay( int ias, float te, float ate, float altitude, float temp, float volt, float s2fd, float s2f, float acl, bool s2fmode ){
	if( _menu )
			return;
	// printf("IpsDisplay::drawDisplay  TE=%0.1f\n", te);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ucg->setFont(ucg_font_fub35_hn);  // 52 height
	ucg->setColor(  COLOR_WHITE  );
	// ucg->setColor(1, 127, 127, 127);
	if( int(_te*10) != (int)(te*10) ) {
		if( te < 0 ) {
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
		vTaskDelay(1);
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

	// Altitude
	int alt = (int)(altitude+0.5);
	if( alt != prefalt ) {

		ucg->setPrintPos(FIELD_START,YALT);
		ucg->setFont(ucg_font_fub25_hn);
		ucg->printf("  %-4d ", (int)(alt+0.5)  );
		prefalt = alt;
		vTaskDelay(1);
	}
	// MC Value
	float MC = _setup->get()->_MC;
	if( (int)(MC)*10 != mcalt ) {
			ucg->setFont(ucg_font_fur14_hf);
			ucg->setPrintPos(0,DISPLAY_H);
			ucg->setColor(COLOR_HEADER);
			ucg->printf("MC:");
			ucg->setColor(COLOR_WHITE);
			ucg->printf("%0.1f", MC );
			mcalt=(int)MC;
			vTaskDelay(1);
		}

    // Temperature Value
	if( (int)temp*10 != tempalt ) {
		ucg->setFont(ucg_font_fur14_hf);
		ucg->setPrintPos(FIELD_START+20,DISPLAY_H);
		ucg->printf("%-2.1f\xb0""  ", temp );
		tempalt=(int)temp*10;
		vTaskDelay(1);
	}
	// Battery Symbol
	int charge = (int)(( volt - LOWBAT )*100)/( FULLBAT - LOWBAT );
	if(charge < 0)
		charge = 0;
	if( charge > 100 )
		charge = 100;
	if ( chargealt != charge ) {
		ucg->drawBox( DISPLAY_W-40,DISPLAY_H-12, 36, 12  );  // Bat body square
		ucg->drawBox( DISPLAY_W-4,DISPLAY_H-9, 3, 6  );      // Bat pluspole pimple
		if ( charge > 25 )  // >25% grün
			ucg->setColor( COLOR_GREEN ); // green
		else if ( charge < 25 && charge > 10 )
			ucg->setColor( COLOR_YELLOW ); //  yellow
		else if ( charge < 10 )
			ucg->setColor( COLOR_RED ); // red
		int chgpos=(charge*32)/100;
		if(chgpos <= 4)
			chgpos = 4;
		ucg->drawBox( DISPLAY_W-40+2,DISPLAY_H-10, chgpos, 8  );  // Bat charge state
		ucg->setColor( DARK_GREY );
		ucg->drawBox( DISPLAY_W-40+2+chgpos,DISPLAY_H-10, 32-chgpos, 8  );  // Empty bat bar
		ucg->setColor( COLOR_WHITE );
		ucg->setFont(ucg_font_fur14_hf);
		ucg->setPrintPos(DISPLAY_W-80,DISPLAY_H);
		ucg->printf("%3d%%", charge);
		chargealt = charge;
		vTaskDelay(1);
	}
	// Bluetooth Symbol
	int btq=BTSender::queueFull();
	if( btq != btqueue )
	{
		if( _setup->get()->_blue_enable ) {
			ucg_int_t btx=DISPLAY_W-16;
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
		vTaskDelay(1);
	}

 	if( y != yalt ) {
		// if TE value has changed
		int y = int(_clipte*_pixpmd+0.5);
		// we draw here the positive TE value bar
		if ( y > 0 ) {
		  ucg->setColor( LIGHT_GREEN );
		  if( y > yalt ) {
			ucg->drawBox( DISPLAY_LEFT+6, dmid-y, bw, abs(y-yalt) );
		  }
		  if( y < yalt  )
		  {
			ucg->setColor( COLOR_BLACK );
			ucg->drawBox( DISPLAY_LEFT+6, dmid-yalt, bw, abs(yalt-y) );
			ucg->setColor(  COLOR_WHITE  );
		  }
		  if( yalt < 0 )
		  {
			ucg->setColor( COLOR_BLACK );
			ucg->drawBox( DISPLAY_LEFT+6, dmid, bw, abs(yalt) );
			ucg->setColor(  COLOR_WHITE  );
		  }
		  vTaskDelay(1);
		}
		// and now the negative TE value bar
		else  // y < 0
		{   // we have a bigger negative value so fill the delta
			ucg->setColor( COLOR_RED );
			if( y < yalt ) {
			   ucg->drawBox( DISPLAY_LEFT+6, dmid-yalt, bw, abs(yalt-y) );
			}
			if( y > yalt )
			{  // a smaller negative value, blank the delta
				ucg->setColor( COLOR_BLACK );
				ucg->drawBox( DISPLAY_LEFT+6, dmid-y, bw, abs(y-yalt) );
				ucg->setColor(  COLOR_WHITE  );
			}
			if( yalt > 0 )
			{  // blank the overshoot across zero
				ucg->setColor( COLOR_BLACK );
				ucg->drawBox( DISPLAY_LEFT+6, dmid-yalt, bw, yalt );
				ucg->setColor(  COLOR_WHITE  );
			}
			vTaskDelay(1);
		}
		// Small triangle pointing to actual vario value
		if( !s2fmode ){
			// First blank the old one
			ucg->setColor( COLOR_BLACK );
			ucg->drawTriangle( DISPLAY_LEFT+4+bw+3,         dmid-yalt,
							   DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-yalt+TRISIZE,
							   DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-yalt-TRISIZE );
			ucg->setColor(  COLOR_WHITE  );
			ucg->drawTriangle( DISPLAY_LEFT+4+bw+3,         dmid-y,
							   DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-y+TRISIZE,
							   DISPLAY_LEFT+4+bw+3+TRISIZE, dmid-y-TRISIZE );
		}
	    yalt = y;
	    vTaskDelay(1);
	}
 	if( s2fmode !=  s2fmodealt ){
 		ucg->setColor( COLOR_BLACK );
 		// clear whole area
 		ucg->drawBox( DISPLAY_LEFT+4+bw+3, dmid-MAXS2FTRI, TRISIZE, 2*MAXS2FTRI  );
 		s2fmodealt = s2fmode;
 		vTaskDelay(1);
 	}

    // IAS
 	if( iasalt != ias ) {
 		// draw new
 		ucg->setColor(  COLOR_WHITE  );
 		// print speed values bar
 		ucg->setFont(ucg_font_fub11_hn);
 		ucg->setClipRange( FIELD_START, dmid-(MAXS2FTRI), IASLEN+6, (MAXS2FTRI*2) );
 		for( int speed = ias-MAXS2FTRI-(fh); speed<ias+MAXS2FTRI+(fh); speed++ )
 		{
			if( (speed%20) == 0 && (speed >= 0) ) {
				ucg->setColor( COLOR_BLACK );
				ucg->drawBox( FIELD_START+6,dmid+(speed-ias)-(fh/2)-6, IASLEN-6, fh+12 );
				ucg->setColor(  COLOR_WHITE  );
				ucg->setPrintPos(FIELD_START+8,dmid+(speed-ias)+(fh/2));
				ucg->printf("%3d ""-", speed);
			}
 		}
 		ucg->undoClipRange();
 		// IAS cleartext
 		ucg->setFont(ucg_font_fub14_hn);
 		ucg->setPrintPos(FIELD_START+8, YS2F-fh );
		ucg->printf("%3d ", ias);
		iasalt = ias;
		vTaskDelay(1);
 	}
 	// S2F command trend triangle
 	if( (int)s2fd != s2fdalt ) {
        // Clip S2F delta value
 		int s2fclip = s2fd;
		if( s2fclip > MAXS2FTRI )
			s2fclip = MAXS2FTRI;
		if( s2fclip < -MAXS2FTRI )
			s2fclip = -MAXS2FTRI;

        // Arrow pointing there
		if( s2fmode ){
			// erase old
			ucg->setColor( COLOR_BLACK );
			ucg->drawTriangle( DISPLAY_LEFT+4+bw+3+TRISIZE,  dmid+s2fclipalt,
							   DISPLAY_LEFT+4+bw+3, dmid+s2fclipalt+TRISIZE,
							   DISPLAY_LEFT+4+bw+3, dmid+s2fclipalt-TRISIZE );
			ucg->setColor(  COLOR_WHITE  );
			// Draw a new one at current position
			ucg->drawTriangle( DISPLAY_LEFT+4+bw+3+TRISIZE, dmid+(int)s2fclip,
							   DISPLAY_LEFT+4+bw+3, dmid+(int)s2fclip+TRISIZE,
							   DISPLAY_LEFT+4+bw+3, dmid+(int)s2fclip-TRISIZE );
		}
		vTaskDelay(1);
		// S2F value
 		ucg->setFont(ucg_font_fub14_hn);
		int fa=ucg->getFontAscent();
		int fl=ucg->getStrWidth("100");
		ucg->setPrintPos(DISPLAY_W-fl-4, YS2F-fh);
		ucg->printf("%3d  ", (int)(s2falt+0.5)  );
		vTaskDelay(1);
		// draw S2F Delta
		// erase old
		ucg->setColor(  COLOR_BLACK  );
		char s[10];
		sprintf(s,"%+3d  ",(int)(s2fdalt+0.5));
		fl=ucg->getStrWidth(s);
		ucg->setPrintPos( FIELD_START+S2FST+(S2F_TRISIZE/2)-fl/2,yposalt );
		ucg->printf(s);
		int ypos;
		if( s2fd < 0 )
			ypos = dmid+s2fclip-2;  // slower, up
		else
			ypos = dmid+s2fclip+2+fa;
        // new S2F Delta val
		ucg->setColor(  COLOR_WHITE  );

		sprintf(s,"%+3d  ",(int)(s2fd+0.5));
		fl=ucg->getStrWidth(s);
		ucg->setPrintPos( FIELD_START+S2FST+(S2F_TRISIZE/2)-fl/2,ypos );
		ucg->printf(s);
		vTaskDelay(1);
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
 		vTaskDelay(1);
 		// draw new S2F command triangle
 		if( 1 ){
 			if( s2fd < 0 )
 				ucg->setColor( LIGHT_GREEN );
 			else
 				ucg->setColor( COLOR_RED );
			ucg->drawTriangle( FIELD_START+S2FST, dmid,
							   FIELD_START+S2FST+S2F_TRISIZE, dmid,
							   FIELD_START+S2FST+(S2F_TRISIZE/2), dmid+(int)s2fd );
 		}
 		vTaskDelay(1);
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


