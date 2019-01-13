#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "ESPRotary.h"
#include <rom/ets_sys.h>

gpio_num_t ESPRotary::clk, ESPRotary::dt, ESPRotary::sw;
std::vector<RotaryObserver *> ESPRotary::observers;
uint8_t ESPRotary::_switch;
QueueHandle_t ESPRotary::q1;
TickType_t ESPRotary::xLastWakeTime;
ring_buffer ESPRotary::rb(4);
int ESPRotary::dir=0;
int ESPRotary::last=0;
int ESPRotary::_switch_state=1;

void ESPRotary::attach(RotaryObserver *obs) {
	observers.push_back(obs);
}

ESPRotary::ESPRotary() {
	xMutex = xSemaphoreCreateMutex();
}

void ESPRotary::begin(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw ) {
	clk = aclk;
	dt = adt;
	sw = asw;

	// gpio_set_direction(dt, GPIO_MODE_INPUT);
	// gpio_set_pull_mode(dt, GPIO_PULLUP_ONLY);
	// gpio_set_direction(clk, GPIO_MODE_INPUT);
	// gpio_set_pull_mode(clk, GPIO_PULLUP_ONLY);
	gpio_set_direction(sw, GPIO_MODE_INPUT);
	gpio_set_pull_mode(sw, GPIO_PULLUP_ONLY);

	gpio_config_t gpioConfig;
	memset(&gpioConfig, 0, sizeof(gpioConfig));
	gpioConfig.pin_bit_mask |= (1 << clk);
	gpioConfig.pin_bit_mask |= (1 << dt);
	gpioConfig.mode = GPIO_MODE_INPUT;
	gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
	gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpioConfig.intr_type = GPIO_INTR_ANYEDGE;
	gpio_config(&gpioConfig);
	gpio_install_isr_service(0);
	q1 = xQueueCreate(40, sizeof(int));

	gpio_isr_handler_add(clk, ESPRotary::readPosInt, NULL);
	xTaskCreatePinnedToCore(&ESPRotary::readSwitch, "readSwitch", 4096, NULL, 5, NULL, 0);
	xTaskCreatePinnedToCore(&ESPRotary::readPos, "readPos", 4096, NULL, 5, NULL, 0);
}


// we poll the switch
void ESPRotary::readSwitch(void * args) {
	while (1) {
		TickType_t xLastWakeTime = xTaskGetTickCount();
		int _sw = 0;
		if (gpio_get_level(sw))
			_sw = 1;
		else
			_sw = 0;

		if (_switch != _sw) {
			_switch_state = _sw;
			// printf("sw=%d\n", _sw);
			if (_sw == 0) // we assume that switch pressed pulls down
			{

				// printf("Inform %d Observers press\n", observers.size());
				for (int i = 0; i < observers.size(); i++)
					observers[i]->press();
			} else {
				// printf("Inform %d Observers release \n", observers.size());
				for (int i = 0; i < observers.size(); i++)
					observers[i]->release();
			}
			_switch = _sw;
		}
		vTaskDelayUntil(&xLastWakeTime, 50 / portTICK_PERIOD_MS);
	}
}


// receiving from Interrupt the rotary direction

void ESPRotary::readPos(void * args) {
	while( 1 ){
		int rotary;
		xQueueReceive(q1, &rotary, portMAX_DELAY);

		rb.push( rotary );
		if( (rb[3] == 3 && rb[2] == 0) || (rb[3] == 0 && rb[2] == 3) ){
			if( dir < 0 )
				dir = 0;
			dir++;
		}
		else if ( (rb[3] == 1 && rb[2] == 2) || (rb[3] == 2 && rb[2] == 1) ){
			if( dir > 0 )
				dir = 0;
			dir--;
		}

		if( dir >= 2 ) {
			dir = 0;
			// printf("Inform %d Observers up\n", observers.size());
			for (int i = 0; i < observers.size(); i++)
				observers[i]->up();
		}
		if( dir <= -2 ) {
			dir = 0;
			// printf("Inform %d Observers down\n", observers.size());
			for (int i = 0; i < observers.size(); i++)
				observers[i]->down();
		}
		// fflush(stdout);
	}
}


// interrupt routine for position
void ESPRotary::readPosInt(void * args) {
	ets_delay_us(200); // wait until rotary switch has debounced
	int encoded = 0;
	if (gpio_get_level(dt))
		encoded |= 1;
	if (gpio_get_level(clk))
		encoded |= 2;
	if( last != encoded ) {  // do not sent flows of same signals, useless
		last = encoded;
		xQueueSendToBackFromISR(q1, &encoded, NULL);
	}
}

/////////////////////////////////////////////////////////////////
