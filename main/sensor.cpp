
#include "sensor.h"

#include "Cipher.h"
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
#include "ESPAudio.h"
#include "SetupMenu.h"
#include "ESPRotary.h"
#include "AnalogInput.h"
#include "Atmosphere.h"
#include "IpsDisplay.h"
#include "S2F.h"
#include "Version.h"
#include "Polars.h"
#include "Flarm.h"
#include "Blackboard.h"
#include "SetupMenuValFloat.h"

#include <SPI.h>
#include <AdaptUGC.h>
#include <OTA.h>
#include "SetupNG.h"
#include "Switch.h"
#include "AverageVario.h"

#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "I2Cbus.hpp"
#include "KalmanMPU6050.h"
#include "WifiApp.h"
#include "WifiClient.h"
#include "Serial.h"
#include "LeakTest.h"
#include "Units.h"
#include "Flap.h"
#include "SPL06-007.h"
#include "StraightWind.h"
#include "CircleWind.h"
#include <coredump_to_server.h>
#include "canbus.h"
#include "Router.h"

#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_task_wdt.h>
#include <esp_log.h>
#include <esp32/rom/miniz.h>
#include <esp32-hal-adc.h> // needed for adc pin reset
#include <soc/sens_reg.h> // needed for adc pin reset
#include <esp_sleep.h>
#include <esp_wifi.h>
#include <esp_system.h>
#include <esp_timer.h>
#include <nvs_flash.h>
#include <string>
#include <cstdio>
#include <cstring>
#include "DataMonitor.h"
#include "AdaptUGC.h"
#include "CenterAid.h"

// #include "sound.h"

/*
BMP:
    SCK - This is the SPI Clock pin, its an input to the chip
    SDO - this is the Serial Data Out / Master In Slave Out pin (MISO), for data sent from the BMP183 to your processor
    SDI - this is the Serial Data In / Master Out Slave In pin (MOSI), for data sent from your processor to the BME280
    CS - this is the Chip Select pin, drop it low to start an SPI transaction. Its an input to the chip
 */

#define CS_bme280BA GPIO_NUM_26   // before CS pin 33
#define CS_bme280TE GPIO_NUM_33   // before CS pin 26
#define FREQ_BMP_SPI 13111111/2
#define SPL06_007_BARO 0x77
#define SPL06_007_TE   0x76

#define MGRPS 360

MCP3221 *MCP=0;
DS18B20  ds18b20( GPIO_NUM_23 );  // GPIO_NUM_23 standard, alternative  GPIO_NUM_17

AirspeedSensor *asSensor=0;
StraightWind theWind;

xSemaphoreHandle xMutex=NULL;
xSemaphoreHandle spiMutex=NULL;

S2F Speed2Fly;
Protocols OV( &Speed2Fly );

AnalogInput Battery( (22.0+1.2)/1200, ADC_ATTEN_DB_0, ADC_CHANNEL_7, ADC_UNIT_1 );

TaskHandle_t apid = NULL;
TaskHandle_t bpid = NULL;
TaskHandle_t tpid = NULL;
TaskHandle_t dpid = NULL;

e_wireless_type wireless;

PressureSensor *baroSensor = 0;
PressureSensor *teSensor = 0;

AdaptUGC *MYUCG = 0;  // ( SPI_DC, CS_Display, RESET_Display );
IpsDisplay *display = 0;
CenterAid  *centeraid = 0;

bool topDown = false;

OTA *ota = 0;

ESPRotary Rotary;
SetupMenu  *Menu = 0;
DataMonitor DM;

// Gyro and acceleration sensor
I2C_t& i2c = i2c1;  // i2c0 or i2c1
I2C_t& i2c_0 = i2c0;  // i2c0 or i2c1
MPU_t MPU;         // create an object
mpud::float_axes_t accelG;
mpud::float_axes_t gyroDPS;
mpud::float_axes_t accelG_Prev;
mpud::float_axes_t gyroDPS_Prev;
#define MAXDRIFT 2               // °/s
#define NUM_GYRO_SAMPLES 3000    // 10 per second -> 5 minutes, so T has been settled after power on
static uint16_t num_gyro_samples = 0;
static int32_t cur_gyro_bias[3];

// Magnetic sensor / compass
Compass *compass = 0;
BTSender btsender;

static float baroP=0; // barometric pressure
static float temperature=15.0;

static float battery=0.0;
static float dynamicP; // Pitot

float slipAngle = 0.0;

// global color variables for adaptable display variant
uint8_t g_col_background=255; // black
uint8_t g_col_highlight=0;
uint8_t g_col_header_r=101+g_col_background/5;
uint8_t g_col_header_g=108+g_col_background/5;
uint8_t g_col_header_b=g_col_highlight;
uint8_t g_col_header_light_r=161-g_col_background/4;
uint8_t g_col_header_light_g=168-g_col_background/3;
uint8_t g_col_header_light_b=g_col_highlight;
uint16_t gear_warning_holdoff = 0;

t_global_flags gflags = { true, false, false, false, false, false, false, false, false, false, false, false, false };

int  ccp=60;
float tas = 0;
float aTE = 0;
float alt_external;
float altSTD;
float netto = 0;
float as2f = 0;
float s2f_delta = 0;
float polar_sink = 0;

float      stall_alarm_off_kmh=0;
uint16_t   stall_alarm_off_holddown=0;

int count=0;
int flarm_alarm_holdtime=0;
int the_can_mode = CAN_MODE_MASTER;
int active_screen = 0;  // 0 = Vario

AdaptUGC *egl = 0;

#define GYRO_FS (mpud::GYRO_FS_250DPS)

float getTAS() { return tas; };

bool do_factory_reset() {
	return( SetupCommon::factoryReset() );
}


static float mpu_t_delta = 0;
static float mpu_t_delta_i = 0;
static float mpu_heat_pwm = 0;

void mpu_temp_control(){
	mpu_t_delta = MPU.getTemperature() - 40.0;
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	mpu_heat_pwm = -mpu_t_delta*6.0;          // P part
	mpu_t_delta_i -= (mpu_t_delta)/10.0;	  // I part
	if( mpu_t_delta_i > 10 )
		mpu_t_delta_i = 10;
	if( mpu_t_delta_i < -10 )
		mpu_t_delta_i = -10;
	mpu_heat_pwm += mpu_t_delta_i;
	if( mpu_heat_pwm > 10 )
		mpu_heat_pwm = 10;
	if( mpu_heat_pwm < 0 )
		mpu_heat_pwm = 0;
	if( (count%10) <  rint(mpu_heat_pwm) ){
		gpio_set_level(GPIO_NUM_2, 1 );  // MPU heating on
		ESP_LOGI(FNAME,"Td= %f, ON, pwm: %d", mpu_t_delta, (int)rint(mpu_heat_pwm) );
		gflags.mpu_heat_on = true;
	}else{
		gpio_set_level(GPIO_NUM_2, 0 );  // off
		ESP_LOGI(FNAME,"Td= %f, OFF pwm: %d", mpu_t_delta, (int)rint(mpu_heat_pwm) );
		gflags.mpu_heat_on = false;
	}
}


void drawDisplay(void *pvParameters){
	while (1) {
		if( Flarm::bincom ) {
			if( gflags.flarmDownload == false ) {
				gflags.flarmDownload = true;
				display->clear();
				Flarm::drawDownloadInfo();
			}
			// Flarm IGC download is running, display will be blocked, give Flarm
			// download all cpu power.
			vTaskDelay(20/portTICK_PERIOD_MS);
			continue;
		}
		else if( gflags.flarmDownload == true ) {
			gflags.flarmDownload = false;
			display->clear();
		}
		// TickType_t dLastWakeTime = xTaskGetTickCount();
		if( gflags.inSetup != true ) {
			float t=OAT.get();
			if( gflags.validTemperature == false )
				t = DEVICE_DISCONNECTED_C;
			float airspeed = 0;
			if( airspeed_mode.get() == MODE_IAS )
				airspeed = ias.get();
			else if( airspeed_mode.get() == MODE_TAS )
				airspeed = tas;

			// Stall Warning Screen
			if( stall_warning.get() && gload_mode.get() != GLOAD_ALWAYS_ON ){  // In aerobatics stall warning is contra productive, we concentrate on G-Load Display if permanent enabled
				if( gflags.stall_warning_armed ){
					float acceleration=accelG[0];
					if( acceleration < 0.3 )
						acceleration = 0.3;  // limit acceleration effect to minimum 30% of 1g
					float acc_stall= stall_speed.get() * sqrt( acceleration + ( ballast.get()/100));  // accelerated and ballast(ed) stall speed
					if( ias.get() < acc_stall && ias.get() > acc_stall*0.7 ){
						if( !gflags.stall_warning_active ){
							Audio::alarm( true );
							display->drawWarning( "! STALL !", true );
							gflags.stall_warning_active = true;
						}
					}
					else{
						if( gflags.stall_warning_active ){
							Audio::alarm( false );
							display->clear();
							gflags.stall_warning_active = false;
						}
					}
					if( ias.get() < stall_alarm_off_kmh ){
						stall_alarm_off_holddown++;
						if( stall_alarm_off_holddown > 1200 ){  // ~30 seconds holddown
							gflags.stall_warning_armed = false;
							stall_alarm_off_holddown=0;
						}
					}
					else{
						stall_alarm_off_holddown=0;
					}
				}
				else{
					if( ias.get() > stall_speed.get() ){
						gflags.stall_warning_armed = true;
						stall_alarm_off_holddown=0;
					}
				}
			}
			if( gear_warning.get() ){
				if( !gear_warning_holdoff ){
					int gw = digitalRead( SetupMenu::getGearWarningIO() );
					if( gear_warning.get() == GW_FLAP_SENSOR_INV || gear_warning.get() == GW_S2_RS232_RX_INV ){
						gw = !gw;
					}
					if( gw ){
						if( ESPRotary::readSwitch() ){
							gear_warning_holdoff = 25000;  // 5 min
							Audio::alarm( false );
							display->clear();
							gflags.gear_warning_active = false;
							SetupMenu::catchFocus( false );
						}
						else if( !gflags.gear_warning_active && !gflags.stall_warning_active ){
							Audio::alarm( true );
							display->drawWarning( "! GEAR !", false );
							gflags.gear_warning_active = true;
							SetupMenu::catchFocus( true );
						}
					}
					else{
						if( gflags.gear_warning_active ){
							SetupMenu::catchFocus( false );
							Audio::alarm( false );
							display->clear();
							gflags.gear_warning_active = false;
						}
					}
				}
				else{
					gear_warning_holdoff--;
				}
			}

			// Flarm Warning Screen
			if( flarm_warning.get() && !gflags.stall_warning_active && Flarm::alarmLevel() >= flarm_warning.get() ){ // 0 -> Disable
				// ESP_LOGI(FNAME,"Flarm::alarmLevel: %d, flarm_warning.get() %d", Flarm::alarmLevel(), flarm_warning.get() );
				if( !gflags.flarmWarning ) {
					gflags.flarmWarning = true;
					delay(100);
					display->clear();
					flarm_alarm_holdtime = 250;
				}
			}
			else{
				if( gflags.flarmWarning && (flarm_alarm_holdtime == 0) ){
					gflags.flarmWarning = false;
					display->clear();
					Audio::alarm( false );
				}
			}
			if( gflags.flarmWarning )
				Flarm::drawFlarmWarning();
			// G-Load Display
			// ESP_LOGI(FNAME,"Active Screen = %d", active_screen );
			if( (((float)accelG[0] > gload_pos_thresh.get() || (float)accelG[0] < gload_neg_thresh.get()) && gload_mode.get() == GLOAD_DYNAMIC ) ||
					( gload_mode.get() == GLOAD_ALWAYS_ON ) || ((active_screen << SCREEN_GMETER) & 1)  )
			{
				if( !gflags.gLoadDisplay ){
					gflags.gLoadDisplay = true;
				}
			}
			else{
				if( gflags.gLoadDisplay ) {
					gflags.gLoadDisplay = false;
				}
			}
			if( gflags.gLoadDisplay ) {
				display->drawLoadDisplay( (float)accelG[0] );
			}
			// G-Load Alarm when limits reached
			if( gload_mode.get() != GLOAD_OFF  ){
				if( (float)accelG[0] > gload_pos_limit.get() || (float)accelG[0] < gload_neg_limit.get()  ){
					if( !gflags.gload_alarm ) {
						Audio::alarm( true );
						gflags.gload_alarm = true;
					}
				}else
				{
					if( gflags.gload_alarm ) {
						Audio::alarm( false );
						gflags.gload_alarm = false;
					}
				}
			}
			// Vario Screen
			if( !(gflags.stall_warning_active || gflags.gear_warning_active || gflags.flarmWarning || gflags.gLoadDisplay )  ) {
				// ESP_LOGI(FNAME,"TE=%2.3f", te_vario.get() );
				display->drawDisplay( airspeed, te_vario.get(), aTE, polar_sink, altitude.get(), t, battery, s2f_delta, as2f, average_climb.get(), Switch::getCruiseState(), gflags.standard_setting, flap_pos.get() );
			}
			if( screen_centeraid.get() ){
				if( centeraid ){
					centeraid->tick();
				}
			}
		}
		if( flarm_alarm_holdtime )
			flarm_alarm_holdtime--;
		vTaskDelay(20/portTICK_PERIOD_MS);
		if( uxTaskGetStackHighWaterMark( dpid ) < 512  )
			ESP_LOGW(FNAME,"Warning drawDisplay stack low: %d bytes", uxTaskGetStackHighWaterMark( dpid ) );
	}
}

// depending on mode calculate value for Audio and set values accordingly
void doAudio(){
	polar_sink = Speed2Fly.sink( ias.get() );
	float netto = te_vario.get() - polar_sink;
	as2f = Speed2Fly.speed( netto, !Switch::getCruiseState() );
	s2f_delta = s2f_delta + ((as2f - ias.get()) - s2f_delta)* (1/(s2f_delay.get()*10)); // low pass damping moved to the correct place
	// ESP_LOGI( FNAME, "te: %f, polar_sink: %f, netto %f, s2f: %f  delta: %f", aTES2F, polar_sink, netto, as2f, s2f_delta );
	if( vario_mode.get() == VARIO_NETTO || (Switch::getCruiseState() &&  (vario_mode.get() == CRUISE_NETTO)) ){
		if( netto_mode.get() == NETTO_RELATIVE )
			Audio::setValues( te_vario.get() - polar_sink + Speed2Fly.circlingSink( ias.get() ), s2f_delta );
		else if( netto_mode.get() == NETTO_NORMAL )
			Audio::setValues( te_vario.get() - polar_sink, s2f_delta );
	}
	else {
		Audio::setValues( te_vario.get(), s2f_delta );
	}
}

void audioTask(void *pvParameters){
	while (1)
	{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		if( Flarm::bincom ) {
			// Flarm IGC download is running, audio will be blocked, give Flarm
			// download all cpu power.
			vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
			continue;
		}
		doAudio();
		Router::routeXCV();
		if( uxTaskGetStackHighWaterMark( apid )  < 512 )
			ESP_LOGW(FNAME,"Warning audio task stack low: %d", uxTaskGetStackHighWaterMark( apid ) );
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}


static void grabMPU()
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
	gyroDPS = mpud::gyroDegPerSec(gyroRaw, GYRO_FS);  // raw data to º/s
	// ESP_LOGI(FNAME, "accel X: %+.2f Y:%+.2f Z:%+.2f  gyro X: %+.2f Y:%+.2f Z:%+.2f ABx:%d ABy:%d, ABz=%d\n", -accelG[2], accelG[1], accelG[0] ,  gyroDPS.x, gyroDPS.y, gyroDPS.z, accl_bias.get().x, accl_bias.get().y, accl_bias.get().z );
	if( !(count%60) ){
		ESP_LOGI(FNAME, "Gyro X:%+.2f Y:%+.2f Z:%+.2f T=%f\n", gyroDPS.x, gyroDPS.y, gyroDPS.z, MPU.getTemperature());
	}
	bool goodAccl = true;
	if( abs( accelG.x - accelG_Prev.x ) > 1 || abs( accelG.y - accelG_Prev.y ) > 1 || abs( accelG.z - accelG_Prev.z ) > 1 ) {
		MPU.acceleration(&accelRaw);
		accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);
		if( abs( accelG.x - accelG_Prev.x ) > 1 || abs( accelG.y - accelG_Prev.y ) > 1 || abs( accelG.z - accelG_Prev.z ) > 1 ){
			goodAccl = false;
			ESP_LOGE(FNAME, "accelaration change > 1 g in 0.2 S:  X:%+.2f Y:%+.2f Z:%+.2f", -accelG[2], accelG[1], accelG[0] );
		}
	}
	bool goodGyro = true;
	if( abs( gyroDPS.x - gyroDPS_Prev.x ) > MGRPS || abs( gyroDPS.y - gyroDPS_Prev.y ) > MGRPS || abs( gyroDPS.z - gyroDPS_Prev.z ) > MGRPS ) {
		// ESP_LOGE(FNAME, "gyro sensor out of bounds: X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
		// ESP_LOGE(FNAME, "%04x %04x %04x", gyroRaw.x, gyroRaw.y, gyroRaw.z );
		MPU.rotation(&gyroRaw);
		gyroDPS = mpud::gyroDegPerSec(gyroRaw, GYRO_FS );
		if( abs( gyroDPS.x - gyroDPS_Prev.x ) > MGRPS || abs( gyroDPS.y - gyroDPS_Prev.y ) > MGRPS || abs( gyroDPS.z - gyroDPS_Prev.z ) > MGRPS ) {
			goodGyro = false;
			ESP_LOGE(FNAME, "gyro angle >90 deg/s in 0.2 S: X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
		}
	}
	if( err == ESP_OK ){
		// check low rotation on all 3 axes = on ground
		if( abs( gyroDPS.x ) < MAXDRIFT && abs( gyroDPS.y ) < MAXDRIFT && abs( gyroDPS.z ) < MAXDRIFT ) {
			if( ias.get() < 25 ){  // check no significant IAS
				num_gyro_samples++;
				for(int i=0; i<3; i++){
					cur_gyro_bias[i] += gyroRaw[i];
				}
				if( num_gyro_samples > NUM_GYRO_SAMPLES ){  // every couple of minutes recalculate offset
					mpud::raw_axes_t gb;
					mpud::raw_axes_t gbo = MPU.getGyroOffset();
					ESP_LOGI(FNAME, "Old gyro offset: X:%d Y:%d Z:%d",  gbo.x, gbo.y, gbo.z );
					for(int i=0; i<3; i++){
						gb[i]  = gbo[i] -(( (cur_gyro_bias)[i]/(NUM_GYRO_SAMPLES*4)) ); // translate to 1000 DPS
						cur_gyro_bias[i] = 0;
					}
					ESP_LOGI(FNAME, "Set new gyro offset: X:%d Y:%d Z:%d",  gb.x, gb.y, gb.z );
					MPU.setGyroOffset( gb );
					num_gyro_samples = 0;
				}
			}
		}
	}

	if( err == ESP_OK && goodAccl && goodGyro ) {
		IMU::read();
	}
	gyroDPS_Prev = gyroDPS;
	accelG_Prev = accelG;
}

static void lazyNvsCommit()
{
	uint16_t dummy;
	if ( xQueueReceive(SetupCommon::commitSema, &dummy, 0) ) {
		SetupCommon::commitNow();
	}
}

static void toyFeed()
{
	xSemaphoreTake(xMutex,portMAX_DELAY );
	// reduce also messages from 10 per second to 5 per second to reduce load in XCSoar
	// maybe just 1 or 2 per second
	static char lb[150];

	if( ahrs_rpyl_dataset.get() ){
		OV.sendNMEA( P_AHRS_RPYL, lb, baroP, dynamicP, te_vario.get(), OAT.get(), ias.get(), tas, MC.get(), bugs.get(), ballast.get(), Switch::getCruiseState(), altitude.get(), gflags.validTemperature,
				-accelG[2], accelG[1],accelG[0], gyroDPS.x, gyroDPS.y, gyroDPS.z );
		OV.sendNMEA( P_AHRS_APENV1, lb, baroP, dynamicP, te_vario.get(), OAT.get(), ias.get(), tas, MC.get(), bugs.get(), ballast.get(), Switch::getCruiseState(), altitude.get(), gflags.validTemperature,
				-accelG[2], accelG[1],accelG[0], gyroDPS.x, gyroDPS.y, gyroDPS.z );
	}
	if( nmea_protocol.get() == BORGELT ) {
		OV.sendNMEA( P_BORGELT, lb, baroP, dynamicP, te_vario.get(), OAT.get(), ias.get(), tas, MC.get(), bugs.get(), ballast.get(), Switch::getCruiseState(), altSTD, gflags.validTemperature  );
		OV.sendNMEA( P_GENERIC, lb, baroP, dynamicP, te_vario.get(), OAT.get(), ias.get(), tas, MC.get(), bugs.get(), ballast.get(), Switch::getCruiseState(), altSTD, gflags.validTemperature  );
	}
	else if( nmea_protocol.get() == OPENVARIO ){
		OV.sendNMEA( P_OPENVARIO, lb, baroP, dynamicP, te_vario.get(), OAT.get(), ias.get(), tas, MC.get(), bugs.get(), ballast.get(), Switch::getCruiseState(), altitude.get(), gflags.validTemperature  );
	}
	else if( nmea_protocol.get() == CAMBRIDGE ){
		OV.sendNMEA( P_CAMBRIDGE, lb, baroP, dynamicP, te_vario.get(), OAT.get(), ias.get(), tas, MC.get(), bugs.get(), ballast.get(), Switch::getCruiseState(), altitude.get(), gflags.validTemperature  );
	}
	else if( nmea_protocol.get() == XCVARIO ) {
		OV.sendNMEA( P_XCVARIO, lb, baroP, dynamicP, te_vario.get(), OAT.get(), ias.get(), tas, MC.get(), bugs.get(), ballast.get(), Switch::getCruiseState(), altitude.get(), gflags.validTemperature,
				-accelG[2], accelG[1],accelG[0], gyroDPS.x, gyroDPS.y, gyroDPS.z );
	}
	else if( nmea_protocol.get() == NMEA_OFF ) {
		;
	}
	else
		ESP_LOGE(FNAME,"Protocol %d not supported error", nmea_protocol.get() );
	xSemaphoreGive(xMutex);
}


void clientLoop(void *pvParameters)
{
	int ccount = 0;
	gflags.validTemperature = true;
	while (true)
	{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		ccount++;
		aTE += (te_vario.get() - aTE)* (1/(10*vario_av_delay.get()));
		if( !(ccount%5) )
		{
			double tmpalt = altitude.get(); // get pressure from altitude
			if( (fl_auto_transition.get() == 1) && ((int)( Units::meters2FL( altitude.get() )) + (int)(gflags.standard_setting) > transition_alt.get() ) ) {
				ESP_LOGI(FNAME,"Above transition altitude");
				baroP = baroSensor->calcPressure(1013.25, tmpalt); // above transition altitude
			}
			else {
				baroP = baroSensor->calcPressure( QNH.get(), tmpalt);
			}
			dynamicP = Atmosphere::kmh2pascal(ias.get());
			tas = Atmosphere::TAS2( ias.get(), altitude.get(), OAT.get() );

			if( gflags.haveMPU ) {
				grabMPU();
			}
			if( accelG[0] > gload_pos_max.get() ){
				gload_pos_max.set( (float)accelG[0] );
			}else if( accelG[0] < gload_neg_max.get() ){
				gload_neg_max.set(  (float)accelG[0] );
			}
			toyFeed();
			Router::routeXCV();
			if( true && !(ccount%5) ) { // todo need a mag_hdm.valid() flag
				if( compass_nmea_hdm.get() ) {
					xSemaphoreTake( xMutex, portMAX_DELAY );
					OV.sendNmeaHDM( mag_hdm.get() );
					xSemaphoreGive( xMutex );
				}

				if( compass_nmea_hdt.get() ) {
					xSemaphoreTake( xMutex, portMAX_DELAY );
					OV.sendNmeaHDT( mag_hdt.get() );
					xSemaphoreGive( xMutex );
				}
			}
			lazyNvsCommit();
			esp_task_wdt_reset();
			if( uxTaskGetStackHighWaterMark( bpid ) < 512 )
				ESP_LOGW(FNAME,"Warning client task stack low: %d bytes", uxTaskGetStackHighWaterMark( bpid ) );
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
	}
}

void readSensors(void *pvParameters){
	int client_sync_dataIdx = 0;
	while (1)
	{
		count++;
		TickType_t xLastWakeTime = xTaskGetTickCount();
		if( gflags.haveMPU  )  // 3th Generation HW, MPU6050 avail and feature enabled
		{
			grabMPU();
		}
		bool ok=false;
		float p = 0;
		if( asSensor )
			p = asSensor->readPascal(60, ok);
		if( ok )
			dynamicP = p;
		float iasraw = Atmosphere::pascal2kmh( dynamicP );
		// ESP_LOGI("FNAME","P: %f  IAS:%f", dynamicP, iasraw );
		float T=OAT.get();
		if( !gflags.validTemperature ) {
			T= 15 - ( (altitude.get()/100) * 0.65 );
			// ESP_LOGW(FNAME,"T invalid, using 15 deg");
		}
		float tasraw = 0;
		if( baroP != 0 )
			tasraw =  Atmosphere::TAS( iasraw , baroP, T);  // True airspeed

		static float new_ias = 0;
		new_ias = ias.get() + (iasraw - ias.get())*0.25;
		if( (int( ias.get()+0.5 ) != int( new_ias+0.5 ) ) || !(count%20) ){
			ias.set( new_ias );  // low pass filter
		}
		if( airspeed_max.get() < ias.get() ){
			airspeed_max.set( ias.get() );
		}
		// ESP_LOGI("FNAME","P: %f  IAS:%f IASF: %d", dynamicP, iasraw, ias );
		if( !compass || !(compass->externalData()) ){
			tas += (tasraw-tas)*0.25;       // low pass filter
		}
		// ESP_LOGI(FNAME,"IAS=%f, T=%f, TAS=%f baroP=%f", ias, T, tas, baroP );

		// Slip angle estimation
		float as = tas/3.6;                  // tas in m/s
		const float K = 4000 * 180/M_PI;      // airplane constant and Ay correction factor
		if( tas > 25.0 ){
			slipAngle += ((accelG[1]*K / (as*as)) - slipAngle)*0.09;   // with atan(x) = x for small x
			// ESP_LOGI(FNAME,"AS: %f m/s, CURSL: %f°, SLIP: %f", as, -accelG[1]*K / (as*as), slipAngle );
		}
		xSemaphoreTake(xMutex,portMAX_DELAY );

		float te = bmpVario.readTE( tasraw );
		if( (int( te_vario.get()*20 +0.5 ) != int( te*20 +0.5)) || !(count%10) ){  // a bit more fine granular updates than 0.1 m/s as of sound
			te_vario.set( te );  // max 10x per second
		}
		xSemaphoreGive(xMutex);
		// ESP_LOGI(FNAME,"count %d ccp %d", count, ccp );
		if( !(count % ccp) ) {
			AverageVario::recalcAvgClimb();
		}
		if (FLAP) { FLAP->progress(); }
		xSemaphoreTake(xMutex,portMAX_DELAY );
		baroP = baroSensor->readPressure(ok);   // 10x per second
		xSemaphoreGive(xMutex);
		// ESP_LOGI(FNAME,"Baro Pressure: %4.3f", baroP );
		float altSTD = 0;
		if( Flarm::validExtAlt() && alt_select.get() == AS_EXTERNAL )
			altSTD = alt_external;
		else
			altSTD = baroSensor->calcAVGAltitudeSTD( baroP );
		float new_alt = 0;
		if( alt_select.get() == AS_TE_SENSOR ) // TE
			new_alt = bmpVario.readAVGalt();
		else if( alt_select.get() == AS_BARO_SENSOR  || alt_select.get() == AS_EXTERNAL ){ // Baro or external
			if(  alt_unit.get() == ALT_UNIT_FL ) { // FL, always standard
				new_alt = altSTD;
				gflags.standard_setting = true;
				// ESP_LOGI(FNAME,"au: %d", alt_unit.get() );
			}else if( (fl_auto_transition.get() == 1) && ((int)Units::meters2FL( altSTD ) + (int)(gflags.standard_setting) > transition_alt.get() ) ) { // above transition altitude
				new_alt = altSTD;
				gflags.standard_setting = true;
				ESP_LOGI(FNAME,"auto:%d alts:%f ss:%d ta:%f", fl_auto_transition.get(), altSTD, gflags.standard_setting, transition_alt.get() );
			}
			else {
				if( Flarm::validExtAlt() && alt_select.get() == AS_EXTERNAL )
					new_alt = altSTD + ( QNH.get()- 1013.25)*8.2296;  // correct altitude according to ISA model = 27ft / hPa
				else
					new_alt = baroSensor->calcAVGAltitude( QNH.get(), baroP );
				gflags.standard_setting = false;
				// ESP_LOGI(FNAME,"QNH %f baro: %f alt: %f SS:%d", QNH.get(), baroP, alt, gflags.standard_setting  );
			}
		}
		if( (int( new_alt +0.5 ) != int( altitude.get() +0.5 )) || !(count%20) ){
			// ESP_LOGI(FNAME,"New Altitude: %.1f", new_alt );
			altitude.set( new_alt );
		}

		aTE = bmpVario.readAVGTE();
		doAudio();

		if( !Flarm::bincom && ((count % 2) == 0 ) ){
			toyFeed();
			vTaskDelay(2/portTICK_PERIOD_MS);
		}
		Router::routeXCV();
		// ESP_LOGI(FNAME,"Compass, have sensor=%d  hdm=%d ena=%d", compass->haveSensor(),  compass_nmea_hdt.get(),  compass_enable.get() );
		if( compass ){
			if( !Flarm::bincom && ! compass->calibrationIsRunning() ) {
				// Trigger heading reading and low pass filtering. That job must be
				// done periodically.
				bool ok;
				float heading = compass->getGyroHeading( &ok );
				if(ok){
					if( (int)heading != (int)mag_hdm.get() && !(count%10) ){
						mag_hdm.set( heading );
					}
					if( !(count%5) && compass_nmea_hdm.get() == true ) {
						xSemaphoreTake( xMutex, portMAX_DELAY );
						OV.sendNmeaHDM( heading );
						xSemaphoreGive( xMutex );
					}
				}
				else{
					if( mag_hdm.get() != -1 )
						mag_hdm.set( -1 );
				}
				float theading = compass->filteredTrueHeading( &ok );
				if(ok){
					if( (int)theading != (int)mag_hdt.get() && !(count%10) ){
						mag_hdt.set( theading );
					}
					if( !(count%5) && ( compass_nmea_hdt.get() == true )  ) {
						xSemaphoreTake( xMutex, portMAX_DELAY );
						OV.sendNmeaHDT( theading );
						xSemaphoreGive( xMutex );
					}
				}
				else{
					if( mag_hdt.get() != -1 )
						mag_hdt.set( -1 );
				}
			}
		}
		if( accelG[0] > gload_pos_max.get() ){
			gload_pos_max.set( (float)accelG[0] );
		}else if( accelG[0] < gload_neg_max.get() ){
			gload_neg_max.set(  (float)accelG[0] );
		}

		// Check on new clients connecting
		if ( CAN && CAN->GotNewClient() ) { // todo use also for Wifi client?!
			while( client_sync_dataIdx < SetupCommon::numEntries() ) {
				if ( SetupCommon::syncEntry(client_sync_dataIdx++) ) {
					break; // Hit entry to actually sync and send data
				}
			}
			if ( client_sync_dataIdx >= SetupCommon::numEntries() ) {
				// Synch complete
				client_sync_dataIdx = 0;
				CAN->ResetNewClient();
			}
		}
		lazyNvsCommit();

		// MPU temperature PI control
		// if( gflags.haveMPU && !CAN->hasSlopeSupport() ){ // series 2023 does not have slope support on CAN bus but MPU temperature control
		// 	mpu_temp_control();
		// }
		esp_task_wdt_reset();
		if( uxTaskGetStackHighWaterMark( bpid ) < 512 )
			ESP_LOGW(FNAME,"Warning sensor task stack low: %d bytes", uxTaskGetStackHighWaterMark( bpid ) );
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}

static int ttick = 0;
static float temp_prev = -3000;

void readTemp(void *pvParameters){

	while (1) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		float t=15.0;
		battery = Battery.get();
		// ESP_LOGI(FNAME,"Battery=%f V", battery );
		if( !SetupCommon::isClient() ) {  // client Vario will get Temperature info from main Vario
			t = ds18b20.getTemp();
			if( t ==  DEVICE_DISCONNECTED_C ) {
				if( gflags.validTemperature == true ) {
					ESP_LOGI(FNAME,"Temperatur Sensor disconnected, please plug Temperature Sensor");
					gflags.validTemperature = false;
				}
			}
			else
			{
				if( gflags.validTemperature == false ) {
					ESP_LOGI(FNAME,"Temperatur Sensor connected, temperature valid");
					gflags.validTemperature = true;
				}
				// ESP_LOGI(FNAME,"temperature=%2.1f", temperature );
				temperature +=  (t - temperature) * 0.3; // A bit low pass as strategy against toggling
				temperature = std::round(temperature*10)/10;
				if( temperature != temp_prev ){
					OAT.set( temperature );
					ESP_LOGI(FNAME,"NEW temperature=%2.1f, prev T=%2.1f", temperature, temp_prev );
					temp_prev = temperature;
				}
			}
			ESP_LOGV(FNAME,"temperature=%f", temperature );
			Flarm::tick();
			if( compass )
				compass->tick();
		}else{
			if( (OAT.get() > -55.0) && (OAT.get() < 85.0) )
				gflags.validTemperature = true;
		}
		theWind.tick();
		CircleWind::tick();
		Flarm::progress();
		vTaskDelayUntil(&xLastWakeTime, 1000/portTICK_PERIOD_MS);
		esp_task_wdt_reset();
		if( (ttick++ % 50) == 0) {
			ESP_LOGI(FNAME,"Free Heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT) );
			if( uxTaskGetStackHighWaterMark( tpid ) < 256 )
				ESP_LOGW(FNAME,"Warning temperature task stack low: %d bytes", uxTaskGetStackHighWaterMark( tpid ) );
			if( heap_caps_get_free_size(MALLOC_CAP_8BIT) < 20000 )
				ESP_LOGW(FNAME,"Warning heap_caps_get_free_size getting low: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));
		}
	}
}

static esp_err_t _coredump_to_server_begin_cb(void * priv)
{
	ets_printf("================= CORE DUMP START =================\r\n");
	return ESP_OK;
}

static esp_err_t _coredump_to_server_end_cb(void * priv)
{
	ets_printf("================= CORE DUMP END ===================\r\n");
	return ESP_OK;
}

static esp_err_t _coredump_to_server_write_cb(void * priv, char const * const str)
{
	ets_printf("%s\r\n", str);
	return ESP_OK;
}

void register_coredump() {
	coredump_to_server_config_t coredump_cfg = {
			.start = _coredump_to_server_begin_cb,
			.end = _coredump_to_server_end_cb,
			.write = _coredump_to_server_write_cb,
			.priv = NULL,
	};
	if( coredump_to_server(&coredump_cfg) != ESP_OK ){  // Dump to console and do not clear (will done after fetched from Webserver)
		ESP_LOGI( FNAME, "+++ All green, no coredump found in FLASH +++");
	}
}


// Sensor board init method. Herein all functions that make the XCVario are launched and tested.
void system_startup(void *args){
	accelG[0] = 1;  // earth gravity default = 1 g
	accelG[1] = 0;
	accelG[2] = 0;
	gyroDPS.x = 0;
	gyroDPS.y = 0;
	gyroDPS.z = 0;
	cur_gyro_bias[0] = 0;
	cur_gyro_bias[1] = 0;
	cur_gyro_bias[2] = 0;

	bool selftestPassed=true;
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
	ESP_LOGI( FNAME, "Log level set globally to INFO %d; Max Prio: %d Wifi: %d",  ESP_LOG_INFO, configMAX_PRIORITIES, ESP_TASKD_EVENT_PRIO-5 );
	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	ESP_LOGI( FNAME,"This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
			chip_info.cores,
			(chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
					(chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");
	ESP_LOGI( FNAME,"Silicon revision %d, ", chip_info.revision);
	ESP_LOGI( FNAME,"%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
			(chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");
	ESP_LOGI(FNAME, "QNH.get() %.1f hPa", QNH.get() );
	NVS.begin();
	register_coredump();
	Polars::begin();

	the_can_mode = can_mode.get(); // initialize variable for CAN mode
	if( hardwareRevision.get() != 2 ){
		gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);     // 2020 series 1, analog in default
		gpio_pullup_en( GPIO_NUM_2 );
	}
	if( display_orientation.get() ){
		ESP_LOGI( FNAME, "TopDown display mode flag set");
		topDown = true;
	}

	wireless = (e_wireless_type)(wireless_type.get()); // we cannot change this on the fly, so get that on boot
	AverageVario::begin();
	stall_alarm_off_kmh = stall_speed.get()/3;
	if( Cipher::checkKeyAHRS() ){
		ESP_LOGI( FNAME, "AHRS key valid=%d", gflags.ahrsKeyValid );
	}else{
		ESP_LOGI( FNAME, "AHRS key invalid=%d, disable AHRS Sensor", gflags.ahrsKeyValid );
		if( attitude_indicator.get() )
			attitude_indicator.set(0);
	}
	Battery.begin();  // for battery voltage
	xMutex=xSemaphoreCreateMutex();
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ccp = (int)(core_climb_period.get()*10);
	SPI.begin( SPI_SCLK, SPI_MISO, SPI_MOSI, CS_bme280BA );
	xSemaphoreGive(spiMutex);

	egl = new AdaptUGC();
	egl->begin();
	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ESP_LOGI( FNAME, "setColor" );
	egl->setColor( 0, 255, 0 );
	ESP_LOGI( FNAME, "drawLine" );
	egl->drawLine( 20,20, 20,80 );
	ESP_LOGI( FNAME, "finish Draw" );
	xSemaphoreGive(spiMutex);

	MYUCG = egl; // new AdaptUGC( SPI_DC, CS_Display, RESET_Display );
	display = new IpsDisplay( MYUCG );
	Flarm::setDisplay( MYUCG );
	DM.begin( MYUCG );
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
		ota->begin();
		ota->doSoftwareUpdate( display );
	}
	esp_err_t err=ESP_ERR_NOT_FOUND;
	MPU.setBus(i2c);  // set communication bus, for SPI -> pass 'hspi'
	MPU.setAddr(mpud::MPU_I2CADDRESS_AD0_LOW);  // set address or handle, for SPI -> pass 'mpu_spi_handle'
	err = MPU.reset();
	ESP_LOGI( FNAME,"MPU Probing returned %d MPU enable: %d ", err, attitude_indicator.get() );
	if( err == ESP_OK ){
		hardwareRevision.set(3);  // wow, there is MPU6050 gyro and acceleration sensor
		gflags.haveMPU = true;
		ESP_LOGI( FNAME,"MPU initialize");
		MPU.initialize();  // this will initialize the chip and set default configurations
		MPU.setSampleRate(50);  // in (Hz)
		MPU.setAccelFullScale(mpud::ACCEL_FS_8G);
		MPU.setGyroFullScale( GYRO_FS );
		MPU.setDigitalLowPassFilter(mpud::DLPF_5HZ);  // smoother data
		mpud::raw_axes_t gb = gyro_bias.get();
		mpud::raw_axes_t ab = accl_bias.get();
		char ahrs[30];
		if( (gb.isZero() || ab.isZero()) || ahrs_autozero.get() ) {
			ESP_LOGI( FNAME,"MPU computeOffsets");
			ahrs_autozero.set(0);
			MPU.computeOffsets( &ab, &gb );  // returns Offsets in 16G scale
			gyro_bias.set( gb );
			accl_bias.set( ab );
			MPU.setGyroOffset(gb);
			MPU.setAccelOffset(ab);
			ESP_LOGI( FNAME,"MPU new offsets accl:%d/%d/%d gyro:%d/%d/%d ZERO:%d", ab.x, ab.y, ab.z, gb.x,gb.y,gb.z, gb.isZero() );
			if( hardwareRevision.get() != 3 )
				hardwareRevision.set(3);
		}else{
			MPU.setAccelOffset(ab);
			MPU.setGyroOffset(gb);
		}
		delay( 50 );
		mpud::raw_axes_t accelRaw;
		float accel = 0;
		for( auto i=0; i<11; i++ ){
			esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
			if( err != ESP_OK )
				ESP_LOGE(FNAME, "AHRS acceleration I2C read error");
			accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);  // raw data to gravity
			ESP_LOGI( FNAME,"MPU %.2f", accelG[0] );
			delay( 5 );
			if( i>0 )
				accel += accelG[0];
		}
		sprintf( ahrs,"AHRS Sensor: OK (%.2f g)", accel/10 );
		display->writeText( line++, ahrs );
		logged_tests += "MPU6050 AHRS test: PASSED\n";
		IMU::init();
		IMU::read();
		ESP_LOGI( FNAME,"MPU current offsets accl:%d/%d/%d gyro:%d/%d/%d ZERO:%d", ab.x, ab.y, ab.z, gb.x,gb.y,gb.z, gb.isZero() );
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
	if( wireless == WL_BLUETOOTH ) {
		wireless_id="Bluetooth ID: ";
		btsender.begin();
	}
	else
		wireless_id="WLAN SID: ";
	wireless_id += SetupCommon::getID();
	display->writeText(line++, wireless_id.c_str() );

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
		ias.set( Atmosphere::pascal2kmh( dynamicP ) );
		ESP_LOGI(FNAME,"Aispeed sensor current speed=%f", ias.get() );
		if( !offset_plausible && ( ias.get() < 50 ) ){
			ESP_LOGE(FNAME,"Error: air speed presure sensor offset out of bounds, act value=%d", offset );
			display->writeText( line++, "AS Sensor: NEED ZERO" );
			logged_tests += "AS Sensor offset test: FAILED\n";
			selftestPassed = false;
		}
		else {
			ESP_LOGI(FNAME,"air speed offset test PASSED, readout value in bounds=%d", offset );
			char s[40];
			if( ias.get() > 50 ) {
				sprintf(s, "AS Sensor: %d km/h", (int)(ias.get()+0.5) );
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
	if( !SetupCommon::isClient()  ) {
		ESP_LOGI(FNAME,"Now start T sensor test");
		ds18b20.begin();
		temperature = ds18b20.getTemp();
		ESP_LOGI(FNAME,"End T sensor test");
		if( temperature == DEVICE_DISCONNECTED_C ) {
			ESP_LOGE(FNAME,"Error: Self test Temperatur Sensor failed; returned T=%2.2f", temperature );
			display->writeText( line++, "Temp Sensor: NOT FOUND");
			gflags.validTemperature = false;
			logged_tests += "External Temperature Sensor: NOT FOUND\n";
		}else
		{
			ESP_LOGI(FNAME,"Self test Temperatur Sensor PASSED; returned T=%2.2f", temperature );
			display->writeText( line++, "Temp Sensor: OK");
			gflags.validTemperature = true;
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
		gpio_set_pull_mode(CS_bme280BA, GPIO_PULLUP_ONLY );
		gpio_set_pull_mode(CS_bme280TE, GPIO_PULLUP_ONLY );

	}
	bool tetest=true;
	bool batest=true;
	delay(200);

	if( !baroSensor->selfTest( ba_t, ba_p)  ) {
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
		if( (abs(ba_t - te_t) >4.0)  && ( ias.get() < 50 ) ) {   // each sensor has deviations, and new PCB has more heat sources
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
		float delta = 2.5; // in factory we test at normal temperature, so temperature change is ignored.
		if( abs(factory_volt_adjust.get() - 0.00815) < 0.00001 )
			delta += 1.8; // plus 1.5 Pa per Kelvin, for 60K T range = 90 Pa or 0.9 hPa per Sensor, for both there is 2.5 plus 1.8 hPa to consider
		if( (abs(ba_p - te_p) >delta)  && ( ias.get() < 50 ) ) {
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

	// 2021 series 3, or 2022 model with new digital poti CAT5171 also features CAN bus
	String resultCAN;
	if( Audio::haveCAT5171() ) // todo && CAN configured
	{
		CAN = new CANbus();
		if( CAN->selfTest(false) ){  // series 2023 has fixed slope control, prio slope bit for AHRS temperature control
			resultCAN = "OK-";
			ESP_LOGE(FNAME,"CAN Bus selftest (no RS): OK");
			logged_tests += "CAN Interface: OK\n";
		}
		else{
			if( CAN->selfTest(true) ){  // if slope bit is to be handled, there is no temperature control
				resultCAN = "OK";
				ESP_LOGE(FNAME,"CAN Bus selftest RS: OK");
				logged_tests += "CAN Interface: OK\n";
			}
			else{
				resultCAN = "FAIL";
				logged_tests += "CAN Bus selftest: FAILED\n";
				ESP_LOGE(FNAME,"Error: CAN Interface failed");
			}
		}
	}

	float bat = Battery.get(true);
	if( bat < 1 || bat > 28.0 ){
		ESP_LOGE(FNAME,"Error: Battery voltage metering out of bounds, act value=%f", bat );
		if( resultCAN.length() )
			display->writeText( line++, "Bat Meter/CAN: ");
		else
			display->writeText( line++, "Bat Meter/CAN: Fail/" + resultCAN );
		logged_tests += "Battery Voltage Sensor: FAILED\n";
		selftestPassed = false;
	}
	else{
		ESP_LOGI(FNAME,"Battery voltage metering test PASSED, act value=%f", bat );
		if( resultCAN.length() )
			display->writeText( line++, "Bat Meter/CAN: OK/"+ resultCAN );
		else
			display->writeText( line++, "Bat Meter: OK");
		logged_tests += "Battery Voltage Sensor: PASSED\n";
	}

	Serial::begin();
	// Factory test for serial interface plus cable
	String result("Serial ");
	if( Serial::selfTest( 1 ) )
		result += "S1 OK";
	else
		result += "S1 FAIL";
	if( (hardwareRevision.get() >= 3) && serial2_speed.get() ){
		if( Serial::selfTest( 2 ) )
			result += ",S2 OK";
		else
			result += ",S2 FAIL";
	}
	if( abs(factory_volt_adjust.get() - 0.00815) < 0.00001 ){
		display->writeText( line++, result.c_str() );
	}
	Serial::taskStart();

	if( wireless == WL_BLUETOOTH ) {
		if( btsender.selfTest() ){
			display->writeText( line++, "Bluetooth: OK");
			logged_tests += "Bluetooth test: PASSED\n";
		}
		else{
			display->writeText( line++, "Bluetooth: FAILED");
			logged_tests += "Bluetooth test: FAILED\n";
		}
	}else if ( wireless == WL_WLAN_MASTER || wireless == WL_WLAN_STANDALONE ){
		WifiApp::wifi_init_softap();
	}
	// 2021 series 3, or 2022 model with new digital poti CAT5171 also features CAN bus
	if(  can_speed.get() != CAN_SPEED_OFF && (resultCAN == "OK" || resultCAN == "OK-") && CAN )
	{
		ESP_LOGI(FNAME, "Now start CAN Bus Interface");
		CAN->begin();  // start CAN tasks and driver
	}

	if( compass_enable.get() == CS_CAN ){
		ESP_LOGI( FNAME, "Magnetic sensor type CAN");
		compass = new Compass( 0 );  // I2C addr 0 -> instantiate without I2C bus and local sensor
	}
	else if( compass_enable.get() == CS_I2C ){
		ESP_LOGI( FNAME, "Magnetic sensor type I2C");
		compass = new Compass( 0x0D, ODR_50Hz, RANGE_2GAUSS, OSR_512, &i2c_0 );
	}
	// magnetic sensor / compass selftest
	if( compass ) {
		compass->begin();
		ESP_LOGI( FNAME, "Magnetic sensor enabled: initialize");
		err = compass->selfTest();
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
		compass->start();  // start task
	}

	Speed2Fly.begin();
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
	Menu->begin( display, baroSensor, &Battery );

	if ( wireless == WL_WLAN_CLIENT || the_can_mode == CAN_MODE_CLIENT ){
		ESP_LOGI(FNAME,"Client Mode");
	}
	else if( ias.get() < 50.0 ){
		ESP_LOGI(FNAME,"Master Mode: QNH Autosetup, IAS=%3f (<50 km/h)", ias.get() );
		// QNH autosetup
		float ae = elevation.get();
		float qnh_best = QNH.get();
		bool ok;
		baroP = baroSensor->readPressure(ok);
		if( ae > 0 ) {
			float step=10.0; // 80 m
			float min=1000.0;
			for( float qnh = 870; qnh< 1085; qnh+=step ) {
				float alt = 0;
				if( Flarm::validExtAlt() && alt_select.get() == AS_EXTERNAL )
					alt = alt_external + (qnh  - 1013.25) * 8.2296;  // correct altitude according to ISA model = 27ft / hPa
				else
					alt = baroSensor->readAltitude( qnh, ok);
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
				delay(50);
			}
			ESP_LOGI(FNAME,"Auto QNH=%4.2f\n", qnh_best);
			QNH.set( qnh_best );
		}
		display->clear();
		if( abs(factory_volt_adjust.get() - 0.00815) < 0.00001 ){
			ESP_LOGI(FNAME,"Do Factory Voltmeter adj");
			SetupMenuValFloat::showMenu( 0.0, SetupMenuValFloat::meter_adj_menu );
		}else{
			SetupMenuValFloat::showMenu( qnh_best, SetupMenuValFloat::qnh_menu );
		}
	}
	else
	{
		gflags.inSetup = false;
		display->clear();
	}

	if ( flap_enable.get() ) {
		Flap::init(MYUCG);
	}


	if( hardwareRevision.get() == 2 ){
		Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);
	}
	else {
		Rotary.begin( GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_0);
		gpio_pullup_en( GPIO_NUM_34 );
	}
	delay( 100 );
	if ( SetupCommon::isClient() ){
		if( wireless == WL_WLAN_CLIENT ){
			display->clear();

			int line=1;
			display->writeText( line++, "Wait for WiFi Master" );
			char mxcv[30] = "";
			if( master_xcvario.get() != 0 ){
				sprintf( mxcv+strlen(mxcv), "XCVario-%d", (int) master_xcvario.get() );
				display->writeText( line++, mxcv );
			}
			line++;
			std::string ssid = WifiClient::scan( master_xcvario.get() );
			if( ssid.length() ){
				display->writeText( line++, "Master XCVario found" );
				char id[30];
				sprintf( id, "Wifi ID: %s", ssid.c_str() );
				display->writeText( line++, id );
				display->writeText( line++, "Now start, sync" );
				WifiClient::start();
				delay( 5000 );
				gflags.inSetup = false;
				display->clear();
			}
			else{
				display->writeText( 3, "Abort Wifi Scan" );
			}
		}
		else if( the_can_mode == CAN_MODE_CLIENT ){
			display->clear();
			display->writeText( 1, "Wait for CAN Master" );
			while( 1 ) {
				if( CAN->connectedXCV() ){
					display->writeText( 3, "Master XCVario found" );
					display->writeText( 4, "Now start, sync" );
					delay( 5000 );
					gflags.inSetup = false;
					display->clear();
					break;
				}
				delay( 100 );
				if( Rotary.readSwitch() ){
					display->writeText( 3, "Abort CAN bus wait" );
					break;
				}
			}
		}
	}
	if( screen_centeraid.get() ){
		centeraid = new CenterAid( MYUCG );
	}
	if( SetupCommon::isClient() ){
		xTaskCreatePinnedToCore(&clientLoop, "clientLoop", 4096, NULL, 11, &bpid, 0);
		xTaskCreatePinnedToCore(&audioTask, "audioTask", 4096, NULL, 11, &apid, 0);
	}
	else {
		xTaskCreatePinnedToCore(&readSensors, "readSensors", 5120, NULL, 11, &bpid, 0);
	}
	xTaskCreatePinnedToCore(&readTemp, "readTemp", 3000, NULL, 5, &tpid, 0);       // increase stack by 500 byte
	xTaskCreatePinnedToCore(&drawDisplay, "drawDisplay", 6144, NULL, 4, &dpid, 0); // increase stack by 1K

	Audio::startAudio();
}

extern "C" void  app_main(void)
{
	// Init timer infrastructure
	esp_timer_init();

	Audio::boot();
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

	system_startup( 0 );
	vTaskDelete( NULL );
}
