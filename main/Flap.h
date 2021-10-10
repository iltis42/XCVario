#pragma once


class Ucglib_ILI9341_18x240x320_HWSPI;
class SetupMenu;
class SetupMenuSelect;
class AnalogInput;
class SetupMenuValFloat;

static int select_flap_sens_pin(SetupMenuSelect *p);
static void showWk(SetupMenuSelect * p);
static int flap_speed_act(SetupMenuValFloat *p);
static int flap_lab_act(SetupMenuSelect *p);
static int flap_pos_act(SetupMenuValFloat *p);
static int flap_cal_act(SetupMenuSelect *p);
static int flap_enable_act( SetupMenuSelect *p );

/*
 * This class handels flap display and Flap sensor
 *
 */

class Flap {
private:
    Flap(Ucglib_ILI9341_18x240x320_HWSPI *theUcg);
public:
    ~Flap();
	static Flap* init(Ucglib_ILI9341_18x240x320_HWSPI *ucg);
	void  progress();
	// inline float getLever() { return lever; }
	// inline void setLever( float l ) { lever = l; }
	// recommendations
	float getOptimum( float wks, int& wki );
	inline bool haveSensor() { return sensorAdc != nullptr; }
	void drawSmallBar( int ypos, int xpos, float wkf );
	void drawBigBar( int ypos, int xpos, float wkf, float wksens );
	void drawLever( int xpos, int ypos, int oldypos, bool warn );
	void drawWingSymbol( int ypos, int xpos, int wk, float wksens);
	void redraw() { sensorOldY = -1000; surroundingBox=false; };
	// void redrawLever() { sensorOldY = -1000; };
	static void setupMenue( SetupMenu *parent );
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
	inline unsigned int getSensorRaw(int oversampling=1) {
        return haveSensor() ? sensorAdc->getRaw(oversampling) : 0;
	}
	float sensorToLeverPosition( int sensorreading );
	void  initSpeeds();
    void  initLabels();
	void  initSensPos();
	void  configureADC();
	int   getOptimumInt( float wks );

private:
    static Flap* _instance;
	Ucglib_ILI9341_18x240x320_HWSPI* ucg = nullptr;
	AnalogInput *sensorAdc = nullptr;
	float lever = -1.;
	int   senspos[MAX_NR_POS];
	int   leverold = -2.;
	int   flapSpeeds[MAX_NR_POS];
    char *flapLabels[MAX_NR_POS];
	bool  surroundingBox = false;
	int   optPosOldY = 0;
	int   sensorOldY = 0;
	int   rawFiltered = 0;
	int   tick = 0;
	int   tickopt = 0;
	bool  warn_color = false;
	float g_force = 1.;
};

#define FLAP Flap::FLAP()
