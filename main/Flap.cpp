
#include "Flap.h"

#include "AnalogInput.h"
#include "setup/SubMenuFlap.h"
#include "setup/SetupNG.h"
#include "KalmanMPU6050.h"
#include "sensor.h"
#include "logdefnone.h"

#include <array>


constexpr float GENERAL_V_MIN = 50;

Flap* Flap::_instance = nullptr;
Flap* FLAP = nullptr;
FlapLevel Flap::dummy = {.0, "x", 0};

// all nvs setup data enumerable
struct FLConf
{
    SetupNG<float> *speed;
    SetupNG<int>   *labelidx;
    SetupNG<int>   *sensval;
    constexpr FLConf(SetupNG<float> *s, SetupNG<int> *l, SetupNG<int> *sv) : speed(s), labelidx(l), sensval(sv) {}
    const char *getLabel() const {
        int idx = labelidx->get();
        if (idx >= 0 && idx < 55)
        {
            return flap_labels[idx].data();
        }
        return "";
    }
};

// storage of all flap configuration entries
static const std::array<FLConf, Flap::MAX_NR_POS> FL_STORE = {{
    {&wk_speed_0, &wk_label_0, &wk_sens_pos_0},
    {&wk_speed_1, &wk_label_1, &wk_sens_pos_1},
    {&wk_speed_2, &wk_label_2, &wk_sens_pos_2},
    {&wk_speed_3, &wk_label_3, &wk_sens_pos_3},
    {&wk_speed_4, &wk_label_4, &wk_sens_pos_4},
    {&wk_speed_5, &wk_label_5, &wk_sens_pos_5},
    {&wk_speed_6, &wk_label_6, &wk_sens_pos_6}}};

Flap::Flap() {
    if (flap_sensor.get() > FLAP_SENSOR_CLIENT) {
        // migration from old settings with multiple IO's
        flap_sensor.set(FLAP_SENSOR_ENABLE);
    }
    configureADC(flap_sensor.get());
    prepLevels();
}
Flap::~Flap() {
    if (sensorAdc) {
        delete sensorAdc;
        sensorAdc = nullptr;
    }
    _instance = nullptr;
}
Flap *Flap::theFlap() {
    if (!_instance) {
        _instance = new Flap();
    }
    return _instance;
}

SetupNG<float> *Flap::getSpeedNVS(int idx)
{
    return FL_STORE[idx].speed;
}
SetupNG<int> *Flap::getLblNVS(int idx)
{
    return FL_STORE[idx].labelidx;
}
SetupNG<int>   *Flap::getSensNVS(int idx)
{
    return FL_STORE[idx].sensval;
}

void Flap::initFromNVS()
{
    // Capture configured flap positions
    flevel.clear();
    for (int i = 0; i < MAX_NR_POS; i++)
    {
        float nvsspeed = FL_STORE[i].speed->get();
        if (nvsspeed > 0)
        {
            // a valid entry
            if ( i == MAX_NR_POS-1 && nvsspeed >= flevel.back().nvs_speed ) {
                // last entry must be slower than previous
                nvsspeed = std::max(flevel.back().nvs_speed - 20.0f, 0.f);
            }
            flevel.push_back(FlapLevel{nvsspeed, FL_STORE[i].getLabel(), FL_STORE[i].sensval->get()});
            ESP_LOGI( FNAME, "new flap level %d %s: %.1f (%d)", i, FL_STORE[i].getLabel(), nvsspeed, FL_STORE[i].sensval->get() );
        }
    }
    ESP_LOGI(FNAME, "found %d flap levels", (int)flevel.size());
}

void Flap::saveToNVS()
{
    // go through all levels and write back when changed
    // sensor values are not touched
    for (int i = 0; i < MAX_NR_POS; i++)
    {
        if ( i < (int)flevel.size() )
        {
            // speed
            if ( FL_STORE[i].speed->get() != flevel[i].nvs_speed ) {
                FL_STORE[i].speed->set( flevel[i].nvs_speed, true, false ); // synch, but no action
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
                FL_STORE[i].labelidx->set( lblidx, true, false ); // synch, but no action
            }
        }
        else {
            FL_STORE[i].speed->set(0);
            FL_STORE[i].labelidx->set(0);
        }
    }
}

void Flap::prepLevels()
{
    initFromNVS();

    // adapt speeds to actual wingload
    for ( FlapLevel &fl : flevel ) {
        fl.prep_speed = fl.nvs_speed * sqrt( (ballast.get()+100.0) / 100.0 );
        ESP_LOGI( FNAME, "Adjusted flap speed %.1f", fl.prep_speed );
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
            vdelta = fl.prep_speed - prev->prep_speed;
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

void Flap::modLevel()
{
    // keep sorted by speed
    std::sort(flevel.begin(), flevel.end(), [](const FlapLevel &a, const FlapLevel &b) {
        return a.nvs_speed > b.nvs_speed;
    });
    saveToNVS();
    prepLevels();
}
void Flap::addLevel(FlapLevel &lev)
{
    flevel.push_back(lev);
    modLevel();
}
void Flap::removeLevel(int idx)
{
    flevel.erase(flevel.begin() + idx);
    saveToNVS();
    prepLevels();
}

void Flap::configureADC(int port) {
    ESP_LOGI(FNAME, "Flap::configureADC port: %d", port);
    if (sensorAdc) {
        delete sensorAdc;
        sensorAdc = nullptr;
    }

    if (flap_sensor.get() == FLAP_SENSOR_ENABLE) { // nonzero -> configured, only one port needed for XCV23+ HW
        sensorAdc = new AnalogInput(-1, ADC_CHANNEL_6);
    }
    if (sensorAdc != 0) {
        ESP_LOGI(FNAME, "Flap sensor properly configured");
        sensorAdc->begin(ADC_ATTEN_DB_0, ADC_UNIT_1, false);
        delay(10);
        uint32_t read = sensorAdc->getRaw();
        if (read == 0 || read >= 4096) { // try GPIO pin 34, series 2021-2
            ESP_LOGI(FNAME, "Flap sensor not found or edge value, reading: %d", (int)read);
        } else {
            ESP_LOGI(FNAME, "Flap sensor looks good, reading: %d", (int)read);
        }
    } else {
        ESP_LOGI(FNAME, "Sensor ADC NOT properly configured");
    }
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

void Flap::progress() {
    if (haveSensor()) {
        int wkraw = getSensorRaw();
        if (wkraw > 4096)
            wkraw = 4096;
        if (wkraw < 0) {
            // drop erratic negative readings
            ESP_LOGW(FNAME, "negative flap sensor reading: %d", wkraw);
            return;
        }
        // ESP_LOGI(FNAME,"flap sensor =%d", wkraw );
        rawFiltered = rawFiltered + (wkraw - rawFiltered) / 6;
        tick++;
        if (!(tick % 4)) {
            float lever = sensorToLeverPosition(rawFiltered);
            ESP_LOGI(FNAME, "wk sensor=%1.2f  raw=%d", lever, rawFiltered);
            if (lever < 0.) {
                lever = 0.;
            } else if (lever > flevel.size() - 1) {
                lever = flevel.size() - 1;
            }

            if ((int)(flap_pos.get() * 10) != (int)(lever * 10)) {
                flap_pos.set(lever); // update secondary vario
            }
        }
    }
}

float Flap::getFlapPosition() const
{
    return flap_pos.get();
}

float Flap::getOptimum(float spd) const {
    // Correct for current g load
    g_force += (IMU::getGliderAccelZ() - g_force) * 0.5;
    if (g_force < 0.3) {
        g_force = 0.3; // Ignore meaningless values below 0.3g
    }
    float g_speed = spd / sqrt(g_force); // reduce current speed, instead of increase switch points
    ESP_LOGI(FNAME, "g force: %.1f, g corrected speed: %3.1f", g_force, g_speed);

    int wki = 0; // find the wk index
    for (auto &l : flevel) {
        if (g_speed > l.prep_speed) {
            break;
        }
        wki++;
    }
    if (wki >= flevel.size()) {
        wki = flevel.size() - 1;
    }

    float minv = flevel[wki].prep_speed;
    float fraction = (g_speed - minv) / flevel[wki].speed_delta;
    if (fraction < -1.0) {
        fraction = -1.0;
    }
    fraction = wki + fraction;
    if (g_speed < GENERAL_V_MIN) {
        wki = fraction = flap_takeoff.get();
    } else if (fraction < 0.) {
        wki = fraction = -0.1; // stop indicator a little beyond, catch ceil operator
    } else if (fraction > flevel.size() - 1) {
        wki = fraction = flevel.size() - 1;
    }
    ESP_LOGI(FNAME, "ias:%.1f min:%.1f delta:%.1f wki:%d wkf:%.1f", spd, minv, flevel[wki].speed_delta, wki, fraction);
    return fraction;
}

// get speed band for given flap position wk
// 0 < wk < (# positions - 1)
float Flap::getSpeedBand(float wkf, float &maxv) const
{
    int wki = getWkI(wkf);
    float minv = flevel[wki].prep_speed;
    if( wki == 0 ) {
        maxv = v_max.get();
    }
    else {
        maxv = flevel[wki-1].prep_speed;
    }

    float shift = (wkf-wki)*flevel[wki].speed_delta;
    // ESP_LOGI(FNAME,"shift:%.1f", shift);
    minv += shift;
    maxv += shift;

    // ESP_LOGI(FNAME,"wkf:%.1f minv:%.1f maxv:%.1f", wkf, minv, maxv);
    return minv;
}

float Flap::getSpeed(float wkf)
{
    int wki = getWkI(wkf);
    return flevel[wki].prep_speed + (wkf-wki)*flevel[wki].speed_delta;
}
