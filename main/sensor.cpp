#include <btstack.h> 

#include <esp_log.h>
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
#include "mcp3221.h"
#include "ESP32NVS.h"
#include "MP5004DP.h"
#include "BMPVario.h"
#include "BTSender.h"
#include "OpenVario.h"
#include "DS18B20.h"
#include "SetupCMD.h"
#include "esp_sleep.h"
#include "ESPAudio.h"
#include <esp_wifi.h>
#include "SetupMenu.h"
#include "ESPRotary.h"
#include "BatVoltage.h"
#include "DotDisplay.h"
#include "sensor.h"
#include "PWMOut.h"
#include "S2F.h"

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

const gpio_num_t SCLK_bme280 = GPIO_NUM_14; //
const gpio_num_t MISO_bme280 = GPIO_NUM_27; // SDA Master Input Slave Output
#ifdef VARIO
const gpio_num_t CS_bme280TE = GPIO_NUM_33; // CS pin
#else
const gpio_num_t CS_bme280TE = GPIO_NUM_26; // CS pin 26
#endif


const gpio_num_t MOSI_bme280 = GPIO_NUM_32; //  SDO Master Output Slave Input ESP32=Master,BME280=slave

const gpio_num_t CS_bme280BA = GPIO_NUM_33; //CS pin

BME280_ESP32_SPI bmpTE(SCLK_bme280, MOSI_bme280, MISO_bme280, CS_bme280TE, 2000000);
BME280_ESP32_SPI bmpBA(SCLK_bme280, MOSI_bme280, MISO_bme280, CS_bme280BA, 2000000);

float baroP=0;
float temperature=15.0;
float battery=0.0;
float TE=0;
float speedP;
bool  enableBtTx=true;

DS18B20  ds18b20( GPIO_NUM_23 );  // GPIO_NUM_23 worked before
MP5004DP MP5004DP;
OpenVario OV;
xSemaphoreHandle xMutex=NULL;
SetupCMD setup( &enableBtTx );
BatVoltage ADC ( &setup );
PWMOut pwm1;
S2F  s2f( &setup );

//                     mosi,    miso,         scl,           dc,       reset,        cs
DotDisplay display( MOSI_bme280, MISO_bme280, SCLK_bme280, GPIO_NUM_15, GPIO_NUM_5, GPIO_NUM_13 );
/*

u8g2_esp32_hal.mosi  = mosi; // MOSI_bme280;
u8g2_esp32_hal.clk   = scl; // SCLK_bme280;
u8g2_esp32_hal.dc    = dc; // GPIO_NUM_15;
u8g2_esp32_hal.reset = reset; // GPIO_NUM_5;
u8g2_esp32_hal.cs    = cs; // GPIO_NUM_26;


 */


ESPRotary Rotary;
SetupMenu  Menu;

void handleRfcommRx( char * rx, uint16_t len ){
	printf("RFCOMM packet, %s, len %d %d\n", rx, len, strlen( rx ));
	setup.doSetup( rx );

}

BTSender btsender( handleRfcommRx );

void readBMP(void *pvParameters){
	while (1) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		if( Audio.getDisable() != true )
		{
			TE = bmpVario.readTE();
			baroP = bmpBA.readPressure();
			speedP = MP5004DP.readPascal(30);
			float alt;
			if( setup.get()->_alt_select == 0 ) // TE
			   alt = bmpVario.readAVGalt();
			else
			   alt = bmpBA.calcAVGAltitude( setup.get()->_QNH, baroP );
			xSemaphoreTake(xMutex,portMAX_DELAY );
			char lb[100];
			if( enableBtTx ) {
				OV.makeNMEA( lb, baroP, speedP, TE, temperature );
				btsender.send( lb );
			}
			xSemaphoreGive(xMutex);
			Audio.setTE( TE );
			battery = ADC.getBatVoltage();
			float speed = MP5004DP.pascal2km( speedP, temperature );
			float aTE = bmpVario.readAVGTE();
			float aCl = bmpVario.readAvgClimb();
			float as2f = s2f.speed( aTE );
			float s2f_delta = as2f - speed;
			// printf("V %f, S2F %f delta: %f\n", speed, as2f, s2f_delta );
			// printf("TE %0.1f avTE %0.1f\n", TE, aTE );
			display.setData( TE, aTE, alt, temperature, battery, s2f_delta, as2f, aCl );
			setup.tick();
		}
		esp_task_wdt_reset();
		vTaskDelayUntil(&xLastWakeTime, 100/portTICK_PERIOD_MS);
	}
}

void readTemp(void *pvParameters){
	while (1) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		if( Audio.getDisable() != true )
		{
			float t = ds18b20.getTemp();

			xSemaphoreTake(xMutex,portMAX_DELAY );
			temperature = t;
			xSemaphoreGive(xMutex);
		}
		esp_task_wdt_reset();
		vTaskDelayUntil(&xLastWakeTime, 2000/portTICK_PERIOD_MS);
	}
}


void sensor(void *args){
	esp_wifi_set_mode(WIFI_MODE_NULL);
	esp_log_level_set("*", ESP_LOG_INFO);
	NVS.begin();

	setup.begin( &btsender );
	display.begin( &setup );
	Audio.begin( DAC_CHANNEL_1, GPIO_NUM_0, &setup );

	xMutex=xSemaphoreCreateMutex();
	uint8_t t_sb = 0;   //stanby 0: 0,5 mS 1: 62,5 mS 2: 125 mS
	uint8_t filter = 0; //filter O = off
	uint8_t osrs_t = 2; //OverSampling Temperature
	uint8_t osrs_p = 5; //OverSampling Pressure (5:x16 4:x8, 3:x4 2:x2 )
	uint8_t osrs_h = 0; //OverSampling Humidity x4
	uint8_t Mode = 3;   //Normal mode
	printf("BMP280 sensors init..\n");

    bmpTE.begin(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);
	bmpBA.begin(t_sb, filter, osrs_t, osrs_p, osrs_h, Mode);
	bmpVario.begin( &bmpTE, &setup );
	bmpVario.setup();

	printf("Speed sensors init..\n");
	MP5004DP.begin( GPIO_NUM_21, GPIO_NUM_22, setup.get()->_speedcal, &setup);  // sda, scl
	MP5004DP.doOffset();
	ADC.begin();

	pwm1.init( GPIO_NUM_18 );
    pwm1.setContrast( setup.get()->_contrast_adj );
    s2f.change_polar();
	s2f.change_mc_bal();
	xTaskCreatePinnedToCore(&readBMP, "readBMP", 16000, NULL, 5, NULL, 0);
	printf("Mute end..\n");

	Audio.mute( false );
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);  // blue LED, maybe use for BT connection
	if( setup.get()->_blue_enable ) {
		hci_power_control(HCI_POWER_ON);
		printf("BT Sender init, device name: %s\n", setup.getBtName() );
		btsender.begin( &enableBtTx, setup.getBtName() );
	}
	else
		printf("Bluetooth disabled\n");
	// vTaskDelay(20000 / portTICK_PERIOD_MS);
	ds18b20.begin();
	xTaskCreatePinnedToCore(&readTemp, "readTemp", 2048, NULL, 5, NULL, 0);
	Rotary.begin( GPIO_NUM_4, GPIO_NUM_2, GPIO_NUM_0);
	Menu.begin( &display, &Rotary, &setup, &bmpBA );

	vTaskDelete( NULL );

}

extern "C" int btstack_main(int argc, const char * argv[]){
	xTaskCreatePinnedToCore(&sensor, "sensor", 8096, NULL, 5, NULL, 0);
	return 0;
}
