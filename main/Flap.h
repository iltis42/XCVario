#pragma once

#include <cstdint>
#include <string_view>
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
class SetupMenu;
class SetupMenuSelect;
class SetupMenuValFloat;

extern const std::string_view flap_labels[55];

int select_flap_sens_pin(SetupMenuSelect *p);
int flap_cal_act(SetupMenuSelect *p);

struct FlapLevel {
	float speed;
	float speed_delta;
	const char* label;
	int sensval;
	int sens_delta;
	FlapLevel( float s, const char* l, int sv ) : speed(s), speed_delta(0.), label(l), sensval(sv), sens_delta(0) {}
};

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
	float getOptimum(float speed);
	float getSpeedBand(float wkf, float &maxv);
	float getFlapPosition() { return lever; };
	inline bool haveSensor() { return sensorAdc != nullptr; }
	const FlapLevel* getFL(int idx) const { return (idx < flevel.size()) ? &flevel[idx] : &dummy; }

	// obsolete
    // void setBarPosition(int16_t x, int16_t y);
    // void setSymbolPosition(int16_t x, int16_t y);
	// void drawSmallBar( float wkf );
	// void drawBigBar( float wkf, float wksens );
	// void drawLever( int16_t xpos, int16_t ypos, int16_t &oldypos, bool warn, bool good );
	// void drawWingSymbol(int16_t wk, float wksens);
	// void redraw() { sensorOldY = -1000; dirty=true; };
	static void setupMenue(SetupMenu *parent);

	// sensor access
	unsigned int getSensorRaw();
	void initFromNVS();
	void saveToNVS();
	void prepLevels();
    int getNrPositions() const { return flevel.size(); }
    static inline Flap* FLAP() { return _instance; }
    static constexpr const int MAX_NR_POS = 7;

private: // helper
    friend int select_flap_sens_pin(SetupMenuSelect *p);
	static void setupSensorMenueEntries(SetupMenu *wkm);
    static void position_labels_menu_create(SetupMenu* top);

	bool sensorToLeverPosition( int sensorreading, float& lever);
	void configureADC( int port );
	void drawFrame(int16_t xpos, int16_t ypos);

private:
    static Flap* _instance;
	AnalogInput *sensorAdc = nullptr;
	float lever = -1.;
	int16_t leverold = -2.;
	std::vector<FlapLevel> flevel;
	bool _sens_ordered = true; // if true, sensval are in descending order from flap level 0 upwards
	static FlapLevel dummy;

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
