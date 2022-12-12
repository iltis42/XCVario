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
#include "DataMonitor.h"
#include "AdaptUGC.h"
#include "canbus.h"
#include "CenterAid.h"
#include "vector_3d.h"

// Display 4 Wire SPI and Display CS
#define RESET_Display  GPIO_NUM_5       // Reset pin for Display
#define CS_Display     GPIO_NUM_13      // CS pin 13 is for Display
#define SPI_SCLK       GPIO_NUM_14      // SPI Clock pin 14
#define SPI_DC         GPIO_NUM_15      // SPI Data/Command pin 15
#define SPI_MOSI       GPIO_NUM_27      // SPI SDO Master Out Slave In pin
#define SPI_MISO       GPIO_NUM_32      // SPI SDI Master In Slave Out

typedef struct global_flags{
	bool inSetup :1;
	bool haveMPU :1;
	bool ahrsKeyValid  :1;
	bool gload_alarm :1;
	bool  standard_setting :1;
	bool stall_warning_active :1;
	bool stall_warning_armed :1;
	bool flarmWarning :1 ;
	bool gLoadDisplay :1;
	bool gear_warning_active :1;
	bool flarmDownload :1 ; // Flarm IGC download flag
	bool validTemperature :1 ;
	bool mpu_pwm_initalized: 1;
} t_global_flags;

extern t_global_flags gflags;

extern CANbus* CAN;
extern StraightWind theWind;
extern xSemaphoreHandle xMutex;
extern int active_screen;
extern CenterAid *centeraid;

extern SetupMenu  *Menu;
extern xSemaphoreHandle display_mutex;

extern e_wireless_type wireless;

// MPU6050 sensor
extern mpud::float_axes_t accelG;
extern mpud::float_axes_t gyroDPS;

extern float getTAS();
void doAudio( float te );

extern I2C_t& i2c;
extern I2C_t& i2c_0;
extern AnalogInput *AnalogInWk;

extern float airspeed;
extern float aTE;
extern float tas;
extern float aTES2F;
extern float as2f;
extern float s2f_delta;
extern float polar_sink;
extern float alt_external;
extern float wksensor;
extern float slipAngle;

extern S2F Speed2Fly;
extern float meanClimb;
extern Protocols OV;
extern int the_can_mode;

extern IpsDisplay *display;

extern ESPRotary Rotary;

extern DataMonitor DM;

extern xSemaphoreHandle spiMutex;

extern Compass *compass;

class AdaptUGC;
extern AdaptUGC *MYUCG;

extern vector_ijk gravity_vector;

#define NEED_VOLTAGE_ADJUST (abs(factory_volt_adjust.get() - 0.00815) < 0.00001)

extern float mpu_target_temp;

#endif
