#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "MPU.hpp"        // main file, provides the class itself
#include "AnalogInput.h"
#include "Protocols.h"
#include "MP5004DP.h"
#include "MS4525DO.h"
#include "IpsDisplay.h"
#include "ESPRotary.h"
#include "Compass.h" // 3-Axis Magnetic Sensor
#include <hal/gpio_types.h>
#include "SetupMenu.h"
#include "S2F.h"
#include "Wind.h"

extern Wind theWind;

extern bool haveMPU;
extern bool ahrsKeyValid;
extern SetupMenu  *Menu;

// MPU6050 sensor
extern mpud::float_axes_t accelG;
extern mpud::float_axes_t gyroDPS;

extern float ias;

extern float getTAS();
extern float getTE();
void doAudio( float te );

extern I2C_t& i2c;
extern I2C_t& i2c_0;
extern AnalogInput *AnalogInWk;

extern float temperature;
extern bool validTemperature;
extern float airspeed;
extern float TE;
extern float aTE;
extern float battery;
extern float dynamicP; // Pitot
extern float baroP;    // barometric pressure
extern float ias,tas;
extern float aTES2F;
extern float as2f;
extern float s2f_delta;
extern float polar_sink;
extern float alt;
extern float wksensor;

extern S2F Speed2Fly;
extern float meanClimb;
extern Protocols OV;
extern bool inSetup;
extern bool stall_warning_active;

extern IpsDisplay *display;

extern ESPRotary Rotary;

extern xSemaphoreHandle spiMutex;

extern Compass compass;

#endif
