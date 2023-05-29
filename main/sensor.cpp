
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

#include "UbloxGNSSdecode.h"

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
#include "MPU.hpp"

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

#define DegToRad (M_PI / 180)

MCP3221 *MCP=0;
DS18B20  ds18b20( GPIO_NUM_23 );  // GPIO_NUM_23 standard, alternative  GPIO_NUM_17

AirspeedSensor *asSensor=0;
StraightWind theWind;

xSemaphoreHandle xMutex=NULL;
xSemaphoreHandle spiMutex=NULL;

S2F Speed2Fly;
Protocols OV( &Speed2Fly );

AnalogInput Battery( (22.0+1.2)/1200, ADC_ATTEN_DB_0, ADC_CHANNEL_7, ADC_UNIT_1 );

// Fligth Test
TaskHandle_t mpid = NULL;
// Fligth Test

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

#define MAXDRIFT 2                // °/s maximum drift that is automatically compensated on ground
#define NUM_GYRO_SAMPLES 3000     // 10 per second -> 5 minutes, so T has been settled after power on

// Fligth Test
float deltaGyroModule = 0.0;	// gyro module alfa/beta filter for gyro stability test
float GyroModulePrimFilt = 0.0;
float GyroModuleFilt = 0.0;
float GyroModulePrimLevel = 0.0;
float deltaAccelModule = 0.0;	// accel module alfa/beta filter for gyro stability test
float AccelModulePrimFilt = 0.0;
float AccelModuleFilt = 9.807;
float AccelModulePrimLevel = 0.0;


// Magnetic sensor / compass
Compass *compass = 0;
BTSender btsender;

// IMU variables	
mpud::float_axes_t accelISUNEDMPU;
mpud::float_axes_t accelISUNEDBODY;	
mpud::float_axes_t gyroRPS;
mpud::float_axes_t gyroISUNEDMPU;
mpud::float_axes_t gyroISUNEDBODY;
static float AccelGravModuleFilt = 9.807;
static int32_t gyrobiastemptimer = 0;
static float integralFBx = 0.0;
static float integralFBy = 0.0;
static float integralFBz = 0.0;
static float BankFilt = 0.0;
static float alternategzBias = 0.0;
static float Pitch = 0.0;
static float Roll = 0.0;
static float Yaw = 0.0;
static float q0 = 1.0;
static float q1 = 0.0;
static float q2 = 0.0;
static float q3 = 0.0;
static float free_Pitch = 0.0;
static float free_Roll = 0.0;
static float free_Yaw = 0.0;
static float free_q0 = 1.0;
static float free_q1 = 0.0;
static float free_q2 = 0.0;
static float free_q3 = 0.0;
static float free_halfex = 0.0;
static float delta_errx = 0.0;
static float filt_errx = 0.0;
static float errx_prim = 0.0;
static float free_halfey = 0.0;
static float delta_erry = 0.0;
static float filt_erry = 0.0;
static float erry_prim = 0.0;
static float free_halfez = 0.0;
static float delta_errz = 0.0;
static float filt_errz = 0.0;
static float errz_prim = 0.0;
static float BiasQuatGx = 0.0;
static float BiasQuatGy = 0.0;
static float BiasQuatGz = 0.0;
static float Kgain = 1.0;
static float cosRoll = 1.0;
static float sinRoll = 0.0;
static float cosPitch = 1.0;
static float sinPitch = 0.0;
static float Vh = 0.0;
static float DHeading = 0.0;
static float cosDHeading = 1.0;
static float sinDHeading = 0.0;
static float Up = 0.0;
static float Vp = 0.0;
static float Wp = 0.0;
static float UiPrim = 0.0;
static float ViPrim = 0.0;
static float WiPrim = 0.0;
// variables for gravity estimation
mpud::float_axes_t GravIMU;	
float deltaUiPrim = 0.0;
float UiPrimPrim = 0.0;
float UiPrimFilt = 0.0;
float deltaViPrim = 0.0;
float ViPrimPrim = 0.0;
float ViPrimFilt = 0.0;
float deltaWiPrim = 0.0;
float WiPrimPrim = 0.0;
float WiPrimFilt = 0.0;
float UpPrimFilt = 0.0;
float VpPrimFilt = 0.0;
float WpPrimFilt = 0.0;

// installation and calibration variables
mpud::float_axes_t currentAccelBias;
mpud::float_axes_t currentAccelGain;
mpud::float_axes_t currentGyroBias;
// get installation parameters tilt, sway, distCG
// compute trigonometry
float DistCGVario = 0.0; // distance from CG to vario
float Sway = 0.0; // vario installation roll angle
float Tilt = 0.0; // vario installation pitch angle
float S_S = 0.0; // sinus Sway
float S_T = 0.0; // sinus Tilt
float C_S = 0.0; // cos Sway
float C_T = 0.0; // cos tilt	
float STmultSS = 0.0; // ST * SS
float STmultCS = 0.0; // ST * CS
float SSmultCT = 0.0; // SS * CT
float CTmultCS = 0.0; // CT * CS


static char str[500]; 	// string for flight test message broadcast on wireless // TODO reduce size
static int64_t ProcessTimeIMU = 0.0;
static int64_t ProcessTimeSensors = 0.0;
static int64_t gyroTime;  // time stamp for gyros
static int64_t prevgyroTime;
static float dtGyr = 0.025; // period between last gyro samples
static int64_t statTime; // time stamp for statP
static int64_t prevstatTime;
static float dtstat = 0.1;
static float statP=0; // raw static pressure
static int64_t teTime; // time stamp for teP
static float teP=0; // raw te pressure
static int64_t dynPTime;
static int64_t prevdynPTime;
static float dtdynP = 0.1;
static float dynP=0; // raw dynamic pressure
static float OATemp = 15; // OAT for pressure corrections (real or from standard atmosphere) 
static float MPUtempcel; // MPU chip temperature
static float GNSSRouteraw;
static float GNSSRoutePrim = 0.0;
static float GNSSRoute = 0.0;

static int32_t cur_gyro_bias[3];

bool IMUstream = false; // IMU FT stream
bool SENstream = false; // Sensors FT stream
bool CALstream = false; // accel calibration stream
float localGravity = 9.807; // local gravity used during accel calibration. Value is entered using BT $CAL command
uint16_t BIAS_Init = 0; // Bias initialization status (0= no init, n = nth bias calculation
bool BIASInFLASH = false; // New BIAS stored in FLASH

static float GRAVITY = 9.807;

static float dynamicP; // filtered dynamic pressure
static float baroP=0; // barometric pressure
static float temperature=15.0;
static float XCVTemp=15.0;//External temperature for MPU temp control

static float Rho;
static float CASraw;
static float deltaCAS;
static float CASprim;
static float CAS = 0.0;
static float Rhocorr;
static float TAS;
static float TASprim;
static float ALTraw;
static float deltaALT;
static float Vzbaro = 0.0;
static float ALT = 0.0;
#define NCAS 7 // CAS alpha/beta filter coeff
#define alphaCAS (2.0 * (2.0 * NCAS - 1.0) / NCAS / (NCAS + 1))
#define betaCAS (6.0 / NCAS / (NCAS + 1) / 0.1)
#define NALT 7 // ALT alpha/beta coeff
#define alphaALT (2.0 * (2.0 * NALT - 1.0) / NALT / (NALT + 1))
#define betaALT (6.0 / NALT / (NALT + 1) / 0.1)
#define RhoSLISA 1.225

mpud::float_axes_t Vbi;
mpud::float_axes_t VbiPrim;
static float deltaUp = 0.0;
static float UpPrim = 0.0;
static float UpFilt = 0.0;
static float deltaVp = 0.0;
static float VpPrim = 0.0;
static float VpFilt = 0.0;	
static float deltaWp = 0.0;
static float WpPrim = 0.0;
static float WpFilt = 0.0;	
static float Vzbi = 0.0;
static float VziPrim = 0.0;
static float Vztotbiraw = 0.0;
static float deltaVztot = 0.0;
static float Vztot = 0.0;
static float VztotPrim = 0.0;
#define NVztot 7 // CAS alpha/beta filter coeff
#define alphaVztot (2.0 * (2.0 * NVztot - 1.0) / NVztot / (NVztot + 1.0))
#define betaVztot (6.0 / NVztot / (NVztot + 1.0) / 0.1)
#define NVelAcc 7 // pneumatic velocity variation alpha/beta filter coeff
#define alphaVelAcc (2.0 * (2.0 * NVelAcc - 1.0) / NVelAcc / (NVelAcc + 1.0))
#define betaVelAcc (6.0 / NVelAcc / (NVelAcc + 1.0) / 0.1)






static float battery=0.0;

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
uint8_t gyro_flash_savings=0;

t_global_flags gflags = { true, false, false, false, false, false, false, false, false, false, false, false, false, false };

int  ccp=60;
float tas = 0;
float cas = 0;
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

float mpu_target_temp=45.0;

static int mtick = 0; //counter to schedule specific tasks within a function*

AdaptUGC *egl = 0;

// Fligth Test
extern UbloxGnssDecoder s1UbloxGnssDecoder;
extern UbloxGnssDecoder s2UbloxGnssDecoder;


#define GYRO_FS (mpud::GYRO_FS_250DPS)

float getTAS() { return tas; };

bool do_factory_reset() {
	return( SetupCommon::factoryReset() );
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
			else if( airspeed_mode.get() == MODE_CAS )
				airspeed = cas;
			else
				airspeed = ias.get();

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
					int gw = 0;
					if( gear_warning.get() == GW_EXTERNAL ){
						gw = gflags.gear_warn_external;
					}else{
						gw = digitalRead( SetupMenu::getGearWarningIO() );
						if( gear_warning.get() == GW_FLAP_SENSOR_INV || gear_warning.get() == GW_S2_RS232_RX_INV ){
							gw = !gw;
						}
					}
					if( gw ){
						if( ESPRotary::readSwitch() ){   // Acknowledge Warning -> Warning OFF
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
// modif gfm affichage d'une tension batterie nulle tant que les biais gyros n'ont pas été initialisés
				if (BIAS_Init > 0){
					display->drawDisplay( airspeed, Vztot /*te_vario.get()*/, aTE, polar_sink, altitude.get(), t, battery, s2f_delta, as2f, average_climb.get(), Switch::getCruiseState(), gflags.standard_setting, flap_pos.get() );
				}	
				else {
					display->drawDisplay( airspeed, Vztot /*te_vario.get()*/, aTE, polar_sink, altitude.get(), t, 0.0, s2f_delta, as2f, average_climb.get(), Switch::getCruiseState(), gflags.standard_setting, flap_pos.get() );
				}
// fin modif gfm
				}
			if( screen_centeraid.get() ){
				if( centeraid ){
					centeraid->tick();
				}
			}
		}
		if( flarm_alarm_holdtime )
			flarm_alarm_holdtime--;
//		drawDisplayTime = (esp_timer_get_time()/1000.0) - drawDisplayTime;
//		ESP_LOGI(FNAME,"drawDisplay: %0.1f  / %0.1f", drawDisplayTime, 20.0 );
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
//		audioTaskTime = (esp_timer_get_time()/1000.0) - audioTaskTime;
//		ESP_LOGI(FNAME,"audioTask: %0.1f  / %0.1f", audioTaskTime, 100.0 );
		if( uxTaskGetStackHighWaterMark( apid )  < 512 )
			ESP_LOGW(FNAME,"Warning audio task stack low: %d", uxTaskGetStackHighWaterMark( apid ) );
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}

void MahonyUpdateIMU(float dt, float gxraw, float gyraw, float gzraw,
					float ax, float ay, float az, 
					float &Bias_Gx, float &Bias_Gy, float &Bias_Gz ) {

#define Nbias 2000 // very long period for extracting error rate of change between IMU quaternion and free quaternion
#define alphaBias (2.0 * (2.0 * Nbias - 1.0) / Nbias / (Nbias + 1.0))
#define betaBias (6.0 / Nbias / (Nbias + 1.0) / 0.025)
#define Kbias 2 // gain to apply to error rate to be homogeneous to gyro bias. experimental, TODO need to be adjusted
#define fcGrav 3.0 // 3Hz low pass to filter for testing stability criteria
#define fcgrav1 (40.0/(40.0+fcGrav))
#define fcgrav2 (1.0-fcgrav1)
#define Nlimit 0.15 // stability criteria for gravity estimation from accels in m/s²
#define Kp 2.5 // proportional feedback to sync quaternion
#define Ki 0.15 // integral feedback to sync quaternion

#define WingLevel 0.085 // max lateral gravity acceleration (normalized acceleration) to consider wings are ~ < 5° bank

#define NGz 1200// Very long period alpha/beta for Gz bias estimation. Sample rate is 40 hz / period 0.025 second. When GNSS is avilable, GNSS route variation is used to improve bias estimation.
#define alphaGz (2.0 * (2.0 * NGz - 1.0) / NGz / (NGz + 1.0))
#define betaGz (6.0 / NGz / (NGz + 1.0) / 0.025)

float gx, gy, gz;
float AccelGravModule, QuatModule, recipNorm;
float halfvx, halfvy, halfvz, halfex, halfey, halfez, qa, qb, qc, free_halfvx, free_halfvy, free_halfvz;
float GravityModuleErr, dynKp, dynKi;
float deltaBiasGz;
float GzPrim = 0.0;


	
	// To estimate gyro Bias:
	// - compute error between vertical vector from IMU quaternion and free quaternion
	// - estimate bias by computing error rate of change on each axis using long term alpha/beta filter
	//
	// Estimate direction of gravity from IMU quaternion
	halfvx = q1 * q3 - q0 * q2;
	halfvy = q0 * q1 + q2 * q3;
	halfvz = q0 * q0 - 0.5 + q3 * q3;
	// Estimate direction of vertical from free quaternion
	free_halfvx = free_q1 * free_q3 - free_q0 * free_q2;
	free_halfvy = free_q0 * free_q1 + free_q2 * free_q3;
	free_halfvz = free_q0 * free_q0 - 0.5 + free_q3 * free_q3;
	// compute error between IMU and free quaternions vertical
	free_halfex = free_halfvz * halfvy - free_halfvy * halfvz;
	free_halfey = free_halfvx * halfvz - free_halfvz * halfvx;
	free_halfez = free_halfvy * halfvx - free_halfvx * halfvy;
	// compute error rate of change for 3 axes using alpha/beta filters
	delta_errx = free_halfex - filt_errx;
	errx_prim = errx_prim + betaBias * delta_errx;
	filt_errx = filt_errx + alphaBias * delta_errx + errx_prim * dt;
	delta_erry = free_halfey - filt_erry;
	erry_prim = erry_prim + betaBias * delta_erry;
	filt_erry = filt_erry + alphaBias * delta_erry + erry_prim * dt;
	delta_errz = free_halfez - filt_errz;
	errz_prim = errz_prim + betaBias * delta_errz;
	filt_errz = filt_errz + alphaBias * delta_errz + errz_prim * dt;
	// Apply experimental gain to error to be homogeneous to gyro biases
	Bias_Gx = Kbias * errx_prim;
	Bias_Gy = Kbias * erry_prim;
	
	// gyro are corrected using accelerometers only when module of accelerometer is close from local gravity (GRAVITY)
	// correction coefficients are lower when acceleration module moves away from GRAVITY
	AccelGravModule = sqrt( ax * ax + ay * ay + az * az );
	// asymetrical low pass filter with immediate rise and slow decay
	if ( AccelGravModuleFilt < AccelGravModule ) {
		AccelGravModuleFilt = AccelGravModule;
	} else {
		AccelGravModuleFilt = fcgrav1 * AccelGravModuleFilt + fcgrav2 * AccelGravModule;
	}
	// compute acceleration module difference with local gravity (GRAVITY) to allow alternate Gz bias estimation and dynamic correction of IMU quaternion
	// Nlimit is the coefficient to statidfy staibility criteria
	GravityModuleErr = Nlimit - abs(AccelGravModuleFilt-GRAVITY);
	if ( GravityModuleErr < -4.0 ) GravityModuleErr = -4.0;

	// When bank is low , considering average Gz should be ~0 when wings are close to tbe leveld
	BankFilt = fcgrav1 * BankFilt + fcgrav2 * 2.0 * halfvy;	// low pass filter on estimated Bank to reduce noise
	if ( abs(BankFilt) < WingLevel  ) {
        deltaBiasGz = (gzraw - GNSSRoutePrim) - Bias_Gz;
		GzPrim = GzPrim + betaGz * deltaBiasGz;
        Bias_Gz = Bias_Gz + alphaGz * deltaBiasGz + GzPrim * dt;
	} else {
		Bias_Gz = gzraw;
	}
	
	// Update free quaternion by integrating rate of change
	gx = gxraw * 0.5 * dt;
	gy = gyraw * 0.5 * dt;
	gz = gzraw * 0.5 * dt;
	qa = free_q0;
	qb = free_q1;
	qc = free_q2;
	free_q0 +=(-qb * gx - qc * gy - free_q3 * gz);
	free_q1 += (qa * gx + qc * gz - free_q3 * gy);
	free_q2 += (qa * gy - qb * gz + free_q3 * gx);
	free_q3 += (qa * gz + qb * gy - qc * gx);
	// Normalise raw gyro quaternion
	QuatModule = sqrt(free_q0 * free_q0 + free_q1 * free_q1 + free_q2 * free_q2 + free_q3 * free_q3);
	if ( QuatModule != 0.0) {	
		recipNorm = 1.0 / QuatModule;	
		free_q0 *= recipNorm;
		free_q1 *= recipNorm;
		free_q2 *= recipNorm;
		free_q3 *= recipNorm;
	}	

	// Update IMU quaternion
	// IMU quaternion is updated by integrating rate of change (gyro)
	// gyros are corrected using estimated bias and estimaded error with vertical from accelerometer (gravity)
	//
	// correct raw gyro with estimated gyro bias
	gx = gxraw + Bias_Gx;
	gy = gyraw + Bias_Gy;
	gz = gzraw + Bias_Gz;	
	// Compute feedback error between gravity and quaternion only if accelerometer measurement valid (avoids NaN in accelerometer normalisation)
	if ( AccelGravModule != 0.0) {
		// gyro are corrected using accelerometers only when module of accelerometer is close to local gravity (GRAVITY)
		// compute dynamic correction coefficients dynKp and dynKi in function of acceleration gravitity module
		// when acceleration module to GRAVITY error is lower than Nlimit, Kp and Ki coefficients are used
		// when acceleration module to GRAVITY error is higher than Nlimit, reduce Kp and Ki coefficients are used. TODO adjust Kp and Ki
		if ( GravityModuleErr > 0.0 ) {
			dynKp = Kp;
			dynKi = Ki;
		} else {
			Kgain = pow( 10.0, GravityModuleErr * 1.5 );
			dynKp = Kgain * Kp;
			dynKi = Kgain * Ki;
		}		
		// Normalise accelerometer measurement
		recipNorm = 1.0 / AccelGravModule;
		ax *=recipNorm;
		ay *=recipNorm;
		az *=recipNorm;
		// Estimate error between vertical from IMU quaternion and vertical from accels. Error is sum of cross product between Quaternion and accels gravity estimations
		halfex = (ay * halfvz - az * halfvy);
		halfey = (az * halfvx - ax * halfvz);
		halfez = (ax * halfvy - ay * halfvx);
		// integral feedback
		integralFBx = integralFBx + dynKi * halfex * dt;
		integralFBy = integralFBy + dynKi * halfey * dt;
		integralFBz = integralFBz + dynKi * halfez * dt;
		// apply integral feedback to gyros
		gx = gx + integralFBx; 
		gy = gy + integralFBy;
		gz = gz + integralFBz;
		// Apply proportional feedback to gyros
		gx = gx + dynKp * halfex;
		gy = gy + dynKp * halfey;
		gz = gz + dynKp * halfez;
	}

	// Integrate rate of change of IMU quaternion
	gx = gx * 0.5 * dt;
	gy = gy * 0.5 * dt;
	gz = gz * 0.5 * dt;
	qa = q0;
	qb = q1;
	qc = q2;
	q0 +=(-qb * gx - qc * gy - q3 * gz);
	q1 += (qa * gx + qc * gz - q3 * gy);
	q2 += (qa * gy - qb * gz + q3 * gx);
	q3 += (qa * gz + qb * gy - qc * gx);

	// Normalise IMU quaternion
	QuatModule = sqrt(q0 * q0 + q1 * q1 + q2 * q2 + q3 * q3);
	if ( QuatModule != 0.0) {	
		recipNorm = 1.0 / QuatModule;	
		q0 *= recipNorm;
		q1 *= recipNorm;
		q2 *= recipNorm;
		q3 *= recipNorm;
	}	
}

static void processIMU(void *pvParameters)
{
// processIMU process reads all sensors information required for processing total energy calculation, including attitude estimation
// it is also able to stream flight test data to BT which is used in post processing to validate algorithms.
// This process gets following information:
// - MPU, converts accels and gyros to NED frame in International System Units : m/s² for accels and rad/s for rotation rates.
// - Sensors data, static, TE, dynamic pressure, OAT and MPU temp
// - Ublox GNSS data. 

	// MPU data
	#define NAccel 7 // accel alpha/beta filter coeff
	#define NGyro 7 // gyro alpha/beta coeff
	#define alfaAccelModule (2.0 * (2.0 * NAccel - 1.0) / NAccel / (NAccel + 1))
	#define betaAccelModule (6.0 / NAccel / (NAccel + 1) / 0.025)
	#define alfaGyroModule (2.0 * (2.0 * NGyro - 1.0) / NGyro / (NGyro + 1))
	#define betaGyroModule (6.0 / NGyro / (NGyro + 1) / 0.025)
	#define fcAccelLevel 3.0 // 3Hz low pass to filter 
	#define fcAL1 (40.0/(40.0+fcAccelLevel))
	#define fcAL2 (1.0-fcAL1)
	#define fcGyroLevel 3.0 // 3Hz low pass to filter 
	#define fcGL1 (40.0/(40.0+fcGyroLevel))
	#define fcGL2 (1.0-fcGL1)
	#define GroundAccelprimlimit 0.9 // m/s²
	#define	GroundGyroprimlimit 0.3 // rad/s²
	#define PeriodAcc 5 // period in second for Kinetic acceleration and speed complementary filter
	#define fcAcc1 (40.0/(40.0+ (1/PeriodAcc)))
	#define fcAcc2 (1.0-fcAcc1)	

	mpud::raw_axes_t accelRaw;
	mpud::raw_axes_t gyroRaw;
	
	#define NAccelPrim 7	// ~6 Hz alpha/beta filter coeff for accel derivative estimation
	#define alphaAcc (2.0 * (2.0 * NAccelPrim - 1.0) / NAccelPrim / (NAccelPrim + 1.0))
	#define betaAcc (6.0 / NAccelPrim / (NAccelPrim + 1.0) / 0.025)	

	// variables for accel calibration
	float accelMaxx = 0.0;
	float accelMaxy = 0.0;
	float accelMaxz = 0.0;
	float accelMinx = 0.0;
	float accelMiny = 0.0;
	float accelMinz = 0.0;
	float accelAvgx = 0.0;	
	float accelAvgy = 0.0;	
	float accelAvgz = 0.0;	
	int16_t gyromodulestable = 0;

	GravIMU.x = 0.0;
	GravIMU.y = 0.0;
	GravIMU.z = 0.0;
	float UiPrimPrimFilt = 0.0;
	float ViPrimPrimFilt = 0.0;
	float WiPrimPrimFilt = 0.0;	
	
	// variables for bias estimation
	int16_t gyrostable = 0;
	int16_t averagecount = 0;
	float GxBias = 0.0;
	float GyBias = 0.0;
	float GzBias = 0.0;
	// variables for gravity estimation
	float Gravx = 0.0;
	float Gravy = 0.0;
	float Gravz = 0.0;	
	// variables for Euler/quaternion initialisation
	float PitchInit = 0.0;
	float RollInit = 0.0;
	float YawInit = 0.0;
	
		
	mpud::float_axes_t gravISUNEDBODY;

	// TODO estimation of gyro gain
	float Module = 9.807;
	
	while (1) {

		TickType_t xLastWakeTime_mpu =xTaskGetTickCount();
		
		// get gyro data
		if( MPU.rotation(&gyroRaw) == ESP_OK ){
			prevgyroTime = gyroTime;
			gyroTime = esp_timer_get_time()/1000.0; // record time of gyro measurement in milli second
			dtGyr = (gyroTime - prevgyroTime) / 1000.0; // period between last two valid samples in second
			gyroDPS = mpud::gyroDegPerSec(gyroRaw, GYRO_FS); // For compatibility with Eckhard code only. Convert raw gyro to Gyro_FS full scale in degre per second 
			gyroRPS = mpud::gyroRadPerSec(gyroRaw, GYRO_FS); // convert raw gyro to Gyro_FS full scale
			// convert gyro coordinates to ISU : rad/s NED MPU and remove bias
			gyroISUNEDMPU.x = -(gyroRPS.z - currentGyroBias.z);
			gyroISUNEDMPU.y = -(gyroRPS.y - currentGyroBias.y);
			gyroISUNEDMPU.z = -(gyroRPS.x - currentGyroBias.x);
			// convert NEDMPU to NEDBODY
			gyroISUNEDBODY.x = C_T * gyroISUNEDMPU.x + STmultSS * gyroISUNEDMPU.y + STmultCS * gyroISUNEDMPU.z;
			gyroISUNEDBODY.y = C_S * gyroISUNEDMPU.y - S_S * gyroISUNEDMPU.z;
			gyroISUNEDBODY.z = -S_T * gyroISUNEDMPU.x + SSmultCT  * gyroISUNEDMPU.y + CTmultCS * gyroISUNEDMPU.z;
			// filter gyro module with alfa/beta filter
			deltaGyroModule =  sqrt( gyroISUNEDBODY.x * gyroISUNEDBODY.x + gyroISUNEDBODY.y * gyroISUNEDBODY.y + gyroISUNEDBODY.z * gyroISUNEDBODY.z ) - GyroModuleFilt;
			GyroModulePrimFilt = GyroModulePrimFilt + betaGyroModule * deltaGyroModule;
			GyroModuleFilt = GyroModuleFilt + alfaGyroModule * deltaGyroModule + GyroModulePrimFilt * dtGyr;
			if ( GyroModulePrimLevel < abs(GyroModulePrimFilt) ) {
				GyroModulePrimLevel = abs(GyroModulePrimFilt);
			} else {
				GyroModulePrimLevel = fcGL1 * GyroModulePrimLevel +  fcGL2 * abs(GyroModulePrimFilt);
			}			
		}
		// get accel data
		if( MPU.acceleration(&accelRaw) == ESP_OK ){
			accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);  // For compatibility with Eckhard code only. Convert raw data to to 8G full scale
			// convert accels coordinates to ISU : m/s² NED MPU
			accelISUNEDMPU.x = ((-accelG.z*9.807) - currentAccelBias.x ) * currentAccelGain.x;
			accelISUNEDMPU.y = ((-accelG.y*9.807) - currentAccelBias.y ) * currentAccelGain.y;
			accelISUNEDMPU.z = ((-accelG.x*9.807) - currentAccelBias.z ) * currentAccelGain.z;
			// convert from MPU to BODY
			accelISUNEDBODY.x = C_T * accelISUNEDMPU.x + STmultSS * accelISUNEDMPU.y + STmultCS * accelISUNEDMPU.z - ( gyroISUNEDBODY.y * gyroISUNEDBODY.y + gyroISUNEDBODY.z * gyroISUNEDBODY.z ) * DistCGVario;
			accelISUNEDBODY.y = C_S * accelISUNEDMPU.y - S_S * accelISUNEDMPU.z;
			accelISUNEDBODY.z = -S_T * accelISUNEDMPU.x + SSmultCT * accelISUNEDMPU.y + CTmultCS * accelISUNEDMPU.z;
			// filter acceleration module with alfa/beta filter
			Module = sqrt( accelISUNEDBODY.x * accelISUNEDBODY.x + accelISUNEDBODY.y * accelISUNEDBODY.y + accelISUNEDBODY.z * accelISUNEDBODY.z );
			deltaAccelModule =  Module - AccelModuleFilt;
			AccelModulePrimFilt = AccelModulePrimFilt + betaAccelModule * deltaAccelModule;
			AccelModuleFilt = AccelModuleFilt + alfaAccelModule * deltaAccelModule + AccelModulePrimFilt * dtGyr;			
			if ( AccelModulePrimLevel < abs(AccelModulePrimFilt) ) {
				AccelModulePrimLevel = abs(AccelModulePrimFilt);
			} else {
				AccelModulePrimLevel = fcAL1 * AccelModulePrimLevel +  fcAL2 * abs(AccelModulePrimFilt);
			}
		}		

		// if moving, speed > 10 m/s or ground bias estimation has ran more than "10" times TODO when operational BIAS_Init should be up to 10.
		if (TAS > 10.0  || BIAS_Init > 10 ) {
			// first time in movement, if biais initialiazation was achieved, store bias and local gravity in FLASH
			if ( !BIASInFLASH && BIAS_Init > 1 ) {
				gyro_bias.set(currentGyroBias);
				gravity.set(GRAVITY);
				BIASInFLASH = true;
			}

			if ( TAS > 10.0 ) {
			// estimate gravity in body frame taking into account centrifugal corrections
				gravISUNEDBODY.x = accelISUNEDBODY.x - gyroISUNEDBODY.y * Vbi.z + gyroISUNEDBODY.z * Vbi.y;
				gravISUNEDBODY.y = accelISUNEDBODY.y - gyroISUNEDBODY.z * Vbi.x + gyroISUNEDBODY.x * Vbi.z;
				gravISUNEDBODY.z = accelISUNEDBODY.z + gyroISUNEDBODY.y * Vbi.x - gyroISUNEDBODY.x * Vbi.y;
			} else {
				// estimate gravity in body frame using accels only
				gravISUNEDBODY.x = accelISUNEDBODY.x;
				gravISUNEDBODY.y = accelISUNEDBODY.y;
				gravISUNEDBODY.z = accelISUNEDBODY.z;
			}

			// Update IMU quaternion
			// gyroISUNEDBODY corresponds to raw gyro and BiasQuatGx,y,z to the gyros bias
			MahonyUpdateIMU( dtGyr, gyroISUNEDBODY.x, gyroISUNEDBODY.y, gyroISUNEDBODY.z,
							-gravISUNEDBODY.x, -gravISUNEDBODY.y, -gravISUNEDBODY.z,
							BiasQuatGx, BiasQuatGy, BiasQuatGz );
							
			// Euler angles from IMU quaternion
			if ( abs(q1 * q3 - q0 * q2) < 0.5 ) {
				Pitch = asin(-2.0 * (q1 * q3 - q0 * q2));
			} else {
				Pitch = M_PI / 2.0 * signbit((q0 * q2 - q1 * q3 ));
			}
			Roll = atan2((q0 * q1 + q2 * q3), (0.5 - q1 * q1 - q2 * q2));
			Yaw = atan2(2.0 * (q1 * q2 + q0 * q3), (q0 * q0 + q1 * q1 - q2 * q2 - q3 * q3));
			if (Yaw < 0.0 ) Yaw = Yaw + 2.0 * M_PI;
			if (Yaw > 2.0 * M_PI) Yaw = Yaw - 2.0 * M_PI;
			
			// Euler angles from free drifting quaternion
			if ( abs(free_q1 * free_q3 - free_q0 * free_q2) < 0.5 ) {
				free_Pitch = asin(-2.0 * (free_q1 * free_q3 - free_q0 * free_q2));
			} else {
				free_Pitch = M_PI / 2.0 * signbit((free_q0 * free_q2 - free_q1 * free_q3 ));
			}
			free_Roll = atan2((free_q0 * free_q1 + free_q2 * free_q3), (0.5 - free_q1 * free_q1 - free_q2 * free_q2));
			free_Yaw = atan2(2.0 * (free_q1 * free_q2 + free_q0 * free_q3), (free_q0 * free_q0 + free_q1 * free_q1 - free_q2 * free_q2 - free_q3 * free_q3));
			if (free_Yaw < 0.0 ) free_Yaw = free_Yaw + 2.0 * M_PI;
			if (free_Yaw > 2.0 * M_PI) free_Yaw = free_Yaw - 2.0 * M_PI;

			// compute sin and cos for Roll and Pitch from IMU quaternion since theyr are used in multiple calculations
			cosRoll = cos( Roll );
			sinRoll = sin( Roll );
			cosPitch = cos( Pitch );
			sinPitch = sin( Pitch );

			// compute kinetic acceleration from accels, gravity from IMU and centrifugal forces
			GravIMU.x = GRAVITY * 2.0 * (q1 * q3 - q0 * q2);
			GravIMU.y = -GRAVITY * 2.0 * (q2 * q3 + q0 * q1);
			GravIMU.z = -GRAVITY * (q0 * q0 - q1 * q1 - q2 * q2 + q3 * q3);
			UiPrim = accelISUNEDBODY.x - GravIMU.x - gyroISUNEDBODY.y * Vbi.z + gyroISUNEDBODY.z * Vbi.y;
			ViPrim = accelISUNEDBODY.y - GravIMU.y - gyroISUNEDBODY.z * Vbi.x + gyroISUNEDBODY.x * Vbi.z;			
			WiPrim = accelISUNEDBODY.z - GravIMU.z + gyroISUNEDBODY.y * Vbi.x - gyroISUNEDBODY.x * Vbi.y;

			// compute UiPrim, ViPrim and WiPrim derivative with alpha beta filter
			deltaUiPrim = UiPrim - UiPrimFilt;
			UiPrimPrim = UiPrimPrim + betaAcc * deltaUiPrim;
			UiPrimFilt = UiPrimFilt + alphaAcc * deltaUiPrim + UiPrimPrim * dtGyr;
			deltaViPrim = ViPrim - ViPrimFilt;
			ViPrimPrim = ViPrimPrim + betaAcc * deltaViPrim;
			ViPrimFilt = ViPrimFilt + alphaAcc * deltaViPrim + ViPrimPrim * dtGyr;
			deltaWiPrim = WiPrim - WiPrimFilt;
			WiPrimPrim = WiPrimPrim + betaAcc * deltaWiPrim;
			WiPrimFilt = WiPrimFilt + alphaAcc * deltaWiPrim + WiPrimPrim * dtGyr;
			// Small Low Pass  on UiPrim, ViPrim and WiPrim derivatives to reduce noise. TODO check if LP is necessary. alpha/beta + LP ~3 Hz response
			UiPrimPrimFilt = 0.5 * UiPrimPrimFilt + 0.5 * UiPrimPrim;
			ViPrimPrimFilt = 0.5 * ViPrimPrimFilt + 0.5 * ViPrimPrim;
			WiPrimPrimFilt = 0.5 * WiPrimPrimFilt + 0.5 * WiPrimPrim;			
			// compute baro internial acceleration with complementary filter
			VbiPrim.x = fcAcc1 * ( VbiPrim.x + UiPrimPrimFilt * dtGyr ) + fcAcc2 * UpPrimFilt;
			VbiPrim.y = fcAcc1 * ( VbiPrim.y + ViPrimPrimFilt * dtGyr ) + fcAcc2 * VpPrimFilt;
			VbiPrim.z = fcAcc1 * ( VbiPrim.z + WiPrimPrimFilt * dtGyr ) + fcAcc2 * WpPrimFilt;
			// compute baro inertial speed with complementary filter
			Vbi.x = fcAcc1 * ( Vbi.x + UiPrimFilt * dtGyr ) + fcAcc2 * UpFilt;
			Vbi.y = fcAcc1 * ( Vbi.y + ViPrimFilt * dtGyr ) + fcAcc2 * VpFilt;
			Vbi.z = fcAcc1 * ( Vbi.z + WiPrimFilt * dtGyr ) + fcAcc2 * WpFilt;
			// compute inertial vertical acceleration in earth frame
			VziPrim =sinPitch * UiPrimFilt + sinRoll * cosPitch * ViPrimFilt + cosRoll * cosPitch * WiPrimFilt;
			// compute baro inertial vertical speed  with complementary filter
			Vzbi = fcAcc1 * (Vzbi + VziPrim * dtGyr) + fcAcc2 * Vzbaro;
				
		} else {
			// Not moving
			// When there is MPU temperature control and temperature is locked   or   when there is no temperature control
			if ( (HAS_MPU_TEMP_CONTROL && (MPU.getSiliconTempStatus() == MPU_T_LOCKED)) || !HAS_MPU_TEMP_CONTROL ) {
				// count cycles when temperature is locked
				gyrobiastemptimer++;
				// detect if gyro and accel variations is below stability threshold using an alpha/beta filter to estimate variation over short period of time
				// if temperature conditions has been stable for more than 30 seconds (1200 = 30x40hz) but less than 20 minutes and there is very little angular and acceleration variation
				if ( (gyrobiastemptimer > 1200) && (GyroModulePrimLevel < GroundGyroprimlimit) && (AccelModulePrimLevel < GroundAccelprimlimit) ) {
					gyrostable++;
					// during first 2.5 seconds, initialize gyro data
					if ( gyrostable < 100 ) {
						GxBias = gyroRPS.x;
						GyBias = gyroRPS.y;
						GzBias = gyroRPS.z;
						Gravx = accelISUNEDBODY.x;
						Gravy = accelISUNEDBODY.y;
						Gravz = accelISUNEDBODY.z;
						RollInit = atan(accelISUNEDBODY.y / accelISUNEDBODY.z);
						PitchInit = asin(accelISUNEDBODY.x/Module);						
						averagecount = 1;
						
					} else {
						// between 2.5 seconds and 22.5 seconds, accumulate gyro data
						if ( gyrostable <900 ) {
							averagecount++;
							GxBias += gyroRPS.x;
							GyBias += gyroRPS.y;
							GzBias += gyroRPS.z;
							Gravx += accelISUNEDBODY.x;
							Gravy += accelISUNEDBODY.y;
							Gravz += accelISUNEDBODY.z;
							RollInit += atan(accelISUNEDBODY.y / accelISUNEDBODY.z);
							PitchInit += asin(accelISUNEDBODY.x/Module);
						} else {
							// If not bias yet, after 25 seconds calculate average bias, gravity and roll/pitch
							// If already have bias, calulate after 2 minutes to avoid risk of perturbation before takeoff
							if ( (BIAS_Init == 0 && gyrostable > 1000) || (BIAS_Init > 0 && gyrostable > 4800) ) {
								currentGyroBias.x = GxBias / averagecount;
								currentGyroBias.y = GyBias / averagecount;
								currentGyroBias.z = GzBias / averagecount;
								Gravx /= averagecount;
								Gravy /= averagecount;
								Gravz /= averagecount;
								GRAVITY = sqrt(Gravx*Gravx+Gravy*Gravy+Gravz*Gravz);
								AccelGravModuleFilt = GRAVITY;
								RollInit  /= averagecount;
								PitchInit /= averagecount;
								YawInit   = 0.0;
								// Initialisation du quaternion
								q0=((cos(RollInit/2.0)*cos(PitchInit/2.0)*cos(YawInit/2.0)+sin(RollInit/2.0)*sin(PitchInit/2.0)*sin(YawInit/2.0)));
								q1=((sin(RollInit/2.0)*cos(PitchInit/2.0)*cos(YawInit/2.0)-cos(RollInit/2.0)*sin(PitchInit/2.0)*sin(YawInit/2.0)));
								q2=((cos(RollInit/2.0)*sin(PitchInit/2.0)*cos(YawInit/2.0)+sin(RollInit/2.0)*cos(PitchInit/2.0)*sin(YawInit/2.0)));
								q3=((cos(RollInit/2.0)*cos(PitchInit/2.0)*sin(YawInit/2.0)-sin(RollInit/2.0)*sin(PitchInit/2.0)*cos(YawInit/2.0)));
								free_q0 = q0;
								free_q1 = q1;
								free_q2 = q2;
								free_q3 = q3;
								
								BIAS_Init++;
								if ( BIAS_Init == 1 ) {
									gyro_bias.set(currentGyroBias);
									gravity.set(GRAVITY);
									BIASInFLASH = false;
								}								
								gyrostable = 0;
							}
						} 
					}
				} else {
					gyrostable = 0; // reset gyro stability counter if temperature not stable or movement detected
				}
			} 
		} 			
		
		// If required stream IMU data
		if ( IMUstream  ) {
			/*
			IMU data in ISU and NED orientation
				$I,
				MPU (gyro) time in milli second,
				Acceleration in BODY X-Axis in tenth milli m/s²,
				Acceleration in BODY Y-Axis in tenth milli m/s²,
				Acceleration in BODU Z-Axis in tenth milli m/s²,				
				Rotation BODY X-Axis in hundredth of milli rad/s,
				Rotation BODY Y-Axis in hundredth of milli rad/s,
				Rotation BODY Z-Axis in hundredth of milli rad/s,				
				<CR><LF>	
			*/			
			sprintf(str,"$I,%lld,%i,%i,%i,%i,%i,%i\r\n",
				gyroTime,
				(int32_t)(accelISUNEDBODY.x*10000.0), (int32_t)(accelISUNEDBODY.y*10000.0), (int32_t)(accelISUNEDBODY.z*10000.0),
				(int32_t)(gyroISUNEDBODY.x*100000.0), (int32_t)(gyroISUNEDBODY.y*100000.0),(int32_t)(gyroISUNEDBODY.z*100000.0) );
			Router::sendXCV(str);
		}


		// If required stream accel calibration data
		if ( CALstream && BIAS_Init > 0 ) {
			// If gyro are stable
			if ( GyroModulePrimLevel < GroundGyroprimlimit ) {
				if ( gyromodulestable == 0 ) {
					accelAvgx = -accelG.z*9.807;
					accelAvgy = -accelG.y*9.807;
					accelAvgz = -accelG.x*9.807;
					gyromodulestable = 1;
				} else {
					accelAvgx = 0.95 * accelAvgx + 0.05 * (-accelG.z*9.807);
					accelAvgy = 0.95 * accelAvgy + 0.05 * (-accelG.y*9.807);
					accelAvgz = 0.95 * accelAvgz + 0.05 * (-accelG.x*9.807);
				}					
				// store max and min
				if ( accelAvgx > accelMaxx ) accelMaxx = accelAvgx;
				if ( accelAvgy > accelMaxy ) accelMaxy = accelAvgy;
				if ( accelAvgz > accelMaxz ) accelMaxz = accelAvgz;
				if ( accelAvgx < accelMinx ) accelMinx = accelAvgx;
				if ( accelAvgy < accelMiny ) accelMiny = accelAvgy;
				if ( accelAvgz < accelMinz ) accelMinz = accelAvgz;
			} else {
				gyromodulestable = 0;
			}
			/*
			CAL data
			$CAL,
			Acceleration in X-Axis in m/s²,
			Acceleration max in X-Axis in m/s²,
			Acceleration min in X-Axis in m/s²,				
			Acceleration in Y-Axis in m/s²,
			Acceleration max in Y-Axis in m/s²,
			Acceleration min in Y-Axis in  m/s²,				
			Acceleration in Z-Axis in m/s²,
			Acceleration max in Z-Axis in m/s²,
			Acceleration min in Z-Axis in m/s²,
			Acceleration bias x in m/s²,
			Acceleration bias y in m/s²
			Acceleration bias z in m/s²	
			Acceleration gain x in m/s²,
			Acceleration gain y in m/s²
			Acceleration gain z in m/s²				
			<CR><LF>	
			*/
			if ( gyromodulestable > 0 ) {			
				sprintf(str,"$CAL,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f,%.4f\r\n",
					accelAvgx, accelMaxx, accelMinx, accelAvgy, accelMaxy, accelMiny, accelAvgz, accelMaxz, accelMinz,
					(accelMaxx+accelMinx)/2, (accelMaxy+accelMiny)/2, (accelMaxz+accelMinz)/2,
					localGravity /((accelMaxx-accelMinx)/2), localGravity /((accelMaxy-accelMiny)/2), localGravity/((accelMaxz-accelMinz)/2) );
			} else {
				sprintf(str,"$CAL, ----------\r\n");
			}
			Router::sendXCV(str);
		}		

		ProcessTimeIMU = (esp_timer_get_time()/1000.0) - gyroTime;
		if ( ProcessTimeIMU > 5 ) {
//			ESP_LOGI(FNAME,"processIMU: %i / 25", (int16_t)(ProcessTimeIMU) );
//TODO			sprintf(str,"IMU Process : %i ms\r\n",(int16_t)(ProcessTimeIMU) );
//			Router::sendXCV(str);			
		}		

		mtick++;
		
		vTaskDelayUntil(&xLastWakeTime_mpu, 25/portTICK_PERIOD_MS);  // 25 ms = 40 Hz loop
		if( (mtick % 40) == 0) {  // test stack every second
			if( uxTaskGetStackHighWaterMark( mpid ) < 1024 )
				 ESP_LOGW(FNAME,"Warning MPU and sensor task stack low: %d bytes", uxTaskGetStackHighWaterMark( mpid ) );
		}
	}		
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
			if( airspeed_mode.get() == MODE_CAS )
				cas = Atmosphere::CAS( dynamicP );
			XCVTemp = bmpVario.bmpTemp;
			if( gflags.haveMPU && HAS_MPU_TEMP_CONTROL ){
				MPU.temp_control( ccount,XCVTemp );
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
//		clientLoopTime = (esp_timer_get_time()/1000.0) - clientLoopTime;
//		ESP_LOGI(FNAME,"clientLoop: %0.1f  / %0.1f", clientLoopTime, 100.0 );
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
	}
}

void readSensors(void *pvParameters){
	
	float CL = 0.0;
	float prevCL = 0.0;
	float dAoA = 0.0;
	float AoARaw = 0.0;
	float WingLoad = 40.0;
	float AoA = 0.0;
	float AoB = 0.0;
	float CLA = 5.75; // CLA=2*PI/(1+2/AR) = 5.75 for LS6 5.98 for Ventus 3
	float KAoB = 200; // 200 for LS6  5.98 for Ventus 3
	float KGx = 4.1; // 4.1 for LS6 and 12 for Ventus 3 
	#define FreqAlpha 1.5 // Hz
	#define fcAoA1 (10.0/(10.0+FreqAlpha))
	#define fcAoA2 (1-fcAoA1)
	#define FreqBeta 1.0 // Hz
	#define fcAoB1 (10.0/(10.0+FreqBeta))
	#define fcAoB2 (1-fcAoB1)
	
	float deltaGNSSRoute;	
	#define NGNSS 7 // GNSS alpha/beta. Sample rate is 0 Hz = 0.1 second
	#define alphaGNSSRoute (2.0 * (2.0 * NGNSS - 1.0) / NGNSS / (NGNSS + 1.0))
	#define betaGNSSRoute (6.0 / NGNSS / (NGNSS + 1.0) / 0.1)	
	
	int client_sync_dataIdx = 0;

	while (1)
	{
		count++;

		TickType_t xLastWakeTime = xTaskGetTickCount();
		
		ProcessTimeSensors = (esp_timer_get_time()/1000.0);
		
		// get raw static pressure
		bool ok=false;
		float p = 0;
		p = baroSensor->readPressure(ok);
		if ( ok ) {
			prevstatTime = statTime;
			statTime = esp_timer_get_time()/1000.0; // record static time in milli second
			dtstat = (statTime - prevstatTime) / 1000.0; // period between last two valid static pressure samples in second	
			statP = p;
			// for compatibility with Eckhard code
			baroP = p;
		}
		
		// get raw te pressure
		xSemaphoreTake(xMutex,portMAX_DELAY );
		p = teSensor->readPressure(ok);
		if ( ok ) {
			teTime = esp_timer_get_time()/1000.0; // record TE time in milli second
			teP = p;
			// not sure what is required for compatibility with Eckhard code
		}
		xSemaphoreGive(xMutex);
		
		// get raw dynamic pressure
		if( asSensor )
			p = asSensor->readPascal(0, ok);
		if( ok ) {
			prevdynPTime = dynPTime;
			dynPTime = esp_timer_get_time()/1000.0; // record dynPTimeTE time in milli second		
			dtdynP = (dynPTime - prevdynPTime) / 1000.0; // period between last two valid dynamic pressure samples in second			
			dynP = 0;
			if ( p > 60 ) dynP = p; // TODO decide if a dynP should be aboce certain value to be valid
			// for compatibility with Eckhard code
			dynamicP = 0;
			if ( p > 60 ) dynamicP = p; 
		}
		
		// get XCVTemp
		XCVTemp = bmpVario.bmpTemp;
		OATemp = OAT.get();
		if( !gflags.validTemperature ) {
			OATemp = 15 - ( (altitude.get()/100) * 0.65 );
			ESP_LOGI(FNAME,"OATemp: %0.1f  Altitude %0.1f", OATemp, altitude.get() );
		}
		
		// get MPU temp
		MPUtempcel = MPU.getTemperature();
		
		// get Ublox GNSS data
		// when GNSS receiver is connected to S1 interface
		const gnss_data_t *gnss1 = s1UbloxGnssDecoder.getGNSSData(1);
		// when GNSS receiver is connected to S2 interface
		const gnss_data_t *gnss2 = s2UbloxGnssDecoder.getGNSSData(2);
		// select gnss with better fix
		const gnss_data_t *chosenGnss = (gnss2->fix >= gnss1->fix) ? gnss2 : gnss1;
		GNSSRouteraw = chosenGnss->route;
		// alpha/beta filter on GNSS route to reduce noise and get derivative
		deltaGNSSRoute = GNSSRouteraw - GNSSRoute;
		GNSSRoutePrim = GNSSRoutePrim + betaGNSSRoute * deltaGNSSRoute;
		GNSSRoute = GNSSRoute + alphaGNSSRoute * deltaGNSSRoute + GNSSRoutePrim * 0.1;	//TODO consider changing 0.1 with actual GNSS time difference	

		// compute CAS, ALT and Vzbaro using alpha/beta filters.  TODO consider using atmospher.h functions
		Rho = (100.0 * statP / 287.058 / (273.15 + OATemp));
		CASraw = sqrt(2 * dynP / RhoSLISA);
		deltaCAS = CASraw - CAS;
		CASprim = CASprim + betaCAS * deltaCAS;
		CAS = CAS + alphaCAS * deltaCAS + CASprim * dtdynP;
		Rhocorr = sqrt(RhoSLISA/Rho);
		TAS = Rhocorr * CAS;
		TASprim = Rhocorr * CASprim;
		ALTraw = (1.0 - pow( (statP-(QNH.get()-1013.25)) * 0.000986923 , 0.1902891634 ) ) * (273.15 + OATemp) * 153.846153846;		
		deltaALT = ALTraw - ALT;
		Vzbaro = Vzbaro + betaALT * deltaALT;
		ALT = ALT + alphaALT * deltaALT + Vzbaro * dtstat;

		// compute AoA (Angle of attack) and AoB (Angle od slip)
		WingLoad = gross_weight.get() / polar_wingarea.get();  // should be only computed when pilot change weight settings in XCVario
		if ( CAS > 10 && dynP > 0.0 && accelISUNEDBODY.z != 0.0 ) { // compute AoA and AoB only when speed is above 10 m/s
			CL = accelISUNEDBODY.z * 2 / RhoSLISA * WingLoad / CAS / CAS;
			dAoA = ( CL - prevCL ) / CLA;
			prevCL = CL;
			AoARaw = -(accelISUNEDBODY.x / accelISUNEDBODY.z) + Speed2Fly.cw( CAS ) / Speed2Fly.getN();
			AoA = fcAoA1 * ( AoA + dAoA ) + fcAoA2 * AoARaw ;
			AoB = fcAoB1 * AoB + fcAoB2 * ( KAoB * WingLoad * accelISUNEDBODY.y / dynP - KGx * gyroISUNEDBODY.x / TAS);	
		} else {
			AoA = 0.0;
			AoB = 0.0;
		}
		// Compute trajectory pneumatic speeds components in body frame NEDBODY
		// Vh corresponds to the trajectory horizontal speed and Vzbaro corresponds to the vertical speed in earth frame
		Vh = TAS * cos( Pitch + cosRoll * AoA + sinRoll * AoB );
		// Pitch and Roll correspond to the attitude of the glider and DHeading corresponds to the heading deviation due to the Beta and Alpha.
		DHeading =(AoB * cosRoll - AoA * sinRoll ) / ( cosPitch + AoB * sinPitch * sinRoll + AoA * sinPitch * cosRoll );
		cosDHeading = cos( DHeading );
		sinDHeading = sin( DHeading );
		// applying DCM from earth to body frame (using Pitch, Roll and DHeading Yaw angles) to Vh and Vzbaro trajectory components in earth frame to reproject speed in TE referential onto body axis. 
		Up = cosPitch * cosDHeading * Vh - sinPitch * Vzbaro;
		Vp = ( sinRoll * sinPitch * cosDHeading - cosRoll * sinDHeading ) * Vh + sinRoll * cosPitch * Vzbaro;
		Wp = ( cosRoll * sinPitch * cosDHeading + sinRoll * sinDHeading ) * Vh + cosRoll * cosPitch * Vzbaro;
		// compute acceleration from pneumatic velocities
		deltaUp = Up - UpFilt;
		UpPrim = UpPrim + betaVelAcc * deltaUp;
		UpFilt = UpFilt + alphaVelAcc * deltaUp + UpPrim * dtdynP;
		deltaVp = Vp - VpFilt;
		VpPrim = VpPrim + betaVelAcc * deltaVp;
		VpFilt = VpFilt + alphaVelAcc * deltaVp + VpPrim * dtdynP;
		deltaWp = Wp - WpFilt;
		WpPrim = WpPrim + betaVelAcc * deltaWp;
		WpFilt = WpFilt + alphaVelAcc * deltaWp + WpPrim * dtdynP;
		// Small Low Pass on prim values to reduce noise. TODO check if LP is necessary
		UpPrimFilt = 0.5 * UpPrimFilt + 0.5 * UpPrim;
		VpPrimFilt = 0.5 * VpPrimFilt + 0.5 * VpPrim;
		WpPrimFilt = 0.5 * WpPrimFilt + 0.5 * WpPrim;			
		// compute baro inertial total energy
		Vztotbiraw = Vzbi + ( Vbi.x * VbiPrim.x + Vbi.y * VbiPrim.y + Vbi.z * VbiPrim.z ) / GRAVITY;
		// filter raw total energy
		deltaVztot = Vztotbiraw - Vztot;
		VztotPrim = VztotPrim + betaVztot * deltaVztot;
		Vztot = Vztot + alphaVztot * deltaVztot + VztotPrim * dtdynP;		
		
		if ( SENstream ) {
		/* Sensor data
			$S,			
			static time in milli second,
			static pressure in Pa,
			TE time in milli second,
			TE pressure in deci Pa,
			Dynamic pressure in tenth of Pa,
			Outside Air Temperature in tenth of °C,
			MPU temperature in tenth °C,
			GNSS fix 0 to 5   3=3D   4= 3D diff,
			GNSS number of satelites used, 
			GNSS time in milli second,
			GNSS altitude in centimeter,
			GNSS speed x or north in centimeters/s,
			GNSS speed y or east in centimeters/s,
			GNSS speed z or down in centimeters/s,
			GNSS route in tenth of °,			
			Pitch in milli rad,
			Roll in milli rad,
			Yaw in milli rad,
			Gyro bias x in hundredth of milli rad/s,
			Gyro bias y in hundredth of milli rad/s,
			Gyro bias z in hundredth of milli rad/s,
			Alternate gyro bias z in hundredth of milli rad/s,
			Gravity module from accel in hundredth of milli m/s²,
			Local gravity on ground estimation in hundredth of milli m/s²,
			Number of biases and local gravity on ground estimations,
			Gyro x bias ground estimation,
			Gyro y bias ground estimation,
			Gyro z bias ground estimation,			
			Gyro module variation level in hundredth of milli,
			Accel module variation level in hundredth of milli,
			CAS in cm/s,
			CASprim in cm/s²,
			TAS in cm/s,
			TASprim in cm/s²,
			ALT in cm,
			Vzbaro in cm/s,
			AoA angle in mrad,
			AoB  angle in mrad,
			UiPrim in cm/s²,
			ViPrim in cm/s²,
			WiPrim in cm/s²,
			Vbi.x in cm/s,
			Vbi.y in cm/s,
			Vbi.z in cm/s, 
			Vztot in cm/s
			<CR><LF>		
		*/
	
			sprintf(str,"$S,%lld,%i,%lld,%i,%i,%i,%i,%1d,%2d,%lld,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,,%i,%i,%i%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i,%i\r\n",
				statTime, (int32_t)(statP*100.0), teTime,(int32_t)(teP*100.0), (int16_t)(dynP*10), (int16_t)(OATemp*10.0), (int16_t)(MPUtempcel*10.0), chosenGnss->fix, chosenGnss->numSV,
				(int64_t)(chosenGnss->time*1000.0), (int32_t)(chosenGnss->coordinates.altitude*100), (int16_t)(chosenGnss->speed.x*100), (int16_t)(chosenGnss->speed.y*100), (int16_t)(chosenGnss->speed.z*100), (int16_t)(GNSSRouteraw*10),
				(int32_t)(Pitch*1000.0), (int32_t)(Roll*1000.0), (int32_t)(Yaw*1000.0),(int32_t)(free_Pitch*1000.0), (int32_t)(free_Roll*1000.0), (int32_t)(free_Yaw*1000.0),
				(int32_t)(BiasQuatGx*100000.0), (int32_t)(BiasQuatGy*100000.0), (int32_t)(BiasQuatGz*100000.0), (int32_t)(alternategzBias*100000.0), (int32_t)(AccelGravModuleFilt*100000.0),(int32_t)(GRAVITY*100000.0),(int16_t)BIAS_Init, (int32_t)(-currentGyroBias.z*100000), (int32_t)(-currentGyroBias.y*100000), (int32_t)(-currentGyroBias.x*100000),
				(int32_t)(GyroModulePrimLevel*100000.0), (int32_t)(AccelModulePrimLevel*100000.0),
				(int32_t)(CAS*100), (int32_t)(CASprim*100), (int32_t)(TAS*100), (int32_t)(TASprim*100), (int32_t)(ALT*100), (int32_t)(Vzbaro*100),
				(int32_t)(AoA*1000), (int32_t)(AoB*1000),
				(int32_t)(UiPrim*100), (int32_t)(ViPrim*100), (int32_t)(WiPrim*100), (int32_t)(Vbi.x*100), (int32_t)(Vbi.y*100), (int32_t)(Vbi.z*100),				
				(int32_t)(Vztot*100) );
			Router::sendXCV(str);
		}		
		
		
		//
		// Eckhard code
		//

		float iasraw = Atmosphere::pascal2kmh( dynamicP );

		float T = OATemp;
		float tasraw = 0;
		if( baroP != 0 )
			tasraw =  Atmosphere::TAS( iasraw , baroP, T);  // True airspeed in km/h

		if( airspeed_mode.get() == MODE_CAS ){
			float casraw=Atmosphere::CAS( dynamicP );
			cas += (casraw-cas)*0.25;       // low pass filter
			// ESP_LOGI(FNAME,"IAS=%f, TAS=%f CAS=%f baro=%f", iasraw, tasraw, cas, baroP );
		}
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
		// Flight test
		//xSemaphoreTake(xMutex,portMAX_DELAY );
		//baroP = baroSensor->readPressure(ok);   // 10x per second
		//xSemaphoreGive(xMutex);
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
		XCVTemp = bmpVario.bmpTemp;
		if( gflags.haveMPU && HAS_MPU_TEMP_CONTROL ){
			// ESP_LOGI(FNAME,"MPU temp control; T=%.2f", MPU.getTemperature() );
			MPU.temp_control( count,XCVTemp);
		}

		ProcessTimeSensors = (esp_timer_get_time()/1000.0) - ProcessTimeSensors;
		if ( ProcessTimeSensors > 40 ) {
			ESP_LOGI(FNAME,"readSensors: %i / 100", (int16_t)(ProcessTimeSensors) );
		}	

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
//		readTempTime = (esp_timer_get_time()/1000.0) - readTempTime;
//		ESP_LOGI(FNAME,"readTemp: %0.1f  / %0.1f", readTempTime, 1000.0 );
		vTaskDelayUntil(&xLastWakeTime, 1000/portTICK_PERIOD_MS);
		esp_task_wdt_reset();
		if( (ttick++ % 5) == 0) {
			//ESP_LOGI(FNAME,"Free Heap: %d bytes", heap_caps_get_free_size(MALLOC_CAP_8BIT) );
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
	i2c.begin(GPIO_NUM_21, GPIO_NUM_22, 400000 );
	Router::begin();
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
	if( hardwareRevision.get() == HW_UNKNOWN ){  // per default we assume there is XCV-20
		ESP_LOGI( FNAME, "Hardware Revision unknown, set revision 2 (XCV-20)");
		hardwareRevision.set(XCVARIO_20);
	}

	if( display_orientation.get() ){
		ESP_LOGI( FNAME, "TopDown display mode flag set");
		topDown = true;
	}

	wireless = (e_wireless_type)(wireless_type.get()); // we cannot change this on the fly, so get that on boot
	AverageVario::begin();
	stall_alarm_off_kmh = stall_speed.get()/3;

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
	std::string ver( " Ver.: " );
	ver += V.version();
	char hw[24];
	sprintf( hw,", XCV-%d", hardwareRevision.get()+18);  // plus 18, e.g. 2 = XCVario-20
	std::string hwrev( hw );
	ver += hwrev;
	display->writeText(line++, ver.c_str() );
	sleep(1);
	bool doUpdate = software_update.get();
	if( Rotary.readSwitch() ){
		doUpdate = true;
		ESP_LOGI(FNAME,"Rotary pressed: Do Software Update");
	}
	if( doUpdate ) {
		if( hardwareRevision.get() == XCVARIO_20) { // only XCV-20 uses this GPIO for Rotary
			ESP_LOGI( FNAME,"Hardware Revision detected 2");
			Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);
		}
		else  {
			ESP_LOGI( FNAME,"Hardware Revision detected 3");
			Rotary.begin( GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_0);
			gpio_pullup_en( GPIO_NUM_34 );
			if( gflags.haveMPU && HAS_MPU_TEMP_CONTROL && !gflags.mpu_pwm_initalized  ){ // series 2023 does not have slope support on CAN bus but MPU temperature control
				MPU.pwm_init();
				gflags.mpu_pwm_initalized = true;
			//gpio_set_level(GPIO_NUM_34,0);
			}
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
		if( hardwareRevision.get() < XCVARIO_21 ){
			ESP_LOGI( FNAME,"MPU avail, increase hardware revision to 3 (XCV-21)");
			hardwareRevision.set(XCVARIO_21);  // there is MPU6050 gyro and acceleration sensor, at least we got an XCV-21
		}
		gflags.haveMPU = true;
		mpu_target_temp = mpu_temperature.get();
		ESP_LOGI( FNAME,"MPU initialize");
		MPU.initialize();  // this will initialize the chip and set default configurations
		MPU.setSampleRate(400);  // in (Hz)
		MPU.setAccelFullScale(mpud::ACCEL_FS_8G);
		MPU.setGyroFullScale( GYRO_FS );
		MPU.setDigitalLowPassFilter(mpud::DLPF_42HZ);  // smoother data
		
		// clear gyro and accel MPU offsets, just in case
		mpud::raw_axes_t zeroBias;
		zeroBias.x = 0.0;
		zeroBias.y = 0.0;
		zeroBias.z = 0.0;
		MPU.setGyroOffset(zeroBias);
		MPU.setAccelOffset(zeroBias);
	
		mpud::raw_axes_t accelRaw;
		delay( 50 );

		// check GRAVITY and accels offset and gain just in case FLASH is not correct
		GRAVITY = gravity.get();
		if(abs(GRAVITY-9.807) > 0.5 ) GRAVITY = 9.807;
		currentAccelBias = accl_bias.get();
		currentAccelGain = accl_gain.get();		
		if ( abs(currentAccelBias.x)>1.0 || abs(currentAccelBias.y)>1.0 || abs(currentAccelBias.z)>1.0 || 
			abs(currentAccelGain.x-1.0)>0.1 || abs(currentAccelGain.y-1.0)>0.1 || abs(currentAccelGain.z-1.0)>0.1 ){
				currentAccelBias.x = 0.0;
				currentAccelBias.y = 0.0;
				currentAccelBias.z = 0.0;
				currentAccelGain.x = 1.0;
				currentAccelGain.y = 1.0;
				currentAccelGain.z = 1.0;
		}
		
		// get last known ground gyro biases
		currentGyroBias = gyro_bias.get();
		if ( abs(currentGyroBias.x)>0.1 || abs(currentGyroBias.y)>0.1 || abs(currentGyroBias.z)>0.1 ) {
				currentGyroBias.x = 0.0;
				currentGyroBias.y = 0.0;
				currentGyroBias.z = 0.0;
		}

		// get installation parameters tilt, sway, distCG
		// compute trigonometry
		DistCGVario = distCG.get();
		if ( DistCGVario < 0.0 || DistCGVario > 3.0 ) DistCGVario = 0.0;
		Sway = sway.get();
		Tilt = tilt.get();
		if ( abs(Sway) > 0.4 || abs(Tilt) > 0.4 ) {
			Sway = 0.0;
			Tilt = 0.0;
		}
		S_S = sin(Sway);
		C_S = cos(Sway);
		S_T = sin(Tilt);
		C_T = cos(Tilt);
		STmultSS = S_T * S_S;
		STmultCS = S_T * C_S;
		SSmultCT = S_S * C_T;
		CTmultCS = C_T * C_S;
	
		char ahrs[50];
		float accel = 0;
		for( auto i=0; i<11; i++ ){
			esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
			if( err != ESP_OK )
				ESP_LOGE(FNAME, "AHRS acceleration I2C read error");
			accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_8G);  // raw data to gravity
			ESP_LOGI( FNAME,"MPU %.2f", accelG[0] );
			delay( 5 );
			if( i>0 )
				accel += sqrt(accelG[0]*accelG[0]+accelG[1]*accelG[1]+accelG[2]*accelG[2]);
		}
		sprintf( ahrs,"AHRS Sensor: OK (%.2f g)", accel/10 );
		display->writeText( line++, ahrs );
		logged_tests += "MPU6050 AHRS test: PASSED\n";
		IMU::init();
		IMU::read();
		// TODO not compatible with flight test code( FNAME,"MPU current offsets accl:%d/%d/%d gyro:%d/%d/%d ZERO:%d", ab.x, ab.y, ab.z, gb.x,gb.y,gb.z, gb.isZero() );
	}
	else{
		ESP_LOGI( FNAME,"MPU reset failed, check HW revision: %d",hardwareRevision.get() );
		if( hardwareRevision.get() >= XCVARIO_21 ) {
			ESP_LOGI( FNAME,"hardwareRevision detected = 3, XCVario-21+");
			display->writeText( line++, "AHRS Sensor: NOT FOUND");
			logged_tests += "MPU6050 AHRS test: NOT FOUND\n";
		}
	}
	char id[16] = { 0 };
	strcpy( id, custom_wireless_id.get().id );
	ESP_LOGI(FNAME,"Custom Wirelss-ID from Flash: %s len: %d", id, strlen(id) );
	if( strlen( id ) == 0 ){
		custom_wireless_id.set( SetupCommon::getDefaultID() ); // Default ID created from MAC address CRC
		ESP_LOGI(FNAME,"Empty ID: Initialize empty Wirelss-ID: %s", custom_wireless_id.get().id );
	}
	ESP_LOGI(FNAME,"Custom Wirelss-ID: %s", custom_wireless_id.get().id );

	String wireless_id;
	if( wireless == WL_BLUETOOTH ) {
		wireless_id="BT ID: ";
		btsender.begin();
	}
	else
		wireless_id="WLAN SID: ";
	wireless_id += SetupCommon::getID();
	display->writeText(line++, wireless_id.c_str() );
	Cipher::begin();
	if( Cipher::checkKeyAHRS() ){
		ESP_LOGI( FNAME, "AHRS key valid=%d", gflags.ahrsKeyValid );
	}else{
		ESP_LOGI( FNAME, "AHRS key invalid=%d, disable AHRS Sensor", gflags.ahrsKeyValid );
		if( attitude_indicator.get() )
			attitude_indicator.set(0);
	}

	ESP_LOGI(FNAME,"Airspeed sensor init..  type configured: %d", airspeed_sensor_type.get() );
	int offset;
	bool found = false;
	if( hardwareRevision.get() >= XCVARIO_21 ){ // autodetect new type of sensors
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
			resultCAN = "OK";
			ESP_LOGE(FNAME,"CAN Bus selftest (no RS): OK");
			logged_tests += "CAN Interface: OK\n";
			if( hardwareRevision.get() != XCVARIO_23 ){
				ESP_LOGI(FNAME,"CAN Bus selftest without RS control OK: set hardwareRevision 5 (XCV-23)");
				hardwareRevision.set(XCVARIO_23);  // XCV-23, including AHRS temperature control
			}
		}
		else{
			if( CAN->selfTest(true) ){  // if slope bit is to be handled, there is no temperature control
				resultCAN = "OK";
				ESP_LOGE(FNAME,"CAN Bus selftest RS: OK");
				logged_tests += "CAN Interface: OK\n";
				if( hardwareRevision.get() != XCVARIO_22 ){
					hardwareRevision.set(XCVARIO_22);  // XCV-22, CAN but no AHRS temperature control
				}
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
	if( (hardwareRevision.get() >= XCVARIO_21) && serial2_speed.get() ){
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
	if(  can_speed.get() != CAN_SPEED_OFF && (resultCAN == "OK") && CAN )
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
		if( NEED_VOLTAGE_ADJUST ){
			ESP_LOGI(FNAME,"Do Factory Voltmeter adj");
			SetupMenuValFloat::showMenu( 0.0, SetupMenuValFloat::meter_adj_menu );
		}else{
			SetupMenuValFloat *qnh_menu = SetupMenu::createQNHMenu();
			SetupMenuValFloat::showMenu( qnh_best, qnh_menu );
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
	if( hardwareRevision.get() == XCVARIO_20 ){
		Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);  // XCV-20 uses GPIO_2 for Rotary
	}
	else {
		Rotary.begin( GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_0);
		gpio_pullup_en( GPIO_NUM_34 );
		if( gflags.haveMPU && HAS_MPU_TEMP_CONTROL && !gflags.mpu_pwm_initalized  ){ // series 2023 does not have slope support on CAN bus but MPU temperature control
			MPU.pwm_init();
			gflags.mpu_pwm_initalized = true;
			///gpio_set_level(GPIO_NUM_34,0);

		}

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
				if( CAN && CAN->connectedXCV() ){
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
	
	xTaskCreatePinnedToCore(&processIMU, "processIMU", 4096, NULL, 15, &mpid, 0);
	
	if( SetupCommon::isClient() ){
		xTaskCreatePinnedToCore(&clientLoop, "clientLoop", 4096, NULL, 11, &bpid, 0);
		xTaskCreatePinnedToCore(&audioTask, "audioTask", 4096, NULL, 11, &apid, 0);
	}
	else {
		xTaskCreatePinnedToCore(&readSensors, "readSensors", 5120, NULL, 14, &bpid, 0);

	}
	xTaskCreatePinnedToCore(&readTemp, "readTemp", 3000, NULL, 5, &tpid, 0);       // increase stack by 500 byte
	xTaskCreatePinnedToCore(&drawDisplay, "drawDisplay", 6144, NULL, 7, &dpid, 0); // increase stack by 1K

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
	Cipher::begin();
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
