#include "driver/gpio.h"
#include "driver/twai.h"
#include "logdef.h"
#include "canbus.h"
#include "string.h"
#include "esp_err.h"
#include "sensor.h"

// TWAI_IO_UNUSED

int CANbus::_tick = 0;

#define MY_TWAI_GENERAL_CONFIG_DEFAULT(tx_io_num, rx_io_num, op_mode) {.mode = op_mode, .tx_io = tx_io_num, .rx_io = rx_io_num,     \
                                                                    .clkout_io = TWAI_IO_UNUSED, .bus_off_io = TWAI_IO_UNUSED,          \
                                                                    .tx_queue_len = 5, .rx_queue_len = 5,                           \
                                                                    .alerts_enabled = TWAI_ALERT_ALL,  .clkout_divider = 0,         \
                                                                    .intr_flags = ESP_INTR_FLAG_LEVEL1}


void CANbus::begin( gpio_num_t tx_io, gpio_num_t rx_io )
{
    //Initialize configuration structures using macro initializers
    twai_general_config_t g_config = MY_TWAI_GENERAL_CONFIG_DEFAULT(tx_io, rx_io, TWAI_MODE_NO_ACK);
    // twai_general_config_t g_config = MY_TWAI_GENERAL_CONFIG_DEFAULT(tx_io, rx_io, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS(); // TWAI_TIMING_CONFIG_500KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    //Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(FNAME,"Driver installed");
    } else {
        ESP_LOGI(FNAME,"Failed to install driver");
        return;
    }

    //Start TWAI driver
    if (twai_start() == ESP_OK) {
        ESP_LOGI(FNAME,"Driver started");
    } else {
        ESP_LOGI(FNAME,"Failed to start driver");
        return;
    }

    // Set RS pin
    // bus_off_io may operate invers, so for now set this here
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2, 0 );
}

void CANbus::receive(){
	twai_message_t rx_message;
	esp_err_t ret = twai_receive(&rx_message, pdMS_TO_TICKS(5) );
	if(ret == ESP_OK){
		char rec[9];
		rec[0] = 0;
		if( rx_message.data_length_code <= 8 ){
			rec[rx_message.data_length_code] = 0;
			memcpy( rec,rx_message.data, rx_message.data_length_code );
		}
		ESP_LOGI(FNAME,"RX CAN bus message id:%04x bytes:%d, data:%s", rx_message.identifier, rx_message.data_length_code, rec );
	}
	/*
	else
		ESP_LOGI(FNAME,"RX CAN bus receive error");
	*/
}

void CANbus::tick(){
	_tick++;
	if( !(_tick%10) ){
		test();
	}
}

void CANbus::test(){
	gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
	gpio_set_level(GPIO_NUM_2, 0 );
	// send( "12345678" );
	// gpio_set_direction(GPIO_NUM_2, GPIO_MODE_INPUT);
	receive();
}


void CANbus::send( char* msg){
	ESP_LOGI(FNAME,"CANbus::send %d bytes", strlen(msg) );
	int len = strlen( msg );
	twai_message_t message;
	memset( &message, 0, len );
	message.identifier = 0x555;
	message.extd = 0;
	message.self = 0;
	// message.flags = 0;
	message.data_length_code = len;

	for (int i = 0; i < len; i++) {
	    message.data[i] = msg[i];
	}

	//Queue message for transmission
	esp_err_t error = twai_transmit(&message, pdMS_TO_TICKS(1000));
	if(error == ESP_OK)
		ESP_LOGI(FNAME,"Send CAN bus message okay");
	else
		ESP_LOGI(FNAME,"Send CAN bus message failed, errno:%s ret:%d",  strerror(errno), error );

}



