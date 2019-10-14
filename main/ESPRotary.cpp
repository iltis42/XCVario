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
	q1 = xQueueCreate(4, sizeof(int));
	gpio_isr_handler_add(clk, ESPRotary::readPosInt, NULL);
	gpio_isr_handler_add(sw, ESPRotary::readPosInt, NULL);
	xTaskCreatePinnedToCore(&ESPRotary::readPos, "readPos", 8192, NULL, 10, NULL, 0);
}

// receiving from Interrupt the rotary direction and switch
int n=0;

void ESPRotary::readPos(void * args) {
	int rotary = 0;
	int num = xQueueReceive(q1, &rotary, portMAX_DELAY);
	while( num > 0 ){
        // printf("rotary byte: %x num msg: %d\n", rotary, num );
		rb.push( rotary & 0x03 );
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
			printf("no rotary change\n");
		int newsw = (rotary & 0x04);

		if( _switch_state != newsw ){
			_switch_state = newsw;
			if( newsw ){      // pullup, so not pressed is 1
					// printf("sw high");
					for (int i = 0; i < observers.size(); i++)
						observers[i]->release();
				}
				else{
					// printf("sw low");
					for (int i = 0; i < observers.size(); i++)
						observers[i]->press();
				}
		}
		else{
				printf("no sw change\n");
		}
		num = xQueueReceive(q1, &rotary, portMAX_DELAY);
	}
}

void ESPRotary::readPosInt(void * args) {
	int encoded = 0;
	int dtv=0;
	int clkv=0;
	int swv=0;
	for( int i=0; i<8; i++ ) {
		ets_delay_us(20);
		if (gpio_get_level(dt))
			dtv++;
		if (gpio_get_level(clk))
			clkv++;
		if (gpio_get_level(sw))
			swv++;
	}
	if( clkv > 4 )
		encoded |= 1;
	if( dtv > 4 )
		encoded |= 2;
	if( swv > 4 )
		encoded |= 4;
	if( last != encoded ) {  // do not sent flows of same signals, useless
		xQueueSendToBackFromISR(q1, &encoded, NULL);
		last = encoded;
	}
}

/////////////////////////////////////////////////////////////////
