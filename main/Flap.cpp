#include "sensor.h"
#include "Setup.h"
#include "Units.h"
#include "Flap.h"
#include "SetupMenuSelect.h"
#include "SetupMenuValFloat.h"


Flap* Flap::_instance = nullptr;

#define NUMPOS  (int)( flap_pos_max.get() +1 - flap_neg_max.get() )
#define MINPOS  flap_neg_max.get()
#define MAXPOS  flap_pos_max.get()


// predefined flap labels         // -9,.,-2,-1,+0,+1,+2,.,+9
static char flap_labels[][4] = { "-9", "-8", "-7", "-6", ".-5", "-4", "-3", "-2", "-1", "+0",
                                    "+1", "+2", "+3", "+4", "+5", "+6", "+7", "+8", "+9",
                                    // 0,1,2,3,.,20
                                    " 0", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10",
                                    "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
                                    // N,L,S
                                    " N", " L", " S", "3a", "3b", " A", "" };

static void showWk(SetupMenuSelect *p){
	p->ucg->setPrintPos(1,140);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->printf("Sensor: %d    ", FLAP->getSensorRaw(256) );
	xSemaphoreGive(spiMutex);
	delay(10);
}

// Action Routines
static int select_flap_sens_pin(SetupMenuSelect *p){
    p->clear();
    if( FLAP ) {
        FLAP->configureADC();
        if ( FLAP->haveSensor() ) {
            p->ucg->setPrintPos(1,30);
            p->ucg->setFont(ucg_font_ncenR14_hr, true );
            xSemaphoreTake(spiMutex,portMAX_DELAY );
            p->ucg->printf("Check Sensor Reading,");
            p->ucg->setPrintPos(1,60);
            p->ucg->printf("Press Button to exit");
            xSemaphoreGive(spiMutex);
            while( !p->_rotary->readSwitch() ){
                p->ucg->setPrintPos(1,90);
                xSemaphoreTake(spiMutex,portMAX_DELAY );
                p->ucg->printf("Sensor: %d       ", FLAP->getSensorRaw(256) );
                xSemaphoreGive(spiMutex);
                delay(20);
            }
        }
    }
    p->clear();
    Flap::setupSensorMenueEntries(p->_parent);

	return 0;
}

static void wk_cal_show( SetupMenuSelect *p, int wk ){
	p->ucg->setPrintPos(1,60);
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	p->ucg->printf("Set Flap %+d   ", wk );
	xSemaphoreGive(spiMutex);
	delay(500);
	while( !p->_rotary->readSwitch() )
		showWk(p);
}

static int flap_speed_act( SetupMenuValFloat *p ){
	FLAP->initSpeeds();
	return 0;
}

static int flap_lab_act( SetupMenuSelect *p ){
	FLAP->initLabels();
	return 0;
}

static int flap_pos_act( SetupMenuValFloat *p ){
	FLAP->initSensPos();
	return 0;
}

static int flap_cal_act( SetupMenuSelect *p )
{
	ESP_LOGI(FNAME,"WK calibration ( %d ) ", p->getSelect() );
	if( !FLAP->haveSensor() ){
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
			wk_sens_pos_plus_3.set( FLAP->getSensorRaw(256) );
		}
		if( flap_pos_max.get() > 1 ){
			wk_cal_show( p,2 );
			wk_sens_pos_plus_2.set( FLAP->getSensorRaw(256) );
		}
		if( flap_pos_max.get() > 0 ){
			wk_cal_show( p,1 );
			wk_sens_pos_plus_1.set( FLAP->getSensorRaw(256) );
		}
		wk_cal_show( p,0 );
		wk_sens_pos_0.set( FLAP->getSensorRaw(256) );

		if( flap_neg_max.get() < 0 ){
			wk_cal_show( p,-1 );
			wk_sens_pos_minus_1.set( FLAP->getSensorRaw(256) );
		}
		if( flap_neg_max.get() < -1 ){
			wk_cal_show( p,-2 );
			wk_sens_pos_minus_2.set( FLAP->getSensorRaw(256) );
		}
		if( flap_neg_max.get() < -2 ){
			wk_cal_show( p,-3 );
			wk_sens_pos_minus_3.set( FLAP->getSensorRaw(256) );
		}

		p->ucg->setPrintPos(1,60);
		p->ucg->printf("Saved");
		FLAP->initSensPos();
		ESP_LOGI(FNAME,"Push Button pressed");
		delay(500);
		p->clear();
	}
	return 0;
}

static int flap_enable_act( SetupMenuSelect *p ){
	Flap::setupIndicatorMenueEntries(p->_parent);
	return 0;
}

void Flap::setupSensorMenueEntries(MenuEntry *wkm)
{
    static SetupMenuSelect *wkes = 0;
    static SetupMenuSelect *wkcal = 0;

    if ( !wkes && flap_enable.get() ) {
        wkes = new SetupMenuSelect( "Flap Sensor", false, select_flap_sens_pin, true, &flap_sensor );
        wkes->addEntry( "Disable");
        wkes->addEntry( "Enable IO-2");
        wkes->addEntry( "Enable IO-34");
        wkes->addEntry( "Enable IO-26");
        wkes->setHelp(PROGMEM"Option to enable Flap sensor on corresponding IO pin, hardware may differ: check where you get a valid reading");
        wkm->addEntry( wkes );

        if ( !SetupCommon::isClient() ) {
            wkcal = new SetupMenuSelect( "Sensor Calibration", true, flap_cal_act, false  );
            wkcal->addEntry( "Cancel");
            wkcal->addEntry( "Start");
            wkcal->setHelp( PROGMEM "Option to calibrate flap Sensor (WK), to indicate current flap setting: Press button after each setting" );
            wkm->addEntry( wkcal, wkes );
        }
    }
    // else if ( wkes && !flap_enable.get() ) { // todo nice to have
    //     wkm->delEntry( wkes );
    //     wkes = 0;
    // }
    // if ( wkcal && (!wkes || SetupCommon::isClient()) ) {
    //     wkm->delEntry( wkcal );
    //     wkcal = 0;
    // }
}

void Flap::setupIndicatorMenueEntries(MenuEntry *wkm)
{
    static SetupMenuValFloat *nflpos = 0;
    static SetupMenuValFloat *nflneg = 0;
    static SetupMenuValFloat *flgnd = 0;
    static SetupMenu *flapss = 0;
    static SetupMenu *flapls = 0;

    ESP_LOGI(FNAME,"Flap Indicator Menue");
    if ( ! nflpos && flap_enable.get() && !SetupCommon::isClient() ) {

        nflpos = new SetupMenuValFloat("Max positive Flap", nullptr, "", 0., 3., 1., flap_pos_act, false, &flap_pos_max);
        nflpos->setHelp(PROGMEM"Maximum positive flap position to be displayed");
        wkm->addEntry( nflpos, wkm->getFirst() );

        nflneg = new SetupMenuValFloat("Max negative Flap", nullptr, "", -3., 0., 1., flap_pos_act, false, &flap_neg_max);
        nflneg->setHelp(PROGMEM"Maximum negative flap position to be displayed");
        wkm->addEntry( nflneg, nflpos );

        flgnd = new SetupMenuValFloat("Takeoff Flap", 0, "", -3, 3, 1, 0, false, &flap_takeoff  );
        flgnd->setHelp(PROGMEM"Flap position to be set on ground for takeoff, when there is no airspeed");
        wkm->addEntry( flgnd, nflneg );

        flapss = new SetupMenu( "Flap Speeds Setup" );
        wkm->addEntry( flapss, flgnd );

        SetupMenuValFloat *plus3 = new SetupMenuValFloat("Speed +3 to +2", 0, sunit.c_str(),  20, 150, 1, flap_speed_act, false, &flap_plus_2  );
        plus3->setHelp(PROGMEM"Speed for transition from +3 to +3 flap setting");
        flapss->addEntry( plus3 );

        SetupMenuValFloat *plus2 = new SetupMenuValFloat("Speed +2 to +1", 0, sunit.c_str(),  20, 150, 1, flap_speed_act, false, &flap_plus_1  );
        plus2->setHelp(PROGMEM"Speed for transition from +2 to +1 flap setting");
        flapss->addEntry( plus2 );

        SetupMenuValFloat *plus1 = new SetupMenuValFloat("Speed +1 to 0", 0, sunit.c_str(),  20, Units::Airspeed2Kmh(v_max.get()), 1, flap_speed_act, false, &flap_0  );
        plus1->setHelp(PROGMEM"Speed for transition from +1 to 0 flap setting");
        flapss->addEntry( plus1 );

        SetupMenuValFloat *min1 = new SetupMenuValFloat("Speed 0 to -1", 0, sunit.c_str(),   20, Units::Airspeed2Kmh(v_max.get()), 1, flap_speed_act, false, &flap_minus_1  );
        min1->setHelp(PROGMEM"Speed for transition from 0 to -1 flap setting");
        flapss->addEntry( min1 );

        SetupMenuValFloat *min2 = new SetupMenuValFloat("Speed -1 to -2", 0, sunit.c_str(),  50, Units::Airspeed2Kmh(v_max.get()), 1, flap_speed_act, false, &flap_minus_2  );
        min2->setHelp(PROGMEM"Speed for transition from -1 to -2 flap setting");
        flapss->addEntry( min2 );

        SetupMenuValFloat *min3 = new SetupMenuValFloat("Speed -2 to -3", 0, sunit.c_str(),  50, Units::Airspeed2Kmh(v_max.get()), 1, flap_speed_act, false, &flap_minus_3  );
        min3->setHelp(PROGMEM"Speed for transition from -2 to -3 flap setting");
        flapss->addEntry( min3 );

        flapls = new SetupMenu( "Flap Position Labels" );
        wkm->addEntry( flapls, flapss );

        SetupMenuSelect *flab = new SetupMenuSelect( "Flap Label +3",	false, flap_lab_act, false, &wk_label_plus_3 );
        flapls->addEntry( flab );
        flab->addEntry( *flap_labels ); // Initialize Flap Label Entries
        flab = new SetupMenuSelect( "Flap Label +2",	false, flap_lab_act, false, &wk_label_plus_2 );
        flapls->addEntry( flab );
        flab->addEntry( *flap_labels );
        flab = new SetupMenuSelect( "Flap Label +1",	false, flap_lab_act, false, &wk_label_plus_1 );
        flapls->addEntry( flab );
        flab->addEntry( *flap_labels );
        flab = new SetupMenuSelect( "Flap Label  0",	false, flap_lab_act, false, &wk_label_null_0 );
        flapls->addEntry( flab );
        flab->addEntry( *flap_labels );
        flab = new SetupMenuSelect( "Flap Label -1",	false, flap_lab_act, false, &wk_label_minus_1 );
        flapls->addEntry( flab );
        flab->addEntry( *flap_labels );
        flab = new SetupMenuSelect( "Flap Label -2",	false, flap_lab_act, false, &wk_label_minus_2 );
        flapls->addEntry( flab );
        flab->addEntry( *flap_labels );
        flab = new SetupMenuSelect( "Flap Label -3",	false, flap_lab_act, false, &wk_label_minus_3 );
        flapls->addEntry( flab );
        flab->addEntry( *flap_labels );
    }
    // else if ( nflpos && (!flap_sensor.get() || SetupCommon::isClient()) ) { // todo
    //     ESP_LOGI(FNAME,"Flap Indicator Menue>>del");
    //     wkm->delEntry( flapls );
    //     wkm->delEntry( flapss );
    //     wkm->delEntry( flgnd );
    //     wkm->delEntry( nflneg );
    //     wkm->delEntry( nflpos );
    //     nflpos = 0;
    // }

    setupSensorMenueEntries(wkm);
}

void Flap::setupMenue( SetupMenu *parent ){
    static MenuEntry* wkm = 0;

	ESP_LOGI(FNAME,"Flap::setupMenue");
	if( ! parent || wkm ){
        return;
    }

	wkm = new SetupMenu( "Flap (WK) Indicator" );
    parent->addEntry( wkm );

	SetupMenuSelect * wke = new SetupMenuSelect( "Flap Indicator", false, flap_enable_act, true, &flap_enable );
	wke->addEntry( "Disable");
	wke->addEntry( "Enable");
	wke->setHelp(PROGMEM"Option to enable Flap (WK) Indicator to assist optimum flap setting depending on speed and ballast");
	wkm->addEntry( wke );

    setupIndicatorMenueEntries(wkm);
}


void Flap::drawSmallBar( float wkf ){
	ucg->setFont(ucg_font_profont22_mr );
	int	lfh = ucg->getFontAscent()+4;
	int lfw = ucg->getStrWidth( "+2" );
	int top = barpos_y-lfh/2;
	if( dirty ) {
		ucg->setColor(COLOR_HEADER);
		ucg->drawFrame(barpos_x-5, top-3, lfw+4, 2*lfh);
		int tri = barpos_y+lfh/2-3;
		ucg->setColor(COLOR_GREEN);
		ucg->drawTriangle( barpos_x-10, tri-5,  barpos_x-10,tri+5, barpos_x-5, tri );
		ucg->setColor(COLOR_WHITE);
        drawWingSymbol(wkf, 0);
		dirty = false;
	}
	ucg->setClipRange( barpos_x-2, top-2, lfw, 2*lfh-2 );
	for( int wk=int(wkf-1); wk<=int(wkf+1) && wk<=2; wk++ ){
		char position[6];
		if(wk<-3)
			continue;

		sprintf( position,"%s", flapLabels[wk+3]);
		int y=top+(lfh+4)*(5-(wk+2))+(int)((wkf-2)*(lfh+4));
		ucg->setPrintPos(barpos_x-2, y );
		ucg->setColor(COLOR_WHITE);
		ucg->printf(position);
		if( wk != -3 ) {
			ucg->drawHLine(barpos_x-5, y+3, lfw+4 );
		}
	}
	ucg->undoClipRange();
}

void Flap::drawLever( int16_t xpos, int16_t ypos, int16_t oldypos, bool warn ){
	ucg->setColor(COLOR_BLACK);
	ucg->drawBox( xpos-25, oldypos-4, 19, 8 );
	ucg->drawBox( xpos-6, oldypos-2, 4, 4 );
    if( warn ){
    	// Blink effekt
    	if( warn_color ){
    		ucg->setColor(COLOR_WHITE);
    		warn_color = false;
    	}
    	else{
    		ucg->setColor(COLOR_RED);
    		warn_color = true;
    	}
    }
    else{
    	ucg->setColor(COLOR_WHITE);
    }
	ucg->drawBox( xpos-25, ypos-4, 19, 8 );
	ucg->drawBox( xpos-6, ypos-2, 4, 4 );
}

void Flap::drawBigBar( float wkf, float wksens ){
	ucg->setFont(ucg_font_profont22_mr );
	ucg->setFontPosCenter();
	int16_t lfh = 24; // ucg->getFontAscent()+10;  // a bit place around number
	int16_t lfw = ucg->getStrWidth( "+2" );
	int16_t size = NUMPOS*lfh;
	// draw Frame around and a triangle
	if( dirty ) {
		for( int wk=MINPOS; wk<=MAXPOS; wk++ ){
			char position[6];
			// ESP_LOG_BUFFER_HEXDUMP(FNAME, flapLabels[wk+3], 4, ESP_LOG_INFO);
			sprintf( position,"%s", flapLabels[wk+3]);
			int y= barpos_y + lfh*wk;  // negative WK eq. lower position
			// ESP_LOGI(FNAME,"Y: %d lfh:%d wk:%d",y, lfh, wk );
			ucg->setPrintPos(barpos_x+2, y+1);
			ucg->setColor(COLOR_WHITE);
			// print digit
			ucg->print(position);
			// Frame around digit
			ucg->setColor(COLOR_HEADER);
			ucg->drawFrame(barpos_x-2, y-(lfh/2), lfw+6, lfh );
		}
		if ( MINPOS > -3 ) { drawWingSymbol(wkf, wksens); }
		dirty = false;
	}
	// ESP_LOGI(FNAME,"np: %d size: %d",  NUMPOS, size );
	int16_t yclip = barpos_y+MINPOS*lfh-(lfh/2);
	ucg->setClipRange( barpos_x-15, yclip, 15, size );
	int16_t y = barpos_y + (int)((wkf)*(lfh) + 0.5 );
	int16_t ys = barpos_y + (int)(( wksens )*(lfh) + 0.5 );
	// ESP_LOGI(FNAME,"wkf: %f", wkf);

	tickopt++;
	if( optPosOldY != y || !(tickopt%10)) {  // redraw on change or when wklever is near
		ucg->setColor(COLOR_BLACK);
		ucg->drawTriangle( barpos_x-15,optPosOldY-5,  barpos_x-15,optPosOldY+5,  barpos_x-2,optPosOldY );
		ucg->setColor(COLOR_GREEN);
		ucg->drawTriangle( barpos_x-15,y-5,       barpos_x-15,y+5,       barpos_x-2,y );
		optPosOldY = y;
	}
	bool warn=false;
	if( abs( wkf - wksens) > 1 )
		warn = true;
	if( sensorOldY != ys || warn ) {  // redraw on change or when wklever is near
		if( flap_sensor.get() ) {
			// ESP_LOGI(FNAME,"wk lever redraw, old=%d", sensorOldY );
			drawLever( barpos_x, ys, sensorOldY, warn );
			sensorOldY = ys;
		}
	}
	ucg->setFontPosBottom();
	ucg->undoClipRange();
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
		ucg->setColor( COLOR_RED );
	}else{
		ucg->setColor( COLOR_HEADER );
	}
	ucg->drawDisc( symbolpos_x, symbolpos_y, DISCRAD, UCG_DRAW_ALL );
	ucg->drawBox( symbolpos_x, symbolpos_y-DISCRAD, BOXLEN, DISCRAD*2+1  );
	ucg->setColor( COLOR_BLACK );
	ucg->drawTriangle( symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y-DISCRAD,
			symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y+DISCRAD+1,
			symbolpos_x+DISCRAD+BOXLEN-2+FLAPLEN, symbolpos_y+wk_old*4 );
	if(warn){
		ucg->setColor( COLOR_RED );
	}else{
		ucg->setColor( COLOR_GREEN );
	}
	ucg->drawTriangle( symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y-DISCRAD,
			symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y+DISCRAD+1,
			symbolpos_x+DISCRAD+BOXLEN-2+FLAPLEN, symbolpos_y+wk*4 );

    wk_old = wk;
    warn_old = warn;
}

void  Flap::initSpeeds(){
    // Fixme needs a refresh after synch fromm aster -> blackboard action
	flapSpeeds[0] = 280;
	flapSpeeds[1] = flap_minus_3.get();
	flapSpeeds[2] = flap_minus_2.get();
	flapSpeeds[3] = flap_minus_1.get();
	flapSpeeds[4] = flap_0.get();
	flapSpeeds[5] = flap_plus_1.get();
	flapSpeeds[6] = flap_plus_2.get();
	flapSpeeds[7] = 50;
}

void  Flap::initLabels(){
    // Fixme needs a refresh after synch from master -> blackboard action
    flapLabels[0] = flap_labels[wk_label_minus_3.get()];
    flapLabels[1] = flap_labels[wk_label_minus_2.get()];
    flapLabels[2] = flap_labels[wk_label_minus_1.get()];
    flapLabels[3] = flap_labels[wk_label_null_0.get()];
    flapLabels[4] = flap_labels[wk_label_plus_1.get()];
    flapLabels[5] = flap_labels[wk_label_plus_2.get()];
    flapLabels[6] = flap_labels[wk_label_plus_3.get()];
}

void Flap::configureADC(){
	ESP_LOGI( FNAME, "Flap::configureADC");
	if( sensorAdc ) {
		delete sensorAdc;
        sensorAdc = nullptr;
    }
    flap_pos.init(); // remove display until set properly
    if ( SetupCommon::isClient() ) {
        return; // no analog pin config for the client
    }
	if( flap_sensor.get() == FLAP_SENSOR_GPIO_2 ) {
		sensorAdc = new AnalogInput( -1, ADC_ATTEN_DB_0, ADC_CHANNEL_2, ADC_UNIT_2, true );
	}
    else if( flap_sensor.get() == FLAP_SENSOR_GPIO_34 ) {
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

}

Flap::Flap(AdaptUGC *theUcg) :
	ucg(theUcg)
{}
Flap::~Flap()
{
	if( sensorAdc ) {
		delete sensorAdc;
        sensorAdc = nullptr;
    }
    _instance = nullptr;
}
Flap*  Flap::init(AdaptUGC *ucg)
{
    if ( ! _instance ) {
        _instance = new Flap(ucg);
    }
	_instance->configureADC();
	_instance->initSpeeds();
    _instance->initLabels();
    _instance->initSensPos();

    return _instance;
}

float Flap::sensorToLeverPosition( int wks ){
	// ESP_LOGI(FNAME,"getSensorWkPos %d", wks);
	int wk=0;
	int max = ZERO_INDEX +1 + flap_pos_max.get();
	int min = ZERO_INDEX -1 + flap_neg_max.get();
	// ESP_LOGI(FNAME,"getSensorWkPos %d min:%d max:%d", wks, min, max );
	for( int i=min; i<=max; i++ ){
		if( ((senspos[i] < wks) && (wks < senspos[i+1]))  ||
				((senspos[i] > wks) && (wks > senspos[i+1])) ) {
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
			lever = sensorToLeverPosition( rawFiltered );
			// ESP_LOGI(FNAME,"wk sensor=%1.2f  raw=%d", lever, rawFiltered );
			if( lever < flap_neg_max.get()-0.5 )
				lever = flap_neg_max.get()-0.5;
			else if( lever > flap_pos_max.get()+0.5 )
				lever = flap_pos_max.get()+0.5;

			if( (int)(flap_pos.get()*10) != (int)(lever*10) ){
				flap_pos.set( lever );   // update secondary vario
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

	if( (int)flap_neg_max.get() < -2 )
		senspos[ZERO_INDEX-3] = wk_sens_pos_minus_3.get();
	else{
        // extrapolated
		senspos[ZERO_INDEX-3] = wk_sens_pos_minus_2.get() - ( wk_sens_pos_minus_1.get() - wk_sens_pos_minus_2.get());
		if( senspos[ZERO_INDEX-3] > 4095 )
			senspos[ZERO_INDEX-3] = 4095;
		if( senspos[ZERO_INDEX-3] < 0 )
			senspos[ZERO_INDEX-3] = 0;
	}

	if(  (int)flap_neg_max.get() < -1)
		senspos[ZERO_INDEX-2] = wk_sens_pos_minus_2.get();
	else{
        // extrapolated
		senspos[ZERO_INDEX-2] = wk_sens_pos_minus_1.get() - ( wk_sens_pos_0.get() - wk_sens_pos_minus_1.get());
		if( senspos[ZERO_INDEX-2] > 4095 )
			senspos[ZERO_INDEX-2] = 4095;
		if( senspos[ZERO_INDEX-3] < 0 )
			senspos[ZERO_INDEX-3] = 0;
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
	if( (wk_sens_pos_0.get() - wk_sens_pos_minus_1.get()) < 0 )
		senspos[ZERO_INDEX+4] = 0;
	else
		senspos[ZERO_INDEX+4] = 4095;
	for( int i=0; i<=8; i++ ){
		ESP_LOGI(FNAME,"lever: %d  senspos[i]: %d", i-4, senspos[i]  );
	}
}


float Flap::getOptimum( float wks, int& wki )
{
	// Correct for current g load
	g_force += (accelG[0] - g_force) * 0.02;  // lowpass filtering, to smooth display in rough air
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
