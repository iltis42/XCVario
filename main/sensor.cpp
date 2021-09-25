#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include "Cipher.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "BME280_ESP32_SPI.h"
#include <driver/adc.h>
#include <driver/gpio.h>
#include "mcp3221.h"
#include "mcp4018.h"
#include "ESP32NVS.h"
#include "MP5004DP.h"
#include "MS4525DO.h"
#include "ABPMRR.h"
#include "BMPVario.h"
#include "BTSender.h"
#include "Protocols.h"
#include "DS18B20.h"
#include "Setup.h"
#include "esp_sleep.h"
#include "ESPAudio.h"
#include <esp_wifi.h>
#include "SetupMenu.h"
#include "ESPRotary.h"
#include "AnalogInput.h"
#include "Atmosphere.h"
#include "IpsDisplay.h"
#include "sensor.h"
#include "S2F.h"
#include "Version.h"
#include "Polars.h"
#include "Flarm.h"

#include <SPI.h>
#include <Ucglib.h>
#include <OTA.h>
#include "SetupNG.h"
#include <logdef.h>
#include "Switch.h"
#include "AverageVario.h"

#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "I2Cbus.hpp"
#include "WifiApp.h"
#include "WifiClient.h"
#include "Serial.h"
#include "Cipher.h"
#include <esp32/rom/miniz.h>
#include "esp32-hal-adc.h" // needed for adc pin reset
#include "soc/sens_reg.h" // needed for adc pin reset
#include "LeakTest.h"
#include "Units.h"
#include "Flap.h"
#include "SPL06-007.h"
#include "Wind.h"
#include "CircleWind.h"

// #include "sound.h"
// modif gfm
#include "../components/gps/include/UBX_Parser.h"
#include "ahrs.hpp" /*"KalmanMPU6050.h"*/
#include "deadReckoning.h"
#include "estAltitude.h"
float estimated_altitude = 0;
float vze_fusion = 0;
int gps_nav_valid = 0;
int dead_reckon_clock = 0;
float Ground_Speed_gps = 0;
float Vsx_gps = 0;
float Vsy_gps = 0;
float Vsz_gps = 0;
float u,v,w;
float vx,vy,vz;
// fin modif gfm
/*
BMP:
    SCK - This is the SPI Clock pin, its an input to the chip
    SDO - this is the Serial Data Out / Master In Slave Out pin (MISO), for data sent from the BMP183 to your processor
    SDI - this is the Serial Data In / Master Out Slave In pin (MOSI), for data sent from your processor to the BME280
    CS - this is the Chip Select pin, drop it low to start an SPI transaction. Its an input to the chip
 */

#define SPI_SCLK GPIO_NUM_14  // SPI Clock pin 14
#define SPI_DC   GPIO_NUM_15  // SPI Data/Command pin 15
#define SPI_MOSI GPIO_NUM_27  // SPI SDO Master Out Slave In pin
#define SPI_MISO GPIO_NUM_32  // SPI SDI Master In Slave Out ESP32=Master,BME280=slave pin

#define CS_bme280BA GPIO_NUM_26   // before CS pin 33
#define CS_bme280TE GPIO_NUM_33   // before CS pin 26

#define CS_Display GPIO_NUM_13    // CS pin 13 for Display
#define RESET_Display GPIO_NUM_5  // Reset pin for Display
#define FREQ_BMP_SPI 13111111/2

#define SPL06_007_BARO 0x77
#define SPL06_007_TE   0x76
//Modif JLD
TaskHandle_t *apid;
// fin modif JLD

MCP3221 *MCP=0;
DS18B20  ds18b20( GPIO_NUM_23 );  // GPIO_NUM_23 standard, alternative  GPIO_NUM_17

AirspeedSensor *asSensor=0;
Wind theWind;

xSemaphoreHandle xMutex=NULL;

S2F Speed2Fly;
Protocols OV( &Speed2Fly );

AnalogInput Battery( (22.0+1.2)/1200, ADC_ATTEN_DB_0, ADC_CHANNEL_7, ADC_UNIT_1 );

TaskHandle_t *bpid;
TaskHandle_t *tpid;
TaskHandle_t *dpid;

xSemaphoreHandle spiMutex=NULL;

PressureSensor *baroSensor = 0;
PressureSensor *teSensor = 0;

Ucglib_ILI9341_18x240x320_HWSPI *myucg;  // ( SPI_DC, CS_Display, RESET_Display );
IpsDisplay *display;
bool topDown = false;

OTA *ota = 0;

ESPRotary Rotary;
SetupMenu  *Menu = 0;

// Gyro and acceleration sensor
I2C_t& i2c = i2c1;  // i2c0 or i2c1
I2C_t& i2c_0 = i2c0;  // i2c0 or i2c1
MPU_t MPU;         // create an object
mpud::float_axes_t accelG;
mpud::float_axes_t gyroDPS;
mpud::float_axes_t accelG_Prev;
mpud::float_axes_t gyroDPS_Prev;

// Magnetic sensor / compass
Compass compass( 0x0D, ODR_50Hz, RANGE_2GAUSS, OSR_512 );

BTSender btsender;

float baroP=0;
float temperature=15.0;
bool  validTemperature=false;
float battery=0.0;
float TE=0;
float rawdynamicP;
float dynamicP;

// global color variables for adaptable display variant
int g_col_background=255;
int g_col_highlight=0;
int g_col_header_r=101+g_col_background/5;
int g_col_header_g=108+g_col_background/5;
int g_col_header_b=g_col_highlight;
int g_col_header_light_r=161-g_col_background/4;
int g_col_header_light_g=168-g_col_background/3;
int g_col_header_light_b=g_col_highlight;


bool haveMPU=false;  
bool ahrsKeyValid=false;
bool gload_alarm=false;
int  ccp=60;
float ias = 0;
float tas = 0;
float aTE = 0;
float alt;
float altSTD;
float meanClimb = 0;
float netto = 0;
float as2f = 0;
float s2f_delta = 0;
float polar_sink = 0;
bool  standard_setting = false;
bool inSetup=true;
bool stall_warning_active=false;
bool stall_warning_armed=false;
float stall_speed_kmh=0;
float stall_alarm_off_kmh=0;
int   stall_alarm_off_holddown=0;
int count=0;
bool flarmWarning = false;
bool gLoadDisplay = false;
int hold_alarm=0;

float getTAS() { return tas; };
float getTE() { return TE; };


void drawDisplay(void *pvParameters){
	while (1) {
		// TickType_t dLastWakeTime = xTaskGetTickCount();
		if( inSetup != true ) {
			float t=temperature;
			if( validTemperature == false )
				t = DEVICE_DISCONNECTED_C;
			float airspeed = 0;
			if( airspeed_mode.get() == MODE_IAS )
				airspeed = ias;
			else if( airspeed_mode.get() == MODE_TAS )
				airspeed = tas;
			// Stall Warning Screen
			if( stall_warning.get() ){
				if( stall_warning_armed ){
					float acceleration=accelG[0];
					if( acceleration < 0.3 )
						acceleration = 0.3;  // limit acceleration effect to minimum 30% of 1g
					float acc_stall= stall_speed_kmh * sqrt( acceleration + (ballast.get()/100));  // accelerated and ballast(ed) stall speed
					if( ias < acc_stall && ias > acc_stall*0.7 ){
						if( !stall_warning_active ){
							Audio::alarm( true );
							display->drawWarning( "! STALL !", true );
							stall_warning_active = true;
						}
					}
					else{
						if( stall_warning_active ){
							Audio::alarm( false );
							display->clear();
							stall_warning_active = false;
						}
					}
					if( ias < stall_alarm_off_kmh ){
						stall_alarm_off_holddown++;
						if( stall_alarm_off_holddown > 1200 ){  // ~30 seconds holddown
							stall_warning_armed = false;
							stall_alarm_off_holddown=0;
						}
					}
					else{
						stall_alarm_off_holddown=0;
					}
				}
				else{
					if( ias > stall_speed_kmh ){
						stall_warning_armed = true;
						stall_alarm_off_holddown=0;
					}
				}
			}
			// Flarm Warning Screen
			if( flarm_warning.get() && !stall_warning_active && Flarm::alarmLevel() >= flarm_warning.get()  ){ // 0 -> Disable
				// ESP_LOGI(FNAME,"Flarm::alarmLevel: %d, flarm_warning.get() %d", Flarm::alarmLevel(), flarm_warning.get() );
				if( !flarmWarning ) {
					flarmWarning = true;
					display->clear();
					hold_alarm = 250;
				}
			}
			else{
				if( flarmWarning && (hold_alarm == 0) ){
					flarmWarning = false;
					display->clear();
					Audio::alarm( false );
				}
			}
			if( flarmWarning )
				Flarm::drawFlarmWarning();
			// G-Load Display
			if( (((float)accelG[0] > gload_pos_thresh.get() || (float)accelG[0] < gload_neg_thresh.get()) && gload_mode.get() == GLOAD_DYNAMIC ) ||
					( gload_mode.get() == GLOAD_ALWAYS_ON ) )
			{
				if( !gLoadDisplay ){
					gLoadDisplay = true;
					display->clear();
				}
			}
			else{
				if( gLoadDisplay ) {
					gLoadDisplay = false;
					display->clear();
				}
			}
			if( gLoadDisplay ) {
				display->drawLoadDisplay( (float)accelG[0] );
			}
			// G-Load Alarm when limits reached
			if( gload_mode.get() != GLOAD_OFF  ){
				if( (float)accelG[0] > gload_pos_limit.get() || (float)accelG[0] < gload_neg_limit.get() ){
					if( !gload_alarm ) {
						Audio::alarm( true );
						gload_alarm = true;
					}
				}else
				{
					if( gload_alarm ) {
						Audio::alarm( false );
						gload_alarm = false;
					}
				}
			}

			// Vario Screen
			if( !(stall_warning_active || flarmWarning || gLoadDisplay ) ) {
				display->drawDisplay( airspeed, TE, aTE, polar_sink, alt, t, battery, s2f_delta, as2f, meanClimb, Switch::cruiseMode(), standard_setting, Flap::getLever() );
			}
		}
		if( hold_alarm )
			hold_alarm--;
		vTaskDelay(20/portTICK_PERIOD_MS);
		if( uxTaskGetStackHighWaterMark( dpid ) < 1024  )
			ESP_LOGW(FNAME,"Warning drawDisplay stack low: %d bytes", uxTaskGetStackHighWaterMark( dpid ) );
	}
}

// depending on mode calculate value for Audio and set values accordingly
void doAudio(){
	polar_sink = Speed2Fly.sink( ias );
	float aTES2F = bmpVario.readS2FTE();
	float netto = aTES2F - polar_sink;
	as2f = Speed2Fly.speed( netto, !Switch::cruiseMode() );
	s2f_delta = as2f - ias;
	// ESP_LOGI( FNAME, "te: %f, polar_sink: %f, netto %f, s2f: %f  delta: %f", te, polar_sink, netto, as2f, s2f_delta );
	if( vario_mode.get() == VARIO_NETTO || (Switch::cruiseMode() &&  (vario_mode.get() == CRUISE_NETTO)) ){
		if( netto_mode.get() == NETTO_RELATIVE )
			Audio::setValues( TE - polar_sink + Speed2Fly.circlingSink( ias ), s2f_delta );
		else if( netto_mode.get() == NETTO_NORMAL )
			Audio::setValues( TE - polar_sink, s2f_delta );
	}
	else {
		Audio::setValues( TE, s2f_delta );
	}
}
//modif JLD
static int atick = 0;

void readAHRS(void *pvParameters){

while (1) {
                      		TickType_t xLastWakeTime = xTaskGetTickCount();
                      		xSemaphoreTake(xMutex,portMAX_DELAY );

                      		if( haveMPU && IMU::getInitdone() )  // 3th Generation HW, MPU6050 avail and feature enabled
                      		{
                      			mpud::raw_axes_t accelRaw;     // holds x, y, z axes as int16
                      			mpud::raw_axes_t gyroRaw;      // holds x, y, z axes as int16
                      			esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
                      			if( err != ESP_OK )
                      				ESP_LOGE(FNAME, "accel I2C error, X:%+.2f Y:%+.2f Z:%+.2f", -accelG[2], accelG[1], accelG[0] );
                      			err |= MPU.rotation(&gyroRaw);       // fetch raw data from the registers
                      			if( err != ESP_OK )
                      				ESP_LOGE(FNAME, "gyro I2C error, X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
                      			accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);  // raw data to gravity
                      			gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);  // raw data to º/s

                    			if( err == ESP_OK ) {
                    				IMU::read();
                    				dead_reckon();
                    				estAltitude();
                    			}
                      		}

                    		esp_task_wdt_reset();
                              vTaskDelayUntil(&xLastWakeTime, 10/portTICK_PERIOD_MS);  // 10 for 10 ms = 100 Hz loop

                              if( (atick++ % 100) == 0) {  // toutes les secondes on teste l’état du stack

                                            if( uxTaskGetStackHighWaterMark( apid ) < 1024 )

                                                           ESP_LOGW(FNAME,"Warning AHRS task stack low: %d bytes", uxTaskGetStackHighWaterMark( apid ) );

                              }

               }

}
//fin modif JLD
void audioTask(void *pvParameters){
	while (1)
	{
		doAudio();
		vTaskDelay(20/portTICK_PERIOD_MS);
	}
}

void readBMP(void *pvParameters){
	while (1)
	{
		count++;
		TickType_t xLastWakeTime = xTaskGetTickCount();
		xSemaphoreTake(xMutex,portMAX_DELAY );

		if( haveMPU && IMU::getInitdone() )  // 3th Generation HW, MPU6050 avail and feature enabled
		{
			mpud::raw_axes_t accelRaw;     // holds x, y, z axes as int16
			mpud::raw_axes_t gyroRaw;      // holds x, y, z axes as int16
			esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
			if( err != ESP_OK )
				ESP_LOGE(FNAME, "accel I2C error, X:%+.2f Y:%+.2f Z:%+.2f", -accelG[2], accelG[1], accelG[0] );
			err |= MPU.rotation(&gyroRaw);       // fetch raw data from the registers
			if( err != ESP_OK )
				ESP_LOGE(FNAME, "gyro I2C error, X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
			accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);  // raw data to gravity
			gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);  // raw data to º/s
			// ESP_LOGI(FNAME, "accel X: %+.2f Y:%+.2f Z:%+.2f  gyro X: %+.2f Y:%+.2f Z:%+.2f\n", -accelG[2], accelG[1], accelG[0] ,  gyroDPS.x, gyroDPS.y, gyroDPS.z);
			bool goodAccl = true;
			if( abs( accelG.x - accelG_Prev.x ) > 1 || abs( accelG.y - accelG_Prev.y ) > 1 || abs( accelG.z - accelG_Prev.z ) > 1 ) {
				MPU.acceleration(&accelRaw);
				accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);
				if( abs( accelG.x - accelG_Prev.x ) > 1 || abs( accelG.y - accelG_Prev.y ) > 1 || abs( accelG.z - accelG_Prev.z ) > 1 ){
					goodAccl = false;
					ESP_LOGE(FNAME, "accelaration change > g in 0.2 S:  X:%+.2f Y:%+.2f Z:%+.2f", -accelG[2], accelG[1], accelG[0] );
				}
			}
			bool goodGyro = true;
			if( abs( gyroDPS.x - gyroDPS_Prev.x ) > 90 || abs( gyroDPS.y - gyroDPS_Prev.y ) > 90 || abs( gyroDPS.z - gyroDPS_Prev.z ) > 90 ) {
				// ESP_LOGE(FNAME, "gyro sensor out of bounds: X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
				// ESP_LOGE(FNAME, "%04x %04x %04x", gyroRaw.x, gyroRaw.y, gyroRaw.z );
				MPU.rotation(&gyroRaw);
				gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);
				if( abs( gyroDPS.x - gyroDPS_Prev.x ) > 90 || abs( gyroDPS.y - gyroDPS_Prev.y ) > 90 || abs( gyroDPS.z - gyroDPS_Prev.z ) > 90 ) {
					goodGyro = false;
					ESP_LOGE(FNAME, "gyro angle >90 deg/s in 0.2 S: X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
				}
			}
			//modif gfm : suppression dest tests goodgyros & goodaccel pour appeler l'AHRS
			//if( err == ESP_OK && goodAccl && goodGyro ) {
			if( err == ESP_OK ) {
				IMU::read();
				dead_reckon();
				estAltitude();
			}
			gyroDPS_Prev = gyroDPS;
			accelG_Prev = accelG;
		}

		bool ok=false;
		float p = 0;
		if( asSensor )
			p = asSensor->readPascal(0, ok); // removed minimum Dp test to allow use of rawdynamicP
		if( ok ) {
			rawdynamicP = p;
			dynamicP = p;
		}
		if( dynamicP < 60 ) dynamicP = 0;
		float iasraw = Atmosphere::pascal2kmh( dynamicP );
		// ESP_LOGI("FNAME","P: %f  IAS:%f", dynamicP, iasraw );
		float T=temperature;
		if( !validTemperature ) {
			T= 15 - ( (alt/100) * 0.65 );
			// ESP_LOGW(FNAME,"T invalid, using 15 deg");
		}
		float tasraw = 0;
		if( baroP != 0 )
			tasraw =  Atmosphere::TAS( iasraw , baroP, T);  // True airspeed
		ias = ias + (iasraw - ias)*0.25;  // low pass filter

		// ESP_LOGI("FNAME","P: %f  IAS:%f IASF: %d", dynamicP, iasraw, ias );
		tas += (tasraw-tas)*0.25;       // low pass filter
		// ESP_LOGI(FNAME,"IAS=%f, T=%f, TAS=%f baroP=%f", ias, T, tas, baroP );

		TE = bmpVario.readTE( tasraw );  // 10x per second
		xSemaphoreGive(xMutex);
		// ESP_LOGI(FNAME,"count %d ccp %d", count, ccp );
		if( !(count % ccp) ) {
			AverageVario::recalcAvgClimb();
			meanClimb = AverageVario::readAvgClimb();
		}

		Flap::progress();
		xSemaphoreTake(xMutex,portMAX_DELAY );
		baroP = baroSensor->readPressure(ok);   // 5x per second
		// ESP_LOGI(FNAME,"Baro Pressure: %4.3f", baroP );
		float altSTD = baroSensor->calcAVGAltitudeSTD( baroP );
		if( alt_select.get() == 0 ) // TE
			alt = bmpVario.readAVGalt();
		else { // Baro
			if(  alt_unit.get() == 2 ) { // FL, always standard
				alt = altSTD;
				standard_setting = true;
				// ESP_LOGI(FNAME,"au: %d", alt_unit.get() );
			}else if( (fl_auto_transition.get() == 1) && ((int)altSTD*0.0328084 + (int)(standard_setting) > transition_alt.get() ) ) {
				alt = altSTD;
				standard_setting = true;
				// ESP_LOGI(FNAME,"auto:%d alts:%f ss:%d ta:%f", fl_auto_transition.get(), altSTD, standard_setting, transition_alt.get() );
			}
			else {
				alt = baroSensor->calcAVGAltitude( QNH.get(), baroP );
				standard_setting = false;
				// ESP_LOGI(FNAME,"QNH %f baro: %f alt: %f SS:%d", QNH.get(), baroP, alt, standard_setting  );
			}
		}
		aTE = bmpVario.readAVGTE();
		xSemaphoreGive(xMutex);
		doAudio();

		if( (inSetup != true) && !Flarm::bincom && ((count % 2) == 0 ) ){
			xSemaphoreTake(xMutex,portMAX_DELAY );
			// reduce also messages from 10 per second to 5 per second to reduce load in XCSoar
			char lb[510];
			if( nmea_protocol.get() == FLIGHT_TEST ) {
				OV.sendNMEA( P_FLIGHT_TEST, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature,
						time_gps,gps_nav_valid,latitude,longitude,estimated_altitude,Vsx_gps,Vsy_gps,Vsz_gps,vze_fusion,Ground_Speed_gps,
						u,v,w,vx,vy,vz);
			}
			else if( nmea_protocol.get() == BORGELT ) {
				OV.sendNMEA( P_BORGELT, lb, baroP, rawdynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature,
						time_gps,gps_nav_valid,latitude,longitude,estimated_altitude,Vsx_gps,Vsy_gps,Vsz_gps,vze_fusion,Ground_Speed_gps,
						u,v,w,vx,vy,vz);
				OV.sendNMEA( P_GENERIC, lb, baroP, rawdynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature,
						time_gps,gps_nav_valid,latitude,longitude,estimated_altitude,Vsx_gps,Vsy_gps,Vsz_gps,vze_fusion,Ground_Speed_gps,
						u,v,w,vx,vy,vz);
			}
			else if( nmea_protocol.get() == OPENVARIO ){
				OV.sendNMEA( P_OPENVARIO, lb, baroP, rawdynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature,
						time_gps,gps_nav_valid,latitude,longitude,estimated_altitude,Vsx_gps,Vsy_gps,Vsz_gps,vze_fusion,Ground_Speed_gps,
						u,v,w,vx,vy,vz  );
			}
			else if( nmea_protocol.get() == CAMBRIDGE ){
				OV.sendNMEA( P_CAMBRIDGE, lb, baroP, rawdynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature,
						time_gps,gps_nav_valid,latitude,longitude,estimated_altitude,Vsx_gps,Vsy_gps,Vsz_gps,vze_fusion,Ground_Speed_gps,
						u,v,w,vx,vy,vz );
			}
			else if( nmea_protocol.get() == XCVARIO ) {
				OV.sendNMEA( P_XCVARIO,lb, baroP, rawdynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature,
						time_gps,gps_nav_valid,latitude,longitude,estimated_altitude,Vsx_gps,Vsy_gps,Vsz_gps,vze_fusion,Ground_Speed_gps,
						u,v,w,vx,vy,vz );
			}
			else if( nmea_protocol.get() == XCVARIO_DEVEL ) {
				OV.sendNMEA( P_XCVARIO_DEVEL, lb, baroP, rawdynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature,
						time_gps,gps_nav_valid,latitude,longitude,estimated_altitude,Vsx_gps,Vsy_gps,Vsz_gps,vze_fusion,Ground_Speed_gps,
						u,v,w,vx,vy,vz);
			}
			else
				ESP_LOGE(FNAME,"Protocol %d not supported error", nmea_protocol.get() );

			vTaskDelay(2/portTICK_PERIOD_MS);
			xSemaphoreGive(xMutex);
		}

		// ESP_LOGI(FNAME,"Compass, have sensor=%d  hdm=%d ena=%d", compass.haveSensor(),  compass_nmea_hdt.get(),  compass_enable.get() );
		if( compass_enable.get() == true  && !Flarm::bincom && ! Compass::calibrationIsRunning() ) {
			// Trigger heading reading and low pass filtering. That job must be
			// done periodically.
			bool hok;
			compass.calculateHeading( &hok );

			if( (count % 5 ) == 0 && hok == true && compass_nmea_hdm.get() == true ) {
				xSemaphoreTake( xMutex, portMAX_DELAY );
				OV.sendNmeaHDM( compass.magnHeading( &ok ) );
				xSemaphoreGive( xMutex );
			}
			if( (count % 5 ) == 0 && hok == true && compass_nmea_hdt.get() == true ) {
				xSemaphoreTake( xMutex, portMAX_DELAY );
				OV.sendNmeaHDT( compass.trueHeading( &ok ) );
				xSemaphoreGive( xMutex );
			}
		}
		if( uxTaskGetStackHighWaterMark( bpid )  < 1024 )
			ESP_LOGW(FNAME,"Warning bmpTask stack low: %d", uxTaskGetStackHighWaterMark( bpid ) );

		if( accelG[0] > gload_pos_max.get() ){
			gload_pos_max.set( (float)accelG[0] );
		}else if( accelG[0] < gload_neg_max.get() ){
			gload_neg_max.set(  (float)accelG[0] );
		}
		esp_task_wdt_reset();
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}

static int ttick = 0;
static int temp_prev = -3000;

void readTemp(void *pvParameters){
	while (1) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		float t=15.0;
		if( inSetup != true )
		{
			battery = Battery.get();
			// ESP_LOGI(FNAME,"Battery=%f V", battery );
			if( blue_enable.get() != WL_WLAN_CLIENT ) {  // client Vario will get Temperature info from main Vario
				t = ds18b20.getTemp();
				if( t ==  DEVICE_DISCONNECTED_C ) {
					if( validTemperature == true ) {
						ESP_LOGI(FNAME,"Temperatur Sensor disconnected, please plug Temperature Sensor");
						validTemperature = false;
					}
				}
				else
				{
					if( validTemperature == false ) {
						ESP_LOGI(FNAME,"Temperatur Sensor connected, temperature valid");
						validTemperature = true;
					}
					temperature = t;
					if( rint(temperature*10) != temp_prev ){
//modif gfm						OV.sendTemperatureChange( temperature  );
						temp_prev = rint(temperature*10);
						ESP_LOGI(FNAME,"NEW temperature=%f  rint10: %d", temperature, temp_prev );
					}
				}
				ESP_LOGV(FNAME,"temperature=%f", temperature );
			}
		}
		Flarm::progress();
		theWind.tick();
		CircleWind::tick();
		vTaskDelayUntil(&xLastWakeTime, 1000/portTICK_PERIOD_MS);

		if( (ttick++ % 100) == 0) {
			if( uxTaskGetStackHighWaterMark( tpid ) < 1024 )
				ESP_LOGW(FNAME,"Warning temperature task stack low: %d bytes", uxTaskGetStackHighWaterMark( tpid ) );
			if( heap_caps_get_free_size(MALLOC_CAP_8BIT) < 10000 )
				ESP_LOGW(FNAME,"Warning heap_caps_get_free_size getting low: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));
		}
	}
}

static bool init_done=false;

// Sensor board init method. Herein all functions that make the XCVario are launched and tested.
void sensor(void *args){
	accelG[0] = 1;  // earth gravity default = 1 g
	accelG[1] = 0;
	accelG[2] = 0;
	gyroDPS.x = 0;
	gyroDPS.y = 0;
	gyroDPS.z = 0;

	bool selftestPassed=true;
	if( init_done )
		ESP_LOGI( FNAME, "sensor init already called");
	init_done=true;
	int line = 1;
	ESP_LOGI( FNAME, "Now setup I2C bus IO 21/22");
	i2c.begin(GPIO_NUM_21, GPIO_NUM_22, 100000 );

	theWind.begin();

	MCP = new MCP3221();
	MCP->setBus( &i2c );
	gpio_set_drive_capability(GPIO_NUM_23, GPIO_DRIVE_CAP_1);

	esp_wifi_set_mode(WIFI_MODE_NULL);
	spiMutex = xSemaphoreCreateMutex();
	Menu = new SetupMenu();
	// esp_log_level_set("*", ESP_LOG_INFO);
	ESP_LOGI( FNAME, "Log level set globally to INFO %d",  ESP_LOG_INFO);
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	ESP_LOGI( FNAME,"This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
			chip_info.cores,
			(chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
					(chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
	ESP_LOGI( FNAME,"Silicon revision %d, ", chip_info.revision);
	ESP_LOGI( FNAME,"%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
			(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
	ESP_LOGI(FNAME, "QNH.get() %f", QNH.get() );

	NVS.begin();
	if( display_orientation.get() ){
		ESP_LOGI( FNAME, "TopDown display mode flag set");
		topDown = false;
	}

	AverageVario::begin();
	Flap::initSensor();
	stall_speed_kmh = Units::Airspeed2Kmh( stall_speed.get() );
	stall_alarm_off_kmh = stall_speed_kmh/3;

	if( Cipher::checkKeyAHRS() ){
		ESP_LOGI( FNAME, "AHRS key valid=%d", ahrsKeyValid );
	}else{
		ESP_LOGI( FNAME, "AHRS key invalid=%d, disable AHRS Sensor", ahrsKeyValid );
		if( attitude_indicator.get() )
			attitude_indicator.set(0);
	}
	Battery.begin();  // for battery voltage
	xMutex=xSemaphoreCreateMutex();

	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ccp = (int)(core_climb_period.get()*10);
	SPI.begin( SPI_SCLK, SPI_MISO, SPI_MOSI, CS_bme280BA );
	xSemaphoreGive(spiMutex);

	myucg = new Ucglib_ILI9341_18x240x320_HWSPI( SPI_DC, CS_Display, RESET_Display );
	display = new IpsDisplay( myucg );
	Flarm::setDisplay( myucg );
	display->begin();
	display->bootDisplay();

	// int valid;
	String logged_tests;
	logged_tests += "\n\n\n";
	Version V;
	std::string ver( "Version: " );
	ver += V.version();
	display->writeText(line++, ver.c_str() );
	sleep(1);
	bool doUpdate = software_update.get();
	if( Rotary.readSwitch() ){
		doUpdate = true;
		ESP_LOGI(FNAME,"Rotary pressed: Do Software Update");
	}
	if( doUpdate ) {
		if( hardwareRevision.get() == 2) {
			ESP_LOGI( FNAME,"Hardware Revision detected 2");
			Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);
		}
		else  {
			ESP_LOGI( FNAME,"Hardware Revision detected 3");
			Rotary.begin( GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_0);

		}
		ota = new OTA();
		ota->begin( &Rotary );
		ota->doSoftwareUpdate( display );
	}

	esp_err_t err=ESP_ERR_NOT_FOUND;
	MPU.setBus(i2c);  // set communication bus, for SPI -> pass 'hspi'
	MPU.setAddr(mpud::MPU_I2CADDRESS_AD0_LOW);  // set address or handle, for SPI -> pass 'mpu_spi_handle'
	err = MPU.reset();
	ESP_LOGI( FNAME,"MPU Probing returned %d MPU enable: %d ", err, attitude_indicator.get() );
	if( err == ESP_OK ){
		hardwareRevision.set(3);  // wow, there is MPU6050 gyro and acceleration sensor
		haveMPU = true;
		ESP_LOGI( FNAME,"MPU initialize");
		MPU.initialize();  // this will initialize the chip and set default configurations
		MPU.setSampleRate(250);  // in (Hz)
		MPU.setAccelFullScale(mpud::ACCEL_FS_8G);
		MPU.setGyroFullScale(mpud::GYRO_FS_500DPS);
		MPU.setDigitalLowPassFilter(mpud::DLPF_98HZ);  // smoother data
		display->writeText( line++, "AHRS Sensor: OK");
		logged_tests += "MPU6050 AHRS test: PASSED\n";
		// BIAS MPU6050
		mpud::raw_axes_t gb = gyro_bias.get();
		mpud::raw_axes_t ab = accl_bias.get();
		ESP_LOGI( FNAME,"MPU current offsets accl:%d/%d/%d gyro:%d/%d/%d ZERO:%d", ab.x, ab.y, ab.z, gb.x,gb.y,gb.z, gb.isZero() );
		if( (gb.isZero() || ab.isZero()) || ahrs_autozero.get() ) {
			ESP_LOGI( FNAME,"MPU computeOffsets");
			ahrs_autozero.set(0);
			MPU.computeOffsets( &ab, &gb );
			gyro_bias.set( gb );
			accl_bias.set( ab );
			MPU.setGyroOffset(gb);
			MPU.setAccelOffset(ab);
			ESP_LOGI( FNAME,"MPU new offsets accl:%d/%d/%d gyro:%d/%d/%d ZERO:%d", ab.x, ab.y, ab.z, gb.x,gb.y,gb.z, gb.isZero() );
			if( hardwareRevision.get() != 3 )
				hardwareRevision.set(3);
		}
		else{
			MPU.setAccelOffset(ab);
			MPU.setGyroOffset(gb);
		}
		// modif gfm pour mettre les offsets accéléro à zéro
		ab.x=0;
		ab.y=0;
		ab.z=0;
		MPU.setAccelOffset(ab);
	}
	else{
		ESP_LOGI( FNAME,"MPU reset failed, check HW revision: %d",hardwareRevision.get() );
		if( hardwareRevision.get() != 2 ){
			ESP_LOGI( FNAME,"hardwareRevision detected = 2, XCVario-20");
			hardwareRevision.set(2);
		}
		if( hardwareRevision.get() == 3 ) {
			ESP_LOGI( FNAME,"hardwareRevision detected = 3, XCVario-21");
			display->writeText( line++, "AHRS Sensor: NOT FOUND");
			logged_tests += "MPU6050 AHRS test: NOT FOUND\n";
		}
	}

	String wireless_id;
	if( blue_enable.get() == WL_BLUETOOTH ) {
		wireless_id="Bluetooth ID: ";
		btsender.begin();
	}
	else
		wireless_id="WLAN SID: ";
	wireless_id += SetupCommon::getID();
	display->writeText(line++, wireless_id.c_str() );

	compass.setBus( &i2c_0 );
	// Check for magnetic sensor / compass
	if( compass_enable.get() ) {
		compass.begin();
		ESP_LOGI( FNAME, "Magnetic sensor enabled: initialize");
		err = compass.selfTest();
		if( err == ESP_OK )		{
		// Activate working of magnetic sensor
			ESP_LOGI( FNAME, "Magnetic sensor selftest: OKAY");
			display->writeText( line++, "Compass: OK");
			logged_tests += "Compass test: OK\n";
		}
		else{
			ESP_LOGI( FNAME, "Magnetic sensor selftest: FAILED");
			display->writeText( line++, "Compass: FAILED");
			logged_tests += "Compass test: FAILED\n";
			selftestPassed = false;
		}
	}

	ESP_LOGI(FNAME,"Airspeed sensor init..  type configured: %d", airspeed_sensor_type.get() );
	int offset;
	bool found = false;
	if( hardwareRevision.get() == 3 ){ // autodetect
		ESP_LOGI(FNAME," HW revision 3, check configured airspeed sensor");
		bool valid_config=true;
		switch( airspeed_sensor_type.get() ){
		case PS_TE4525:
			asSensor = new MS4525DO();
			ESP_LOGI(FNAME,"MS4525DO configured");
			break;
		case PS_ABPMRR:
			asSensor = new ABPMRR();
			ESP_LOGI(FNAME,"ABPMRR configured");
			break;
		case PS_MP3V5004:
			asSensor = new MP5004DP();
			ESP_LOGI(FNAME,"PS_MP3V5004 configured");
			break;
		default:
			valid_config = false;
			ESP_LOGI(FNAME,"No valid config found");
			break;
		}
		if( valid_config ){
			ESP_LOGI(FNAME,"There is valid config for airspeed sensor: check this one..");
			asSensor->setBus( &i2c );
			if( asSensor->selfTest( offset ) ){
				ESP_LOGI(FNAME,"Selftest for configured sensor OKAY");
				found = true;
			}
			else
				delete asSensor;
		}
		if( !found ){   // behaves same as above, so we can't detect this, needs to be setup in factory
			ESP_LOGI(FNAME,"Configured sensor not found");
			asSensor = new MS4525DO();
			asSensor->setBus( &i2c );
			ESP_LOGI(FNAME,"Try MS4525");
			if( asSensor->selfTest( offset ) ){
				airspeed_sensor_type.set( PS_ABPMRR );
				found = true;
			}
			else
				delete asSensor;
		}
		if( !found ){
			ESP_LOGI(FNAME,"MS4525 sensor not found");
			asSensor = new ABPMRR();
			asSensor->setBus( &i2c );
			ESP_LOGI(FNAME,"Try ABPMRR");
			if( asSensor->selfTest( offset ) ){
				airspeed_sensor_type.set( PS_ABPMRR );
				found = true;
			}
			else
				delete asSensor;
		}
		if( !found ){
			ESP_LOGI(FNAME,"ABPMRR sensor not found");
			ESP_LOGI(FNAME,"Try MP5004DP");
			asSensor = new MP5004DP();
			if( asSensor->selfTest( offset ) ){
				ESP_LOGI(FNAME,"MP5004DP selfTest OK");
				airspeed_sensor_type.set( PS_MP3V5004 );
				found = true;
			}
			else
				delete asSensor;
		}
	}
	else {
		ESP_LOGI(FNAME,"HW revision 2");
		ESP_LOGI(FNAME,"Aispeed sensor set MP3V5004" );
		if( airspeed_sensor_type.get() != PS_MP3V5004 )
			airspeed_sensor_type.set( PS_MP3V5004 );
		asSensor = new MP5004DP();
		if( asSensor->selfTest( offset ) ){
			ESP_LOGI(FNAME,"MP5004DP selfTest OK");
			airspeed_sensor_type.set( PS_MP3V5004 );
			found = true;
		}
		else
			ESP_LOGI(FNAME,"MP5004DP selfTest FAILED");
	}

	if( found ){
		ESP_LOGI(FNAME,"AS Speed sensors self test PASSED, offset=%d", offset);
		asSensor->doOffset();
		bool offset_plausible = asSensor->offsetPlausible( offset );
		bool ok=false;
		float p=asSensor->readPascal(60, ok);
		if( ok )
			dynamicP = p;
		ias = Atmosphere::pascal2kmh( dynamicP );
		ESP_LOGI(FNAME,"Aispeed sensor current speed=%f", ias );
		if( !offset_plausible && ( ias < 50 ) ){
			ESP_LOGE(FNAME,"Error: air speed presure sensor offset out of bounds, act value=%d", offset );
			display->writeText( line++, "AS Sensor: NEED ZERO" );
			logged_tests += "AS Sensor offset test: FAILED\n";
			selftestPassed = false;
		}
		else {
			ESP_LOGI(FNAME,"air speed offset test PASSED, readout value in bounds=%d", offset );
			char s[40];
			if( ias > 50 ) {
				sprintf(s, "AS Sensor: %d km/h", (int)(ias+0.5) );
				display->writeText( line++, s );
			}
			else
				display->writeText( line++, "AS Sensor: OK" );
			logged_tests += "AS Sensor offset test: PASSED\n";
		}
	}
	else{
		ESP_LOGE(FNAME,"Error with air speed pressure sensor, no working sensor found!");
		display->writeText( line++, "AS Sensor: NOT FOUND");
		logged_tests += "AS Sensor: NOT FOUND\n";
		selftestPassed = false;
		asSensor = 0;
	}
	ESP_LOGI(FNAME,"Now start T sensor test");
	// Temp Sensor test
	if( blue_enable.get() != WL_WLAN_CLIENT ) {
		ESP_LOGI(FNAME,"Now start T sensor test");
		ds18b20.begin();
		temperature = ds18b20.getTemp();
		ESP_LOGI(FNAME,"End T sensor test");
		if( temperature == DEVICE_DISCONNECTED_C ) {
			ESP_LOGE(FNAME,"Error: Self test Temperatur Sensor failed; returned T=%2.2f", temperature );
			display->writeText( line++, "Temp Sensor: NOT FOUND");
			validTemperature = false;
			logged_tests += "External Temperature Sensor: NOT FOUND\n";
		}else
		{
			ESP_LOGI(FNAME,"Self test Temperatur Sensor PASSED; returned T=%2.2f", temperature );
			display->writeText( line++, "Temp Sensor: OK");
			validTemperature = true;
			logged_tests += "External Temperature Sensor:PASSED\n";

		}
	}
	ESP_LOGI(FNAME,"Absolute pressure sensors init, detect type of sensor type..");

	float ba_t, ba_p, te_t, te_p;

	SPL06_007 *splBA = new SPL06_007( SPL06_007_BARO );
	SPL06_007 *splTE = new SPL06_007( SPL06_007_TE );
	splBA->setBus( &i2c );
	splTE->setBus( &i2c );
	bool baok =  splBA->begin();
	bool teok =  splTE->begin();
	if( baok || teok ){
		ESP_LOGI(FNAME,"SPL06_007 type detected");
		i2c.begin(GPIO_NUM_21, GPIO_NUM_22, 100000 );  // higher speed, we have 10K pullups on that board
		baroSensor = splBA;
		teSensor = splTE;
	}
	else{
		delete splBA;
		ESP_LOGI(FNAME,"No SPL06_007 chip detected, now check BMP280");
		BME280_ESP32_SPI *bmpBA = new BME280_ESP32_SPI();
		BME280_ESP32_SPI *bmpTE= new BME280_ESP32_SPI();
		bmpBA->setSPIBus(SPI_SCLK, SPI_MOSI, SPI_MISO, CS_bme280BA, FREQ_BMP_SPI);
		bmpTE->setSPIBus(SPI_SCLK, SPI_MOSI, SPI_MISO, CS_bme280TE, FREQ_BMP_SPI);
		bmpTE->begin();
		bmpBA->begin();
		baroSensor = bmpBA;
		teSensor = bmpTE;
	}
	bool tetest=true;
	bool batest=true;
	delay(200);
	if( ! baroSensor->selfTest( ba_t, ba_p)  ) {
		ESP_LOGE(FNAME,"HW Error: Self test Barometric Pressure Sensor failed!");
		display->writeText( line++, "Baro Sensor: NOT FOUND");
		selftestPassed = false;
		batest=false;
		logged_tests += "Baro Sensor Test: NOT FOUND\n";
	}
	else {
		ESP_LOGI(FNAME,"Baro Sensor test OK, T=%f P=%f", ba_t, ba_p);
		display->writeText( line++, "Baro Sensor: OK");
		logged_tests += "Baro Sensor Test: PASSED\n";
	}
	if( !teSensor->selfTest(te_t, te_p) ) {
		ESP_LOGE(FNAME,"HW Error: Self test TE Pressure Sensor failed!");
		display->writeText( line++, "TE Sensor: NOT FOUND");
		selftestPassed = false;
		tetest=false;
		logged_tests += "TE Sensor Test: NOT FOUND\n";
	}
	else {
		ESP_LOGI(FNAME,"TE Sensor test OK,   T=%f P=%f", te_t, te_p);
		display->writeText( line++, "TE Sensor: OK");
		logged_tests += "TE Sensor Test: PASSED\n";
	}
	if( tetest && batest ) {
		ESP_LOGI(FNAME,"Both absolute pressure sensor TESTs SUCCEEDED, now test deltas");
		if( (abs(ba_t - te_t) >4.0)  && ( ias < 50 ) ) {   // each sensor has deviations, and new PCB has more heat sources
			selftestPassed = false;
			ESP_LOGE(FNAME,"Severe T delta > 4 °C between Baro and TE sensor: °C %f", abs(ba_t - te_t) );
			display->writeText( line++, "TE/Baro Temp: Unequal");
			logged_tests += "TE/Baro Sensor T diff. <4°C: FAILED\n";
		}
		else{
			ESP_LOGI(FNAME,"Abs p sensors temp. delta test PASSED, delta: %f °C",  abs(ba_t - te_t));
			// display->writeText( line++, "TE/Baro Temp: OK");
			logged_tests += "TE/Baro Sensor T diff. <2°C: PASSED\n";
		}
		if( (abs(ba_p - te_p) >2.5)  && ( ias < 50 ) ) {
			selftestPassed = false;
			ESP_LOGI(FNAME,"Abs p sensors deviation delta > 2.5 hPa between Baro and TE sensor: %f", abs(ba_p - te_p) );
			display->writeText( line++, "TE/Baro P: Unequal");
			logged_tests += "TE/Baro Sensor P diff. <2hPa: FAILED\n";
		}
		else
			ESP_LOGI(FNAME,"Abs p sensor deta test PASSED, delta: %f hPa", abs(ba_p - te_p) );
		// display->writeText( line++, "TE/Baro P: OK");
		logged_tests += "TE/Baro Sensor P diff. <2hPa: PASSED\n";

	}
	else
		ESP_LOGI(FNAME,"Absolute pressure sensor TESTs failed");

	bmpVario.begin( teSensor, baroSensor, &Speed2Fly );
	bmpVario.setup();
	esp_task_wdt_reset();
	ESP_LOGI(FNAME,"Audio begin");
	Audio::begin( DAC_CHANNEL_1 );
	ESP_LOGI(FNAME,"Poti and Audio test");
	if( !Audio::selfTest() ) {
		ESP_LOGE(FNAME,"Error: Digital potentiomenter selftest failed");
		display->writeText( line++, "Digital Poti: Failure");
		selftestPassed = false;
		logged_tests += "Digital Audio Poti test: FAILED\n";
	}
	else{
		ESP_LOGI(FNAME,"Digital potentiometer test PASSED");
		logged_tests += "Digital Audio Poti test: PASSED\n";
		display->writeText( line++, "Digital Poti: OK");
	}

	float bat = Battery.get(true);
	if( bat < 1 || bat > 28.0 ){
		ESP_LOGE(FNAME,"Error: Battery voltage metering out of bounds, act value=%f", bat );
		display->writeText( line++, "Bat Sensor: Failure");
		logged_tests += "Battery Voltage Sensor: FAILED\n";
		selftestPassed = false;
	}
	else{
		ESP_LOGI(FNAME,"Battery voltage metering test PASSED, act value=%f", bat );
		display->writeText( line++, "Bat Sensor: OK");
		logged_tests += "Battery Voltage Sensor: PASSED\n";
	}
	Serial::begin();
	// Factory test for serial interface plus cable
	if( abs(factory_volt_adjust.get() - 0.00815) < 0.00001 ) {
		String result("Serial ");
		if( Serial::selfTest( 1 ) )
			result += "S1 OK";
		else
			result += "S1 FAIL";
		if( hardwareRevision.get() >= 3 ){
			if( Serial::selfTest( 2 ) )
				result += ",S2 OK";
			else
				result += ",S2 FAIL";
		}
		display->writeText( line++, result.c_str() );
	}
	Serial::taskStart();

	if( blue_enable.get() == WL_BLUETOOTH ) {
		if( btsender.selfTest() ){
			display->writeText( line++, "Bluetooth: OK");
			logged_tests += "Bluetooth test: PASSED\n";
		}
		else{
			display->writeText( line++, "Bluetooth: FAILED");
			logged_tests += "Bluetooth test: FAILED\n";
		}
	}else if ( blue_enable.get() == WL_WLAN ){
		wifi_init_softap();
	}

	Speed2Fly.change_polar();
	Speed2Fly.change_mc_bal();
	Version myVersion;
	ESP_LOGI(FNAME,"Program Version %s", myVersion.version() );
	ESP_LOGI(FNAME,"%s", logged_tests.c_str());
	if( !selftestPassed )
	{
		ESP_LOGI(FNAME,"\n\n\nSelftest failed, see above LOG for Problems\n\n\n");
		display->writeText( line++, "Selftest FAILED");
		if( !Rotary.readSwitch() )
			sleep(4);
	}
	else{
		ESP_LOGI(FNAME,"\n\n\n*****  Selftest PASSED  ********\n\n\n");
		display->writeText( line++, "Selftest PASSED");
		if( !Rotary.readSwitch() )
			sleep(2);
	}
	if( Rotary.readSwitch() )
	{
		LeakTest::start( baroSensor, teSensor, asSensor );
	}

	Menu->begin( display, &Rotary, baroSensor, &Battery );

	if ( blue_enable.get() == WL_WLAN_CLIENT ){
		display->clear();
		display->writeText( 2, "Wait for Master XCVario" );
		std::string ssid = WifiClient::scan();
		if( ssid.length() ){
			display->writeText( 3, "Master XCVario Found" );
			char id[30];
			sprintf( id, "Wifi ID: %s", ssid.c_str() );
			display->writeText( 4, id );
			display->writeText( 5, "Now start" );
			WifiClient::start();
			delay( 2000 );
			inSetup = false;
			display->clear();
		}
		else{
			display->writeText( 3, "Abort Wifi Scan" );
		}
	}
	else if( ias < 50.0 ){
		ESP_LOGI(FNAME,"QNH Autosetup, IAS=%3f (<50 km/h)", ias );
		// QNH autosetup
		float ae = elevation.get();
		bool ok;
		baroP = baroSensor->readPressure(ok);
		if( ae > 0 ) {
			float step=10.0; // 80 m
			float min=1000.0;
			float qnh_best = 1013.2;
			for( float qnh = 870; qnh< 1085; qnh+=step ) {
				float alt = baroSensor->readAltitude( qnh, ok);
				float diff = alt - ae;
				// ESP_LOGI(FNAME,"Alt diff=%4.2f  abs=%4.2f", diff, abs(diff) );
				if( abs( diff ) < 100 )
					step=1.0;  // 8m
				if( abs( diff ) < 10 )
					step=0.05;  // 0.4 m
				if( abs( diff ) < abs(min) ) {
					min = diff;
					qnh_best = qnh;
					// ESP_LOGI(FNAME,"New min=%4.2f", min);
				}
				if( diff > 1.0 ) // we are ready, values get already positive
					break;
				// esp_task_wdt_reset();
			}
			ESP_LOGI(FNAME,"Auto QNH=%4.2f\n", qnh_best);
			float &qnh = QNH.getRef();
			qnh = qnh_best;
		}
		display->clear();
		SetupMenuValFloat::showQnhMenu();
	}
	else
	{
		inSetup = false;
		display->clear();
	}
	Flap::init(myucg);

	if( hardwareRevision.get() == 2 )
		Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);
	else {
		Rotary.begin( GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_0);
		gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);     // 2020 series 1, analog in
		gpio_pullup_en( GPIO_NUM_2 );
		gpio_pullup_en( GPIO_NUM_34 );
	}
	gpio_set_pull_mode(RESET_Display, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_Display, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_bme280BA, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_bme280TE, GPIO_PULLUP_ONLY );

	if( blue_enable.get() != WL_WLAN_CLIENT ) {
		xTaskCreatePinnedToCore(&readBMP, "readBMP", 4096*4, NULL, 30, bpid, 0);
	}
	if( blue_enable.get() == WL_WLAN_CLIENT ){
		xTaskCreatePinnedToCore(&audioTask, "audioTask", 4096, NULL, 30, bpid, 0);
	}
	xTaskCreatePinnedToCore(&readTemp, "readTemp", 4096, NULL, 6, tpid, 0);
	xTaskCreatePinnedToCore(&drawDisplay, "drawDisplay", 8000, NULL, 13, dpid, 0);
	// modif JLD
	//xTaskCreatePinnedToCore(&readAHRS, "readAHRS", 4096, NULL, 35, apid, 0);
	// 4096: taille du stack en octets. A ajuster en fonction des besoins. Plus on a de déclarations globales et moins d’appels avec paramètres et moins on a besoin de se soucier du stack.
	// 35 : haute priorité
	// 0 : core utilisé (un seul core utilisable pour le moment L)
	 // fin modif JLD

	Audio::startAudio();
	//modif gfm
		Init_UBX_Parser();
		IMU::init(true);
		logged_tests += "MPU6050 AHRS init: PASSED\n";
		IMU::read();
		logged_tests += "MPU6050 AHRS read: PASSED\n";
		altimeter_calibrate();
		// fin modif gfm
}



extern "C" void  app_main(void){
	ESP_LOGI(FNAME,"app_main" );
	ESP_LOGI(FNAME,"Now init all Setup elements");
	bool setupPresent;
	SetupCommon::initSetup( setupPresent );
	if( !setupPresent ){
		if( Cipher::init() )
			attitude_indicator.set(1);
	}
	else
		ESP_LOGI(FNAME,"Setup already present");
	esp_log_level_set("*", ESP_LOG_INFO);
	sensor( 0 );
	vTaskDelete( NULL );
}

