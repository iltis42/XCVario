/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include <cmath>
#include <vector>


// Flap level internal representation
// ==================================
// Levels enumerate from 0 (most negative flap) to n-1 (most positive flap) for n flap levels.
// Each level defines a speed (in km/h), a label, and a sensor value (ADC reading).
// The NVS stored speeds are adjusted according to the actual wingload on initialization.
// The flap level l is recommended for speeds >= speed(l)!
// For the sake of least possible NVS entries, there is no speed for level (n-1) defined (when n=7). Then
// the last level n-1 is recommended for all speeds < speed(n-2).
//
// NVS storage order is defined through the FLAP_STORE array.
//
// The sensor values can be in ascending or descending order, depending on the flap sensor wiring.
//  


class AnalogInput;
template <typename T>
class SetupNG;

struct FlapLevel
{
    float nvs_speed;
    float prep_speed;
    float speed_delta;
    const char *label;
    int sensval;
    int sens_delta;
    FlapLevel(float s, const char *l, int sv) : nvs_speed(s), prep_speed(0.), speed_delta(0.), label(l), sensval(sv), sens_delta(0) {}
};

/*
 * This class handels flap display and Flap sensor
 *
 */

class Flap
{
private:
    Flap();

public:
    ~Flap();
    static Flap *theFlap();

    // config access
    static SetupNG<float> *getSpeedNVS(int idx);
    static SetupNG<int> *getLblNVS(int idx);
    static SetupNG<int> *getSensNVS(int idx);
    void initFromNVS();
    void saveToNVS();
    void prepLevels();
    void modLevel();
    void addLevel(FlapLevel &lev);
    void removeLevel(int idx);

    void progress();
    // recommendations
    float getOptimum(float speed) const;
    float getSpeedBand(float wkf, float &maxv) const;
    float getSpeed(float wkf);
    float getFlapPosition() const;
    bool haveSensor() const { return sensorAdc != nullptr; }
    const FlapLevel *getFL(int idx) const { return (idx < flevel.size()) ? &flevel[idx] : &dummy; }

    // sensor access
    unsigned int getSensorRaw() const;
    void configureADC(int port);
    int getNrPositions() const { return flevel.size(); }
    static constexpr const int MAX_NR_POS = 7;

private:
    // helper
    static int getWkI(float wkf) { return ( wkf < 0.01 ) ? 0 : (int)std::ceilf(wkf); }
    float sensorToLeverPosition(int sensorreading) const;
    // attributes
    static Flap *_instance;
    AnalogInput *sensorAdc = nullptr;
    std::vector<FlapLevel> flevel;
    bool _sens_ordered = true; // if true, sensval are in descending order from flap level 0 upwards
    static FlapLevel dummy;
    int rawFiltered = 0;
    int tick = 0;
    mutable float g_force = 1.;
};

extern Flap* FLAP;
