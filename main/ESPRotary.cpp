#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "ESPRotary.h"
#include <rom/ets_sys.h>
#include <sys/time.h>

gpio_num_t ESPRotary::clk, ESPRotary::dt, ESPRotary::sw;
std::vector<RotaryObserver *> ESPRotary::observers;
uint8_t ESPRotary::_switch;
QueueHandle_t ESPRotary::q1;
TickType_t ESPRotary::xLastWakeTime;
ring_buffer ESPRotary::rb(2);
int ESPRotary::dir=0;
int ESPRotary::last_dir=0;
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
int n=0;

void ESPRotary::readPos(void * args) {
	while( 1 ){
		int rotary;
		xQueueReceive(q1, &rotary, portMAX_DELAY);

		rb.push( rotary );
		n++;
		if(  (rb[1] == 3 && rb[0] == 0) /* ||  (rb[1] == 0 && rb[0] == 3) */ ){
				dir++;
		}
		else if (  (rb[1] == 1 && rb[0] == 2) /* ||  (rb[1] == 2 && rb[0] == 1) */ ){
				dir--;
		}
        int delta = dir - last_dir;
		printf("%d: delta: %d  rs: <%d %d>  \n", n, delta, rb[0], rb[1] );

		if( delta > 0 ) {
			// printf("Inform %d Observers up\n", observers.size());
			for (int i = 0; i < observers.size(); i++)
				observers[i]->up();
			last_dir = dir;
		}
		if( delta  < 0 ) {
			// printf("Inform %d Observers down\n", observers.size());
			for (int i = 0; i < observers.size(); i++)
				observers[i]->down();
			last_dir = dir;
		}
		else
			printf("no change\n");
	}
}

long gettime() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return ( tv.tv_sec*1000 + tv.tv_usec );
}

void ESPRotary::readPosInt(void * args) {
	int encoded = 0;
	int dtv=0;
	int clkv=0;
	for( int i=0; i<4; i++ ) {
		ets_delay_us(20);
		if (gpio_get_level(dt))
			dtv++;
		if (gpio_get_level(clk))
			clkv++;
	}
	if( dtv > 2 )
		encoded |= 2;
	if( clkv > 2 )
		encoded |= 1;
	if( last != encoded ) {  // do not sent flows of same signals, useless
		xQueueSendToBackFromISR(q1, &encoded, NULL);
		last = encoded;
	}
}

/////////////////////////////////////////////////////////////////
