#pragma once

#include "MPU.hpp"        // main file, provides the class itself
#include "AnalogInput.h"
#include "MP5004DP.h"
#include "MS4525DO.h"
#include "Compass.h" // 3-Axis Magnetic Sensor
#include "S2F.h"
#include "AdaptUGC.h"
#include "vector_3d.h"
#include "BMPVario.h"
#include "AirspeedSensor.h"

#include <hal/gpio_types.h>
#include <esp_timer.h>

#include <string>

// Display 4 Wire SPI and Display CS
#define RESET_Display  GPIO_NUM_5       // Reset pin for Display
#define CS_Display     GPIO_NUM_13      // CS pin 13 is for Display
#define SPI_SCLK       GPIO_NUM_14      // SPI Clock pin 14
#define SPI_DC         GPIO_NUM_15      // SPI Data/Command pin 15
#define SPI_MOSI       GPIO_NUM_27      // SPI SDO Master Out Slave In pin
#define SPI_MISO       GPIO_NUM_32      // SPI SDI Master In Slave Out

#define GYRO_FS (mpud::GYRO_FS_250DPS)

typedef struct global_flags{
	bool inSetup :1;
	bool haveMPU :1;
	bool ahrsKeyValid  :1;
	bool gload_alarm :1;
	bool standard_setting :1;
	bool stall_warning_active :1;
	bool stall_warning_armed :1;
	bool flarmWarning :1 ;
	bool gLoadDisplay :1;
	bool horizon :1;
	bool gear_warning_active :1;
	bool validTemperature :1 ;
	bool mpu_pwm_initalized: 1;
	bool gear_warn_external :1;
	bool schedule_reboot :1;
	bool intrDevices :1;
} t_global_flags;

class CANbus;
class SerialLine;
class Clock;
class ESPRotary;
class AnalogInput;
class PressureSensor;
class SetupRoot;
class WatchDog_C;

AnalogInput* getBattery();

extern t_global_flags gflags;
extern BMPVario bmpVario;
extern CANbus* CAN;
extern SerialLine *S1,*S2;
extern Clock *MY_CLOCK;
extern SemaphoreHandle_t xMutex;
extern AirspeedSensor *asSensor;
extern PressureSensor *baroSensor;
extern SetupRoot *Menu;
extern WatchDog_C *uiMonitor;

extern std::string logged_tests;

extern SemaphoreHandle_t display_mutex;

extern float getTAS();

extern I2C_t& i2c;

extern AnalogInput *AnalogInWk;

extern float airspeed;
extern float aTE;
extern float tas;
extern float cas;
extern float aTES2F;
extern float as2f;
extern float s2f_delta;
extern float polar_sink;
extern float alt_external;
extern float wksensor;
extern float slipAngle;

extern S2F Speed2Fly;
extern float meanClimb;
extern float baroP;    // Static pressure
extern float dynamicP; // Pitot pressure

extern long unsigned int _gps_millis;

extern ESPRotary *Rotary;

extern SemaphoreHandle_t spiMutex;

class AdaptUGC;
extern AdaptUGC *MYUCG;

extern vector_ijk gravity_vector;
extern float batteryVoltage;

#define NEED_VOLTAGE_ADJUST (abs(factory_volt_adjust.get() - 0.00815) < 0.00001)

extern float mpu_target_temp;

extern MPU_t MPU;

// Arduino.h remains
inline unsigned long millis()
{
    return (unsigned long) (esp_timer_get_time() / 1000ULL);
}

inline void delay(uint32_t ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}

int sign(int num);

void startClientSync();
