#include "Flarm.h"
#include "logdef.h"
#include "Colors.h"
#include "ESPAudio.h"
#include "IpsDisplay.h"
#include "math/Trigenometry.h"
#include "sensor.h"
#include "wind/CircleWind.h"
#include <cmath>
#include <ctime>
#include <sys/time.h>

int Flarm::RX = 0;
int Flarm::TX = 0;
int Flarm::GPS = 0;
int Flarm::Power = 0;
int Flarm::AlarmLevel = 0;
int Flarm::RelativeBearing = 0;
int Flarm::AlarmType = 0;
int Flarm::RelativeVertical = 0;
int Flarm::RelativeDistance = 0;
float Flarm::gndSpeedKnots = 0;
float Flarm::gndCourse = 0;
bool Flarm::myGPS_OK = false;
char Flarm::ID[20] = "";
AdaptUGC* Flarm::ucg;

#define CENTERX 120
#define CENTERY 120

int Flarm::oldDist = 0;
int Flarm::oldVertical = 0;
int Flarm::oldBear = 0;
int Flarm::alarmOld=0;
int Flarm::_tick=0;
int Flarm::ext_alt_timer=0;
int Flarm::_numSat=0;
int Flarm::clock_timer=0;
bool Flarm::time_sync=false;



void Flarm::progress(){  // once per second
	// ESP_LOGI(FNAME,"progress, timeout=%d", timeout );
	clock_timer++;
	if( !(clock_timer%3600) ){  // every hour reset sync flag to wait for next valid GPS time
		time_sync = false;
	}
}


void Flarm::tick(){
	if( ext_alt_timer )
		ext_alt_timer--;
};


int rbOld = -500; // outside normal range

void Flarm::drawClearTriangle( int x, int y, int rb, int dist, int size, int factor ) {
	if( rbOld != -500 ){
		drawTriangle( x,y, rbOld, dist, size, factor, true );
	}
	drawTriangle( x,y, rb, dist, size, factor );
	rbOld = rb;
}

int rbVert = -500;

void Flarm::drawClearVerticalTriangle( int x, int y, int rb, int dist, int size, int factor ) {
	if( rbVert != -500 ){
		drawTriangle( x,y, rbVert, dist, size, factor, true );
	}
	drawTriangle( x,y, rb, dist, size, factor );
	rbVert = rb;
}

void Flarm::drawTriangle( int x, int y, int rb, int dist, int size, int factor, bool erase ) {
	float s = fast_sin_deg(rb);
	float c = fast_cos_deg(rb);
	int tipx = (int)(x + s*dist );
	int tipy = (int)(y - c*dist );
	float mx =  x + s*(dist+size);
	float my =  y - c*(dist+size);
	int ax = (int)( mx + (size/factor)*c);
	int ay = (int)( my + (size/factor)*s);
	int bx = (int)( mx - (size/factor)*c);
	int by = (int)( my - (size/factor)*s);
	if( erase )
		ucg->setColor( COLOR_BLACK );
	else
		ucg->setColor( COLOR_RED );
	// ESP_LOGI(FNAME,"s: %f c:%f tipx: %d tipy:%d  mx:%2.2f my:%2.2f  ax:%d ay:%d", s,c, tipx, tipy, mx,my, ax,ay);
	ucg->drawTriangle( tipx, tipy, ax,ay, bx, by );
}

void Flarm::drawAirplane( int x, int y, bool fromBehind, bool smallSize ){
	// ESP_LOGI(FNAME,"drawAirplane x:%d y:%d small:%d", x, y, smallSize );
	if( fromBehind ){
		ucg->drawTetragon( x-30,y-2, x-30,y+2, x+30,y+2, x+30,y-2 );
		ucg->drawTetragon( x-2,y-2, x-2,y-10, x+2,y-10, x+2,y-2 );
		ucg->drawTetragon( x-8,y-12, x-8,y-16, x+8,y-16, x+8,y-12 );
		ucg->drawDisc( x,y, 4, UCG_DRAW_ALL );
	}else{
		if( smallSize ){
			ucg->drawTetragon( x-15,y-1, x-15,y+1, x+15,y+1, x+15,y-1 );  // wings
			ucg->drawTetragon( x-1,y+10, x-1,y-3, x+1,y-3, x+1,y+10 ); // fuselage
			ucg->drawTetragon( x-4,y+10, x-4,y+9, x+4,y+9, x+4,y+10 ); // elevator

		}else{
			ucg->drawTetragon( x-30,y-2, x-30,y+2, x+30,y+2, x+30,y-2 );  // wings
			ucg->drawTetragon( x-2,y+25, x-2,y-10, x+2,y-10, x+2,y+25 ); // fuselage
			ucg->drawTetragon( x-8,y+25, x-8,y+21, x+8,y+21, x+8,y+25 ); // elevator
		}
	}
}

void Flarm::initFlarmWarning(){
	ucg->setPrintPos(15, 25 );
	ucg->setFontPosCenter();
	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub20_hr);
	ucg->printf( "Traffic Alert" );
	ucg->setColor( COLOR_HEADER );
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(130,50);
	ucg->printf("o'Clock");
	ucg->setPrintPos(130,110);
	ucg->printf("Distance %s", Units::DistanceUnit() );
	ucg->setPrintPos(130,190);
	ucg->printf("Vertical %s", Units::AltitudeUnitMeterOrFeet() );

	oldDist = 0;
	oldVertical = 0;
	oldBear = 0;
	// AUDIO->alarm(AUDIO_ALARM_FLARM_1);
}

void Flarm::drawFlarmWarning(){
	// ESP_LOGI(FNAME,"drawFlarmWarning");
	if( !( screens_init & INIT_DISPLAY_FLARM ) ){
		initFlarmWarning();
		screens_init |= INIT_DISPLAY_FLARM;
		ESP_LOGI(FNAME,"init drawFlarmWarning");
	}
	_tick++;
	if( _tick > 500 ) {
		// age FLARM alarm in case there is no more input  50 per second = 10 sec
		AlarmLevel = 0;
	}
	bool dirty = false;
	if( oldDist !=  RelativeDistance ) {
		ucg->setPrintPos(130, 140 );
		ucg->setFontPosCenter();
		ucg->setColor( COLOR_WHITE );
		ucg->setFont(ucg_font_fub25_hr, true);
		char d[32] = "\0";
		int dist = rint(Units::Distance(RelativeDistance)/10)*10;
		sprintf(d,"%d   ",dist);
		ucg->printf( d );
		oldDist = RelativeDistance;
	}
	static int altDiff = 1;
	if( oldVertical !=  RelativeVertical ) {
		int currDiff = RelativeVertical / 50 + 1;
		if ( currDiff < 0 ) { currDiff = 0; }
		if ( currDiff > 1 ) { currDiff = 1; }
		dirty = currDiff != altDiff;
		altDiff = currDiff;
		ucg->setPrintPos(130, 220 );
		ucg->setFontPosCenter();
		ucg->setColor( COLOR_WHITE );
		ucg->setFont(ucg_font_fub25_hr, true);
		char v[32];
		int vdiff = RelativeVertical;
		if( alt_unit.get() != 0 ){  // then its ft or FL -> feet
			vdiff = rint((vdiff/10)*10);
		}
		sprintf(v,"%d    ",  vdiff );
		ucg->printf( v );
		float relDist =  (float)RelativeDistance;
		if( RelativeBearing < 0 )
			relDist = -relDist;
		float horizontalAngle = rad2deg( atan2( relDist, (float)RelativeVertical) );
		ESP_LOGI(FNAME,"horizontalAngle: %f  vert:%d", horizontalAngle, RelativeVertical );

		drawClearVerticalTriangle( 70, 220, horizontalAngle, 0, 50, 6 );
		ucg->setColor( COLOR_WHITE );
		drawAirplane( 70, 220, true );
		oldVertical = RelativeVertical;
	}
	static int side = 1;
	if( oldBear != RelativeBearing ){
		int currSide = RelativeBearing / 30 + 1;
		if ( currSide < 0 ) { currSide = 0; }
		if ( currSide > 1 ) { currSide = 1; }
		dirty |= currSide != side;
		side = currSide;
		ucg->setPrintPos(130, 80 );
		ucg->setFontPosCenter();
		ucg->setColor( COLOR_WHITE );
		ucg->setFont(ucg_font_fub25_hr, true );
		char b[32];
		int quant=15;
		if( RelativeBearing < 0 )
			quant=-15;
		int clock = int((RelativeBearing+quant)/30);
		if( clock <= 0 )
			clock += 12;
		sprintf(b,"  %d  ", clock );
		ucg->printf( b );
		drawClearTriangle( 70,120, RelativeBearing, 0, 50, 4 );
		ucg->setColor( COLOR_WHITE );
		drawAirplane( 70, 120 );
		oldBear = RelativeBearing;
	}

	if( AlarmLevel != alarmOld ) {
		ucg->setPrintPos(200, 25 );
		ucg->setFontPosCenter();
		ucg->setColor( COLOR_WHITE );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->printf( "%d ", AlarmLevel );
		alarmOld = AlarmLevel;
		dirty = true;
	}
	if( dirty && AlarmLevel > 0 ) {
		uint16_t alarm = Audio::encFlarmParam(AUDIO_ALARM_FLARM, AlarmLevel, side, altDiff);
		AUDIO->startSound(alarm);
	}

}
