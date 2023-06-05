#pragma once

#include "AnalogInput.h"

class AdaptUGC;
class SetupMenu;
class SetupMenuSelect;
class SetupMenuValFloat;

int select_flap_sens_pin(SetupMenuSelect *p);
void showWk(SetupMenuSelect * p);
int flap_speed_act(SetupMenuValFloat *p);
int flap_lab_act(SetupMenuSelect *p);
int flap_pos_act(SetupMenuValFloat *p);
int flap_cal_act(SetupMenuSelect *p);
int flap_enable_act( SetupMenuSelect *p );

/*
 * This class handels flap display and Flap sensor
 *
 */

class Flap {
private:
    Flap(AdaptUGC *theUcg);
public:
    ~Flap();
	static Flap* init(AdaptUGC *ucg);
	void  progress();
	// inline float getLever() { return lever; }
	// inline void setLever( float l ) { lever = l; }
	// recommendations
	float getOptimum( float wks, int& wki );
	float getFlapPosition() { return lever; };
	inline bool haveSensor() { return sensorAdc != nullptr; }
    void setBarPosition(int16_t x, int16_t y);
    void setSymbolPosition(int16_t x, int16_t y);
	void drawSmallBar( float wkf );
	void drawBigBar( float wkf, float wksens );
	void drawLever( int16_t xpos, int16_t ypos, int16_t oldypos, bool warn, bool good );
	void drawWingSymbol(int16_t wk, float wksens);
	void redraw() { sensorOldY = -1000; dirty=true; };
	// void redrawLever() { sensorOldY = -1000; };
	static void setupMenue( MenuEntry *parent );
	unsigned int getSensorRaw(int oversampling=1);
    static inline Flap* FLAP() { return _instance; }
    static const int MAX_NR_POS = 9;
    static const int ZERO_INDEX = 4;

private: // helper
    friend int select_flap_sens_pin(SetupMenuSelect *p);
    friend void showWk(SetupMenuSelect *p);
    friend int flap_speed_act(SetupMenuValFloat *p);
    friend int flap_lab_act(SetupMenuSelect *p);
    friend int flap_pos_act(SetupMenuValFloat *p);
    friend int flap_cal_act(SetupMenuSelect *p);
    friend int flap_enable_act( SetupMenuSelect *p );
	static void setupSensorMenueEntries(MenuEntry *wkm);
    static void setupIndicatorMenueEntries(MenuEntry *wkm);
    static void position_labels_menu_create(MenuEntry* top);
    static void speeds_setup_menu_create(MenuEntry* top);

	bool sensorToLeverPosition( int sensorreading, float& lever);
	void  initSpeeds();
    void  initLabels();
	void  initSensPos();
	void  configureADC( int port );
	int   getOptimumInt( float wks );

private:
    static Flap* _instance;
	AdaptUGC* ucg = nullptr;
	AnalogInput *sensorAdc = nullptr;
	float lever = -1.;
	int   senspos[MAX_NR_POS];
	int16_t leverold = -2.;
	int   flapSpeeds[MAX_NR_POS];
	char *flapLabels[MAX_NR_POS];
	bool  dirty = true;
	int16_t optPosOldY = 0;
	int16_t sensorOldY = 0;
	int   rawFiltered = 0;
	int   tick = 0;
	int   tickopt = 0;
	bool  warn_color = false;
	float g_force = 1.;
    float wkf_old = 0.;
	int16_t barpos_x = 0;
	int16_t barpos_y = 0;
	int16_t symbolpos_x = 0;
	int16_t symbolpos_y = 0;
};

#define FLAP Flap::FLAP()
