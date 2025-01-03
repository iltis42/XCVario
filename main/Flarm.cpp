#include "Flarm.h"
#include "logdef.h"
#include "Colors.h"
#include "math.h"
#include "ESPAudio.h"
#include "IpsDisplay.h"
#include "sensor.h"
#include "CircleWind.h"
#include "Router.h"
#include <time.h>
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
int Flarm::bincom = 0;
AdaptUGC* Flarm::ucg;

extern xSemaphoreHandle spiMutex;

#define CENTERX 120
#define CENTERY 120

#define RTD(x) (x*RAD_TO_DEG)
#define DTR(x) (x*DEG_TO_RAD)

int Flarm::oldDist = 0;
int Flarm::oldVertical = 0;
int Flarm::oldBear = 0;
int Flarm::alarmOld=0;
int Flarm::_tick=0;
int Flarm::timeout=0;
int Flarm::ext_alt_timer=0;
int Flarm::_numSat=0;
int Flarm::bincom_port=0;
int Flarm::clock_timer=0;
bool Flarm::time_sync=false;



void Flarm::progress(){  // once per second
	if( timeout ){
		timeout--;
	}
	// ESP_LOGI(FNAME,"progress, timeout=%d", timeout );
	clock_timer++;
	if( !(clock_timer%3600) ){  // every hour reset sync flag to wait for next valid GPS time
		time_sync = false;
	}
}

bool Flarm::connected(){
	// ESP_LOGI(FNAME,"timeout=%d", timeout );
	if( timeout > 0 )
		return true;
	else
		return false;
};


void Flarm::drawDownloadInfo() {
	// ESP_LOGI(FNAME,"---> Flarm::drawDownloadInfo is called"  );
	xSemaphoreTake(spiMutex, portMAX_DELAY );
	ucg->setColor( COLOR_WHITE );
	ucg->setFont(ucg_font_fub20_hr);
	ucg->setPrintPos(60, 140);
	ucg->printf("Flarm IGC");
	ucg->setPrintPos(60, 170);
	ucg->printf("download");
	ucg->setPrintPos(60, 200);
	ucg->printf("is running");
	ucg->setFont(ucg_font_fub11_hr);
	ucg->setPrintPos(20, 280);
	ucg->printf("(restarts on end download)");
	xSemaphoreGive(spiMutex);
}

void Flarm::tick(){
	// ESP_LOGI(FNAME,"Flarm tick, bincom: %d", bincom );
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
	// Audio::alarm( true, 30, AUDIO_ALARM_FLARM_1 );
}

void Flarm::drawFlarmWarning(){
	// ESP_LOGI(FNAME,"drawFlarmWarning");
	if( !( screens_init & INIT_DISPLAY_FLARM ) ){
		initFlarmWarning();
		screens_init |= INIT_DISPLAY_FLARM;
		ESP_LOGI(FNAME,"init drawFlarmWarning");
	}
	_tick++;
	if( _tick > 500 ) // age FLARM alarm in case there is no more input  50 per second = 10 sec
		AlarmLevel = 0;
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	float volume=0;
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
		ucg->setPrintPos(200, 25 );
		ucg->setFontPosCenter();
		ucg->setColor( COLOR_WHITE );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->printf( "%d ", AlarmLevel );
		alarmOld = AlarmLevel;
	}
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
	if( oldVertical !=  RelativeVertical ) {
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
		float horizontalAngle = RTD( atan2( relDist, (float)RelativeVertical) );
		ESP_LOGI(FNAME,"horizontalAngle: %f  vert:%d", horizontalAngle, RelativeVertical );

		drawClearVerticalTriangle( 70, 220, horizontalAngle, 0, 50, 6 );
		ucg->setColor( COLOR_WHITE );
		drawAirplane( 70, 220, true );
		oldVertical = RelativeVertical;
	}
	if( oldBear != RelativeBearing ){
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

	xSemaphoreGive(spiMutex);
}
