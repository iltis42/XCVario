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
#include "PWMOut.h"
#include "S2F.h"
#include "Version.h"
#include "Switch.h"
#include "Polars.h"
#include "SetupVolt.h"

#include <SPI.h>
#include <Ucglib.h>


/*

BMP280:

  BMP  ESP   col  ESP32 GPIO_NUM optimized
                  TE-bmp280      Baro-bmp280
# VCC  GPIO13     13             13
# GND  GND    sw  GND            GND
# SCL  SCK    br  12             12
# SDA  MISO   rt  14             14
# CSB  CS     or  27             25    (diff)
# SDO  MOSI   ge  26             26

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
float speedP;
bool  enableBtTx=true;

DS18B20  ds18b20( GPIO_NUM_23 );  // GPIO_NUM_23 standard, alternative  GPIO_NUM_17
MP5004DP MP5004DP;
OpenVario OV;
xSemaphoreHandle xMutex=NULL;
Setup mysetup;
SetupVolt setupv;
BatVoltage ADC ( &mysetup, &setupv );
PWMOut pwm1;
S2F  s2f( &mysetup );
Switch VaSoSW;
TaskHandle_t *bpid;
TaskHandle_t *spid;
TaskHandle_t *tpid;
TaskHandle_t *dpid;

xSemaphoreHandle spiMutex=NULL;

BME280_ESP32_SPI bmpBA(SPI_SCLK, SPI_MOSI, SPI_MISO, CS_bme280BA, FREQ_BMP_SPI);
BME280_ESP32_SPI bmpTE(SPI_SCLK, SPI_MOSI, SPI_MISO, CS_bme280TE, FREQ_BMP_SPI);
Ucglib_ILI9341_18x240x320_HWSPI myucg( SPI_DC, CS_Display, RESET_Display );
IpsDisplay display( &myucg );

ESPRotary Rotary;
SetupMenu  Menu;

static float speed = 0;
static float aTE = 0;
static float alt;
static float aCl = 0;
static float netto = 0;
static float as2f = 0;
static float s2f_delta = 0;
static bool s2fmode = false;
static int ias = 0;
static float polar_sink = 0;
long millisec = millis();

void handleRfcommRx( char * rx, uint16_t len ){
	printf("RFCOMM packet, %s, len %d %d\n", rx, len, strlen( rx ));
}


BTSender btsender( handleRfcommRx  );

bool lastAudioDisable = false;

void drawDisplay(void *pvParameters){
	while (1) {
		TickType_t dLastWakeTime = xTaskGetTickCount();
		bool dis = Audio.getDisable();
		if( dis != true ) {
			display.drawDisplay( ias, TE, aTE, polar_sink, alt, temperature, battery, s2f_delta, as2f, aCl, s2fmode );
		}
		vTaskDelayUntil(&dLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}

void readBMP(void *pvParameters){

	while (1)
	{
		TickType_t xLastWakeTime = xTaskGetTickCount();
		if( Audio.getDisable() != true )
		{
			// long newmsec = millis();
			// if( abs (newmsec - millisec - 100 ) > 2 )
			// 	printf("Unsharp != 100: %d ms\n", int( newmsec - millisec ) );
			// millisec = newmsec;
			TE = bmpVario.readTE();
			// vTaskDelay(1);
			baroP = bmpBA.readPressure();
			speedP = MP5004DP.readPascal(30);
			// vTaskDelay(1);
			if( mysetup.get()->_alt_select == 0 ) // TE
			   alt = bmpVario.readAVGalt();
			else {
			   alt = bmpBA.calcAVGAltitude( mysetup.get()->_QNH, baroP );
			   // printf("BA p=%f alt=%f QNH=%f\n", baroP, alt, mysetup.get()->_QNH );
			}
			xSemaphoreTake(xMutex,portMAX_DELAY );
			if( enableBtTx ) {
				char lb[120];
				OV.makeNMEA( lb, baroP, speedP, TE, temperature );
				btsender.send( lb );
				// vTaskDelay(1);
			}
			xSemaphoreGive(xMutex);

			speed = speed + (MP5004DP.pascal2km( speedP, temperature ) - speed)*0.1;
			aTE = bmpVario.readAVGTE();
			aCl = bmpVario.readAvgClimb();
			vTaskDelay(1);
			polar_sink = s2f.sink( speed );
			netto = aTE - polar_sink;
			as2f = s2f.speed( netto );
			s2f_delta = as2f - speed;
			// vTaskDelay(1);
			ias = (int)(speed+0.5);

			switch( mysetup.get()->_audio_mode ) {
				case 0: // Vario
					s2fmode = false;
					break;
				case 1: // S2F
					s2fmode = true;
					break;
				case 2: // Switch
					s2fmode = VaSoSW.isClosed();
					break;
				case 3: // Auto
					if( (speed > mysetup.get()->_s2f_speed)  or VaSoSW.isClosed())
						s2fmode = true;
					else
						s2fmode = false;
					break;
			}
			Audio.setS2FMode( s2fmode );
// 			if( uxTaskGetStackHighWaterMark( bpid ) < 1000 )
//				printf("Warning Stack low: %d bytes\n", uxTaskGetStackHighWaterMark( bpid ) );
		}
		esp_task_wdt_reset();
		// delay(85);
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}

void audioTask(void *pvParameters){
	while (1) {
			TickType_t xLastWakeTime = xTaskGetTickCount();
			if( Audio.getDisable() != true )
			{
				Audio.setValues( TE, s2f_delta );
			}
			vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
		}
}


bool no_t_sensor=false;

void readTemp(void *pvParameters){
	while (1) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		if( Audio.getDisable() != true )
		{
			battery = ADC.getBatVoltage();
			// printf("Battery=%f V\n", battery );
			float t = ds18b20.getTemp();
			// xSemaphoreTake(xMutex,portMAX_DELAY );
			if( t ==  DEVICE_DISCONNECTED_C ) {
				validTemperature = false;
				temperature = DEVICE_DISCONNECTED_C;
				if( no_t_sensor == false ) {
					printf("Warning: No Temperatur Sensor found, please plug Temperature Sensor\n");
					no_t_sensor = true;
				}
			}
			else
			{
				validTemperature = true;
				temperature = temperature + (t-temperature)*0.2;
			}
			// printf("temperature=%f\n", temperature );
			// xSemaphoreGive(xMutex);
		}
		esp_task_wdt_reset();
		vTaskDelayUntil(&xLastWakeTime, 200/portTICK_PERIOD_MS);
	}
}


void sensor(void *args){
	bool selftestPassed=true;
	int line = 1;
	esp_wifi_set_mode(WIFI_MODE_NULL);
	spiMutex = xSemaphoreCreateMutex();
	esp_log_level_set("*", ESP_LOG_ERROR);
	NVS.begin();
	mysetup.begin();
	setupv.begin();
	ADC.begin();  // for battery voltage
	if( mysetup.get()->_blue_enable ) {
		// hci_power_control(HCI_POWER_ON);
		printf("BT Sender init, device name: %s\n", mysetup.getBtName() );
		btsender.begin( &enableBtTx, mysetup.getBtName() );
	}
	else
		printf("Bluetooth disabled\n");

	sleep( 1 );

	xMutex=xSemaphoreCreateMutex();
	uint8_t t_sb = 0;   //stanby 0: 0,5 mS 1: 62,5 mS 2: 125 mS
	uint8_t filter = 0; //filter O = off
	uint8_t osrs_t = 5; //OverSampling Temperature
	uint8_t osrs_p = 5; //OverSampling Pressure (5:x16 4:x8, 3:x4 2:x2 )
	uint8_t osrs_h = 0; //OverSampling Humidity x4
	uint8_t Mode = 3;   //Normal mode

	xSemaphoreTake(spiMutex,portMAX_DELAY );

	ds18b20.begin();
	SPI.begin( SPI_SCLK, SPI_MISO, SPI_MOSI, CS_bme280BA );
	xSemaphoreGive(spiMutex);
	display.begin( &mysetup );
	display.bootDisplay();

    // int valid;
	temperature = ds18b20.getTemp();
	String failed_tests;
	failed_tests += "\n\n\n";
	if( temperature == DEVICE_DISCONNECTED_C ) {
		printf("Error: Self test Temperatur Sensor failed; returned T=%2.2f\n", temperature );
		display.writeText( line++, "Temp Sensor: Not found");
		validTemperature = true;
		failed_tests += "External Temperature Sensor: NOT FOUND\n";
	}else
	{
		printf("Self test Temperatur Sensor PASSED; returned T=%2.2f\n", temperature );
		display.writeText( line++, "Temp Sensor:PASSED");
		validTemperature = false;
		failed_tests += "External Temperature Sensor:PASSED\n";

	}
	printf("BMP280 sensors init..\n");


	bmpBA.begin(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);
	bmpTE.begin(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);
	sleep( 1 );

	float ba_t, ba_p, te_t, te_p;

	if( ! bmpBA.selfTest( ba_t, ba_p) ) {
	    printf("HW Error: Self test Barometric Pressure Sensor failed!\n");
		display.writeText( line++, "Baro Sensor: Not found");
	    selftestPassed = false;
	    failed_tests += "Baro Sensor Test: NOT FOUND\n";
	}
	else {
		printf("Barometric Sensor T=%f P=%f\n", ba_t, ba_p);
	    display.writeText( line++, "Baro Sensor: OK");
	    failed_tests += "Baro Sensor Test: PASSED\n";
	}


	// bmpTE.selfTest(te_t, te_p);
	if( ! bmpTE.selfTest(te_t, te_p) ) {
	    printf("HW Error: Self test TE Pressure Sensor failed!\n");
	    display.writeText( line++, "TE Sensor: Not found");
	    selftestPassed = false;
	    failed_tests += "TE Sensor Test: NOT FOUND\n";
	}
	else {
		printf("TE Sensor         T=%f P=%f\n", te_t, te_p);
	    display.writeText( line++, "TE Sensor: OK");
	    failed_tests += "TE Sensor Test: PASSED\n";
	}

    if( selftestPassed ) {
		if( abs(ba_t - te_t) >2.0 ) {
			selftestPassed = false;
			printf("Severe Temperature deviation delta > 2 °C between Baro and TE sensor: °C %f\n", abs(ba_t - te_t) );
			display.writeText( line++, "TE/Baro Temp: Unequal");
			failed_tests += "TE/Baro Sensor T diff. <2°C: FAILED\n";
		}
		else{
			printf("BMP 280 Temperature deviation test PASSED, dev: %f\n",  abs(ba_t - te_t));
		    display.writeText( line++, "TE/Baro Temp: OK");
		    failed_tests += "TE/Baro Sensor T diff. <2°C: PASSED\n";
		}

		if( abs(ba_p - te_p) >2.0 ) {
			selftestPassed = false;
			printf("Severe Pressure deviation delta > 2 hPa between Baro and TE sensor: %f\n", abs(ba_p - te_p) );
			display.writeText( line++, "TE/Baro P: Unequal");
			failed_tests += "TE/Baro Sensor P diff. <2hPa: FAILED\n";
		}
		else
			printf("BMP 280 Pressure deviation test PASSED, dev: %f\n", abs(ba_p - te_p) );
		    display.writeText( line++, "TE/Baro P: OK");
		    failed_tests += "TE/Baro Sensor P diff. <2hPa: PASSED\n";

    }


	bmpVario.begin( &bmpTE, &mysetup );
	bmpVario.setup();
	VaSoSW.begin( GPIO_NUM_12 );

	printf("Speed sensors init..\n");
	MP5004DP.begin( GPIO_NUM_21, GPIO_NUM_22, mysetup.get()->_speedcal, &mysetup);  // sda, scl
	uint16_t val;
	bool works=MP5004DP.selfTest( val );

	if( !works ){
		printf("Error reading air speed pressure sensor MP5004DP->MCP3321 I2C returned error\n");
		display.writeText( line++, "IAS Sensor: Not found");
		failed_tests += "IAS Sensor: NOT FOUND\n";
		selftestPassed = false;
	}

	if( !MP5004DP.offsetPlausible( val )  ){
		printf("Error: IAS P sensor offset MP5004DP->MCP3321 out of bounds (608-1034), act value=%d\n", val );
		display.writeText( line++, "IAS Sensor: Offset Error");
		failed_tests += "IAS Sensor offset test: FAILED\n";
		selftestPassed = false;
	}
	else {
		printf("MP5004->MCP3321 test PASSED, readout value in bounds (608-1034)=%d\n", val );
		display.writeText( line++, "IAS Sensor: OK");
		failed_tests += "IAS Sensor offset test: PASSED\n";
	}

	MP5004DP.doOffset();

	Audio.begin( DAC_CHANNEL_1, GPIO_NUM_0, &mysetup );
    sleep( 0.1 );
	// TBD audio Poti test here.
	if( !Audio.selfTest() ) {
		printf("Error: Digital potentiomenter selftest failed\n");
		display.writeText( line++, "Digital Poti: Failure");
		selftestPassed = false;
		failed_tests += "Digital Audio Poti test: FAILED\n";
	}
	else{
		printf("Digital potentiometer test PASSED\n");
		failed_tests += "Digital Audio Poti test: PASSED\n";
		display.writeText( line++, "Digital Poti: OK");
	}


	float bat = ADC.getBatVoltage(true);
	if( bat < 1 || bat > 28.0 ){
		printf("Error: Battery voltage metering out of bounds, act value=%f\n", bat );
		display.writeText( line++, "Bat Sensor: Failure");
		failed_tests += "Battery Voltage Sensor: FAILED\n";
		selftestPassed = false;
	}
	else{
		printf("Battery voltage metering test PASSED, act value=%f\n", bat );
		display.writeText( line++, "Bat Sensor: OK");
		failed_tests += "Battery Voltage Sensor: PASSED\n";
	}

	sleep( 0.1 );
	if( mysetup.get()->_blue_enable ) {
		if( btsender.selfTest() ){
			display.writeText( line++, "Bluetooth: OK");
			failed_tests += "Bluetooth test: PASSED\n";
		}
		else{
			display.writeText( line++, "Bluetooth: FAILED");
			failed_tests += "Bluetooth test: FAILED\n";
		}
	}




	s2f.change_polar();
	s2f.change_mc_bal();


	Version myVersion;
	printf("Program Version %s\n", myVersion.version() );



	// Audio.mute( false );
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);  // blue LED, maybe use for BT connection

	printf("%s", failed_tests.c_str());
	if( !selftestPassed )
	{
		printf("\n\n\nSelftest failed, see above LOG for Problems\n\n\n");
		sleep(6);
	}
	else{
		printf("\n\n\n*****  Selftest PASSED  ********\n\n\n");
		display.writeText( line++, "Selftest PASSED");
		sleep(3);
	}


	sleep(1);
	speedP=MP5004DP.readPascal(30);
	speed = MP5004DP.pascal2km( speedP, temperature );
	printf("Speed=%f\n", speed);
	display.initDisplay();
	Menu.begin( &display, &Rotary, &mysetup, &setupv, &bmpBA, &ADC );
	if( speed < 50.0 ){
		xSemaphoreTake(xMutex,portMAX_DELAY );
		printf("QNH Autosetup, speed=%3f (<50 km/h)\n", speed );
		// QNH autosetup
		float ae = mysetup.get()->_elevation;
		baroP = bmpBA.readPressure();
		if( ae > 0 ) {
			float step=10.0; // 80 m
			float min=1000.0;
			float qnh_best = 1013.2;
			for( float qnh = 900; qnh< 1080; qnh+=step ) {
				float alt = bmpBA.readAltitude( qnh );
				float diff = alt - ae;
				printf("Alt diff=%4.2f  abs=%4.2f\n", diff, abs(diff) );
				if( abs( diff ) < 100 )
					step=1.0;  // 8m
				if( abs( diff ) < 10 )
					step=0.05;  // 0.4 m
				if( abs( diff ) < abs(min) ) {
					min = diff;
					qnh_best = qnh;
					printf("New min=%4.2f\n", min);
				}
				if( diff > 1.0 ) // we are ready, values get already positive
					break;
			}
			printf("qnh=%4.2f\n\n\n", qnh_best);
			mysetup.get()->_QNH = qnh_best;
		}

		SetupMenuValFloat::showQnhMenu();
		xSemaphoreGive(xMutex);
	}

	Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);

	gpio_set_pull_mode(RESET_Display, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_Display, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_bme280BA, GPIO_PULLUP_ONLY );
	gpio_set_pull_mode(CS_bme280TE, GPIO_PULLUP_ONLY );

	xTaskCreatePinnedToCore(&readBMP, "readBMP", 8000, NULL, 15, bpid, 0);
	xTaskCreatePinnedToCore(&audioTask, "audioTask", 4096, NULL, 30, 0, 0);

	xTaskCreatePinnedToCore(&drawDisplay, "drawDisplay", 8000, NULL, 10, dpid, 0);
	xTaskCreatePinnedToCore(&readTemp, "readTemp", 8000, NULL, 3, tpid, 0);




	printf("Free Stack: S:%d \n", uxTaskGetStackHighWaterMark( spid ) );

	vTaskDelete( NULL );

}

extern "C" int btstack_main(int argc, const char * argv[]){
	xTaskCreatePinnedToCore(&sensor, "sensor", 8000, NULL, 5, spid, 0);
	return 0;
}
