#ifndef _SENSOR_H_
#define _SENSOR_H_

#include "MPU.hpp"        // main file, provides the class itself
extern bool haveMPU;
extern int  hardwareRevision;
typedef enum e_as_sensor { SENSOR_MP3V5004DP, SENSOR_MS4525DO, SENSOR_XGZ6899, SENSOR_NONE } t_as_sensor;
extern t_as_sensor as_sensor;

// MPU6050 sensor
extern mpud::float_axes_t accelG;
extern mpud::float_axes_t gyroDPS;

extern float ox;
extern float oy;
extern float oz;

#endif
