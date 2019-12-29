#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include "ESPRotary.h"
#include <esp32/rom/ets_sys.h>
#include <sys/time.h>
#include <Arduino.h>


gpio_num_t ESPRotary::clk, ESPRotary::dt, ESPRotary::sw;
std::vector<RotaryObserver *> ESPRotary::observers;
uint8_t ESPRotary::_switch;
QueueHandle_t ESPRotary::q1;
QueueHandle_t ESPRotary::q2;
TickType_t ESPRotary::xLastWakeTime;
ring_buffer ESPRotary::rb(2);
int ESPRotary::dir=0;
int ESPRotary::last_dir=0;
int ESPRotary::last=0;
int ESPRotary::_switch_state=1;
int ESPRotary::n=0;
long ESPRotary::lastmilli=0;
int ESPRotary::errors=0;
uint64_t ESPRotary::swtime=0;
SemaphoreHandle_t ESPRotary::xBinarySemaphore;


void ESPRotary::attach(RotaryObserver *obs) {
	observers.push_back(obs);
}

ESPRotary::ESPRotary() {
	swMutex = xSemaphoreCreateMutex();
}

void ESPRotary::begin(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw ) {
	clk = aclk;
	dt = adt;
	sw = asw;
	gpio_config_t gpioConfig;
	memset(&gpioConfig, 0, sizeof(gpioConfig));
	gpioConfig.pin_bit_mask |= (1 << clk);
	gpioConfig.pin_bit_mask |= (1 << dt);
	gpioConfig.pin_bit_mask |= (1 << sw);
	gpioConfig.mode = GPIO_MODE_INPUT;
	gpioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
	gpioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
	gpioConfig.intr_type = GPIO_INTR_ANYEDGE;
	gpio_config(&gpioConfig);
	gpio_install_isr_service(0);
	q1 = xQueueCreate(20, sizeof(struct _rotbyte) );
	q2 = xQueueCreate(20, sizeof(enum _event));
	rb.push( 0 );
	rb.push( 0 );
	gpio_isr_handler_add(clk, ESPRotary::readPosInt, NULL);
	gpio_isr_handler_add(sw, ESPRotary::readPosInt, NULL);
	xTaskCreatePinnedToCore(&ESPRotary::readPos, "readPos", 4096, NULL, 20, NULL, 0);
	xTaskCreatePinnedToCore(&ESPRotary::informObservers, "informObservers", 8192, NULL, 5, NULL, 0);
	xBinarySemaphore = xSemaphoreCreateBinary();
}

void ESPRotary::informObservers( void * args )
{
	enum _event info;
	int num = xQueueReceive(q2, &info, portMAX_DELAY);
	while( num > 0 ){
		if( info == UP ) {
			printf("Rotary up\n");
			for (int i = 0; i < observers.size(); i++)
				observers[i]->up();
		}
		else if( info == DOWN ) {
			printf("Rotary down\n");
			for (int i = 0; i < observers.size(); i++)
				observers[i]->down();
		}
		else if( info == RELEASE ){      // pullup, so not pressed is 1
			printf("Switch released action\n");
			for (int i = 0; i < observers.size(); i++)
				observers[i]->release();
			printf("End Switch released action\n");
		}
		else if ( info == PRESS ){
			printf("Switch pressed action\n");
			for (int i = 0; i < observers.size(); i++)
				observers[i]->press();
			printf("End Switch pressed action\n");
		}
		else if ( info == LONG_PRESS ){
					printf("Switch long pressed action\n");
					for (int i = 0; i < observers.size(); i++)
						observers[i]->longPress();
					printf("End Switch long pressed action\n");
				}
		else if ( info == ERROR ){
					printf("ERROR ROTARY SEQ %d\n", errors++ );
		}
		num = xQueueReceive(q2, &info, portMAX_DELAY);
	}
}

// receiving from Interrupt the rotary direction and switch
void ESPRotary::readPos(void * args) {
    struct _rotbyte rotary;
	int num = xQueueReceive(q1, &rotary, portMAX_DELAY);
	enum _event var;
	while( num > 0 ){
		n++;
        rb.push( rotary.rot & 3 );

		if( (rb[0] == 0) && (rb[1] == 3) ) {
			dir--;
		}
		else if ( (rb[0] == 1) && (rb[1] == 2) ) {
			dir++;
		}
		else if( (rb[0] == 3) && (rb[1] == 0) ) {
			 printf("Rotary 30\n");
		}
		else if ( (rb[0] == 2) && (rb[1] == 1) ) {
			 printf("Rotary 21\n");
		}
		else {
			var = ERROR;
			xQueueSend(q2, &var, portMAX_DELAY );
		}
		int delta = dir - last_dir;

		if( delta < 0 ) {
			// printf("Rotary up\n");
			var = UP;
			xQueueSend(q2, &var, portMAX_DELAY );
		}
		if( delta  > 0 ) {
			// printf("Rotary down\n");
			var = DOWN;
			xQueueSend(q2, &var, portMAX_DELAY );
		}
		last_dir = dir;

		int newsw = (rotary.rot & 0x04);
		if( _switch_state != newsw ){
			_switch_state = newsw;
			if( newsw ){      // pullup, so not pressed is 1
				if( (swtime != 0) && (rotary.time - swtime) > 3000 ){
					var = LONG_PRESS;
					xQueueSend(q2, &var, portMAX_DELAY );
					swtime = 0;
					}
				var = RELEASE;
				xQueueSend(q2, &var, portMAX_DELAY );

				}
				else{
					var = PRESS;
					xQueueSend(q2, &var, portMAX_DELAY );
					swtime = rotary.time;
				}
		}
		num = xQueueReceive(q1, &rotary, portMAX_DELAY);
	}
}

#define NUM_LOOPS 100


void ESPRotary::readPosInt(void * args) {
	struct _rotbyte var;
	var.rot = 0;
	var.time = esp_timer_get_time()/1000;
	int dtv=0;
	int swv=0;
	int clkv=0;
	// debounce//
	for( int i=0; i<NUM_LOOPS; i++ ) {
		ets_delay_us(10);
		if (gpio_get_level(dt))
			dtv++;
		if (gpio_get_level(sw))
			swv++;
		if (gpio_get_level(clk))
			clkv++;
	}
	if( dtv > NUM_LOOPS/2 )
		var.rot |= 1;
	if( clkv > NUM_LOOPS/2 )
			var.rot |= 2;
	if( swv > NUM_LOOPS/2 )
		var.rot |= 4;

	xQueueSendToBackFromISR(q1, &var, NULL );
}

/////////////////////////////////////////////////////////////////
