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

#include "ESP32NVS.h"
#include "BTSender.h"
#include "Setup.h"
#include "esp_sleep.h"
#include <esp_wifi.h>
#include "sensor.h"
#include "Version.h"
#include "rom/uart.h"

#include "nvs_flash.h"
#include "driver/uart.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "soc/uart_struct.h"


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


static uint8_t* data;
bool  enableBtTx=true;
static const uart_port_t uart_num = UART_NUM_0;

xSemaphoreHandle xMutex=NULL;
Setup setup;
TaskHandle_t *bpid;
TaskHandle_t *spid;
TaskHandle_t *tpid;

//                     mosi,    miso,         scl,           dc,       reset,        cs



void handleRfcommRx( char * rx, uint16_t len ){
	printf("RFCOMM packet, %s, len %d %d\n", rx, len, strlen( rx ));
	int l = uart_write_bytes(uart_num, (const char*) rx, len);
	if( l < 0) {
		printf("Error sending BT -> serial TX %s %d\n", rx, len);
	}
}


BTSender btsender( handleRfcommRx  );

#define BUF_SIZE 2048

void readData(void *pvParameters){
	while (1) {
//		TickType_t xLastWakeTime = xTaskGetTickCount();
		int len = uart_read_bytes(uart_num, data, 10, 200 / portTICK_RATE_MS);
		// data[len+1] = 0;
		//Write data back to BT
		if( len > 0 ) {
//			printf( "UART RX: %d %s\n", len, (char *)data);
			btsender.send( (char *)data, len );
		}
		esp_task_wdt_reset();
		// vTaskDelayUntil(&xLastWakeTime, 500/portTICK_PERIOD_MS);
	}
}


#define UART0_TXD  (1)
#define UART0_RXD  (3)




void sensor(void *args){
	esp_wifi_set_mode(WIFI_MODE_NULL);
	esp_log_level_set("*", ESP_LOG_INFO);
	NVS.begin();

	uart_config_t uart_config = {
	    .baud_rate = 19200,
	    .data_bits = UART_DATA_8_BITS,
	    .parity = UART_PARITY_DISABLE,
	    .stop_bits = UART_STOP_BITS_1,
	    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
	    .rx_flow_ctrl_thresh = 0,
		.use_ref_tick = false
	};
	// Configure UART parameters
	ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
	uart_set_pin(uart_num, UART0_TXD, UART0_RXD, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
	data = (uint8_t*) malloc(BUF_SIZE);
	//Install UART driver (we don't need an event queue here)
	uart_driver_install(uart_num, BUF_SIZE, 0, 0, NULL, 0);

	setup.begin();
	sleep( 1 );

	xMutex=xSemaphoreCreateMutex();
	xTaskCreatePinnedToCore(&readData, "readData", 8000, NULL, 6, bpid, 0);
	Version myVersion;
	printf("Program Version %s\n", myVersion.version() );


	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);  // blue LED, maybe use for BT connection
	hci_power_control(HCI_POWER_ON);
	printf("BT Sender init, device name: %s\n", setup.getBtName() );
	btsender.begin( &enableBtTx, setup.getBtName() );

	printf("Free Stack: S:%d \n", uxTaskGetStackHighWaterMark( spid ) );
	vTaskDelete( NULL );

}

extern "C" int btstack_main(int argc, const char * argv[]){
	xTaskCreatePinnedToCore(&sensor, "sensor", 8000, NULL, 5, spid, 0);
	return 0;
}
