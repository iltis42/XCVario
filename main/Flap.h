#pragma once

#include <cstdint>
#include <string_view>

class AnalogInput;
class SetupMenu;
class SetupMenuSelect;
class SetupMenuValFloat;

extern const std::string_view flap_labels[];

int select_flap_sens_pin(SetupMenuSelect *p);
int flap_pos_act(SetupMenuValFloat *p);
int flap_cal_act(SetupMenuSelect *p);

/*
 * This class handels flap display and Flap sensor
 *
 */

class Flap {
private:
    Flap();
public:
    ~Flap();
	static Flap* theFlap();
	void  progress();
	// recommendations
	float getOptimum( float wks, int& wki );
	float getFlapPosition() { return lever; };
	inline bool haveSensor() { return sensorAdc != nullptr; }
    void setBarPosition(int16_t x, int16_t y);
    void setSymbolPosition(int16_t x, int16_t y);
	void drawSmallBar( float wkf );
	void drawBigBar( float wkf, float wksens );
	void drawLever( int16_t xpos, int16_t ypos, int16_t &oldypos, bool warn, bool good );
	void drawWingSymbol(int16_t wk, float wksens);
	void redraw() { sensorOldY = -1000; dirty=true; };
	static void setupMenue(SetupMenu *parent);
	unsigned int getSensorRaw();
	void  initSpeeds();
    void  initLabels();
	int getPosMax() const { return flap_pos_max; }
	int getNegMax() const { return flap_neg_max; }
    static inline Flap* FLAP() { return _instance; }
    static const int MAX_NR_POS = 9;
    static const int ZERO_INDEX = 4;

private: // helper
    friend int select_flap_sens_pin(SetupMenuSelect *p);
    friend int flap_pos_act(SetupMenuValFloat *p);
    friend int flap_cal_act(SetupMenuSelect *p);
	static void setupSensorMenueEntries(SetupMenu *wkm);
    static void position_labels_menu_create(SetupMenu* top);

	bool sensorToLeverPosition( int sensorreading, float& lever);
	void  initSensPos();
	void  configureADC( int port );
	int   getOptimumInt( float wks );
	void drawFrame(int16_t xpos, int16_t ypos);

private:
    static Flap* _instance;
	AnalogInput *sensorAdc = nullptr;
	float lever = -1.;
	int   senspos[MAX_NR_POS];
	int16_t leverold = -2.;
	int   flapSpeeds[MAX_NR_POS];
	const char *flapLabels[MAX_NR_POS];
	int flap_pos_max = 0; // defined through speeds available or not (set to 0)
	int flap_neg_max = 0;
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
	unsigned int _millis;
};

extern Flap* FLAP;
