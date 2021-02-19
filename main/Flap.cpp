#include "sensor.h"
#include "Setup.h"
#include "Units.h"
#include "Flap.h"
#include "SetupMenuSelect.h"
#include "SetupMenuValFloat.h"


#define ZERO_INDEX 4
#define NUMBER_POS 9

AnalogInput * Flap::sensorAdc = 0;
float Flap::lever=-1;
int   Flap::leverold=-2;
int   Flap::senspos[NUMBER_POS];
int   Flap::flapSpeeds[NUMBER_POS];
Ucglib_ILI9341_18x240x320_HWSPI* Flap::ucg;
bool Flap::surroundingBox = false;
int Flap::optPosOldY;
int Flap::sensorOldY;

#define NUMPOS  (int)( flap_pos_max.get() +1 - flap_neg_max.get() )
#define MINPOS  flap_neg_max.get()
#define MAXPOS  flap_pos_max.get()

MenuEntry* wkm = 0;
MenuEntry *flapssm = 0;
SetupMenuValFloat * plus3 = 0;
SetupMenuValFloat * plus2 = 0;
SetupMenuValFloat * plus1 = 0;
SetupMenuValFloat * min1 = 0;
SetupMenuValFloat * min2 = 0;
SetupMenuValFloat * min3 = 0;

SetupMenuSelect * flabp3 = 0;
SetupMenuSelect * flabp2 = 0;
SetupMenuSelect * flabp1 = 0;
SetupMenuSelect * flab0 = 0;
SetupMenuSelect * flabm1 = 0;
SetupMenuSelect * flabm2 = 0;
SetupMenuSelect * flabm3 = 0;

SetupMenuSelect *flapLabels[NUMBER_POS];

void showWk(SetupMenuSelect * p){
	p->ucg->setPrintPos(1,120);
	p->ucg->printf("Sensor: %d    ", Flap::getSensorRaw() );
	delay(20);
}

void wk_cal_show( SetupMenuSelect * p, int wk ){
	p->ucg->setPrintPos(1,60);
	p->ucg->printf("Set Flap %+d   ", wk );
	delay(1000);
	while( !p->_rotary->readSwitch() )
		showWk(p);
}

int flap_speed_act( SetupMenuValFloat * p ){
	Flap::initSpeeds();
	return 0;
}

int flap_pos_act( SetupMenuValFloat * p ){
	Flap::initSensor();
	return 0;
}

// Action Routines
int wk_cal( SetupMenuSelect * p )
{
	ESP_LOGI(FNAME,"WK calibration ( %d ) ", p->getSelect() );
	if( !Flap::haveSensor() ){
		p->clear();
		p->ucg->setPrintPos(1,60);
		p->ucg->printf("No Sensor, Abort");
		delay(2000);
		ESP_LOGI(FNAME,"Abort calibration, no signal");
		return 0;
	}
	if( p->getSelect() ){
		p->clear();
		p->ucg->setFont(ucg_font_fub25_hr);
		if( flap_pos_max.get() > 2 ){
			wk_cal_show( p,3 );
			wk_sens_pos_plus_3.set( Flap::getSensorRaw() );
		}
		if( flap_pos_max.get() > 1 ){
			wk_cal_show( p,2 );
			wk_sens_pos_plus_2.set( Flap::getSensorRaw() );
		}
		if( flap_pos_max.get() > 0 ){
			wk_cal_show( p,1 );
			wk_sens_pos_plus_1.set( Flap::getSensorRaw() );
		}
		wk_cal_show( p,0 );
		wk_sens_pos_0.set( Flap::getSensorRaw() );

		if( flap_neg_max.get() < 0 ){
			wk_cal_show( p,-1 );
			wk_sens_pos_minus_1.set( Flap::getSensorRaw() );
		}
		if( flap_neg_max.get() < -1 ){
			wk_cal_show( p,-2 );
			wk_sens_pos_minus_2.set( Flap::getSensorRaw() );
		}
		if( flap_neg_max.get() < -2 ){
			wk_cal_show( p,-3 );
			wk_sens_pos_minus_3.set( Flap::getSensorRaw() );
		}
		p->ucg->setPrintPos(1,60);
		p->ucg->printf("Saved        ");
		Flap::initSensor();
		delay(2000);
		ESP_LOGI(FNAME,"Push Button pressed");
	}
	return 0;
}


void Flap::setupMenue( SetupMenu *parent ){
	ESP_LOGI(FNAME,"Flap::setupMenue");
	SetupMenu * wk = new SetupMenu( "Flap (WK) Indicator" );
	wkm = parent->addMenu( wk );

	SetupMenuSelect * wke = new SetupMenuSelect( "Flap Indicator", 0, false, 0, true, &flap_enable );
	wke->addEntry( "Disable");
	wke->addEntry( "Enable");
	wke->setHelp(PROGMEM"Option to enable Flap (WK) Indicator to assist optimum flap setting depending on speed and ballast");
	wkm->addMenu( wke );

	SetupMenuSelect * wkes = new SetupMenuSelect( "Flap Sensor", 0, true, 0, true, &flap_sensor );
	wkes->addEntry( "Disable");
	wkes->addEntry( "Enable IO-2");
	wkes->addEntry( "Enable IO-34");
	wkes->addEntry( "Enable IO-26");
	wkes->setHelp(PROGMEM"Option to enable Flap sensor on corresponding IO pin, for now its IO-2, later 2021 series will use IO-34");
	wkm->addMenu( wkes );


	static int select_dummy;
	SetupMenuSelect * wkcal = new SetupMenuSelect( "Sensor Calibration", &select_dummy, true, wk_cal, false  );
	wkcal->addEntry( "Cancel");
	wkcal->addEntry( "Start");
	wkcal->setHelp( PROGMEM "Option to calibrate flap Sensor (WK), to indicate current flap setting: Press button after each setting" );
	wkm->addMenu( wkcal );

	SetupMenuValFloat * nflpos = new SetupMenuValFloat("Max positive Flap", 0, "", 0, 3, 1, flap_pos_act, false, &flap_pos_max, true);
	nflpos->setHelp(PROGMEM"Maximum positive flap position to be displayed");
	wkm->addMenu( nflpos );

	SetupMenuValFloat * nflneg = new SetupMenuValFloat("Max negative Flap", 0, "", -3, 0, 1, flap_pos_act, false, &flap_neg_max, true  );
	nflneg->setHelp(PROGMEM"Maximum negative flap position to be displayed");
	wkm->addMenu( nflneg );

	SetupMenuValFloat * flgnd = new SetupMenuValFloat("Takeoff Flap", 0, "", -3, 3, 1, 0, false, &flap_takeoff  );
	flgnd->setHelp(PROGMEM"Flap position to be set on ground for takeoff, when there is no airspeed");
	wkm->addMenu( flgnd );

	SetupMenu * flapss = new SetupMenu( "Flap Speeds Setup" );
	MenuEntry *flapssm = wkm->addMenu( flapss );

	plus3 = new SetupMenuValFloat("Speed +3 to +2", 0, sunit.c_str(),  20, 150, 1, flap_speed_act, false, &flap_plus_2  );
	plus3->setHelp(PROGMEM"Speed for transition from +3 to +3 flap setting");
	flapssm->addMenu( plus3 );

	plus2 = new SetupMenuValFloat("Speed +2 to +1", 0, sunit.c_str(),  20, 150, 1, flap_speed_act, false, &flap_plus_1  );
	plus2->setHelp(PROGMEM"Speed for transition from +2 to +1 flap setting");
	flapssm->addMenu( plus2 );

	plus1 = new SetupMenuValFloat("Speed +1 to 0", 0, sunit.c_str(),  20, v_max.get(), 1, flap_speed_act, false, &flap_0  );
	plus1->setHelp(PROGMEM"Speed for transition from +1 to 0 flap setting");
	flapssm->addMenu( plus1 );

	min1 = new SetupMenuValFloat("Speed 0 to -1", 0, sunit.c_str(),   20, v_max.get(), 1, flap_speed_act, false, &flap_minus_1  );
	min1->setHelp(PROGMEM"Speed for transition from 0 to -1 flap setting");
	flapssm->addMenu( min1 );

	min2 = new SetupMenuValFloat("Speed -1 to -2", 0, sunit.c_str(),  50, v_max.get(), 1, flap_speed_act, false, &flap_minus_2  );
	min2->setHelp(PROGMEM"Speed for transition from -1 to -2 flap setting");
	flapssm->addMenu( min2 );

	min3 = new SetupMenuValFloat("Speed -2 to -3", 0, sunit.c_str(),  50, v_max.get(), 1, flap_speed_act, false, &flap_minus_3  );
	min3->setHelp(PROGMEM"Speed for transition from -2 to -3 flap setting");
	flapssm->addMenu( min3 );

	SetupMenu * flapls = new SetupMenu( "Flap Position Labels" );
	MenuEntry *flaplsm = wkm->addMenu( flapls );

	flabp3 = new SetupMenuSelect( "Flap Label +3",	0, false, 0, false, &wk_label_plus_3 );
	flaplsm->addMenu( flabp3 );
	flabp2 = new SetupMenuSelect( "Flap Label +2",	0, false, 0, false, &wk_label_plus_2 );
	flaplsm->addMenu( flabp2 );
	flabp1 = new SetupMenuSelect( "Flap Label +1",	0, false, 0, false, &wk_label_plus_1 );
	flaplsm->addMenu( flabp1 );
	flab0 = new SetupMenuSelect( "Flap Label  0",		0, false, 0, false, &wk_label_null_0 );
	flaplsm->addMenu( flab0 );
	flabm1 = new SetupMenuSelect( "Flap Label -1",	0, false, 0, false, &wk_label_minus_1 );
	flaplsm->addMenu( flabm1 );
	flabm2 = new SetupMenuSelect( "Flap Label -2",	0, false, 0, false, &wk_label_minus_2 );
	flaplsm->addMenu( flabm2 );
	flabm3 = new SetupMenuSelect( "Flap Label -3",	0, false, 0, false, &wk_label_minus_3 );
	flaplsm->addMenu( flabm3 );
	// Initialize Flap Label Entries
	for( int pos=-9; pos< 10; pos++ ){  // -9,.,-2,-1,+0,+1,+2,.,+9
		char p[5];
		sprintf( p, "%+d", pos );
		flabp1->addEntry( p );
		flabp2->addEntry( p );
		flabp3->addEntry( p );
		flab0->addEntry( p );
		flabm1->addEntry( p );
		flabm2->addEntry( p );
		flabm3->addEntry( p );
	}
	for( int pos=0; pos<=20; pos++ ){  // 0,1,2,3,.,20
		char p[5];
		if(pos<10)
			sprintf( p, " %d", pos );
		else
			sprintf( p, "%d", pos );
		flabp1->addEntry( p );
		flabp2->addEntry( p );
		flabp3->addEntry( p );
		flab0->addEntry( p );
		flabm1->addEntry( p );
		flabm2->addEntry( p );
		flabm3->addEntry( p );
	}
	flab0->addEntry( " N" );   // N,L,S
	flab0->addEntry( " L" );
	flab0->addEntry( " S" );
	flab0->addEntry( "3a" );
	flab0->addEntry( "3b" );
	flab0->addEntry( " A" );

	flabp1->addEntry( " N" );
	flabp1->addEntry( " L" );
	flabp1->addEntry( " S" );
	flabp1->addEntry( "3a" );
	flabp1->addEntry( "3b" );
	flabp1->addEntry( " A" );

	flabp2->addEntry( " N" );
	flabp2->addEntry( " L" );
	flabp2->addEntry( " S" );
	flabp2->addEntry( "3a" );
	flabp2->addEntry( "3b" );
	flabp2->addEntry( " A" );

	flabp3->addEntry( " N" );
	flabp3->addEntry( " L" );
	flabp3->addEntry( " S" );
	flabp3->addEntry( "3a" );
	flabp3->addEntry( "3b" );
	flabp3->addEntry( " A" );

	flabm1->addEntry( " N" );
	flabm1->addEntry( " L" );
	flabm1->addEntry( " S" );
	flabm1->addEntry( " A" );

	flabm2->addEntry( " N" );
	flabm2->addEntry( " L" );
	flabm2->addEntry( " S" );
	flabm2->addEntry( " A" );

	flabm3->addEntry( " N" );
	flabm3->addEntry( " L" );
	flabm3->addEntry( " S" );
	flabm3->addEntry( " A" );

	flapLabels[0] = flabm3;
	flapLabels[1] = flabm2;
	flapLabels[2] = flabm1;
	flapLabels[3] = flab0;
	flapLabels[4] = flabp1;
	flapLabels[5] = flabp2;
	flapLabels[6] = flabp3;

}


void Flap::drawSmallBar( int ypos, int xpos, float wkf ){
	ucg->setFont(ucg_font_profont22_mr );
	int	lfh = ucg->getFontAscent()+4;
	int lfw = ucg->getStrWidth( "+2" );
	int top = ypos-lfh/2;
	if( !surroundingBox ) {
		ucg->drawFrame(xpos-5, top-3, lfw+4, 2*lfh);
		int tri = ypos+lfh/2-3;
		ucg->setColor(COLOR_GREEN);
		ucg->drawTriangle( xpos-10, tri-5,  xpos-10,tri+5, xpos-5, tri );
		ucg->setColor(COLOR_WHITE);
		surroundingBox = true;
	}
	ucg->setClipRange( xpos-2, top-2, lfw, 2*lfh-2 );
	for( int wk=int(wkf-1); wk<=int(wkf+1) && wk<=2; wk++ ){
		char position[6];
		if(wk<-3)
			continue;
		sprintf( position,"%s", flapLabels[wk+3]->getEntry());
		int y=top+(lfh+4)*(5-(wk+2))+(int)((wkf-2)*(lfh+4));
		ucg->setPrintPos(xpos-2, y );
		ucg->setColor(COLOR_WHITE);
		ucg->printf(position);
		if( wk != -3 ) {
			ucg->drawHLine(xpos-5, y+3, lfw+4 );
		}
	}
	ucg->undoClipRange();
}

void Flap::drawLever( int xpos, int ypos, int oldypos ){
	ucg->setColor(COLOR_BLACK);
	ucg->drawBox( xpos-25, oldypos-4, 19, 8 );
	ucg->drawBox( xpos-6, oldypos-2, 4, 4 );

	ucg->setColor(COLOR_WHITE);  // left upper x,y and w,h
	ucg->drawBox( xpos-25, ypos-4, 19, 8 );
	ucg->drawBox( xpos-6, ypos-2, 4, 4 );
}

void Flap::drawBigBar( int ypos, int xpos, float wkf, float wksens ){
	ucg->setFont(ucg_font_profont22_mr );
	ucg->setFontPosCenter();
	int lfh = ucg->getFontAscent()+10;  // a bit place around number
	int lfw = ucg->getStrWidth( "+2" );
	int size = NUMPOS*lfh;
	// draw Frame around and a triangle
	if( !surroundingBox ) {
		for( int wk=MINPOS; wk<=MAXPOS; wk++ ){
			char position[6];
			sprintf( position,"%s", flapLabels[wk+3]->getEntry());
			int y= ypos + lfh*wk;  // negative WK eq. lower position
			ucg->setPrintPos(xpos+2, y+1);
			ucg->setColor(COLOR_WHITE);
			// print digit
			ucg->printf(position);
			// Frame around digit
			ucg->drawFrame(xpos-2, y-(lfh/2), lfw+6, lfh );
		}
		surroundingBox = true;
	}
	ESP_LOGI(FNAME,"np: %d size: %d",  NUMPOS, size );
	int yclip = ypos+MINPOS*lfh-(lfh/2);
	ucg->setClipRange( xpos-15, yclip, 15, size );
	int y = ypos + (int)((wkf)*(lfh) + 0.5 );
	int ys = ypos + (int)(( wksens )*(lfh) + 0.5 );
	ESP_LOGI(FNAME,"wkf: %f", wkf);
	if( optPosOldY != y || ( (sensorOldY != ys) )) {  // redraw on change or when wklever is near
		ucg->setColor(COLOR_BLACK);
		ucg->drawTriangle( xpos-15,optPosOldY-5,  xpos-15,optPosOldY+5,  xpos-2,optPosOldY );
		ucg->setColor(COLOR_GREEN);
		ucg->drawTriangle( xpos-15,y-5,       xpos-15,y+5,       xpos-2,y );
		optPosOldY = y;
		if( flap_sensor.get() ) {
			ESP_LOGI(FNAME,"wk lever redraw, old=%d", sensorOldY );
			drawLever( xpos, ys, sensorOldY );
			sensorOldY = ys;
		}
	}
	ucg->setFontPosBottom();
	ucg->undoClipRange();
}

#define DISCRAD 3
#define BOXLEN  12
#define FLAPLEN 14

void Flap::drawWingSymbol( int ypos, int xpos, int wk, int wkalt ){
	ucg->setColor( COLOR_WHITE );
	ucg->drawDisc( xpos, ypos, DISCRAD, UCG_DRAW_ALL );
	ucg->drawBox( xpos, ypos-DISCRAD, BOXLEN, DISCRAD*2+1  );
	ucg->setColor( COLOR_BLACK );
	ucg->drawTriangle( xpos+DISCRAD+BOXLEN-2, ypos-DISCRAD,
			xpos+DISCRAD+BOXLEN-2, ypos+DISCRAD+1,
			xpos+DISCRAD+BOXLEN-2+FLAPLEN, ypos+wkalt*4 );
	ucg->setColor( COLOR_GREEN );
	ucg->drawTriangle( xpos+DISCRAD+BOXLEN-2, ypos-DISCRAD,
			xpos+DISCRAD+BOXLEN-2, ypos+DISCRAD+1,
			xpos+DISCRAD+BOXLEN-2+FLAPLEN, ypos+wk*4 );
}

void  Flap::initSpeeds(){
	flapSpeeds[0] = 280;
	flapSpeeds[1] = flap_minus_3.get();
	flapSpeeds[2] = flap_minus_2.get();
	flapSpeeds[3] = flap_minus_1.get();
	flapSpeeds[4] = flap_0.get();
	flapSpeeds[5] = flap_plus_1.get();
	flapSpeeds[6] = flap_plus_2.get();
	flapSpeeds[7] = 50;
}

void  Flap::init( Ucglib_ILI9341_18x240x320_HWSPI *theUcg ){
	ucg = theUcg;
	ESP_LOGI( FNAME, "Flap sensor init");

	if( flap_sensor.get() == FLAP_SENSOR_GPIO_2 ) {
		sensorAdc = new AnalogInput( -1, ADC_ATTEN_DB_0, ADC_CHANNEL_2, ADC_UNIT_2, true );
	}else if( flap_sensor.get() == FLAP_SENSOR_GPIO_34 ) {
		sensorAdc = new AnalogInput( -1, ADC_ATTEN_DB_0, ADC_CHANNEL_6, ADC_UNIT_1, true );
	}
	else if( flap_sensor.get() == FLAP_SENSOR_GPIO_26 ) {
		sensorAdc = new AnalogInput( -1, ADC_ATTEN_DB_0, ADC_CHANNEL_9, ADC_UNIT_2, true );
	}
	if( sensorAdc != 0 ) {
		ESP_LOGI( FNAME, "Flap sensor configured");
		sensorAdc->begin();
		delay(10);
		uint32_t read =  sensorAdc->getRaw();
		if( read == 0  || read >= 4096 ) // try GPIO pin 34, series 2021-2
			ESP_LOGI( FNAME, "Flap sensor not found or edge value, reading: %d", read);
		else
			ESP_LOGI( FNAME, "Flap sensor looks good, reading: %d", read );
	}
	initSpeeds();
}

float Flap::getLeverPosition( int wks ){
	// ESP_LOGI(FNAME,"getSensorWkPos %d", wks);
	int wk=0;
	int max = ZERO_INDEX +1 + flap_pos_max.get();
	int min = ZERO_INDEX -1 + flap_neg_max.get();
	// ESP_LOGI(FNAME,"getSensorWkPos %d min:%d max:%d", wks, min, max );
	for( int i=min; i<=max; i++ ){
		if( ((senspos[i] < wks) && (wks < senspos[i+1]))  ||
				((senspos[i] > wks) && (wks > senspos[i+1]))	) {
			wk = i;
			break;
		}
	}
	float delta=senspos[wk]-senspos[wk+1];
	float moved=senspos[wk]-wks;
	float relative=moved/delta;
	float wkf =(wk-ZERO_INDEX) + relative;
	// ESP_LOGI(FNAME,"getLeverPos(%d): lever: %1.2f wk: %d relative: %1.1f  N:%d X%d ", wks, wkf, wk, relative, min, max  );
	return wkf;
}

void  Flap::progress(){
	if( sensorAdc ) {
		int wkraw = sensorAdc->getRaw();
		if( wkraw < 4095 && wkraw > 0 ){
			lever = getLeverPosition( wkraw );
			// ESP_LOGI(FNAME,"wk sensor=%1.2f", lever );
		}
		else
			lever = -10;  // off screen to blank
		if( blue_enable.get() == WL_WLAN ) {
			if( leverold != (int)(lever*10) ){
				OV.sendWkChange( lever );   // update secondary vario
				leverold = (int)(lever*10);
			}
		}
	}
}

void  Flap::initSensor(){
	ESP_LOGI(FNAME,"initSensor");
	senspos[ZERO_INDEX-4] = 4095;

	if( (int)flap_neg_max.get() < -2 )
		senspos[ZERO_INDEX-3] = wk_sens_pos_minus_3.get();
	else{
		senspos[ZERO_INDEX-3] = wk_sens_pos_minus_2.get() - ( wk_sens_pos_minus_1.get() - wk_sens_pos_minus_2.get()); // extrapolated
		if( senspos[ZERO_INDEX-3] > 4095 )
			senspos[ZERO_INDEX-3] = 4095;
	}

	if(  (int)flap_neg_max.get() < -1)
		senspos[ZERO_INDEX-2] = wk_sens_pos_minus_2.get();
	else{
		senspos[ZERO_INDEX-2] = wk_sens_pos_minus_1.get() - ( wk_sens_pos_0.get() - wk_sens_pos_minus_1.get()); // extrapolated
		if( senspos[ZERO_INDEX-2] > 4095 )
			senspos[ZERO_INDEX-2] = 4095;
	}

	if( (int)flap_neg_max.get() < 0 )
		senspos[ZERO_INDEX-1] = wk_sens_pos_minus_1.get();
	else
		senspos[ZERO_INDEX-1] = wk_sens_pos_0.get() - ( wk_sens_pos_plus_1.get() - wk_sens_pos_0.get()); // extrapolated

	senspos[ZERO_INDEX] = wk_sens_pos_0.get();

	if( (int)flap_pos_max.get() > 0 )
		senspos[ZERO_INDEX+1] = wk_sens_pos_plus_1.get();
	else
		senspos[ZERO_INDEX+1] = wk_sens_pos_0.get() - ( wk_sens_pos_minus_1.get() - wk_sens_pos_0.get()); // extrapolated pos pole

	if( (int)flap_pos_max.get() > 1 )
		senspos[ZERO_INDEX+2] = wk_sens_pos_plus_2.get();
	else{
		senspos[ZERO_INDEX+2] = wk_sens_pos_plus_1.get() - ( wk_sens_pos_0.get() - wk_sens_pos_plus_1.get()); // extrapolated pos pole
		if( senspos[ZERO_INDEX+2] < 0 )
			senspos[ZERO_INDEX+2] = 0;
	}
	if( (int)flap_pos_max.get() > 2 )
		senspos[ZERO_INDEX+3] = wk_sens_pos_plus_3.get();
	else{
		senspos[ZERO_INDEX+3] = wk_sens_pos_plus_2.get() - ( wk_sens_pos_plus_1.get() - wk_sens_pos_plus_2.get()); // extrapolated pos pole
		if( senspos[ZERO_INDEX+3] < 0 )
			senspos[ZERO_INDEX+3] = 0;
	}
	senspos[ZERO_INDEX+4] = 0;
	for( int i=0; i<=8; i++ ){
		ESP_LOGI(FNAME,"lever: %d  senspos[i]: %d", i-4, senspos[i]  );
	}
}

float Flap::getOptimum( float wks, int wki ){
	// ESP_LOGI(FNAME,"wks:%f min:%f max:%f", wks, minv, maxv );
	float minv = flapSpeeds[wki+4];
	float maxv = flapSpeeds[wki+3];
	if( wks <= maxv && wks >= minv )
		return ((wks-minv)/(maxv-minv));
	else if( wks > maxv )
		return 1;
	else if( wks < minv )
		return 0.5;
	return 0.5;
}

int Flap::getOptimumInt( float speed )
{
	// ESP_LOGI(FNAME,"getOptimumInt( %3.1f )", speed );
	for( int wk=flap_neg_max.get(); wk<=flap_pos_max.get(); wk++ ){
		// ESP_LOGI(FNAME,"Check Flap: %d %d %d", wk, flapSpeeds[wk+3],  flapSpeeds[wk+4] );
		if( speed <= flapSpeeds[wk+3] && speed >=  flapSpeeds[wk+4] ) {
			// ESP_LOGI(FNAME,"Ret: %d", wk);
			return wk;
		}
	}
	if( speed < flapSpeeds[7] ) {
		// ESP_LOGI(FNAME,"Ret: %d", 1);
		return flap_takeoff.get();
	}
	else if( speed > flapSpeeds[0] ){
		// ESP_LOGI(FNAME,"Ret: %d", -2);
		return flap_neg_max.get();
	}
	else {
		// ESP_LOGI(FNAME,"Ret: %d", 1);
		return 1;
	}
}

