#include <btstack.h> 

#include <string>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "string.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "BME280_ESP32_SPI.h"
#include <driver/adc.h>
#include "driver/gpio.h"
#include "mcp3221.h"
#include "mcp4018.h"
#include "ESP32NVS.h"
#include "MP5004DP.h"
#include "MS4525DO.h"
#include "BMPVario.h"
#include "BTSender.h"
#include "OpenVario.h"
#include "DS18B20.h"
#include "Setup.h"
#include "esp_sleep.h"
#include "ESPAudio.h"
#include <esp_wifi.h>
#include "SetupMenu.h"
#include "ESPRotary.h"
#include "BatVoltage.h"
#include "IpsDisplay.h"
#include "sensor.h"
#include "S2F.h"
#include "Version.h"
#include "Polars.h"

#include <SPI.h>
#include <Ucglib.h>
#include <OTA.h>
#include "SetupNG.h"
#include <logdef.h>
#include "Switch.h"

#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "I2Cbus.hpp"
#include "KalmanMPU6050.h"
#include "WifiApp.h"
#include "Serial.h"

// #include "sound.h"

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


float baroP=0;
float temperature=15.0;
bool  validTemperature=false;
float battery=0.0;
float TE=0;
float dynamicP;

bool haveMPU=false;
int ccp=60;

MCP3221 MCP;
DS18B20  ds18b20( GPIO_NUM_23 );  // GPIO_NUM_23 standard, alternative  GPIO_NUM_17
MP5004DP MP5004DP;
MS4525DO MS4525DO;
xSemaphoreHandle xMutex=NULL;

S2F Speed2Fly;
OpenVario OV( &Speed2Fly );

BatVoltage ADC;

TaskHandle_t *bpid;
TaskHandle_t *apid;
TaskHandle_t *tpid;
TaskHandle_t *dpid;

xSemaphoreHandle spiMutex=NULL;

BME280_ESP32_SPI bmpBA(SPI_SCLK, SPI_MOSI, SPI_MISO, CS_bme280BA, FREQ_BMP_SPI);
BME280_ESP32_SPI bmpTE(SPI_SCLK, SPI_MOSI, SPI_MISO, CS_bme280TE, FREQ_BMP_SPI);
Ucglib_ILI9341_18x240x320_HWSPI myucg( SPI_DC, CS_Display, RESET_Display );
IpsDisplay display( &myucg );
OTA ota;

ESPRotary Rotary;
SetupMenu  Menu;

static float ias = 0;
float tas = 0;
static float aTE = 0;
static float aTES2F = 0;
static float alt;
static float aCl = 0;
static float netto = 0;
static float as2f = 0;
static float s2f_delta = 0;
static float polar_sink = 0;
static bool  standard_setting = false;
long millisec = millis();
t_as_sensor as_sensor;

mpud::float_axes_t accelG;
mpud::float_axes_t gyroDPS;
mpud::float_axes_t accelG_Prev;
mpud::float_axes_t gyroDPS_Prev;

float ox=0;
float oy=0;
float oz=0;

float aox=0;
float aoy=0;
float aoz=0;

// Gyro and acceleration sensor
I2C_t& i2c                     = i2c1;  // i2c0 or i2c1
MPU_t MPU;         // create an object

void handleRfcommRx( char * rx, uint16_t len ){
	ESP_LOGI(FNAME,"RFCOMM packet, %s, len %d %d", rx, len, strlen( rx ));
}

float getTAS() { return tas; };
float getTE() { return TE; };

BTSender btsender( handleRfcommRx  );
bool lastAudioDisable = false;

void drawDisplay(void *pvParameters){
	while (1) {
		// TickType_t dLastWakeTime = xTaskGetTickCount();
		bool dis = Audio.getDisable();
		if( dis != true ) {
			float t=temperature;
			if( validTemperature == false )
				t = DEVICE_DISCONNECTED_C;
			float airspeed = 0;
			if( airspeed_mode.get() == MODE_IAS )
				airspeed = ias;
			else if( airspeed_mode.get() == MODE_TAS )
				airspeed = tas;
			// ESP_LOGI(FNAME,"airseed=%f ",airspeed );
			display.drawDisplay( airspeed, TE, aTE, polar_sink, alt, t, battery, s2f_delta, as2f, aCl, Switch::cruiseMode(), standard_setting );
		}
		vTaskDelay(20/portTICK_PERIOD_MS);
		if( uxTaskGetStackHighWaterMark( dpid ) < 1024  )
			ESP_LOGW(FNAME,"Warning drawDisplay stack low: %d bytes", uxTaskGetStackHighWaterMark( dpid ) );
	}
}

int count=0;
mpud::raw_axes_t accelRawPrev;     // holds x, y, z axes as int16
mpud::raw_axes_t gyroRawPrev;      // holds x, y, z axes as int16



void readBMP(void *pvParameters){
	while (1)
	{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		xSemaphoreTake(xMutex,portMAX_DELAY );
		TE = bmpVario.readTE( tas );  // 10x per second
		if( as_sensor == SENSOR_MP3V5004DP )
			dynamicP = MP5004DP.readPascal(60);
		else if( as_sensor == SENSOR_MS4525DO ) {
			bool ok=false;
			float p=MS4525DO.readPascal(60, ok);
			if( ok )
				dynamicP = p;
		}

		float iasraw = ( MP5004DP.pascal2km( dynamicP ) );
		// ESP_LOGI("FNAME","P: %f  IAS:%f", dynamicP, iasraw );
		float T=temperature;
		if( !validTemperature ) {
			T= 15 - ( (alt/100) * 0.65 );
			// ESP_LOGW(FNAME,"T invalid, using 15 deg");
		}
		float tasraw = 0;
		if( baroP != 0 )
			tasraw = iasraw * sqrt( 1.225 / ( baroP*100.0 / (287.058 * (273.15+T) ) ) );  // True airspeed
		ias = ias + (iasraw - ias)*0.25;  // low pass filter
		// ESP_LOGI("FNAME","P: %f  IAS:%f IASF: %d", dynamicP, iasraw, ias );
		tas += (tasraw-tas)*0.25;       // low pass filter
		// ESP_LOGI(FNAME,"IAS=%f, T=%f, TAS=%f baroP=%f", ias, T, tas, baroP );
		Audio.setValues( TE, s2f_delta, ias );
		TE = bmpVario.readTE( tasraw );  // 10x per second
		xSemaphoreGive(xMutex);
		// ESP_LOGI(FNAME,"count %d ccp %d", count, ccp );
		if( !(count % ccp) ) {
			bmpVario.recalcAvgClimb();
			aCl = bmpVario.readAvgClimb();
		}
		if( (count++ % 2) == 0 ) {
			xSemaphoreTake(xMutex,portMAX_DELAY );
			baroP = bmpBA.readPressure();   // 5x per second
			float alt_standard = bmpBA.calcAVGAltitudeSTD( baroP );
			if( alt_select.get() == 0 ) // TE
				alt = bmpVario.readAVGalt();
			else { // Baro
				if(  alt_unit.get() == 2 || (
						(fl_auto_transition.get() == 1) && ((int)alt_standard*0.0328084 + (int)(standard_setting) > transition_alt.get() ) ) ) // FL or auto transition
				{
					alt = alt_standard;
					standard_setting = true;
				}
				else {
					alt = bmpBA.calcAVGAltitude( QNH.get(), baroP );
					// ESP_LOGI(FNAME,"rbmp QNH %f baro: %f alt: %f", QNH.get(), baroP, alt  );
					standard_setting = false;
				}
			}
			aTE = bmpVario.readAVGTE();
			aTES2F = bmpVario.readS2FTE();

			polar_sink = Speed2Fly.sink( ias );
			netto = aTES2F - polar_sink;
			as2f = Speed2Fly.speed( netto );
			s2f_delta = as2f - ias;
			xSemaphoreGive(xMutex);

			if( Audio.getDisable() != true ){
				if( haveMPU )  // 4th Generation HW, MPU6050
				{
					mpud::raw_axes_t accelRaw;     // holds x, y, z axes as int16
					mpud::raw_axes_t gyroRaw;      // holds x, y, z axes as int16
					esp_err_t err = MPU.acceleration(&accelRaw);  // fetch raw data from the registers
					if( err != ESP_OK )
						ESP_LOGE(FNAME, "accel I2C error, X:%+.2f Y:%+.2f Z:%+.2f", -accelG[2], accelG[1], accelG[0] );
					err |= MPU.rotation(&gyroRaw);       // fetch raw data from the registers
					if( err != ESP_OK )
						ESP_LOGE(FNAME, "gyro I2C error, X:%+.2f Y:%+.2f Z:%+.2f",  gyroDPS.x, gyroDPS.y, gyroDPS.z );
					accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_4G);  // raw data to gravity
					gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);  // raw data to º/s
					// ESP_LOGI(FNAME, "accel X: %+.2f Y:%+.2f Z:%+.2f  gyro X: %+.2f Y:%+.2f Z:%+.2f\n", -accelG[2], accelG[1], accelG[0] ,  gyroDPS.x, gyroDPS.y, gyroDPS.z);
					bool goodAccl = true;
					if( abs( accelG.x - accelG_Prev.x ) > 0.4 || abs( accelG.y - accelG_Prev.y ) > 0.4 || abs( accelG.z - accelG_Prev.z ) > 0.4 ) {
						MPU.acceleration(&accelRaw);
						accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_4G);
						if( abs( accelG.x - accelG_Prev.x ) > 0.4 || abs( accelG.y - accelG_Prev.y ) > 0.4 || abs( accelG.z - accelG_Prev.z ) > 0.4 ){
							goodAccl = false;
							ESP_LOGE(FNAME, "accelaration change >0.4 g in 0.2 S:  X:%+.2f Y:%+.2f Z:%+.2f", -accelG[2], accelG[1], accelG[0] );
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
					if( err == ESP_OK && goodAccl && goodGyro ) {
						IMU::read();
					}
					gyroDPS_Prev = gyroDPS;
					accelG_Prev = accelG;
				}

				xSemaphoreTake(xMutex,portMAX_DELAY );
				// reduce also messages from 10 per second to 5 per second to reduce load in XCSoar
				char lb[120];
				if( nmea_protocol.get() == BORGELT ) {
					OV.makeNMEA( P_BORGELT, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt  );
					btsender.send( lb );
					OV.makeNMEA( P_GENERIC, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt  );
				}
				else if( nmea_protocol.get() == OPENVARIO )
					OV.makeNMEA( P_OPENVARIO, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt  );
				else if( nmea_protocol.get() == CAMBRIDGE )
					OV.makeNMEA( P_CAMBRIDGE, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt  );
				else if( nmea_protocol.get() == EYE_SENSOR_BOX ) {
					OV.makeNMEA( P_EYE_PEYA, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature  );
					btsender.send( lb );
					OV.makeNMEA( P_EYE_PEYI, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature,
							-accelG[2], accelG[1],accelG[0], gyroDPS.x+ox, gyroDPS.y+oy, gyroDPS.z+oz );
				}
				else if( nmea_protocol.get() == XCVARIO ) {
					OV.makeNMEA( P_XCVARIO, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Switch::cruiseMode(), alt, validTemperature,
							-accelG[2], accelG[1],accelG[0], gyroDPS.x+ox, gyroDPS.y+oy, gyroDPS.z+oz );
				}
				else
					ESP_LOGE(FNAME,"Protocol %d not supported error", nmea_protocol.get() );
				btsender.send( lb );
				vTaskDelay(2/portTICK_PERIOD_MS);
				xSemaphoreGive(xMutex);
			}

		}
		if( uxTaskGetStackHighWaterMark( bpid )  < 1024 )
			ESP_LOGW(FNAME,"Warning bmpTask stack low: %d", uxTaskGetStackHighWaterMark( bpid ) );

		esp_task_wdt_reset();
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}

int ttick = 0;

void readTemp(void *pvParameters){
	while (1) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		float t=15.0;
		if( Audio.getDisable() != true )
		{
			battery = ADC.getBatVoltage();
			// ESP_LOGI(FNAME,"Battery=%f V", battery );
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
			}
			ESP_LOGV(FNAME,"temperature=%f", temperature );
		}
		vTaskDelayUntil(&xLastWakeTime, 1000/portTICK_PERIOD_MS);

		if( (ttick++ % 100) == 0) {
			if( uxTaskGetStackHighWaterMark( tpid ) < 1024 )
				ESP_LOGW(FNAME,"Warning temperature task stack low: %d bytes", uxTaskGetStackHighWaterMark( tpid ) );
			if( heap_caps_get_free_size(MALLOC_CAP_8BIT) < 10000 )
				ESP_LOGW(FNAME,"Warning heap_caps_get_free_size getting low: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));
		}
	}
}

bool init_done=false;

void sensor(void *args){
	bool selftestPassed=true;
	if( init_done )
		ESP_LOGI( FNAME, "sensor init already called");
	init_done=true;
	int line = 1;
	// i2c.begin(GPIO_NUM_21, GPIO_NUM_22, GPIO_PULLUP_ENABLE, GPIO_PULLUP_ENABLE );
	i2c.begin(GPIO_NUM_21, GPIO_NUM_22, 20000 );
	MCP.setBus( &i2c );
	gpio_set_drive_capability(GPIO_NUM_23, GPIO_DRIVE_CAP_1);
	esp_wifi_set_mode(WIFI_MODE_NULL);
	spiMutex = xSemaphoreCreateMutex();
	esp_log_level_set("*", ESP_LOG_INFO);
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


	ESP_LOGI(FNAME, "QNH->get() %f", QNH.get() );
	ESP_LOGI( FNAME, "Hardware revision detected %d", hardwareRevision.get() );
	NVS.begin();

	ADC.begin();  // for battery voltage


	xMutex=xSemaphoreCreateMutex();
	uint8_t t_sb = 0;   //stanby 0: 0,5 mS 1: 62,5 mS 2: 125 mS
	uint8_t filter = 0; //filter O = off
	uint8_t osrs_t = 5; //OverSampling Temperature
	uint8_t osrs_p = 5; //OverSampling Pressure (5:x16 4:x8, 3:x4 2:x2 )
	uint8_t osrs_h = 0; //OverSampling Humidity x4
	uint8_t Mode = 3;   //Normal mode

	xSemaphoreTake(spiMutex,portMAX_DELAY );
	ccp = (int)(core_climb_period.get()*10);
	SPI.begin( SPI_SCLK, SPI_MISO, SPI_MOSI, CS_bme280BA );
	xSemaphoreGive(spiMutex);
	display.begin();
	display.bootDisplay();

	// int valid;
	String logged_tests;
	logged_tests += "\n\n\n";

	Version V;
	std::string ver( "Version: " );
	ver += V.version();
	display.writeText(line++, ver.c_str() );
	sleep(2);
	bool doUpdate = software_update.get();
	if( Rotary.readSwitch() ){
		doUpdate = true;
		ESP_LOGI(FNAME,"Rotary pressed: Do Software Update");
	}

	String btname="Bluetooth ID: ";
	btname += SetupCommon::getID();
	display.writeText(line++, btname.c_str() );

	ESP_LOGI(FNAME,"Speed sensors init..");
	int offset;
	bool offset_plausible = false;
	MS4525DO.begin( GPIO_NUM_21, GPIO_NUM_22 );  // sda, scl
	MS4525DO.setBus( &i2c );

	if( MS4525DO.selfTest( offset ) ){
		ESP_LOGI(FNAME,"Speed sensors MS4525DO self test PASSED");
		MS4525DO.doOffset();
		offset_plausible = MS4525DO.offsetPlausible( offset );
		as_sensor = SENSOR_MS4525DO;
		bool ok=false;
		float p=MS4525DO.readPascal(60, ok);
		if( ok )
			dynamicP = p;
		ias = MS4525DO.pascal2km( dynamicP );
		ESP_LOGI(FNAME,"Using speed sensor MS4525DO type, current speed=%f", ias );
	}
	else
	{
		MP5004DP.begin( GPIO_NUM_21, GPIO_NUM_22);  // sda, scl
		if( MP5004DP.selfTest( offset ) ) {
			ESP_LOGI(FNAME,"Speed sensors MP5004DP self test PASSED");
			MP5004DP.doOffset();
			offset_plausible = MP5004DP.offsetPlausible( offset );
			as_sensor = SENSOR_MP3V5004DP;
			dynamicP=MP5004DP.readPascal(60);
			ias = MP5004DP.pascal2km( dynamicP );
			ESP_LOGI(FNAME,"Using speed sensor MP3V5004DP type, current speed=%f", ias );
		}
	}


	if( as_sensor==SENSOR_NONE ){
		ESP_LOGE(FNAME,"Error with air speed pressure sensor, now working sensor found");
		display.writeText( line++, "AS Sensor: NOT FOUND");
		logged_tests += "AS Sensor: NOT FOUND\n";
		selftestPassed = false;
	}else {
		if( !offset_plausible && ( ias < 50 ) ){
			ESP_LOGE(FNAME,"Error: air speed presure sensor offset out of bounds, act value=%d", offset );
			display.writeText( line++, "AS Sensor: NEED ZERO" );
			logged_tests += "AS Sensor offset test: FAILED\n";
			selftestPassed = false;
		}
		else {
			ESP_LOGI(FNAME,"air speed offset test PASSED, readout value in bounds=%d", offset );
			char s[40];
			if( ias > 50 ) {
				sprintf(s, "AS Sensor: %d km/h", (int)(ias+0.5) );
				display.writeText( line++, s );
			}
			else
				display.writeText( line++, "AS Sensor: OK" );

			logged_tests += "AS Sensor offset test: PASSED\n";
		}
	}
	ESP_LOGI(FNAME,"Now start T sensor test");
	sleep( 1 );
	// Temp Sensor test
	ds18b20.begin();
	temperature = ds18b20.getTemp();
	ESP_LOGI(FNAME,"End T sensor test");
	if( temperature == DEVICE_DISCONNECTED_C ) {
		ESP_LOGE(FNAME,"Error: Self test Temperatur Sensor failed; returned T=%2.2f", temperature );
		display.writeText( line++, "Temp Sensor: NOT FOUND");
		validTemperature = false;
		logged_tests += "External Temperature Sensor: NOT FOUND\n";
	}else
	{
		ESP_LOGI(FNAME,"Self test Temperatur Sensor PASSED; returned T=%2.2f", temperature );
		display.writeText( line++, "Temp Sensor: OK");
		validTemperature = true;
		logged_tests += "External Temperature Sensor:PASSED\n";

	}
	ESP_LOGI(FNAME,"BMP280 sensors init..");

	bmpBA.begin(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);
	bmpTE.begin(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);
	sleep( 1 );

	float ba_t, ba_p, te_t, te_p;
	if( ! bmpBA.selfTest( ba_t, ba_p)  ) {
		ESP_LOGE(FNAME,"HW Error: Self test Barometric Pressure Sensor failed!");
		display.writeText( line++, "Baro Sensor: NOT FOUND");
		selftestPassed = false;
		logged_tests += "Baro Sensor Test: NOT FOUND\n";
	}
	else {
		ESP_LOGI(FNAME,"Barometric Sensor T=%f P=%f", ba_t, ba_p);
		display.writeText( line++, "Baro Sensor: OK");
		logged_tests += "Baro Sensor Test: PASSED\n";
	}

	if( ! bmpTE.selfTest(te_t, te_p) ) {
		ESP_LOGE(FNAME,"HW Error: Self test TE Pressure Sensor failed!");
		display.writeText( line++, "TE Sensor: NOT FOUND");
		selftestPassed = false;
		logged_tests += "TE Sensor Test: NOT FOUND\n";
	}
	else {
		ESP_LOGI(FNAME,"TE Sensor         T=%f P=%f", te_t, te_p);
		display.writeText( line++, "TE Sensor: OK");
		logged_tests += "TE Sensor Test: PASSED\n";
	}

	if( selftestPassed ) {
		if( (abs(ba_t - te_t) >4.0)  && ( ias < 50 ) ) {   // each sensor has deviations, and new PCB has more heat sources
			selftestPassed = false;
			ESP_LOGI(FNAME,"Severe Temperature deviation delta > 4 °C between Baro and TE sensor: °C %f", abs(ba_t - te_t) );
			display.writeText( line++, "TE/Baro Temp: Unequal");
			logged_tests += "TE/Baro Sensor T diff. <4°C: FAILED\n";
		}
		else{
			ESP_LOGI(FNAME,"BMP 280 Temperature deviation test PASSED, dev: %f",  abs(ba_t - te_t));
			// display.writeText( line++, "TE/Baro Temp: OK");
			logged_tests += "TE/Baro Sensor T diff. <2°C: PASSED\n";
		}

		if( (abs(ba_p - te_p) >2.5)  && ( ias < 50 ) ) {
			selftestPassed = false;
			ESP_LOGI(FNAME,"Pressure deviation delta > 2 hPa between Baro and TE sensor: %f", abs(ba_p - te_p) );
			display.writeText( line++, "TE/Baro P: Unequal");
			logged_tests += "TE/Baro Sensor P diff. <2hPa: FAILED\n";
		}
		else
			ESP_LOGI(FNAME,"BMP 280 Pressure deviation test PASSED, dev: %f", abs(ba_p - te_p) );
		// display.writeText( line++, "TE/Baro P: OK");
		logged_tests += "TE/Baro Sensor P diff. <2hPa: PASSED\n";

	}

	bmpVario.begin( &bmpTE, &Speed2Fly );
	bmpVario.setup();

	esp_task_wdt_reset();

	ESP_LOGI(FNAME,"Audio begin");
	Audio.begin( DAC_CHANNEL_1, GPIO_NUM_0);
	ESP_LOGI(FNAME,"Poti and Audio test");
	if( !Audio.selfTest() ) {
		ESP_LOGE(FNAME,"Error: Digital potentiomenter selftest failed");
		display.writeText( line++, "Digital Poti: Failure");
		selftestPassed = false;
		logged_tests += "Digital Audio Poti test: FAILED\n";
	}
	else{
		ESP_LOGI(FNAME,"Digital potentiometer test PASSED");
		logged_tests += "Digital Audio Poti test: PASSED\n";
		display.writeText( line++, "Digital Poti: OK");
	}


	float bat = ADC.getBatVoltage(true);
	if( bat < 1 || bat > 28.0 ){
		ESP_LOGE(FNAME,"Error: Battery voltage metering out of bounds, act value=%f", bat );
		display.writeText( line++, "Bat Sensor: Failure");
		logged_tests += "Battery Voltage Sensor: FAILED\n";
		selftestPassed = false;
	}
	else{
		ESP_LOGI(FNAME,"Battery voltage metering test PASSED, act value=%f", bat );
		display.writeText( line++, "Bat Sensor: OK");
		logged_tests += "Battery Voltage Sensor: PASSED\n";
	}
	btsender.begin();  // at least serial init: TBD: rename and split BT part
	Serial::begin();
	sleep( 2 );
	if( blue_enable.get() ) {
		if( btsender.selfTest() ){
			display.writeText( line++, "Bluetooth: OK");
			logged_tests += "Bluetooth test: PASSED\n";
		}
		else{
			display.writeText( line++, "Bluetooth: FAILED");
			logged_tests += "Bluetooth test: FAILED\n";
		}
	}else{
		wifi_init_softap();
	}


	esp_err_t err=ESP_ERR_NOT_FOUND;

	if( attitude_indicator.get() ) {
		MPU.setBus(i2c);  // set communication bus, for SPI -> pass 'hspi'
		MPU.setAddr(mpud::MPU_I2CADDRESS_AD0_LOW);  // set address or handle, for SPI -> pass 'mpu_spi_handle'
		err = MPU.initialize();
		ESP_LOGI( FNAME,"MPU Probing returned %d MPU enable: %d ", err, attitude_indicator.get() );
	}
	if( err == ESP_OK ){
		haveMPU = true;
		hardwareRevision.set(3);  // wow, there is MPU6050 gyro and acceleration sensor
		ESP_LOGI( FNAME,"MPU initialize");
		MPU.initialize();  // this will initialize the chip and set default configurations
		MPU.setSampleRate(50);  // in (Hz)
		MPU.setAccelFullScale(mpud::ACCEL_FS_4G);
		MPU.setGyroFullScale(mpud::GYRO_FS_500DPS);
		MPU.setDigitalLowPassFilter(mpud::DLPF_5HZ);  // smoother data
		display.writeText( line++, "AHRS Sensor: OK");
		logged_tests += "MPU6050 AHRS test: PASSED\n";
		IMU::init();
		IMU::read();

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
	}
	else{
		if( hardwareRevision.get() != 2 )
			hardwareRevision.set(2);
		if( attitude_indicator.get() ) { // log negative message only if AHRS is enabled
			display.writeText( line++, "AHRS Sensor: NOT FOUND");
			logged_tests += "MPU6050 AHRS test: NOT FOUND\n";
			selftestPassed = false;
		}
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
		ota.begin( &Rotary );
		ota.doSoftwareUpdate( &display );
	}

	Speed2Fly.change_polar();
	Speed2Fly.change_mc_bal();
	Version myVersion;
	ESP_LOGI(FNAME,"Program Version %s", myVersion.version() );
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);  // blue LED, maybe use for BT connection

	ESP_LOGI(FNAME,"%s", logged_tests.c_str());
	if( !selftestPassed )
	{
		ESP_LOGI(FNAME,"\n\n\nSelftest failed, see above LOG for Problems\n\n\n");
		if( !Rotary.readSwitch() )
			sleep(4);
	}
	else{
		ESP_LOGI(FNAME,"\n\n\n*****  Selftest PASSED  ********\n\n\n");
		display.writeText( line++, "Selftest PASSED");
		if( !Rotary.readSwitch() )
			sleep(2);
	}

	if( Rotary.readSwitch() )
	{
		ESP_LOGI(FNAME,"Starting Leak test");
		display.clear();
		display.writeText( 1, "** Leak Test **");
		float sba=0;
		float ste=0;
		float sspeed = 0;
		float bad=0;
		float ted=0;
		float speedd=0;
		for( int i=0; i<24; i++ ){  // 180
			char bas[40];
			char tes[40];
			char pis[40];
			float ba=0;
			float te=0;
			float speed=0;
#define LOOPS 150
			int loops_run=0;
			for( int j=0; j< LOOPS; j++ ) {
				if( as_sensor == SENSOR_MP3V5004DP )
					speed += MP5004DP.readPascal(5);
				else if( as_sensor == SENSOR_MS4525DO ){
					bool ok=false;
					float s = MS4525DO.readPascal(5,ok);
					if( ok ){
						speed +=s;
						loops_run++;
					}
				}
				ba += bmpBA.readPressure();
				te += bmpTE.readPressure();
				delay( 33 );
			}
			ba = ba/LOOPS;
			te = te/LOOPS;
			speed = speed/loops_run;
			if( i==0 ) {
				sba = ba;
				ste = te;
				sspeed = speed;
			}
			// esp_task_wdt_reset();
			sprintf(bas, "ST P: %3.2f hPa   ", ba);
			sprintf(tes, "TE P: %3.2f hPa   ", te);
			sprintf(pis, "PI P: %3.2f Pa    ", speed);
			display.writeText( 2, bas);
			display.writeText( 3, tes);
			display.writeText( 4, pis);
			if( i==0 ) {
				ESP_LOGI(FNAME,"%s  %s  %s", bas,tes,pis);
			}
			if( i>=1 ) {
				bad = 100*(ba-sba)/sba;
				ted = 100*(te-ste)/ste;
				speedd = 100*(speed-sspeed)/sspeed;
				sprintf(bas, "ST delta: %2.3f %%   ", bad );
				sprintf(tes, "TE delta: %2.3f %%   ", ted );
				sprintf(pis, "PI delta: %2.2f %%   ", speedd );
				display.writeText( 5, bas);
				display.writeText( 6, tes);
				display.writeText( 7, pis);
				ESP_LOGI(FNAME,"%s%s%s", bas,tes,pis);

			}
			char sec[40];
			sprintf(sec, "Seconds: %d", (i*5)+5 );
			display.writeText( 8, sec );
		}
		if( (abs(bad) > 0.1) || (abs(ted) > 0.1) || ( (sspeed > 10.0) && (abs(speedd) > (1.0) ) ) ) {
			display.writeText( 9, "Test FAILED" );
			ESP_LOGI(FNAME,"FAILED");
		}
		else {
			display.writeText( 9, "Test PASSED" );
			ESP_LOGI(FNAME,"PASSED");
		}
		while(1) {
			delay(5000);
		}
	}


	display.initDisplay();
	Menu.begin( &display, &Rotary, &bmpBA, &ADC );
	if( ias < 50.0 ){
		xSemaphoreTake(xMutex,portMAX_DELAY );
		ESP_LOGI(FNAME,"QNH Autosetup, IAS=%3f (<50 km/h)", ias );
		// QNH autosetup
		float ae = elevation.get();
		baroP = bmpBA.readPressure();
		if( ae > 0 ) {
			float step=10.0; // 80 m
			float min=1000.0;
			float qnh_best = 1013.2;
			for( float qnh = 900; qnh< 1080; qnh+=step ) {
				float alt = bmpBA.readAltitude( qnh );
				float diff = alt - ae;
				ESP_LOGI(FNAME,"Alt diff=%4.2f  abs=%4.2f", diff, abs(diff) );
				if( abs( diff ) < 100 )
					step=1.0;  // 8m
				if( abs( diff ) < 10 )
					step=0.05;  // 0.4 m
				if( abs( diff ) < abs(min) ) {
					min = diff;
					qnh_best = qnh;
					ESP_LOGI(FNAME,"New min=%4.2f", min);
				}
				if( diff > 1.0 ) // we are ready, values get already positive
					break;
				// esp_task_wdt_reset();
			}
			ESP_LOGI(FNAME,"qnh=%4.2f\n", qnh_best);
			float &qnh = QNH.getRef();
			qnh = qnh_best;
		}

		SetupMenuValFloat::showQnhMenu();
		xSemaphoreGive(xMutex);
	}
	else
	{
		Audio.disable(false);
	}
	if( hardwareRevision.get() == 2 )
		Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);
	else
		Rotary.begin( GPIO_NUM_36, GPIO_NUM_39, GPIO_NUM_0);

	gpio_set_pull_mode(RESET_Display, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_Display, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_bme280BA, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_bme280TE, GPIO_PULLUP_ONLY );

	xTaskCreatePinnedToCore(&readBMP, "readBMP", 4096*2, NULL, 15, bpid, 0);  // 10
	xTaskCreatePinnedToCore(&drawDisplay, "drawDisplay", 12000, NULL, 20, dpid, 0);  // 10
	xTaskCreatePinnedToCore(&readTemp, "readTemp", 4096, NULL, 6, tpid, 0);

	Audio.startAudio();

	for( int i=0; i<36; i++ ) {
		if( i != 23 && i < 6 && i > 12  )
			gpio_set_drive_capability((gpio_num_t)i, GPIO_DRIVE_CAP_1);
	}
	// vTaskDelete( NULL );

}

extern "C" void btstack_app(void *ignore);

extern "C" void  app_main(void){
	ESP_LOGI(FNAME,"app_main" );
	ESP_LOGI(FNAME,"Now init all Setup elements");
	SetupCommon::initSetup();
	// xTaskCreatePinnedToCore(&sensor, "sensor", 8192, NULL, 25, 0, 0);
	// sleep( 2 );
	if( blue_enable.get() ) {
		ESP_LOGI(FNAME,"start btstack task");
		xTaskCreatePinnedToCore(&btstack_app, "btstack_app", 4096, NULL, 25, 0, 0);
		sleep( 2 );
	}
	// sleep( 10 );
	sensor( 0 );

	vTaskDelete( NULL );
}



