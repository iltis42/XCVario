
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


static int select_flap_sens_pin(SetupMenuSelect *p);
static int flap_cal_act(SetupMenuSelect *p);

constexpr float GENERAL_V_MIN = 50;

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
struct FLConf
{
    SetupNG<float> *speed;
    SetupNG<int>   *labelidx;
    SetupNG<int>   *sensval;
    constexpr FLConf(SetupNG<float> *s, SetupNG<int> *l, SetupNG<int> *sv) : speed(s), labelidx(l), sensval(sv) {}
    float getSpeed() const {
        return (speed) ? speed->get() : 0;
    }
    const char *getLabel() const {
        int idx = labelidx->get();
        if (idx >= 0 && idx < 55)
        {
            return flap_labels[idx].data();
        }
        return "";
    }
};

// volatile dummy to avoid null pointer checks
SetupNG<float> dummy_speed_6(  "FLAP_PLUS_3", GENERAL_V_MIN, false, SYNC_NONE, VOLATILE);
// storage of all flap configuration entries
static const std::array<FLConf, Flap::MAX_NR_POS> FL_STORE = {{
    {&wk_speed_0, &wk_label_0, &wk_sens_pos_0},
    {&wk_speed_1, &wk_label_1, &wk_sens_pos_1},
    {&wk_speed_2, &wk_label_2, &wk_sens_pos_2},
    {&wk_speed_3, &wk_label_3, &wk_sens_pos_3},
    {&wk_speed_4, &wk_label_4, &wk_sens_pos_4},
    {&wk_speed_5, &wk_label_5, &wk_sens_pos_5},
    {&dummy_speed_6, &wk_label_6, &wk_sens_pos_6}}};

// Action Routines
int select_flap_sens_pin(SetupMenuSelect *p)
{
    ESP_LOGI(FNAME, "select_flap_sens_pin");
    if (p->getSelect() == FLAP_SENSOR_ENABLE)
    {
        p->clear();
        ESP_LOGI(FNAME, "select_flap_sens_pin, have flap");
        FLAP->configureADC(FLAP_SENSOR_ENABLE);
        if (FLAP->haveSensor())
        {
            // ESP_LOGI(FNAME,"select_flap_sens_pin, have sensor");
            MYUCG->setPrintPos(5, 50);
            MYUCG->setFont(ucg_font_ncenR14_hr, true);
            MYUCG->printf("Check Sensor Reading,");
            MYUCG->setPrintPos(5, 80);
            MYUCG->printf("Press Button to exit");
            while (!Rotary->readSwitch())
            {
                // ESP_LOGI(FNAME,"SW wait loop");
                MYUCG->setPrintPos(5, 120);
                MYUCG->printf("Sensor: %d       ", FLAP->getSensorRaw());
                delay(100);
            }
        }
        delay(800);
        p->clear();
    }
    else
    {
        ESP_LOGI(FNAME, "NO flap");
        FLAP->configureADC(FLAP_SENSOR_DISABLE);
    }
    p->getParent()->setDirty();
    p->getParent()->getParent()->setDirty();
    return 0;
}

unsigned int Flap::getSensorRaw() const
{
    return sensorAdc ? sensorAdc->getRaw() : 0;
}

int wk_calib_level(SetupMenuSelect *p, int wk, Average<12> &filter)
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
    Average<12> filter;
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
            flap = wk_calib_level(p, wk, filter);
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

void flap_menu_create_flap_sensor(SetupMenu *wkm)
{
    if ( wkm->getNrChilds() == 0 ) {
        SetupMenuSelect *wkes = new SetupMenuSelect("Flap Sensor", RST_NONE, select_flap_sens_pin, &flap_sensor);
        wkes->mkEnable();
        wkes->addEntry("From Master/Second");
        wkes->setHelp("Presence of a Flap sensor, maybe connected to the master unit");
        wkm->addEntry(wkes);

        SetupMenuSelect *wkcal = new SetupMenuSelect("Sensor Calibration", RST_NONE, flap_cal_act);
        wkcal->addEntry("Cancel");
        wkcal->addEntry("Start");
        wkcal->setHelp("Calibrate the flap sensor to the configured levels. (Press button to proceed)");
        wkm->addEntry(wkcal);
    }
    SetupMenu *wkcal = static_cast<SetupMenu*>(wkm->getEntry(1));
    if ( flap_sensor.get() == FLAP_SENSOR_ENABLE ) {
        wkcal->unlock();
    } else {
        wkcal->lock();
    }
}

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
	if( flap_sensor.get() > FLAP_SENSOR_CLIENT ) {
		// migration from old settings with multiple IO's
		flap_sensor.set( FLAP_SENSOR_ENABLE );
	}
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

float Flap::sensorToLeverPosition( int val ) const
{
    // ESP_LOGI(FNAME,"getSensorWkPos %d", wks);
    int wk = flevel.size()-1;
    for (int i = 0; i < flevel.size(); i++)
    {
        if (_sens_ordered) {
            if (val > flevel[i].sensval) {
                wk = i;
                break;
            }
        }
        else {
            if (val < flevel[i].sensval) {
                wk = i;
                break;
            }
        }
    }
    float wkf = wk + (float)(val - flevel[wk].sensval) / flevel[wk].sens_delta;
    ESP_LOGI(FNAME,"getLeverPos(%d): level %d, out: %1.2f wk: %d", val, flevel[wk].sensval, wkf, wk);
    return wkf;
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
			float lever = sensorToLeverPosition( rawFiltered );
            ESP_LOGI(FNAME,"wk sensor=%1.2f  raw=%d", lever, rawFiltered );
            if ( lever < 0. ) {
                lever = 0.;
            }
            else if ( lever > flevel.size()-1 ) {
                lever = flevel.size()-1;
            }

            if ( (int)(flap_pos.get()*10) != (int)(lever*10) ) {
                flap_pos.set(lever); // update secondary vario
            }
		}
	}
}

float Flap::getFlapPosition() const
{
    return flap_pos.get();
}

float Flap::getOptimum(float spd) const
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
float Flap::getSpeedBand(float wkf, float &maxv) const
{
    int wki = ( wkf < 0.01 ) ? 0 : (int)std::ceilf(wkf);
    float minv = flevel[wki].speed;
    if( wki == 0 ) {
        maxv = v_max.get();
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

