#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "MPU.hpp"        // main file, provides the class itself
#include "AnalogInput.h"

#include "S2F.h"
extern bool haveMPU;
extern bool ahrsKeyValid;
typedef enum e_as_sensor { SENSOR_MP3V5004DP, SENSOR_MS4525DO, SENSOR_XGZ6899, SENSOR_NONE } t_as_sensor;
extern t_as_sensor as_sensor;

// MPU6050 sensor
extern mpud::float_axes_t accelG;
extern mpud::float_axes_t gyroDPS;

extern float ox;
extern float oy;
extern float oz;

extern float aox;
extern float aoy;
extern float aoz;

extern float ias;

float getTAS();
float getTE();

extern I2C_t& i2c;
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


#endif
