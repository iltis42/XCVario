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
#include "StraightWind.h"

extern StraightWind theWind;
extern xSemaphoreHandle xMutex;

extern bool haveMPU;
extern bool ahrsKeyValid;
extern SetupMenu  *Menu;

extern e_wireless_type wireless;

// MPU6050 sensor
extern mpud::float_axes_t accelG;
extern mpud::float_axes_t gyroDPS;

extern float getTAS();
void doAudio( float te );

extern I2C_t& i2c;
extern I2C_t& i2c_0;
extern AnalogInput *AnalogInWk;

extern float temperature;
extern bool validTemperature;
extern float airspeed;
extern float aTE;
extern float battery;
extern float dynamicP; // Pitot
extern float baroP;    // barometric pressure
extern float tas;
extern float aTES2F;
extern float as2f;
extern float s2f_delta;
extern float polar_sink;
extern float alt_external;
extern float wksensor;

extern S2F Speed2Fly;
extern float meanClimb;
extern Protocols OV;
extern bool inSetup;
extern bool stall_warning_active;
extern int the_can_mode;

extern IpsDisplay *display;

extern ESPRotary Rotary;

extern xSemaphoreHandle spiMutex;

extern Compass compass;

class Ucglib_ILI9341_18x240x320_HWSPI;
extern Ucglib_ILI9341_18x240x320_HWSPI *MYUCG;


#endif
