#include "Flarm.h"
#include "logdef.h"
#include "Colors.h"
#include "math.h"
#include "ESPAudio.h"
#include "IpsDisplay.h"
#include "sensor.h"
#include "CircleWind.h"
#include "Router.h"

#include <Arduino.h>

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

// Option to simulate FLARM sentences
const char *flarm[] = {
		"$PFLAU,3,1,2,1,1,-60,2,-100,355,1234*\n",
		"$PFLAU,3,1,2,1,1,-20,2,-100,255,1234*\n",
		"$PFLAU,3,1,2,1,1,-10,2,-80,175,1234*\n",
		"$PFLAU,3,1,2,1,2,10,2,-40,150,1234*\n",
		"$PFLAU,3,1,2,1,2,20,2,-20,130,1234*\n",
		"$PFLAU,3,1,2,1,3,30,2,0,120,1234*\n",
		"$PFLAU,3,1,2,1,3,60,2,20,125,1234*\n",
		"$PFLAU,3,1,2,1,2,80,2,40,160,1234*\n",
		"$PFLAU,3,1,2,1,1,90,2,80,210,1234*\n",
		"$PFLAU,3,1,2,1,1,90,2,80,280,1234*\n"
};
#define NUM_SIM_DATASETS 10
int Flarm::sim_tick=NUM_SIM_DATASETS*2;


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
void Flarm::parsePFLAA( const char *pflaa ){

}

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

void Flarm::flarmSim(){
	// ESP_LOGI(FNAME,"flarmSim sim-tick: %d", sim_tick);
	if( flarm_sim.get() ){
		sim_tick=-3;
		flarm_sim.set( 0 );
	}
	if( sim_tick < NUM_SIM_DATASETS*2 ){
		if( sim_tick >= 0 ){
			int cs = Protocols::calcNMEACheckSum( (char *)flarm[sim_tick/2] );
			char str[80];
			sprintf( str, "%s%02X\r\n", flarm[sim_tick/2], cs );
			// SString sf( str );
			// Router::forwardMsg( sf, s1_rx_q );
			parsePFLAU( str, true );
			ESP_LOGI(FNAME,"Serial FLARM SIM: %s",  str );
		}
		sim_tick++;
	}
}


void Flarm::progress(){  // once per second
	if( timeout ){
		timeout--;
	}
	// ESP_LOGI(FNAME,"progress, timeout=%d", timeout );
	flarmSim();
}

bool Flarm::connected(){
	// ESP_LOGI(FNAME,"timeout=%d", timeout );
	if( timeout > 0 )
		return true;
	else
		return false;
};

/*
eg1. $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
eg2. $GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
     $GPRMC,201914.00,A,4857.58740,N,00856.94735,E,0.172,122.95,310321,,,A*6D

           225446.00    Time of fix 22:54:46 UTC
           A            Navigation receiver warning A = OK, V = warning
           4916.45,N    Latitude 49 deg. 16.45 min North
           12311.12,W   Longitude 123 deg. 11.12 min West
           000.5        Speed over ground, Knots
           054.7        Course Made Good, True
           191194       Date of fix  19 November 1994
           020.3,E      Magnetic variation 20.3 deg East
 *68          mandatory checksum


 */
void Flarm::parseGPRMC( const char *gprmc ) {
	char warn;
	int cs;
	int calc_cs=Protocols::calcNMEACheckSum( gprmc );
	cs = Protocols::getNMEACheckSum( gprmc );
	if( cs != calc_cs ){
		ESP_LOGW(FNAME,"CHECKSUM ERROR: %s; calculcated CS: %d != delivered CS %d", gprmc, calc_cs, cs );
		return;
	}
	sscanf( gprmc+3, "RMC,%*f,%c,%*f,%*c,%*f,%*c,%f,%f,%*d,%*f,%*c*%*02x", &warn, &gndSpeedKnots, &gndCourse);

	//ESP_LOGI(FNAME,"GPRMC myGPS_OK %d warn %c", myGPS_OK, warn );
	if( warn == 'A' ) {
		if( myGPS_OK == false ){
			myGPS_OK = true;
			if( wind_enable.get() != WA_OFF ){
				CircleWind::gpsStatusChange( true);
			}
			ESP_LOGI(FNAME,"GPRMC, GPS status changed to good, rmc:%s gps:%d", gprmc, myGPS_OK );
		}
		theWind.calculateWind();
		// ESP_LOGI(FNAME,"Track: %3.2f, GPRMC: %s", gndCourse, gprmc );
		CircleWind::newSample( Vector( gndCourse, Units::knots2kmh( gndSpeedKnots ) ) );
	}
	else{
		if( myGPS_OK == true  ){
			myGPS_OK = false;
			ESP_LOGI(FNAME,"GPRMC, GPS status changed to bad, rmc:%s gps:%d", gprmc, myGPS_OK );
			if( wind_enable.get() != WA_OFF ){
				CircleWind::gpsStatusChange( false );
				ESP_LOGW(FNAME,"GPRMC, GPS not OK: %s", gprmc );
			}
		}
	}
	timeout = 10;
	// ESP_LOGI(FNAME,"parseGPRMC() GPS: %d, Speed: %3.1f knots, Track: %3.1f° ", myGPS_OK, gndSpeedKnots, gndCourse );
}

/*
  GPGGA

hhmmss.ss = UTC of position
llll.ll = latitude of position
a = N or S
yyyyy.yy = Longitude of position
a = E or W
x = GPS Quality indicator (0=no fix, 1=GPS fix, 2=Dif. GPS fix)
xx = number of satellites in use
x.x = horizontal dilution of precision
x.x = Antenna altitude above mean-sea-level
M = units of antenna altitude, meters
x.x = Geoidal separation
M = units of geoidal separation, meters
x.x = Age of Differential GPS data (seconds)
xxxx = Differential reference station ID

eg. $GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
    $GPGGA,121318.00,4857.58750,N,00856.95715,E,1,05,3.87,247.7,M,48.0,M,,*52
 */


void Flarm::parseGPGGA( const char *gpgga ) {
	// ESP_LOGI(FNAME,"parseGPGGA");
	int numSat;
	int cs;
	// ESP_LOGI(FNAME,"parseG*GGA: %s", gpgga );
	int calc_cs=Protocols::calcNMEACheckSum( gpgga );
	cs = Protocols::getNMEACheckSum( gpgga );
	if( cs != calc_cs ){
		ESP_LOGW(FNAME,"CHECKSUM ERROR: %s; calculcated CS: %d != delivered CS %d", gpgga, calc_cs, cs );
		return;
	}
	int ret=sscanf( gpgga+3, "GGA,%*f,%*f,%*c,%*f,%*c,%*d,%d,%*f,%*f,M,%*f,M,%*f,%*d*%*02x", &numSat);
	// ESP_LOGI(FNAME,"parseG*GGA: %s numSat=%d ssf_ret=%d", gpgga, numSat, ret );
	if( ret >=1 ){
		if( (numSat != _numSat) && (wind_enable.get() != WA_OFF) ){
			_numSat = numSat;
			CircleWind::newConstellation( numSat );
		}
		timeout = 10;
	}
}

// parsePFLAE $PFLAE,A,0,0*33


void Flarm::parsePFLAE( const char *pflae ) {
	ESP_LOGI(FNAME,"parsePFLAE %s", pflae );
	int cs;
	int calc_cs=Protocols::calcNMEACheckSum( pflae );
	cs = Protocols::getNMEACheckSum( pflae );
	if( cs != calc_cs ){
		ESP_LOGW(FNAME,"CHECKSUM ERROR: %s; calculcated CS: %d != delivered CS %d", pflae, calc_cs, cs );
		return;
	}
	timeout = 10;
	const char* pf = "$PFLAE,A,0,0";
	const unsigned short len = strlen(pf);
	if( !strncmp( pflae, pf, len )  && !SetupCommon::isClient() ){
		ESP_LOGI(FNAME,"got PFLAE");
	}
}


void Flarm::parsePFLAU( const char *pflau, bool sim_data ) {
	if( !sim_data && (sim_tick < NUM_SIM_DATASETS*2) ){
		return;  // drop FLARM data during simulation
	}
	// ESP_LOGI(FNAME,"parsePFLAU");
	int cs;
	int id;
	int calc_cs=Protocols::calcNMEACheckSum( pflau );
	cs = Protocols::getNMEACheckSum( pflau );
	if( cs != calc_cs ){
		ESP_LOGW(FNAME,"CHECKSUM ERROR: %s; calculcated CS: %d != delivered CS %d", pflau, calc_cs, cs );
		return;
	}
	sscanf( pflau, "$PFLAU,%d,%d,%d,%d,%d,%d,%d,%d,%d,%x*%02x",&RX,&TX,&GPS,&Power,&AlarmLevel,&RelativeBearing,&AlarmType,&RelativeVertical,&RelativeDistance,&id,&cs);
	// ESP_LOGI(FNAME,"parsePFLAU() RB: %d ALT:%d  DIST %d",RelativeBearing,RelativeVertical, RelativeDistance );
	sprintf( ID,"%06x", id );
	_tick=0;
	timeout = 10;
}

void Flarm::parsePFLAX( const char *msg, int port ) {
	// ESP_LOGI(FNAME,"parsePFLAX");
	// ESP_LOG_BUFFER_HEXDUMP(FNAME, msg.c_str(), msg.length(), ESP_LOG_INFO);
	int start=0;
	/* Solved now by DataLink frame recognition
	if( !strncmp( msg, "\n", 1 )  ){  // catch case when serial.cpp does not correctly dissect at '\n', needs further evaluation, maybe multiple '\n' sent ?
		start=1;
	}
	*/
	// Note, if the Flarm switch to binary mode was accepted, Flarm will answer
	// with $PFLAX,A*2E. In error case you will get as answer $PFLAX,A,<error>*
	// and the Flarm stays in text mode.
	const char* pflax = "$PFLAX,A*2E";
	const unsigned short lenPflax = strlen(pflax);

	if( strlen(msg + start) >= lenPflax && !strncmp(  msg + start, pflax, lenPflax )  && !SetupCommon::isClient() ){
		bincom_port = port;
		int old = bincom;
		bincom = 5;
		ESP_LOGI(FNAME,"bincom: %d --> %d", old, bincom  );
		timeout = 10;
	}
}

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

// $PGRMZ,880,F,2*3A  $PGRMZ,864,F,2*30
void Flarm::parsePGRMZ( const char *pgrmz ) {
	if ( alt_select.get() != AS_EXTERNAL )
		return;
	int cs;
	int alt1013_ft;
	int calc_cs=Protocols::calcNMEACheckSum( pgrmz );
	cs = Protocols::getNMEACheckSum( pgrmz );
	if( cs != calc_cs ){
		ESP_LOGW(FNAME,"CHECKSUM ERROR: %s; calculcated CS: %d != delivered CS %d", pgrmz, calc_cs, cs );
		return;
	}
	sscanf( pgrmz, "$PGRMZ,%d,F,2",&alt1013_ft );

	alt_external = Units::feet2meters( (float)(alt1013_ft + 0.5) );
	ESP_LOGI(FNAME,"parsePGRMZ() %s: ALT(1013):%5.0f m", pgrmz, alt_external );
	timeout = 10;
	ext_alt_timer = 10;  // Fall back to internal Barometer after 10 seconds
}

void Flarm::setColorByID(int color_id) {
	switch (color_id) {
	case COLOR_ID_BLACK:
		ucg->setColor( COLOR_BLACK );
		break;
	case COLOR_ID_WHITE:
		ucg->setColor( COLOR_WHITE );
		break;
	case COLOR_ID_GREY:
		ucg->setColor( COLOR_MGREY );
		break;
	case COLOR_ID_RED:
		ucg->setColor( COLOR_RED );
		break;
	case COLOR_ID_GREEN:
		ucg->setColor( COLOR_GREEN );
		break;
	case COLOR_ID_BLUE:
		ucg->setColor( COLOR_BLUE );
		break;
	case COLOR_ID_YELLOW:
		ucg->setColor( COLOR_YELLOW );
		break;
	default:
		ucg->setColor( COLOR_WHITE );
		break;
	}
}

int rbOld = -500; // outside normal range

void Flarm::drawClearTriangle( int x, int y, int rb, int dist, int size, int factor, int color_id ) {
	if( rbOld != -500 ){
		drawTriangle( x,y, rbOld, dist, size, factor, color_id, true );
	}
	drawTriangle( x,y, rb, dist, size, factor, color_id );
	rbOld = rb;
}

int rbVert = -500;

void Flarm::drawClearVerticalTriangle( int x, int y, int rb, int dist, int size, int factor, int color_id ) {
	if( rbVert != -500 ){
		drawTriangle( x,y, rbVert, dist, size, factor, color_id, true );
	}
	drawTriangle( x,y, rb, dist, size, factor, color_id );
	rbVert = rb;
}

void Flarm::drawTriangle( int x, int y, int rb, int dist, int size, int factor, int color_id, bool erase ) {
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
		setColorByID( color_id );
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
			//ucg->drawTetragon( x-30,y-2, x-30,y+2, x+30,y+2, x+30,y-2 );  // wings
			//ucg->drawTetragon( x-2,y+25, x-2,y-10, x+2,y-10, x+2,y+25 ); // fuselage
			//ucg->drawTetragon( x-8,y+25, x-8,y+21, x+8,y+21, x+8,y+25 ); // elevator
			// use larger size centered on display, numbers pushed to bottom:
			ucg->drawTetragon( x-45,y-3, x-45,y+3, x+45,y+3, x+45,y-3 );     // wings
			ucg->drawTetragon( x-3,y+37, x-3,y-15, x+3,y-15, x+3,y+37 );     // fuselage
			ucg->drawTetragon( x-12,y+37, x-12,y+31, x+12,y+31, x+12,y+37 ); // elevator
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
	ucg->setPrintPos(130,60);
	ucg->printf("o'Clock");
	ucg->setPrintPos(10,220);
	ucg->printf("Distance %s", Units::DistanceUnit() );
	ucg->setPrintPos(130,220);
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
	int volume=0;
	int alarm_duration=0;   // limit alarm duration, restart when alarm level changes
	e_audio_alarm_type_t alarm = AUDIO_ALARM_FLARM_1;
	if( AlarmLevel == 3 ) { // highest, impact 0-8 seconds
		volume = flarm_volume.get();
		alarm = AUDIO_ALARM_FLARM_3;
		alarm_duration = 2000;
	}
	else if( AlarmLevel == 2 ){
		volume = flarm_volume.get()/4;
		alarm = AUDIO_ALARM_FLARM_2;
		alarm_duration = 1200;
	}
	else if( AlarmLevel == 1 ){ // lowest
		volume = flarm_volume.get()/8;
		alarm = AUDIO_ALARM_FLARM_1;
		alarm_duration = 700;
	}else{
		alarm = AUDIO_ALARM_OFF;
	}

	static int32_t alarm_start_time = 0;
	if (millis() > alarm_start_time + alarm_duration)
		alarm = AUDIO_ALARM_OFF;

	if( alarm != AUDIO_ALARM_OFF )
		Audio::alarm( true, volume, alarm );
	else
		Audio::alarm( false );

	static char oldID[20] = "XXXXXX";
	if( AlarmLevel != alarmOld || strcmp(ID,oldID) != 0 ) {
		alarm_start_time = millis();
		ucg->setPrintPos(200, 25 );
		ucg->setFontPosCenter();
		ucg->setColor( COLOR_WHITE );
		ucg->setFont(ucg_font_fub20_hr, true);
		ucg->printf( "%d ", AlarmLevel );
		alarmOld = AlarmLevel;
		strcpy(oldID,ID);
	}
	if( oldDist !=  RelativeDistance ) {
		ucg->setPrintPos(10, 240 );
		ucg->setFontPosCenter();
		ucg->setColor( COLOR_WHITE );
		ucg->setFont(ucg_font_fub25_hr, true);
		char d[32] = "\0";
		int dist = rint(Units::Distance(RelativeDistance)/10)*10;
		sprintf(d,"%d   ",dist);
		ucg->printf( d );
		oldDist = RelativeDistance;
	}

	int color_id = COLOR_ID_RED ;
	int deadband = ((alt_unit.get() != 0) ? 60 : 20);
	if (RelativeVertical >  deadband) color_id = COLOR_ID_YELLOW;
	if (RelativeVertical < -deadband) color_id = COLOR_ID_GREEN;

	if( oldVertical !=  RelativeVertical ) {
		ucg->setPrintPos(130, 240 );
		ucg->setFontPosCenter();
		setColorByID( color_id );
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

		// Skip drawing vertical airplane altogether:
		// drawClearVerticalTriangle( 70, 220, horizontalAngle, 0, 50, 6, color_id );
		// ucg->setColor( COLOR_WHITE );
		// drawAirplane( 70, 220, true );
		oldVertical = RelativeVertical;
	}
	if( oldBear != RelativeBearing ){
		ucg->setPrintPos(50, 70 );
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
		sprintf(b,"  %d ", clock );
		ucg->printf( b );
		drawClearTriangle( 120,160, RelativeBearing, 0, 50, 4, color_id );
		ucg->setColor( COLOR_WHITE );
		drawAirplane( 120, 160 );
		oldBear = RelativeBearing;
	}

	xSemaphoreGive(spiMutex);
}
