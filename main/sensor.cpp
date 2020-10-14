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

#include "MPU.hpp"        // main file, provides the class itself
#include "mpu/math.hpp"   // math helper for dealing with MPU data
#include "mpu/types.hpp"  // MPU data types and definitions
#include "I2Cbus.hpp"


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

DS18B20  ds18b20( GPIO_NUM_23 );  // GPIO_NUM_23 standard, alternative  GPIO_NUM_17
MP5004DP MP5004DP;
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
static float tas = 0;
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

static mpud::float_axes_t accelG;
static mpud::float_axes_t gyroDPS;

// Gyro and acceleration sensor
static I2C_t& i2c                     = i2c0;  // i2c0 or i2c1
MPU_t MPU;         // create an object


void handleRfcommRx( char * rx, uint16_t len ){
	ESP_LOGI(FNAME,"RFCOMM packet, %s, len %d %d", rx, len, strlen( rx ));
}


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
			display.drawDisplay( airspeed, TE, aTE, polar_sink, alt, t, battery, s2f_delta, as2f, aCl, Audio.getS2FMode(), standard_setting );
		}
		vTaskDelay(10);
		if( uxTaskGetStackHighWaterMark( dpid ) < 1024  )
			ESP_LOGW(FNAME,"Warning drawDisplay stack low: %d bytes", uxTaskGetStackHighWaterMark( dpid ) );
	}
}

int count=0;
float ox=0;
float oy=0;
float oz=0;


void readBMP(void *pvParameters){
	while (1)
	{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		xSemaphoreTake(xMutex,portMAX_DELAY );
		TE = bmpVario.readTE( tas );  // 10x per second
		dynamicP = MP5004DP.readPascal(30);
		float iasraw = ( MP5004DP.pascal2km( dynamicP ) );
		float T=temperature;
		if( !validTemperature )
			T= 15 - ( (alt/100) * 0.65 );
		float tasraw = iasraw * sqrt( 1.225 / ( baroP*100.0 / (287.058 * (273.15+T) ) ) );  // True airspeed
		ias = ias + (iasraw - ias)*0.25;  // low pass filter
		tas += (tasraw-tas)*0.25;       // low pass filter
		Audio.setValues( TE, s2f_delta, ias );
		TE = bmpVario.readTE( tasraw );  // 10x per second
		xSemaphoreGive(xMutex);
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
			aCl = bmpVario.readAvgClimb();
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
					MPU.acceleration(&accelRaw);  // fetch raw data from the registers
					MPU.rotation(&gyroRaw);       // fetch raw data from the registers
					accelG = mpud::accelGravity(accelRaw, mpud::ACCEL_FS_4G);  // raw data to gravity
					gyroDPS = mpud::gyroDegPerSec(gyroRaw, mpud::GYRO_FS_500DPS);  // raw data to º/s
					// ESP_LOGI(FNAME, "accel X: %+.2f Y:%+.2f Z:%+.2f\n", -accelG[2], accelG[1], accelG[0]);
					// ESP_LOGI( FNAME, "gyro X: %+.2f Y:%+.2f Z:%+.2f\n", gyroDPS.x+ox, gyroDPS.y+oy, gyroDPS.z+oz);
					if( ox == 0 ){
						ox = -gyroDPS.x;
						oy = -gyroDPS.y;
						oz = -gyroDPS.z;
					}
					// float x, float y, float z, float bank, float pitch, float head
				}
				xSemaphoreTake(xMutex,portMAX_DELAY );
				// reduce also messages from 10 per second to 5 per second to reduce load in XCSoar
				char lb[120];
				if( nmea_protocol.get() == BORGELT ) {
					OV.makeNMEA( P_BORGELT, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Audio.getS2FMode(), alt  );
					btsender.send( lb );
					OV.makeNMEA( P_GENERIC, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Audio.getS2FMode(), alt  );
				}
				else if( nmea_protocol.get() == OPENVARIO )
					OV.makeNMEA( P_OPENVARIO, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Audio.getS2FMode(), alt  );
				else if( nmea_protocol.get() == CAMBRIDGE )
					OV.makeNMEA( P_CAMBRIDGE, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Audio.getS2FMode(), alt  );
				else if( nmea_protocol.get() == EYE_SENSOR_BOX ) {
					OV.makeNMEA( P_EYE_PEYA, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Audio.getS2FMode(), alt, validTemperature  );
					btsender.send( lb );
					OV.makeNMEA( P_EYE_PEYI, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Audio.getS2FMode(), alt, validTemperature,
							     -accelG[2], accelG[1],accelG[0], gyroDPS.x+ox, gyroDPS.y+oy, gyroDPS.z+oz );
					// btsender.send( lb );
					// OV.makeNMEA( P_GENERIC, lb, baroP, dynamicP, TE, temperature, ias, tas, MC.get(), bugs.get(), ballast.get(), Audio.getS2FMode(), alt  );
				}
				else
					ESP_LOGE(FNAME,"Protocol %d not supported error", nmea_protocol.get() );

				btsender.send( lb );
				vTaskDelay(2);
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
			// ESP_LOGI(FNAME,"temperature=%f", temperature );
		}
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);

		if( (ttick++ % 100) == 0) {
			if( uxTaskGetStackHighWaterMark( tpid ) < 1024 )
				ESP_LOGW(FNAME,"Warning temperature task stack low: %d bytes", uxTaskGetStackHighWaterMark( tpid ) );
			if( heap_caps_get_free_size(MALLOC_CAP_8BIT) < 10000 )
				ESP_LOGW(FNAME,"Warning heap_caps_get_free_size getting low: %d", heap_caps_get_free_size(MALLOC_CAP_8BIT));
		}
	}
}



void sensor(void *args){
	bool selftestPassed=true;
	int line = 1;
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

	esp_err_t mpu=ESP_ERR_NOT_FOUND;
	if( attitude_indicator.get() ) {
		i2c.begin(GPIO_NUM_21, GPIO_NUM_22);
		MPU.setBus(i2c0);  // set communication bus, for SPI -> pass 'hspi'
		MPU.setAddr(mpud::MPU_I2CADDRESS_AD0_LOW);  // set address or handle, for SPI -> pass 'mpu_spi_handle'
		mpu = MPU.testConnection();  // test connection with the chip, return is a error code
		if( mpu != ESP_ERR_NOT_FOUND ){
			haveMPU = true;
			MPU.initialize();  // this will initialize the chip and set default configurations
			MPU.setSampleRate(50);  // in (Hz)
			MPU.setAccelFullScale(mpud::ACCEL_FS_4G);
			MPU.setGyroFullScale(mpud::GYRO_FS_500DPS);
			MPU.setDigitalLowPassFilter(mpud::DLPF_5HZ);  // smoother data
			// MPU.setInterruptEnabled(mpud::INT_EN_RAWDATA_READY);  // enable INT pin
		}
		ESP_LOGI(FNAME,"Now init all Setup elements");
	}
	SetupCommon::initSetup();
	ESP_LOGI(FNAME, "QNH->get() %f", QNH.get() );

	NVS.begin();
	btsender.begin();
	ADC.begin();  // for battery voltage
	sleep( 2 );

	xMutex=xSemaphoreCreateMutex();
	uint8_t t_sb = 0;   //stanby 0: 0,5 mS 1: 62,5 mS 2: 125 mS
	uint8_t filter = 0; //filter O = off
	uint8_t osrs_t = 5; //OverSampling Temperature
	uint8_t osrs_p = 5; //OverSampling Pressure (5:x16 4:x8, 3:x4 2:x2 )
	uint8_t osrs_h = 0; //OverSampling Humidity x4
	uint8_t Mode = 3;   //Normal mode

	xSemaphoreTake(spiMutex,portMAX_DELAY );

	SPI.begin( SPI_SCLK, SPI_MISO, SPI_MOSI, CS_bme280BA );
	xSemaphoreGive(spiMutex);
	display.begin();
	display.bootDisplay();

	if( software_update.get() ) {
		Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);
		ota.begin( &Rotary );
		ota.doSoftwareUpdate( &display );
	}

	// int valid;
	String failed_tests;
	failed_tests += "\n\n\n";

	Version V;
	std::string ver( "Version: " );
	ver += V.version();
	display.writeText(line++, ver.c_str() );

	ESP_LOGI(FNAME,"Speed sensors init..");
	MP5004DP.begin( GPIO_NUM_21, GPIO_NUM_22);  // sda, scl
	int val;
	bool works=MP5004DP.selfTest( val );

	MP5004DP.doOffset();
	dynamicP=MP5004DP.readPascal(30);
	ias = MP5004DP.pascal2km( dynamicP );
	ESP_LOGI(FNAME,"Speed=%f", ias );

	if( !works ){
		ESP_LOGE(FNAME,"Error reading air speed pressure sensor MP5004DP->MCP3321 I2C returned error");
		display.writeText( line++, "AS Sensor: Not found");
		failed_tests += "AS Sensor: NOT FOUND\n";
		selftestPassed = false;
	}else {
		if( !MP5004DP.offsetPlausible( val )  && ( ias < 50 ) ){
			ESP_LOGE(FNAME,"Error: AS P sensor offset MP5004DP->MCP3321 out of bounds (608-1067), act value=%d", val );
			display.writeText( line++, "AS Sensor: NEED ZERO" );
			failed_tests += "AS Sensor offset test: FAILED\n";
			selftestPassed = false;
		}
		else {
			ESP_LOGI(FNAME,"MP5004->MCP3321 test PASSED, readout value in bounds (608-1067)=%d", val );
			char s[40];
			if( ias > 50 ) {
				sprintf(s, "AS Sensor: %d km/h", (int)(ias+0.5) );
				display.writeText( line++, s );
			}
			else
				display.writeText( line++, "AS Sensor: OK" );

			failed_tests += "AS Sensor offset test: PASSED\n";
		}
	}

	// Temp Sensor test
	ds18b20.begin();
	temperature = ds18b20.getTemp();
	if( temperature == DEVICE_DISCONNECTED_C ) {
		ESP_LOGE(FNAME,"Error: Self test Temperatur Sensor failed; returned T=%2.2f", temperature );
		display.writeText( line++, "Temp Sensor: Not found");
		validTemperature = false;
		failed_tests += "External Temperature Sensor: NOT FOUND\n";
	}else
	{
		ESP_LOGI(FNAME,"Self test Temperatur Sensor PASSED; returned T=%2.2f", temperature );
		display.writeText( line++, "Temp Sensor: OK");
		validTemperature = true;
		failed_tests += "External Temperature Sensor:PASSED\n";

	}
	ESP_LOGI(FNAME,"BMP280 sensors init..");


	bmpBA.begin(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);
	bmpTE.begin(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);
	sleep( 1 );

	float ba_t, ba_p, te_t, te_p;
	if( ! bmpBA.selfTest( ba_t, ba_p)  ) {
		ESP_LOGE(FNAME,"HW Error: Self test Barometric Pressure Sensor failed!");
		display.writeText( line++, "Baro Sensor: Not found");
		selftestPassed = false;
		failed_tests += "Baro Sensor Test: NOT FOUND\n";
	}
	else {
		ESP_LOGI(FNAME,"Barometric Sensor T=%f P=%f", ba_t, ba_p);
		display.writeText( line++, "Baro Sensor: OK");
		failed_tests += "Baro Sensor Test: PASSED\n";
	}

	if( ! bmpTE.selfTest(te_t, te_p) ) {
		ESP_LOGE(FNAME,"HW Error: Self test TE Pressure Sensor failed!");
		display.writeText( line++, "TE Sensor: Not found");
		selftestPassed = false;
		failed_tests += "TE Sensor Test: NOT FOUND\n";
	}
	else {
		ESP_LOGI(FNAME,"TE Sensor         T=%f P=%f", te_t, te_p);
		display.writeText( line++, "TE Sensor: OK");
		failed_tests += "TE Sensor Test: PASSED\n";
	}

	if( selftestPassed ) {
		if( (abs(ba_t - te_t) >4.0)  && ( ias < 50 ) ) {   // each sensor has deviations, and new PCB has more heat sources
			selftestPassed = false;
			ESP_LOGI(FNAME,"Severe Temperature deviation delta > 4 °C between Baro and TE sensor: °C %f", abs(ba_t - te_t) );
			display.writeText( line++, "TE/Baro Temp: Unequal");
			failed_tests += "TE/Baro Sensor T diff. <4°C: FAILED\n";
		}
		else{
			ESP_LOGI(FNAME,"BMP 280 Temperature deviation test PASSED, dev: %f",  abs(ba_t - te_t));
			// display.writeText( line++, "TE/Baro Temp: OK");
			failed_tests += "TE/Baro Sensor T diff. <2°C: PASSED\n";
		}

		if( (abs(ba_p - te_p) >2.0)  && ( ias < 50 ) ) {
			selftestPassed = false;
			ESP_LOGI(FNAME,"Severe Pressure deviation delta > 2 hPa between Baro and TE sensor: %f", abs(ba_p - te_p) );
			display.writeText( line++, "TE/Baro P: Unequal");
			failed_tests += "TE/Baro Sensor P diff. <2hPa: FAILED\n";
		}
		else
			ESP_LOGI(FNAME,"BMP 280 Pressure deviation test PASSED, dev: %f", abs(ba_p - te_p) );
		// display.writeText( line++, "TE/Baro P: OK");
		failed_tests += "TE/Baro Sensor P diff. <2hPa: PASSED\n";

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
		failed_tests += "Digital Audio Poti test: FAILED\n";
	}
	else{
		ESP_LOGI(FNAME,"Digital potentiometer test PASSED");
		failed_tests += "Digital Audio Poti test: PASSED\n";
		display.writeText( line++, "Digital Poti: OK");
	}


	float bat = ADC.getBatVoltage(true);
	if( bat < 1 || bat > 28.0 ){
		ESP_LOGE(FNAME,"Error: Battery voltage metering out of bounds, act value=%f", bat );
		display.writeText( line++, "Bat Sensor: Failure");
		failed_tests += "Battery Voltage Sensor: FAILED\n";
		selftestPassed = false;
	}
	else{
		ESP_LOGI(FNAME,"Battery voltage metering test PASSED, act value=%f", bat );
		display.writeText( line++, "Bat Sensor: OK");
		failed_tests += "Battery Voltage Sensor: PASSED\n";
	}

	sleep( 0.5 );
	if( blue_enable.get() ) {
		if( btsender.selfTest() ){
			display.writeText( line++, "Bluetooth: OK");
			failed_tests += "Bluetooth test: PASSED\n";
		}
		else{
			display.writeText( line++, "Bluetooth: FAILED");
			failed_tests += "Bluetooth test: FAILED\n";
		}
	}

	Speed2Fly.change_polar();
	Speed2Fly.change_mc_bal();
	Version myVersion;
	ESP_LOGI(FNAME,"Program Version %s", myVersion.version() );
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);  // blue LED, maybe use for BT connection

	ESP_LOGI(FNAME,"%s", failed_tests.c_str());
	if( !selftestPassed )
	{
		ESP_LOGI(FNAME,"\n\n\nSelftest failed, see above LOG for Problems\n\n\n");
		if( !Rotary.readSwitch() )
			sleep(6);
	}
	else{
		ESP_LOGI(FNAME,"\n\n\n*****  Selftest PASSED  ********\n\n\n");
		display.writeText( line++, "Selftest PASSED");
		if( !Rotary.readSwitch() )
			sleep(3);
	}
	sleep(1);

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
			for( int j=0; j< LOOPS; j++ ) {
				speed += MP5004DP.readPascal(5);
				ba += bmpBA.readPressure();
				te += bmpTE.readPressure();
				delay( 33 );
			}
			ba = ba/LOOPS;
			te = te/LOOPS;
			speed = speed/LOOPS;
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
	Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);

	gpio_set_pull_mode(RESET_Display, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_Display, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_bme280BA, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_bme280TE, GPIO_PULLUP_ONLY );

	xTaskCreatePinnedToCore(&readBMP, "readBMP", 4096*2, NULL, 5, bpid, 0);  // 20
	xTaskCreatePinnedToCore(&drawDisplay, "drawDisplay", 4096*2, NULL, 4, dpid, 0);  // 10
	xTaskCreatePinnedToCore(&readTemp, "readTemp", 4096, NULL, 3, tpid, 0);

	Audio.startAudio();

	for( int i=0; i<36; i++ ) {
		if( i != 23 && i < 6 && i > 12  )
			gpio_set_drive_capability((gpio_num_t)i, GPIO_DRIVE_CAP_1);
	}

	vTaskDelete( NULL );

}

extern "C" int btstack_main(int argc, const char * argv[]){
	xTaskCreatePinnedToCore(&sensor, "sensor", 8192, NULL, 16, 0, 0);
	return 0;
}
