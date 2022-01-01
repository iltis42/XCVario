#include "Flarm.h"
#include "logdef.h"
#include "Colors.h"
#include "math.h"
#include "ESPAudio.h"
#include "IpsDisplay.h"
#include "sensor.h"
#include "CircleWind.h"
#include "Router.h"

int Flarm::RX = 0;
int Flarm::TX = 0;
int Flarm::GPS = 0;
int Flarm::Power = 0;
int Flarm::AlarmLevel = 0;
int Flarm::RelativeBearing = 0;
int Flarm::AlarmType = 0;
int Flarm::RelativeVertical = 0;
int Flarm::RelativeDistance = 0;
double Flarm::gndSpeedKnots = 0;
double Flarm::gndCourse = 0;
bool Flarm::gpsOK = false;
char Flarm::ID[8] = "";
int Flarm::bincom = 0;
AdaptUGC* Flarm::ucg;

extern xSemaphoreHandle spiMutex;

// Option to simulate FLARM sentences
const char *flarm[] = {
		"$PFLAU,3,1,2,1,1,-60,2,-100,755,1234*\n",
		"$PFLAU,3,1,2,1,1,-20,2,-100,655,1234*\n",
		"$PFLAU,3,1,2,1,1,-10,2,-80,455,1234*\n",
		"$PFLAU,3,1,2,1,2,10,2,-40,155,1234*\n",
		"$PFLAU,3,1,2,1,2,20,2,-20,155,1234*\n",
		"$PFLAU,3,1,2,1,3,30,2,0,155,1234*\n",
		"$PFLAU,3,1,2,1,3,60,2,20,255,1234*\n",
		"$PFLAU,3,1,2,1,2,80,2,40,455,1234*\n",
		"$PFLAU,3,1,2,1,1,90,2,80,855,1234*\n",
		"$PFLAU,3,1,2,1,1,90,2,80,1555,1234*\n"
};

int sim=100;






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

void Flarm::flarmSim(){
	if( flarm_sim.get() ){
		sim=-3;
		flarm_sim.set( 0 );
	}
	if( sim < 10 ){
		if( sim >= 0 ){
			int cs = Protocols::calcNMEACheckSum( (char *)flarm[sim] );
			char str[80];
			sprintf( str, "%s%02X\r\n", flarm[sim], cs );
			SString sf( str );
			Router::forwardMsg( sf, s1_rx_q );
			ESP_LOGI(FNAME,"Serial FLARM SIM: %s",  sf.c_str() );
		}
		sim++;
	}

}


void Flarm::progress(){  // once per second
	if( timeout )
		timeout--;

	if( !(timeout%2) ){  // every other second
		flarmSim();
	}
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
	float time;
	int date;
	char warn;
	float lat,lon;
	float magvar;
	char dir;
	int cs;
	int calc_cs=Protocols::calcNMEACheckSum( gprmc );
	cs = Protocols::getNMEACheckSum( gprmc );
	if( cs != calc_cs ){
		ESP_LOGW(FNAME,"CHECKSUM ERROR: %s; calculcated CS: %d != delivered CS %d", gprmc, calc_cs, cs );
		return;
	}
	// ESP_LOGI(FNAME,"parseG*RMC: %s", gprmc );
	sscanf( gprmc+3, "RMC,%f,%c,%f,N,%f,E,%lf,%lf,%d,%f,%c*%02x",&time,&warn,&lat,&lon,&gndSpeedKnots,&gndCourse,&date,&magvar,&dir,&cs);
	if( wind_enable.get() != WA_OFF ){
		// ESP_LOGI(FNAME,"Wind enable, gpsOK %d", gpsOK );
		if( warn == 'A' ) {
			if( gpsOK == false ){
				gpsOK = true;
				ESP_LOGI(FNAME,"GPRMC, GPS status changed to good: %s", gprmc );
				CircleWind::gpsStatusChange( true);
			}
			theWind.calculateWind();
			// ESP_LOGI(FNAME,"Track: %3.2f, GPRMC: %s", gndCourse, gprmc );
			CircleWind::newSample( Vector( gndCourse, Units::knots2kmh( gndSpeedKnots ) ) );
		}
		else{
			if( gpsOK == true  ){
				gpsOK = false;
				ESP_LOGI(FNAME,"GPRMC, GPS status changed to bad: %s", gprmc );
				CircleWind::gpsStatusChange( false );
			}
			ESP_LOGI(FNAME,"GPRMC, GPS not OK: %s", gprmc );
		}
	}
	timeout = 10;
	// ESP_LOGI(FNAME,"parseGPRMC() GPS: %d, Speed: %3.1f knots, Track: %3.1f° ", gpsOK, gndSpeedKnots, gndCourse );
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
	float time;
	float lat,lon;
	int Q;
	int numSat;
	float dilutionH;
	float antennaAlt;
	float geoidalSep;
	float age;
	int ID;
	int cs;
	// ESP_LOGV(FNAME,"parseG*GGA: %s", gpgga );
	int calc_cs=Protocols::calcNMEACheckSum( gpgga );
	cs = Protocols::getNMEACheckSum( gpgga );
	if( cs != calc_cs ){
		ESP_LOGW(FNAME,"CHECKSUM ERROR: %s; calculcated CS: %d != delivered CS %d", gpgga, calc_cs, cs );
		return;
	}
	sscanf( gpgga+3, "GGA,%f,%f,N,%f,E,%d,%d,%f,%f,M,%f,M,%f,%d*%02x",&time,&lat,&lon,&Q,&numSat,&dilutionH, &antennaAlt, &geoidalSep, &age, &ID, &cs);

	if( numSat != _numSat && wind_enable.get() != WA_OFF ){
		_numSat = numSat;
		CircleWind::newConstellation( numSat );
	}
	timeout = 10;
}


void Flarm::parsePFLAU( const char *pflau ) {
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

void Flarm::parsePFLAX( SString &msg ) {
	// ESP_LOGI(FNAME,"parsePFLAX");
	// ESP_LOG_BUFFER_HEXDUMP(FNAME, msg.c_str(), msg.length(), ESP_LOG_INFO);
	int start=0;
    if( !strncmp( msg.c_str(), "\n", 1 )  ){
    	start=1;
    }
  // Note, if the Flarm switch to binary mode was accepted, Flarm will answer
  // with $PFLAX,A*2E. In error case you will get as answer $PFLAX,A,<error>*
  // and the Flarm stays in text mode.
  const char* pflax = "$PFLAX,A*2E";
  const unsigned short lenPflax = strlen(pflax);

	if( strlen((msg.c_str()) + start) >= lenPflax &&
	    !strncmp( (msg.c_str()) + start, pflax, lenPflax ) ){
		Flarm::bincom = 5;
		ESP_LOGI(FNAME,"Flarm::bincom %d", Flarm::bincom  );
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
  xSemaphoreGive(spiMutex);
}

void Flarm::tick(){
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
	ucg->printf("Distance");
	ucg->setPrintPos(130,190);
	ucg->printf("Vertical");

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
		ucg->setFont(ucg_font_fub25_hr, true );
		char d[16];
		sprintf(d,"%d m   ", RelativeDistance );
		ucg->printf( d );
		oldDist = RelativeDistance;
	}
    if( oldVertical !=  RelativeVertical ) {
    	ucg->setPrintPos(130, 220 );
    	ucg->setFontPosCenter();
    	ucg->setColor( COLOR_WHITE );
    	ucg->setFont(ucg_font_fub25_hr, true);
    	char v[16];
    	int vdiff = RelativeVertical;
    	const char *unit = "m";
    	if( alt_unit.get() != 0 ){  // then its ft or FL -> feet
    		unit = "ft";
    		vdiff = (vdiff/10)*10;
    	}
    	sprintf(v,"%d %s   ",  vdiff, unit );
    	ucg->printf( v );
    	double relDist =  (double)RelativeDistance;
    	if( RelativeBearing < 0 )
    		relDist = -relDist;
    	float horizontalAngle = RTD( atan2( relDist, (double)RelativeVertical) );
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
    	ucg->setColor( COLOR_WHITE );
    	drawAirplane( 70, 120 );
    	oldBear = RelativeBearing;
    }

    xSemaphoreGive(spiMutex);
}

/*
 * Check the buffer, if the Flarm Exit command can be found. If yes, return
 * true and give back the sequence number of the command.
 */
bool Flarm::checkFlarmTx( uint8_t flarmTx[9], const char* stream, int length, uint8_t* seq )
{
  for( int i=0; i < length; i++ ) {
      // add byte to shift register
      for( int j=1; j < 9; j++ ) {
          flarmTx[j-1] = flarmTx[j];
      }

      flarmTx[8] = stream[i];

      // The Flarm binary exit command has the following content:
      // S: "73 08 00 01 2c 00  12 22 ba" Exit bin mode
      //     |  |---|    |----| |  |---|
      // Start  length   SEQ-NR MSG CRC
      if( flarmTx[0] == 0x73 && // Startframe
          flarmTx[1] == 8 &&    // length 0
          flarmTx[2] == 0 &&    // length 1
          flarmTx[6] == 0x12 ) { // Exit
          // Flarm Exit Sequence found
          seq[0] = flarmTx[4];
          seq[1] = flarmTx[5];
          clearFlarmTx( flarmTx );
          return true;
      }
  }
  return false;
}

/*
 * Check the stream, if the Flarm Exit command is acknowledged. If yes, return
 * true and give back the stream position, where the answer starts.
 */
bool Flarm::checkFlarmRx( uint8_t flarmRx[11],
                          const char* stream,
                          int length,
                          uint8_t* seq,
                          int* start )
{
  for( int i=0; i < length; i++ ) {
      // add byte to shift register
      for( int j=1; j < 11; j++ ) {
          flarmRx[j-1] = flarmRx[j];
      }

      flarmRx[10] = stream[i];

      // R: "73 0a 00 01 5e 12 a0 9e 06 2c 00" Quittung exit bin mode
      //     |  |---|          | |---| |---|
      // Start  length         MSG CRC SEQ-NR-von-S
      if( flarmRx[0] == 0x73 && // Startframe
          flarmRx[1] == 0xa &&  // length 0
          flarmRx[2] == 0 &&    // length 1
          flarmRx[6] == 0xa0 && // ACK
          flarmRx[9] == seq[0] &&
          flarmRx[10] == seq[1] ) {
          // Flarm positive ACK to Exit command found
          *start = i;
          clearFlarmRx( flarmRx );
          return true;
      }
  }
  return false;
}
