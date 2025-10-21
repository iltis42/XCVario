
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
#include "logdefnone.h"

#include <array>


constexpr float GENERAL_V_MIN = 50;
constexpr float GENERAL_V_MAX = 280;

Flap* Flap::_instance = nullptr;
Flap* FLAP = nullptr;
FlapLevel Flap::dummy = {.0, "x", 0};

// predefined flap labels         // -9,..,-2,-1,+0,+1,+2,..,+9
const std::string_view flap_labels[55] = { "-9", "-8", "-7", "-6", "-5", "-4", "-3", "-2", "-1", "+0",  // 9
	"+1", "+2", "+3", "+4", "+5", "+6", "+7", "+8", "+9",     // 18
	// 0,1,2,3,..,20
	" 0", " 1", " 2", " 3", " 4", " 5", " 6", " 7", " 8", " 9", "10",  // 29
	"11", "12", "13", "14", "15", "16", "17", "18", "19", "20",   // 39
	// N,L,S,3a,3b,A,21,..,27
	" N", " L", " S", "3a", "3b", " A", "21", "22", "23", "24", "25", "26", "27", "T", "" };  // L=41  S=42

// all nvs setup data enumerable
struct FLConf {
	SetupNG<float> *speed;
	SetupNG<int>   *labelidx;
	SetupNG<int>   *sensval;
	constexpr FLConf( SetupNG<float> *s, SetupNG<int> *l, SetupNG<int> *sv ) :
		speed(s), labelidx(l), sensval(sv) {}
	float getSpeed() const {
		return (speed) ? speed->get() : 0;
	}
	const char* getLabel() const {
		int idx = labelidx->get();
		if( idx >= 0 && idx < 55 ) {
			return flap_labels[idx].data();
		}
		return "";
	}
};

// volatile dummy to avoid null pointer checks
SetupNG<float> flap_plus_3(  "FLAP_PLUS_3", GENERAL_V_MIN, false, SYNC_NONE, VOLATILE);
// storage of all flap configuration entries
static const std::array<FLConf, Flap::MAX_NR_POS> FL_STORE = {{
	{&flap_minus_3, &wk_label_minus_3, &wk_sens_pos_minus_3},
	{&flap_minus_2, &wk_label_minus_2, &wk_sens_pos_minus_2},
	{&flap_minus_1, &wk_label_minus_1, &wk_sens_pos_minus_1},
	{&flap_0,       &wk_label_null_0,  &wk_sens_pos_0},
	{&flap_plus_1,  &wk_label_plus_1,  &wk_sens_pos_plus_1},
	{&flap_plus_2,  &wk_label_plus_2,  &wk_sens_pos_plus_2},
	{&flap_plus_3,  &wk_label_plus_3,  &wk_sens_pos_plus_3}
	}};


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
				MYUCG->printf("Sensor: %d       ", FLAP->getSensorRaw() );
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

unsigned int Flap::getSensorRaw()
{
    return haveSensor() ? sensorAdc->getRaw() : 0;
}

int wk_cal_show(SetupMenuSelect *p, int wk, Average<7> &filter)
{
    MYUCG->setPrintPos(1, 60);
    MYUCG->printf("Set Flap %s ", FLAP->getFL(wk)->label);
    int flap = 0;
    int i = 0;
    while (!Rotary->readSwitch() && FLAP)
    {
        i++;
        flap = filter((int)(FLAP->getSensorRaw()));
        if (!(i % 10))
        {
            MYUCG->setPrintPos(1, 140);
            MYUCG->printf("Sensor: %d      ", flap);
        }
    }
    return flap;
}

int flap_cal_act(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "WK calibration ( %d ) ", p->getSelect());
    if (!FLAP)
    {
        return 0;
    }
    if (!FLAP->haveSensor())
    {
        p->clear();
        MYUCG->setPrintPos(1, 60);
        MYUCG->printf("No Sensor, Abort");
        delay(2000);
        ESP_LOGI(FNAME, "Abort calibration, no signal");
        return 0;
    }
    Average<7> filter;
    if (p->getSelect())
    {
        p->clear();
        MYUCG->setPrintPos(1, 200);
        MYUCG->setFont(ucg_font_ncenR14_hr, true);
        MYUCG->printf("Press for next");
        MYUCG->setFont(ucg_font_fub25_hr, true);
        int flap;
        for (int wk = 0; wk < FLAP->getNrPositions(); wk++)
        {
            flap = wk_cal_show(p, wk, filter);
            FL_STORE[wk].sensval->set(flap);
        }
        MYUCG->setPrintPos(1, 260);
        MYUCG->setFont(ucg_font_ncenR14_hr, true);
        MYUCG->printf("Saved");
        FLAP->initFromNVS();
        delay(800);
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

// void Flap::drawSmallBar( float wkf ){
	// MYUCG->setFont(ucg_font_profont22_mr, true);
	// int	lfh = MYUCG->getFontAscent()+4;
	// int lfw = MYUCG->getStrWidth( "+2" );
	// int top = barpos_y-lfh/2;
	// if( dirty ) {
	// 	MYUCG->setColor(COLOR_HEADER);
	// 	MYUCG->drawFrame(barpos_x-5, top-4, lfw+4, 2*lfh);
	// 	int tri = barpos_y+lfh/2-3;
	// 	MYUCG->setColor(COLOR_GREEN);
	// 	MYUCG->drawTriangle( barpos_x-10, tri-5,  barpos_x-10,tri+5, barpos_x-5, tri );
	// 	MYUCG->setColor(COLOR_WHITE);
	// 	drawWingSymbol(wkf, 0);
	// 	dirty = false;
	// }
	// MYUCG->setClipRange( barpos_x-2, top-2, lfw, 2*lfh-2 );
	// if ((int)((wkf-2)*(lfh+4)) != (int)((wkf_old-2)*(lfh+4))) {
	// 	wkf_old = wkf;
	// 	MYUCG->setColor(COLOR_BLACK);
	// 	MYUCG->drawBox( barpos_x-2, top-2, lfw, 2*lfh-2 );
	// }

	// for( int wk=int(wkf-1); wk<=int(wkf+1) && wk<=2; wk++ ){
	// 	char position[6];
	// 	if(wk<-3)
	// 		continue;

	// 	sprintf( position,"%s", flapLabels[wk+3]);
	// 	int y=top+(lfh+4)*(5-(wk+2))+(int)((wkf-2)*(lfh+4)+3);
	// 	MYUCG->setPrintPos(barpos_x-2, y );
	// 	MYUCG->setColor(COLOR_WHITE);
	// 	MYUCG->printf(position);
	// 	if( wk != -3 ) {
	// 		MYUCG->drawHLine(barpos_x-5, y+3, lfw+4 );
	// 	}
	// }
	// MYUCG->undoClipRange();
// }

// void Flap::drawFrame(int16_t xpos, int16_t ypos){
	// MYUCG->drawFrame( xpos-16, ypos-4, 11, 7 );
	// MYUCG->drawFrame( xpos-17, ypos-5, 13, 9 );
// }

// void Flap::drawLever( int16_t xpos, int16_t ypos, int16_t& oldypos, bool warn, bool good ){

	// unsigned int curmillis = millis();
	// if( warn ){
	// 	// Blink effekt
	// 	if(curmillis - _millis > 150){
	// 		MYUCG->setColor(COLOR_BLACK);
	// 		drawFrame( xpos, oldypos );
	// 		if( warn_color ){
	// 			// ESP_LOGI(FNAME,"WHITE %d", curmillis - _millis);
	// 			MYUCG->setColor(COLOR_WHITE);
	// 			warn_color = false;
	// 		}
	// 		else{
	// 			// ESP_LOGI(FNAME,"RED %d", curmillis - _millis);
	// 			MYUCG->setColor(COLOR_RED);
	// 			warn_color = true;
	// 		}
	// 		_millis = curmillis;
	// 		drawFrame( xpos, ypos );
	// 		oldypos = ypos;
	// 	}
	// }
	// else{
	// 	MYUCG->setColor(COLOR_BLACK);
	// 	drawFrame( xpos, oldypos );
	// 	if( good ){
	// 		MYUCG->setColor(COLOR_GREEN);
	// 	}
	// 	else{
	// 		MYUCG->setColor(COLOR_WHITE);
	// 	}
	// 	drawFrame( xpos, ypos );
	// 	oldypos = ypos;
	// }
// }

// static bool good_old = false;

// void Flap::drawBigBar( float wkf, float wksens ){
	// MYUCG->setFont(ucg_font_profont22_mr, true);
	// MYUCG->setFontPosCenter();
	// int16_t lfh = 24; // MYUCG->getFontAscent()+10;  // a bit place around number
	// int16_t lfw = MYUCG->getStrWidth( "+2" );
	// int16_t size = 1; //NUMPOS*lfh;
	// // draw Frame around and a triangle
	// if( dirty ) {
	// 	for( int wk=flap_min; wk<=flap_max; wk++ ){
	// 		char position[6];
	// 		// ESP_LOG_BUFFER_HEXDUMP(FNAME, flapLabels[wk+3], 4, ESP_LOG_INFO);
	// 		sprintf( position,"%s", flapLabels[wk+3]);
	// 		int y= barpos_y + lfh*wk;  // negative WK eq. lower position
	// 		// ESP_LOGI(FNAME,"Y: %d lfh:%d wk:%d",y, lfh, wk );
	// 		MYUCG->setPrintPos(barpos_x+2, y+5);
	// 		MYUCG->setColor(COLOR_WHITE);
	// 		// print digit
	// 		MYUCG->print(position);
	// 		// Frame around digit
	// 		MYUCG->setColor(COLOR_HEADER);
	// 		MYUCG->drawFrame(barpos_x-2, y-(lfh/2), lfw+6, lfh );
	// 	}
	// 	if ( flap_min > -3 ) { drawWingSymbol(wkf, wksens); }
	// 	dirty = false;
	// }
	// // ESP_LOGI(FNAME,"np: %d size: %d",  NUMPOS, size );
	// int16_t yclip = barpos_y+flap_min*lfh-(lfh/2);
	// MYUCG->setClipRange( barpos_x-17, yclip, 15, size );
	// int16_t y = barpos_y + (int)((wkf)*(lfh) + 0.5 );
	// int16_t ys = barpos_y + (int)(( wksens )*(lfh) + 0.5 );
	// // ESP_LOGI(FNAME,"wkf: %f", wkf);

	// tickopt++;
	// bool dirty_lever = false;
	// // ESP_LOGI(FNAME,"drawBigBar wkf: %.2f y:%d lfh:%d", wkf, y, lfh );
	// if( optPosOldY != y ) {  // redraw on change or every half second
	// 	MYUCG->setColor(COLOR_BLACK);
	// 	MYUCG->drawTriangle( barpos_x-15,optPosOldY-5,  barpos_x-15,optPosOldY+5,  barpos_x-2,optPosOldY );
	// 	MYUCG->setColor(COLOR_GREEN);
	// 	MYUCG->drawTriangle( barpos_x-15,y-5,       barpos_x-15,y+5,       barpos_x-2,y );
	// 	optPosOldY = y;
	// 	dirty_lever = true;
	// }
	// bool warn=false;
	// bool good=false;
	// if( abs( wkf - wksens) < 0.5 )
	// 	good = true;
	// if( abs( wkf - wksens) > 1 )
	// 	warn = true;
	// if( sensorOldY != ys || warn || good_old != good || dirty_lever) {  // redraw on change or when status changed
	// 	if( flap_sensor.get() ) {
	// 		// ESP_LOGI(FNAME,"wk lever redraw, old=%d", sensorOldY );
	// 		drawLever( barpos_x, ys, sensorOldY, warn, good );
	// 		good_old = good;
	// 		if( abs(y-ys) < 12 ){
	// 			MYUCG->setColor(COLOR_GREEN);  // redraw triangle
	// 			MYUCG->drawTriangle( barpos_x-15,y-5,       barpos_x-15,y+5,       barpos_x-2,y );
	// 		}
	// 	}
	// }
	// MYUCG->setFontPosBottom();
	// MYUCG->undoClipRange();
// }

// #define DISCRAD 3
// #define BOXLEN  12
// #define FLAPLEN 14

// void Flap::drawWingSymbol( int16_t wk, float wksens )
// {
	// static bool warn_old = false;
	// static int16_t wk_old = 0; // warn on either warn or wkopt change

	// bool warn = wksens > -10. && abs( wk - wksens) > 1.;

	// if ( warn == warn_old && wk == wk_old && ! dirty ) return;

	// if(warn){
	// 	MYUCG->setColor( COLOR_RED );
	// }else{
	// 	MYUCG->setColor( COLOR_HEADER );
	// }
	// MYUCG->drawDisc( symbolpos_x, symbolpos_y, DISCRAD, UCG_DRAW_ALL );
	// MYUCG->drawBox( symbolpos_x, symbolpos_y-DISCRAD, BOXLEN, DISCRAD*2+1  );
	// MYUCG->setColor( COLOR_BLACK );
	// MYUCG->drawTriangle( symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y-DISCRAD,
	// 		symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y+DISCRAD+1,
	// 		symbolpos_x+DISCRAD+BOXLEN-2+FLAPLEN, symbolpos_y+wk_old*4 );
	// if(warn){
	// 	MYUCG->setColor( COLOR_RED );
	// }else{
	// 	MYUCG->setColor( COLOR_GREEN );
	// }
	// MYUCG->drawTriangle( symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y-DISCRAD,
	// 		symbolpos_x+DISCRAD+BOXLEN-2, symbolpos_y+DISCRAD+1,
	// 		symbolpos_x+DISCRAD+BOXLEN-2+FLAPLEN, symbolpos_y+wk*4 );

	// wk_old = wk;
	// warn_old = warn;
// }

void Flap::initFromNVS()
{
    // Capture configured flap positions
    bool need_save = false;
    flevel.clear();
    for (int i = 0; i < MAX_NR_POS; i++)
    {
        float speed = FL_STORE[i].getSpeed();
        if (speed > 0)
        {
            // a valid entry
            need_save = need_save || i != (int)flevel.size(); // check for order change
            if ( i == MAX_NR_POS-1 && speed >= flevel.back().speed ) {
                // last entry must be slower than previous
                speed = std::max(flevel.back().speed - 20.0f, 0.f);
            }
            flevel.push_back(FlapLevel{speed, FL_STORE[i].getLabel(), FL_STORE[i].sensval->get()});
            ESP_LOGI( FNAME, "new flap level %d %s: %.1f (%d)", i, FL_STORE[i].getLabel(), speed, FL_STORE[i].sensval->get() );
        }
    }
    ESP_LOGI(FNAME, "found %d flap levels", (int)flevel.size());
    // if ( need_save ) { // would recurse
    //     saveToNVS();
    // }
}

void Flap::saveToNVS()
{
    // go through all levels and write back when changed
    for (int i = 0; i < MAX_NR_POS; i++)
    {
        if ( i < (int)flevel.size() )
        {
            // speed
            if ( FL_STORE[i].getSpeed() != flevel[i].speed ) {
                FL_STORE[i].speed->set( flevel[i].speed );
            }
            // label
            if ( strcmp(FL_STORE[i].getLabel(), flevel[i].label) != 0 ) {
                // find label index
                int lblidx = 0;
                for ( int j=0; j<55; j++ ) {
                    if ( strcmp(flap_labels[j].data(), flevel[i].label) == 0 ) {
                        lblidx = j;
                        break;
                    }
                }
                FL_STORE[i].labelidx->set( lblidx );
            }
            // sensor value
            if ( FL_STORE[i].sensval->get() != flevel[i].sensval ) {
                FL_STORE[i].sensval->set( flevel[i].sensval );
            }
        }
        else {
            FL_STORE[i].speed->set(0);
            FL_STORE[i].labelidx->set(0);
            FL_STORE[i].sensval->set(0);
        }
    }
}

void Flap::prepLevels()
{
    initFromNVS();

    // adapt speeds to actual wingload
    for ( FlapLevel &fl : flevel ) {
        fl.speed = fl.speed * sqrt( (ballast.get()+100.0) / 100.0 );
        ESP_LOGI( FNAME, "Adjusted flap speed %.1f", fl.speed );
    }

    // some precalculations for the flap levels
    _sens_ordered = flevel[0].sensval > flevel.back().sensval;
    FlapLevel* prev = nullptr;
    int sdelta = 0;
    float vdelta = 0.0f;
    for (FlapLevel &fl : flevel) {
        if (prev) {
            sdelta = fl.sensval - prev->sensval;
            if (sdelta == 0) {
                sdelta = _sens_ordered ? -1 : 1; // avoid zero deltas
            }
            prev->sens_delta = sdelta;
            vdelta = fl.speed - prev->speed;
            if (vdelta > -1.f ) {
                vdelta = -1.f;
            }
            prev->speed_delta = vdelta;
            ESP_LOGI( FNAME, "sens delta %d, vdelta %.1f", sdelta, vdelta);
        }
        prev = &fl;
    }
    if (prev) {
        prev->sens_delta = sdelta;
        prev->speed_delta = vdelta;
        ESP_LOGI( FNAME, "sens delta %d, vdelta %.1f", sdelta, vdelta);
    }
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
		sensorAdc = new AnalogInput(-1, ADC_CHANNEL_6);
	}
	if( sensorAdc != 0 ) {
		ESP_LOGI( FNAME, "Flap sensor properly configured");
		sensorAdc->begin(ADC_ATTEN_DB_0, ADC_UNIT_1, false);
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
    configureADC(flap_sensor.get());
    prepLevels();
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

bool Flap::sensorToLeverPosition(int val, float &lever)
{
    // ESP_LOGI(FNAME,"getSensorWkPos %d", wks);
    int wk = flevel.size()-1;
    for (int i = 0; i < flevel.size(); i++)
    {
        if (_sens_ordered)
        {
            if (val > flevel[i].sensval)
            {
                wk = i;
                break;
            }
        }
        else
        {
            if (val < flevel[i].sensval)
            {
                wk = i;
                break;
            }
        }
    }
    float wkf = wk + (float)(val - flevel[wk].sensval) / flevel[wk].sens_delta;
    ESP_LOGI(FNAME,"getLeverPos(%d): level %d, out: %1.2f wk: %d", val, flevel[wk].sensval, wkf, wk);
    lever = wkf;
    return true;
}

void  Flap::progress() {
	if( haveSensor() ) {
		int wkraw = getSensorRaw();
		if( wkraw > 4096 )
			wkraw = 4096;
		if( wkraw < 0 ) {
            // drop erratic negative readings
			ESP_LOGW(FNAME,"negative flap sensor reading: %d", wkraw );
			return;
		}
		// ESP_LOGI(FNAME,"flap sensor =%d", wkraw );
		rawFiltered = rawFiltered + (wkraw - rawFiltered)/6;
		tick++;
		if( !(tick%4) ){
			if( sensorToLeverPosition( rawFiltered, lever ) ){
				ESP_LOGI(FNAME,"wk sensor=%1.2f  raw=%d", lever, rawFiltered );
				if( lever < 0 )
					lever = 0;
				else if( lever > flevel.size()-1 )
					lever = flevel.size()-1;

				if( (int)(flap_pos.get()*10) != (int)(lever*10) ){
					flap_pos.set( lever );   // update secondary vario
				}
			}
		}
	}
}


float Flap::getOptimum(float spd)
{
	// Correct for current g load
	g_force += (IMU::getGliderAccelZ() - g_force) * 0.2;  // lowpass filtering, to smooth display in rough air .. fixme tune factor
	if ( g_force < 0.3 ) {
		g_force = 0.3; // Ignore meaningless values below 0.3g
	}
	float g_speed = spd / sqrt(g_force); // reduce current speed, instead of increase switch points
	ESP_LOGI(FNAME,"g force: %.1f, g corrected speed: %3.1f", g_force, g_speed );

	int wki = 0; // find the wk index
	for( auto &l : flevel) {
		if( g_speed > l.speed ) {
			break;
		}
        wki++;
	}
    if ( wki >= flevel.size() ) {
        wki = flevel.size() - 1;
    }

    float minv = flevel[wki].speed;
	float fraction = (g_speed-minv)/flevel[wki].speed_delta;
	if( fraction < -1.0 ) { fraction = -1.0; }
	fraction = wki + fraction;
	if( g_speed < GENERAL_V_MIN ) {
		wki = fraction = flap_takeoff.get();
	}
	else if ( fraction < 0. ) {
		wki = fraction = -0.1; // stop indicator a little beyond, catch ceil operator
	}
	else if ( fraction > flevel.size()-1 ) {
		wki = fraction = flevel.size()-1;
	}
	ESP_LOGI(FNAME,"ias:%.1f min:%.1f delta:%.1f wki:%d wkf:%.1f", spd, minv, flevel[wki].speed_delta, wki, fraction);
	return fraction;
}

// get speed band for given flap position wk
// 0 < wk < (# positions - 1)
float Flap::getSpeedBand(float wkf, float &maxv)
{
    int wki = ( wkf < 0.01 ) ? 0 : (int)std::ceilf(wkf);
    float minv = flevel[wki].speed;
    if( wki == 0 ) {
        maxv = GENERAL_V_MAX;
    }
    else {
        maxv = flevel[wki-1].speed;
    }

    float shift = (wkf-wki)*flevel[wki].speed_delta;
	// ESP_LOGI(FNAME,"shift:%.1f", shift);
    minv += shift;
    maxv += shift;

	// ESP_LOGI(FNAME,"wkf:%.1f minv:%.1f maxv:%.1f", wkf, minv, maxv);
    return minv;
}

// void Flap::setBarPosition(int16_t x, int16_t y)
// {
// 	barpos_x = x;
// 	barpos_y = y;
// }
// void Flap::setSymbolPosition(int16_t x, int16_t y)
// {
// 	symbolpos_x = x;
// 	symbolpos_y = y;
// }

