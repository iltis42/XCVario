#include "driver/gpio.h"
#include "driver/twai.h"
#include "logdef.h"
#include "canbus.h"
#include "string.h"
#include "esp_err.h"


void CANbus::begin( gpio_num_t tx_io, gpio_num_t rx_io )
{
    //Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(tx_io, rx_io, TWAI_MODE_NORMAL);
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_500KBITS();
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
    gpio_set_direction(GPIO_NUM_2, GPIO_MODE_OUTPUT);
    gpio_set_level(GPIO_NUM_2, 0 );

}

void CANbus::receive(){
	twai_message_t rx_message;
	ESP_ERROR_CHECK(twai_receive(&rx_message, pdMS_TO_TICKS(5) ));
	ESP_LOGI(FNAME,"RX CAN bus message id:%04x bytes:%d, data:%s", rx_message.identifier, rx_message.data_length_code, rx_message.data );
}

void CANbus::send( char* msg){
	ESP_LOGI(FNAME,"CANbus::send %d bytes", strlen(msg) );
	twai_message_t message;
	message.identifier = 0x555;
	message.extd = 1;
	message.self = 1;
	message.data_length_code = strlen(msg);

	for (int i = 0; i < strlen(msg); i++) {
	    message.data[i] = msg[i];
	}

	//Queue message for transmission
	esp_err_t error = twai_transmit(&message, pdMS_TO_TICKS(1000));
	if(error == ESP_OK)
		ESP_LOGI(FNAME,"Send CAN bus message okay");
	else
		ESP_LOGI(FNAME,"Send CAN bus message failed, errno:%s ret:%d",  strerror(errno), error );

}



