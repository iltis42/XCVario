#include "Flarm.h"
#include "logdef.h"
#include "Colors.h"
#include "math.h"
#include "ESPAudio.h"
#include "IpsDisplay.h"

int Flarm::RX = 0;
int Flarm::TX = 0;
int Flarm::GPS = 0;
int Flarm::Power = 0;
int Flarm::AlarmLevel = 0;
int Flarm::RelativeBearing = 0;
int Flarm::AlarmType = 0;
int Flarm::RelativeVertical = 0;
int Flarm::RelativeDistance = 0;
char Flarm::ID[8] = "";
Ucglib_ILI9341_18x240x320_HWSPI* Flarm::ucg;

extern xSemaphoreHandle spiMutex;

/* PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>,<ID>
		$PFLAU,3,1,2,1,2,-30,2,-32,755*FLARM is working properly and currently receives 3 other aircraft.
		The most dangerous of these aircraft is at 11 o’clock, position 32m below and 755m away. It is a level 2 alarm


*/

/*  <AcftType>
 *
0 = unknown
1 = glider / motor glider
2 = tow / tug plane
3 = helicopter / rotorcraft
4 = skydiver
5 = drop plane for skydivers
6 = hang glider (hard)
7 = paraglider (soft)
8 = aircraft with reciprocating engine(s)
9 = aircraft with jet/turboprop engine(s)
A = unknown
B = balloon
C = airship
D = unmanned aerial vehicle (UAV)
E = unknown
F = static object
*/


/*
PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>
e.g.
$PFLAA,0,-1234,1234,220,2,DD8F12,180,,30,-1.4,1*

*/

void Flarm::parsePFLAA( char *pflaa ){

}

#define CENTERX 120
#define CENTERY 120

#define RTD(x) (x*RAD_TO_DEG)
#define DTR(x) (x*DEG_TO_RAD)

int oldDist = 0;
int oldVertical = 0;
int oldBear = 0;
int alarmOld=0;
int tick=0;

void Flarm::parsePFLAU( char *pflau ) {
	int cs;
	int id;
	sscanf( pflau, "$PFLAU,%d,%d,%d,%d,%d,%d,%d,%d,%d,%x*%02x",&RX,&TX,&GPS,&Power,&AlarmLevel,&RelativeBearing,&AlarmType,&RelativeVertical,&RelativeDistance,&id,&cs);
	ESP_LOGI(FNAME,"parsePFLAU() RB: %d ALT:%d  DIST %d",RelativeBearing,RelativeVertical, RelativeDistance );
	sprintf( ID,"%06x", id );
	tick=0;
}

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
	float s = sin( DTR(rb) );
	float c = cos( DTR(rb) );
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

void Flarm::drawAirplane( int x, int y, bool fromBehind ){
	ucg->setColor( COLOR_WHITE );
	if( fromBehind ){
		ucg->drawTetragon( x-30,y-2, x-30,y+2, x+30,y+2, x+30,y-2 );
		ucg->drawTetragon( x-2,y-2, x-2,y-10, x+2,y-10, x+2,y-2 );
		ucg->drawTetragon( x-8,y-12, x-8,y-16, x+8,y-16, x+8,y-12 );
		ucg->drawDisc( x,y, 4, UCG_DRAW_ALL );
	}else{
		ucg->drawTetragon( x-30,y-2, x-30,y+2, x+30,y+2, x+30,y-2 );
		ucg->drawTetragon( x-2,y+25, x-2,y-10, x+2,y-10, x+2,y+25 );
		ucg->drawTetragon( x-8,y+25, x-8,y+21, x+8,y+21, x+8,y+25 );
	}
}

void Flarm::initFlarmWarning(){
	ucg->setPrintPos(15, 20 );
	ucg->setFontPosCenter();
	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub20_hr);
	ucg->printf( "Traffic Alert" );
	ucg->setColor( COLOR_HEADER );
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(130,50);
	ucg->printf("o'Clock");
	ucg->setPrintPos(130,110);
	ucg->printf("Distance");
	ucg->setPrintPos(130,190);
	ucg->printf("Vertical");

	oldDist = 0;
	oldVertical = 0;
	oldBear = 0;
	// Audio::alarm( true, 30, AUDIO_ALARM_FLARM_1 );
}

void Flarm::drawFlarmWarning(){
	ESP_LOGI(FNAME,"drawFlarmWarning");
	if( !( screens_init & INIT_DISPLAY_FLARM ) ){
		initFlarmWarning();
		screens_init |= INIT_DISPLAY_FLARM;
	}
	tick++;
	if( tick > 500 ) // age FLARM alarm in case there is no more input  50 per second = 10 sec
		AlarmLevel = 0;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	int volume=0;
	e_audio_alarm_type_t alarm = AUDIO_ALARM_FLARM_1;
	if( AlarmLevel == 3 ) { // highest, impact 0-8 seconds
		volume = flarm_volume.get();
		alarm = AUDIO_ALARM_FLARM_3;
	}
	else if( AlarmLevel == 2 ){
		volume = flarm_volume.get()/4;
		alarm = AUDIO_ALARM_FLARM_2;
	}
	else if( AlarmLevel == 1 ){ // lowest
		volume = flarm_volume.get()/8;
		alarm = AUDIO_ALARM_FLARM_1;
	}else{
		alarm = AUDIO_ALARM_OFF;
	}

	if( alarm != AUDIO_ALARM_OFF )
		Audio::alarm( true, volume, alarm );
	else
		Audio::alarm( false );

    if( AlarmLevel != alarmOld ) {
    	ucg->setPrintPos(200, 15 );
    	ucg->setFontPosCenter();
    	ucg->setColor( COLOR_WHITE );
    	ucg->setFont(ucg_font_fub20_hr);

    	ucg->printf( "%d ", AlarmLevel );
    	alarmOld = AlarmLevel;
    }
    if( oldDist !=  RelativeDistance ) {
		ucg->setPrintPos(130, 135 );
		ucg->setFontPosCenter();
		ucg->setColor( COLOR_WHITE );
		ucg->setFont(ucg_font_fub20_hr);
		char d[16];
		sprintf(d,"%d m", RelativeDistance );
		ucg->printf( d );
		oldDist = RelativeDistance;
	}
    if( oldVertical !=  RelativeVertical ) {
    	ucg->setPrintPos(130, 215 );
    	ucg->setFontPosCenter();
    	ucg->setColor( COLOR_WHITE );
    	ucg->setFont(ucg_font_fub20_hr);
    	char v[16];
    	sprintf(v,"%d m", RelativeVertical );
    	ucg->printf( v );
    	double relDist =  (double)RelativeDistance;
    	if( RelativeBearing < 0 )
    		relDist = -relDist;
    	float horizontalAngle = RTD( atan2( relDist, (double)RelativeVertical) );
    	ESP_LOGI(FNAME,"horizontalAngle: %f  vert:%d", horizontalAngle, RelativeVertical );

    	drawClearVerticalTriangle( 70, 220, horizontalAngle, 0, 50, 6 );
    	drawAirplane( 70, 220, true );
    	oldVertical = RelativeVertical;
    }
    if( oldBear != RelativeBearing ){
    	ucg->setPrintPos(130, 75 );
    	ucg->setFontPosCenter();
    	ucg->setColor( COLOR_WHITE );
    	ucg->setFont(ucg_font_fub20_hr);
    	char b[16];
    	int quant=15;
    	if( RelativeBearing < 0 )
    		quant=-15;
    	int clock = int((RelativeBearing+quant)/30);
    	if( clock <= 0 )
    		clock += 12;
    	sprintf(b,"  %d  ", clock );
    	ucg->printf( b );
    	drawClearTriangle( 70,120, RelativeBearing, 0, 50, 4 );
    	drawAirplane( 70, 120 );
    	oldBear = RelativeBearing;
    }

    xSemaphoreGive(spiMutex);
}

