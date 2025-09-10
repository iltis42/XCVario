
#include "Flap.h"

#include "AnalogInput.h"
#include "setup/SetupNG.h"
#include "Units.h"
#include "Colors.h"
#include "setup/SetupMenu.h"
#include "setup/SetupMenuSelect.h"
#include "setup/SetupMenuValFloat.h"
#include "average.h"
#include "KalmanMPU6050.h"
#include "sensor.h"
#include "logdef.h"



Flap* Flap::_instance = nullptr;
Flap* FLAP = nullptr;

#define NUMPOS  (int)( flap_pos_max +1 - flap_neg_max )

// predefined flap labels         // -9,..,-2,-1,+0,+1,+2,..,+9
const std::string_view flap_labels[] = { "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1", "+0",  // 9
		"+1", "+2", "+3", "+4", "+5", "+6", "+7", "+8", "+9",     // 18
		// 0,1,2,3,..,20
		" 0", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10",  // 29
		"11", "12", "13", "14", "15", "16", "17", "18", "19", "20",   // 39
		// N,L,S,3a,3b,A,21,..,27
		" N", " L", " S", "3a", "3b", " A", "21", "22", "23", "24", "25", "26", "27", "T", "" };  // L=41  S=42
// Action Routines
int select_flap_sens_pin(SetupMenuSelect *p){
	ESP_LOGI(FNAME,"select_flap_sens_pin");
	p->clear();
	FLAP = Flap::theFlap();
	if( FLAP ) {
		ESP_LOGI(FNAME,"select_flap_sens_pin, have flap");
		FLAP->configureADC( p->getSelect() );
		if ( FLAP->haveSensor() ) {
			// ESP_LOGI(FNAME,"select_flap_sens_pin, have sensor");
			MYUCG->setPrintPos(5,50);
			MYUCG->setFont(ucg_font_ncenR14_hr, true );
			MYUCG->printf("Check Sensor Reading,");
			MYUCG->setPrintPos(5,80);
			MYUCG->printf("Press Button to exit");
			while( !Rotary->readSwitch() ){
				// ESP_LOGI(FNAME,"SW wait loop");
				MYUCG->setPrintPos(5,120);
				MYUCG->printf("Sensor: %d       ", FLAP->getSensorRaw(256) );
				delay(100);
			}
		}
	}else{
		ESP_LOGI(FNAME,"NO flap");
	}
	MYUCG->setPrintPos(5,280);
	MYUCG->printf("Saved");
	delay( 2000 );
	p->clear();
	return 0;
}

unsigned int Flap::getSensorRaw(int oversampling) {
	return haveSensor() ? sensorAdc->getRaw(oversampling) : 0;
}

int wk_cal_show( SetupMenuSelect *p, int wk, Average<25> &filter){
	MYUCG->setPrintPos(1,60);
	MYUCG->printf("Set Flap %+d   ", wk );
	delay(500);
	int flap = 0;
	int i=0;
	while( !Rotary->readSwitch() && FLAP ){
		i++;
		flap = filter( (int)(FLAP->getSensorRaw(64)) );
		if( !(i%10) ){
			MYUCG->setPrintPos(1,140);
			MYUCG->printf("Sensor: %d      ", flap );
		}
	}
	return flap;
}


int flap_pos_act( SetupMenuValFloat *p ){
	if( FLAP ) {
		FLAP->initSensPos();
	}
	return 0;
}

int flap_cal_act( SetupMenuSelect *p )
{
	ESP_LOGI(FNAME,"WK calibration ( %d ) ", p->getSelect() );
	if( ! FLAP ) {
		return 0;
	}
	if( !FLAP->haveSensor() ){
		p->clear();
		MYUCG->setPrintPos(1,60);
		MYUCG->printf("No Sensor, Abort");
		delay(2000);
		ESP_LOGI(FNAME,"Abort calibration, no signal");
		return 0;
	}
	Average<25> filter;
	if( p->getSelect() ){
		p->clear();
		MYUCG->setPrintPos(1,200);
		MYUCG->setFont(ucg_font_ncenR14_hr, true );
		MYUCG->printf("Press for next");
		MYUCG->setFont(ucg_font_fub25_hr, true );
		int flap;
		if( FLAP->getPosMax() > 2 ){
			flap = wk_cal_show( p,3,filter );
			wk_sens_pos_plus_3.set( flap );
		}
		if( FLAP->getPosMax() > 1 ){
			flap = wk_cal_show( p,2,filter );
			wk_sens_pos_plus_2.set( flap );
		}
		if( FLAP->getPosMax() > 0 ){
			flap = wk_cal_show( p,1,filter );
			wk_sens_pos_plus_1.set( flap );
		}
		flap = wk_cal_show( p,0,filter );
		wk_sens_pos_0.set( flap );

		if( FLAP->getNegMax() < 0 ){
			flap = wk_cal_show( p,-1,filter );
			wk_sens_pos_minus_1.set( flap );
		}
		if( FLAP->getNegMax() < -1 ){
			flap = wk_cal_show( p,-2,filter );
			wk_sens_pos_minus_2.set( flap );
		}
		if( FLAP->getNegMax() < -2 ){
			flap = wk_cal_show( p,-3,filter );
			wk_sens_pos_minus_3.set( flap );
		}

		MYUCG->setPrintPos(1,260);
		MYUCG->setFont(ucg_font_ncenR14_hr, true );
		MYUCG->printf("Saved");
		FLAP->initSensPos();
		ESP_LOGI(FNAME,"Push Button pressed");
		delay(2000);
		p->clear();
	}
	return 0;
}

void Flap::setupSensorMenueEntries(SetupMenu *wkm)
{
	SetupMenuSelect *wkes = new SetupMenuSelect( "Flap Sensor", RST_NONE, select_flap_sens_pin, &flap_sensor );
	wkes->addEntry( "Disable");
	wkes->addEntry( "Enable");
	wkes->setHelp("Option to enable Flap sensor on corresponding IO pin, hardware may differ: check where you get a valid reading");
	wkm->addEntry( wkes );

	SetupMenuSelect *wkcal = new SetupMenuSelect( "Sensor Calibration", RST_NONE, flap_cal_act );
	wkcal->addEntry( "Cancel");
	wkcal->addEntry( "Start");
	wkcal->setHelp(  "Option to calibrate flap Sensor (WK), to indicate current flap setting: Press button after each setting" );
	wkm->addEntry( wkcal );
}

void Flap::setupMenue(SetupMenu *parent){
	ESP_LOGI(FNAME,"Flap::setupMenue");
	if( ! parent  ){
		return;
	}
	SetupMenu* wkm = new SetupMenu("Flap Sensor", setupSensorMenueEntries);
	parent->addEntry( wkm );
}

void Flap::drawSmallBar( float wkf ){
	MYUCG->setFont(ucg_font_profont22_mr, true);
	int	lfh = MYUCG->getFontAscent()+4;
	int lfw = MYUCG->getStrWidth( "+2" );
	int top = barpos_y-lfh/2;
	if( dirty ) {
		MYUCG->setColor(COLOR_HEADER);
		MYUCG->drawFrame(barpos_x-5, top-4, lfw+4, 2*lfh);
		int tri = barpos_y+lfh/2-3;
		MYUCG->setColor(COLOR_GREEN);
		MYUCG->drawTriangle( barpos_x-10, tri-5,  barpos_x-10,tri+5, barpos_x-5, tri );
		MYUCG->setColor(COLOR_WHITE);
		drawWingSymbol(wkf, 0);
		dirty = false;
	}
	MYUCG->setClipRange( barpos_x-2, top-2, lfw, 2*lfh-2 );
	if ((int)((wkf-2)*(lfh+4)) != (int)((wkf_old-2)*(lfh+4))) {
		wkf_old = wkf;
		MYUCG->setColor(COLOR_BLACK);
		MYUCG->drawBox( barpos_x-2, top-2, lfw, 2*lfh-2 );
	}

	for( int wk=int(wkf-1); wk<=int(wkf+1) && wk<=2; wk++ ){
		char position[6];
		if(wk<-3)
			continue;

		sprintf( position,"%s", flapLabels[wk+3]);
		int y=top+(lfh+4)*(5-(wk+2))+(int)((wkf-2)*(lfh+4)+3);
		MYUCG->setPrintPos(barpos_x-2, y );
		MYUCG->setColor(COLOR_WHITE);
		MYUCG->printf(position);
		if( wk != -3 ) {
			MYUCG->drawHLine(barpos_x-5, y+3, lfw+4 );
		}
	}
	MYUCG->undoClipRange();
}

void Flap::drawFrame(int16_t xpos, int16_t ypos){
	MYUCG->drawFrame( xpos-16, ypos-4, 11, 7 );
	MYUCG->drawFrame( xpos-17, ypos-5, 13, 9 );
}

void Flap::drawLever( int16_t xpos, int16_t ypos, int16_t& oldypos, bool warn, bool good ){

	unsigned int curmillis = millis();
	if( warn ){
		// Blink effekt
		if(curmillis - _millis > 150){
			MYUCG->setColor(COLOR_BLACK);
			drawFrame( xpos, oldypos );
			if( warn_color ){
				// ESP_LOGI(FNAME,"WHITE %d", curmillis - _millis);
				MYUCG->setColor(COLOR_WHITE);
				warn_color = false;
			}
			else{
				// ESP_LOGI(FNAME,"RED %d", curmillis - _millis);
				MYUCG->setColor(COLOR_RED);
				warn_color = true;
			}
			_millis = curmillis;
			drawFrame( xpos, ypos );
			oldypos = ypos;
		}
	}
	else{
		MYUCG->setColor(COLOR_BLACK);
		drawFrame( xpos, oldypos );
		if( good ){
			MYUCG->setColor(COLOR_GREEN);
		}
		else{
			MYUCG->setColor(COLOR_WHITE);
		}
		drawFrame( xpos, ypos );
		oldypos = ypos;
	}
}

static bool good_old = false;

void Flap::drawBigBar( float wkf, float wksens ){
	MYUCG->setFont(ucg_font_profont22_mr, true);
	MYUCG->setFontPosCenter();
	int16_t lfh = 24; // MYUCG->getFontAscent()+10;  // a bit place around number
	int16_t lfw = MYUCG->getStrWidth( "+2" );
	int16_t size = NUMPOS*lfh;
	// draw Frame around and a triangle
	if( dirty ) {
		for( int wk=flap_neg_max; wk<=flap_pos_max; wk++ ){
			char position[6];
			// ESP_LOG_BUFFER_HEXDUMP(FNAME, flapLabels[wk+3], 4, ESP_LOG_INFO);
			sprintf( position,"%s", flapLabels[wk+3]);
			int y= barpos_y + lfh*wk;  // negative WK eq. lower position
			// ESP_LOGI(FNAME,"Y: %d lfh:%d wk:%d",y, lfh, wk );
			MYUCG->setPrintPos(barpos_x+2, y+5);
			MYUCG->setColor(COLOR_WHITE);
			// print digit
			MYUCG->print(position);
			// Frame around digit
			MYUCG->setColor(COLOR_HEADER);
			MYUCG->drawFrame(barpos_x-2, y-(lfh/2), lfw+6, lfh );
		}
		if ( flap_neg_max > -3 ) { drawWingSymbol(wkf, wksens); }
		dirty = false;
	}
	// ESP_LOGI(FNAME,"np: %d size: %d",  NUMPOS, size );
	int16_t yclip = barpos_y+flap_neg_max*lfh-(lfh/2);
	MYUCG->setClipRange( barpos_x-17, yclip, 15, size );
	int16_t y = barpos_y + (int)((wkf)*(lfh) + 0.5 );
	int16_t ys = barpos_y + (int)(( wksens )*(lfh) + 0.5 );
	// ESP_LOGI(FNAME,"wkf: %f", wkf);

	tickopt++;
	bool dirty_lever = false;
	// ESP_LOGI(FNAME,"drawBigBar wkf: %.2f y:%d lfh:%d", wkf, y, lfh );
	if( optPosOldY != y ) {  // redraw on change or every half second
		MYUCG->setColor(COLOR_BLACK);
		MYUCG->drawTriangle( barpos_x-15,optPosOldY-5,  barpos_x-15,optPosOldY+5,  barpos_x-2,optPosOldY );
		MYUCG->setColor(COLOR_GREEN);
		MYUCG->drawTriangle( barpos_x-15,y-5,       barpos_x-15,y+5,       barpos_x-2,y );
		optPosOldY = y;
		dirty_lever = true;
	}
	bool warn=false;
	bool good=false;
	if( abs( wkf - wksens) < 0.5 )
		good = true;
	if( abs( wkf - wksens) > 1 )
		warn = true;
	if( sensorOldY != ys || warn || good_old != good || dirty_lever) {  // redraw on change or when status changed
		if( flap_sensor.get() ) {
			// ESP_LOGI(FNAME,"wk lever redraw, old=%d", sensorOldY );
			drawLever( barpos_x, ys, sensorOldY, warn, good );
			good_old = good;
			if( abs(y-ys) < 12 ){
				MYUCG->setColor(COLOR_GREEN);  // redraw triangle
				MYUCG->drawTriangle( barpos_x-15,y-5,       barpos_x-15,y+5,       barpos_x-2,y );
			}
		}
	}
	MYUCG->setFontPosBottom();
	MYUCG->undoClipRange();
}

#define DISCRAD 3
#define BOXLEN  12
#define FLAPLEN 14

void Flap::drawWingSymbol( int16_t wk, float wksens )
{
	static bool warn_old = false;
	static int16_t wk_old = 0; // warn on either warn or wkopt change

	bool warn = wksens > -10. && abs( wk - wksens) > 1.;

	if ( warn == warn_old && wk == wk_old && ! dirty ) return;

	if(warn){
		MYUCG->setColor( COLOR_RED );
	}else{
		MYUCG->setColor( COLOR_HEADER );
	}
	MYUCG->drawDisc( symbolpos_x, symbolpos_y, DISCRAD, UCG_DRAW_ALL );
	MYUCG->drawBox( symbolpos_x, symbolpos_y-DISCRAD, BOXLEN, DISCRAD*2+1  );
	MYUCG->setColor( COLOR_BLACK );
	MYUCG->drawTriangle( symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y-DISCRAD,
			symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y+DISCRAD+1,
			symbolpos_x+DISCRAD+BOXLEN-2+FLAPLEN, symbolpos_y+wk_old*4 );
	if(warn){
		MYUCG->setColor( COLOR_RED );
	}else{
		MYUCG->setColor( COLOR_GREEN );
	}
	MYUCG->drawTriangle( symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y-DISCRAD,
			symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y+DISCRAD+1,
			symbolpos_x+DISCRAD+BOXLEN-2+FLAPLEN, symbolpos_y+wk*4 );

	wk_old = wk;
	warn_old = warn;
}

void  Flap::initSpeeds()
{
	flapSpeeds[0] = 0;
	flapSpeeds[1] = flap_minus_3.get();
	flapSpeeds[2] = flap_minus_2.get();
	flapSpeeds[3] = flap_minus_1.get();
	flapSpeeds[4] = flap_0.get();
	flapSpeeds[5] = flap_plus_1.get();
	flapSpeeds[6] = flap_plus_2.get();
	flapSpeeds[7] = 0;
	// Capture min and max flap positions
	flap_pos_max = 0;
	flap_neg_max = 0;
	for ( int i=0; i<MAX_NR_POS-1; i++ ) {
		if (flap_neg_max == 0) {
			if (flapSpeeds[i] > 0) {
				flap_neg_max = i- ZERO_INDEX;
			}
			else {
				flapSpeeds[i] = 280;
			}
		}
		else if (flapSpeeds[i] > 0) {
			flap_pos_max = i - ZERO_INDEX + 1;
		}
		else {
			flapSpeeds[i] = 50;
		}
		ESP_LOGI( FNAME, "flapspeed %d: %d", i, flapSpeeds[i]);
	}
	flap_neg_max = std::min(flap_neg_max, 0);
	flap_pos_max = std::max(flap_pos_max, 0);
	ESP_LOGI( FNAME, "flap_neg_max %d", flap_neg_max);
	ESP_LOGI( FNAME, "flap_pos_max %d", flap_pos_max);
}

void  Flap::initLabels(){
	flapLabels[0] = flap_labels[wk_label_minus_3.get()].data();
	flapLabels[1] = flap_labels[wk_label_minus_2.get()].data();
	flapLabels[2] = flap_labels[wk_label_minus_1.get()].data();
	flapLabels[3] = flap_labels[wk_label_null_0.get()].data();
	flapLabels[4] = flap_labels[wk_label_plus_1.get()].data();
	flapLabels[5] = flap_labels[wk_label_plus_2.get()].data();
	flapLabels[6] = flap_labels[wk_label_plus_3.get()].data();
}

void Flap::configureADC( int port ){
	ESP_LOGI( FNAME, "Flap::configureADC port: %d", port );
	if( sensorAdc ) {
		delete sensorAdc;
		sensorAdc = nullptr;
	}
	// flap_pos.init(); // remove display until set properly, removed, calls itself configureADC
	if ( SetupCommon::isClient() ) {
		ESP_LOGI( FNAME, "Client role: Abort");
		return; // no analog pin config for the client
	}
	if( flap_sensor.get() != FLAP_SENSOR_DISABLE ) // migration from old settings with multiple IO's
		flap_sensor.set( FLAP_SENSOR_ENABLE );
	if( flap_sensor.get() == FLAP_SENSOR_ENABLE ) { // nonzero -> configured, only one port needed for XCV23+ HW
		sensorAdc = new AnalogInput( -1, ADC_ATTEN_DB_0, ADC_CHANNEL_6, ADC_UNIT_1, true );
	}
	if( sensorAdc != 0 ) {
		ESP_LOGI( FNAME, "Flap sensor properly configured");
		sensorAdc->begin();
		delay(10);
		uint32_t read =  sensorAdc->getRaw();
		if( read == 0  || read >= 4096 ) { // try GPIO pin 34, series 2021-2
			ESP_LOGI( FNAME, "Flap sensor not found or edge value, reading: %d", (int)read);
		} else {
			ESP_LOGI( FNAME, "Flap sensor looks good, reading: %d", (int)read );
		}
	}else{
		ESP_LOGI( FNAME, "Sensor ADC NOT properly configured");
	}

}

Flap::Flap()
{
	configureADC( flap_sensor.get() );
	initSpeeds();
	initLabels();
	initSensPos();
}

Flap::~Flap()
{
	if( sensorAdc ) {
		delete sensorAdc;
		sensorAdc = nullptr;
	}
	_instance = nullptr;
}
Flap*  Flap::theFlap()
{
	if ( ! _instance ) {
		_instance = new Flap();
	}
	return _instance;
}

bool Flap::sensorToLeverPosition( int wks, float&lever ){
	// ESP_LOGI(FNAME,"getSensorWkPos %d", wks);
	int wk=-1000;
	int max = ZERO_INDEX +1 + flap_pos_max;
	int min = ZERO_INDEX -1 + flap_neg_max;
	// ESP_LOGI(FNAME,"getSensorWkPos %d min:%d max:%d", wks, min, max );
	bool reverse = false;
	if( (wk_sens_pos_0.get() - wk_sens_pos_minus_1.get()) < 0 )
		reverse = true;
	for( int i=min; i<=max; i++ ){
		if( reverse ){
			if( (senspos[i] > wks) && (wks > senspos[i+1])) {
				wk = i;
				break;
			}

		}
		else{
			if( (senspos[i] < wks) && (wks < senspos[i+1])) {
				wk = i;
				break;
			}
		}
	}
	if( wk != -1000 ){
		float relative=0.0;
		if( senspos[wk] != senspos[wk+1] ){  // its an integer
			float delta=senspos[wk]-senspos[wk+1];
			float moved=senspos[wk]-wks;
			relative=moved/delta;
		}
		float wkf =(wk-ZERO_INDEX) + relative;
		// ESP_LOGI(FNAME,"getLeverPos(%d): lever: %1.2f wk: %d relative: %1.1f  N:%d X%d ", wks, wkf, wk, relative, min, max  );
		lever = wkf;
		return true;
	}
	return false;
}

void  Flap::progress(){
	if( haveSensor() ) {
		int wkraw = getSensorRaw(16);
		if( wkraw > 4096 )
			wkraw = 4096;
		if( wkraw < 0 )  { // drop erratic negative readings
			ESP_LOGW(FNAME,"negative flap sensor reading: %d", wkraw );
			return;
		}
		// ESP_LOGI(FNAME,"flap sensor =%d", wkraw );
		rawFiltered = rawFiltered + (wkraw - rawFiltered)/6;
		tick++;
		if( !(tick%4) ){
			if( sensorToLeverPosition( rawFiltered, lever ) ){
				// ESP_LOGI(FNAME,"wk sensor=%1.2f  raw=%d", lever, rawFiltered );
				if( lever < flap_neg_max-0.5 )
					lever = flap_neg_max-0.5;
				else if( lever > flap_pos_max+0.5 )
					lever = flap_pos_max+0.5;

				if( (int)(flap_pos.get()*10) != (int)(lever*10) ){
					flap_pos.set( lever );   // update secondary vario
				}
			}
		}
	}
}

void  Flap::initSensPos(){
	// Fixme needs a refresh after synch from master -> blackboard action
	ESP_LOGI(FNAME,"initSensPos");
	if( (wk_sens_pos_0.get() - wk_sens_pos_minus_1.get()) < 0 )
		senspos[ZERO_INDEX-4] = 4095;
	else
		senspos[ZERO_INDEX-4] = 0;

	if( (int)flap_neg_max < -2 )
		senspos[ZERO_INDEX-3] = wk_sens_pos_minus_3.get();
	else{
		// extrapolated
		senspos[ZERO_INDEX-3] = wk_sens_pos_minus_2.get() - ( wk_sens_pos_minus_1.get() - wk_sens_pos_minus_2.get());
		if( senspos[ZERO_INDEX-3] > 4095 )
			senspos[ZERO_INDEX-3] = 4095;
		if( senspos[ZERO_INDEX-3] < 0 )
			senspos[ZERO_INDEX-3] = 0;
	}

	if(  (int)flap_neg_max < -1)
		senspos[ZERO_INDEX-2] = wk_sens_pos_minus_2.get();
	else{
		// extrapolated
		senspos[ZERO_INDEX-2] = wk_sens_pos_minus_1.get() - ( wk_sens_pos_0.get() - wk_sens_pos_minus_1.get());
		if( senspos[ZERO_INDEX-2] > 4095 )
			senspos[ZERO_INDEX-2] = 4095;
		if( senspos[ZERO_INDEX-3] < 0 )
			senspos[ZERO_INDEX-3] = 0;
	}

	if( (int)flap_neg_max < 0 )
		senspos[ZERO_INDEX-1] = wk_sens_pos_minus_1.get();
	else
		senspos[ZERO_INDEX-1] = wk_sens_pos_0.get() - ( wk_sens_pos_plus_1.get() - wk_sens_pos_0.get()); // extrapolated

	senspos[ZERO_INDEX] = wk_sens_pos_0.get();

	if( (int)flap_pos_max > 0 )
		senspos[ZERO_INDEX+1] = wk_sens_pos_plus_1.get();
	else
		senspos[ZERO_INDEX+1] = wk_sens_pos_0.get() - ( wk_sens_pos_minus_1.get() - wk_sens_pos_0.get()); // extrapolated pos pole

	if( (int)flap_pos_max > 1 )
		senspos[ZERO_INDEX+2] = wk_sens_pos_plus_2.get();
	else{
		senspos[ZERO_INDEX+2] = wk_sens_pos_plus_1.get() - ( wk_sens_pos_0.get() - wk_sens_pos_plus_1.get()); // extrapolated pos pole
		if( senspos[ZERO_INDEX+2] < 0 )
			senspos[ZERO_INDEX+2] = 0;
	}
	if( (int)flap_pos_max > 2 )
		senspos[ZERO_INDEX+3] = wk_sens_pos_plus_3.get();
	else{
		senspos[ZERO_INDEX+3] = wk_sens_pos_plus_2.get() - ( wk_sens_pos_plus_1.get() - wk_sens_pos_plus_2.get()); // extrapolated pos pole
		if( senspos[ZERO_INDEX+3] < 0 )
			senspos[ZERO_INDEX+3] = 0;
	}
	if( (wk_sens_pos_0.get() - wk_sens_pos_minus_1.get()) < 0 )
		senspos[ZERO_INDEX+4] = 0;
	else
		senspos[ZERO_INDEX+4] = 4095;
	for( int i=0; i<MAX_NR_POS; i++ ){
		ESP_LOGI(FNAME,"lever: %d  senspos[i]: %d", i-ZERO_INDEX, senspos[i]  );
	}
}

float Flap::getOptimum( float wks, int& wki )
{
	// Correct for current g load
	g_force += (IMU::getGliderAccelZ() - g_force) * 0.02;  // lowpass filtering, to smooth display in rough air
	if ( g_force < 0.3 ) {
		g_force = 0.3; // Ignore meaningless values below 0.3g
	}
	float g_speed = wks / sqrt(g_force); // reduce current speed, instead of increase switch points
	// ESP_LOGI(FNAME,"g force: %.1f, g corrected speed: %3.1f", g_force, g_speed );
	wki = getOptimumInt(g_speed);
	float minv = flapSpeeds[wki+4];
	float maxv = flapSpeeds[wki+3];
	// ESP_LOGI(FNAME,"wks:%f min:%f max:%f wki:%d", wks, minv, maxv, wki);
	if( g_speed <= maxv && g_speed >= minv )
		return ((g_speed-minv)/(maxv-minv));
	else if( g_speed > maxv )
		return 1;
	else if( g_speed < minv )
		return 0.5;
	return 0.5;
}
void Flap::setBarPosition(int16_t x, int16_t y)
{
	barpos_x = x;
	barpos_y = y;
}
void Flap::setSymbolPosition(int16_t x, int16_t y)
{
	symbolpos_x = x;
	symbolpos_y = y;
}

int Flap::getOptimumInt( float speed )
{
	// ESP_LOGI(FNAME,"getOptimumInt( %3.1f )", speed );
	for( int wk=flap_neg_max; wk<=flap_pos_max; wk++ ){
		// ESP_LOGI(FNAME,"Check Flap: %d %d %d", wk, flapSpeeds[wk+3],  flapSpeeds[wk+4] );
		if( speed <= flapSpeeds[wk+3] && speed >=  flapSpeeds[wk+4] ) {
			// ESP_LOGI(FNAME,"Ret: %d", wk);
			return wk;
		}
	}
	if( speed < flapSpeeds[7] ) {
		// ESP_LOGI(FNAME,"Ret: %d", (int)flap_takeoff.get() );
		return flap_takeoff.get();
	}
	else if( speed > flapSpeeds[0] ){
		//ESP_LOGI(FNAME,"Ret: %d", (int)flap_neg_max.get() );
		return flap_neg_max;
	}
	else {
		//ESP_LOGI(FNAME,"Ret: %d", 1);
		return 1;
	}
}
