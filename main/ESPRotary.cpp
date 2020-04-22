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
#include "RingBufCPP.h"


gpio_num_t ESPRotary::clk, ESPRotary::dt, ESPRotary::sw;
std::vector<RotaryObserver *> ESPRotary::observers;

pcnt_config_t ESPRotary::enc;
int16_t ESPRotary::r_enc_count = 0;

void ESPRotary::attach(RotaryObserver *obs) {
	observers.push_back(obs);
}

void ESPRotary::begin(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw ) {
	clk = aclk;
	dt = adt;
	sw = asw;
	xTaskCreatePinnedToCore(&ESPRotary::informObservers, "informObservers", 1024*8, NULL, 29, NULL, 0);

	enc.pulse_gpio_num = clk; //Rotary Encoder Chan A
	enc.ctrl_gpio_num = dt;	 //Rotary Encoder Chan B
	enc.unit = PCNT_UNIT_0;
	enc.channel = PCNT_CHANNEL_0;

	enc.pos_mode = PCNT_COUNT_INC; //Count Only On Rising-Edges
	enc.neg_mode = PCNT_COUNT_DIS;	// Discard Falling-Edge

	enc.lctrl_mode = PCNT_MODE_KEEP;    // Rising A on HIGH B = CW Step
	enc.hctrl_mode = PCNT_MODE_REVERSE; // Rising A on LOW B = CCW Step

	enc.counter_h_lim = 100;
	enc.counter_l_lim = -100;

	pcnt_unit_config(&enc);
	pcnt_set_filter_value(PCNT_UNIT_0, 250);  // Filter Runt Pulses
	pcnt_filter_enable(PCNT_UNIT_0);

	gpio_set_direction(sw,GPIO_MODE_INPUT);
	gpio_pullup_en(sw); // Rotary Encoder Button
	gpio_pullup_en(dt);
	gpio_pullup_en(clk);

	pcnt_counter_pause(PCNT_UNIT_0); // Initial PCNT init
	pcnt_counter_clear(PCNT_UNIT_0);
	pcnt_counter_resume(PCNT_UNIT_0);
}

int16_t old_cnt = 0;
int old_button;

void ESPRotary::informObservers( void * args )
{
	while( 1 ) {
		int button = gpio_get_level((gpio_num_t)sw);
		if( button != old_button )
		{
//			printf("Rotary button:%d\n", button);
			int event = NONE;
			if( button==0 )
				event = PRESS;
			else
				event = RELEASE;
			old_button=button;
			if( event == RELEASE ){
				printf("Switch released action\n");
				for (int i = 0; i < observers.size(); i++)
					observers[i]->release();
				printf("End Switch released action\n");
			}
			else if ( event == PRESS ){
				printf("Switch pressed action\n");
				for (int i = 0; i < observers.size(); i++)
					observers[i]->press();
				printf("End Switch pressed action\n");
			}
			else if ( event == LONG_PRESS ){   // tbd
				printf("Switch long pressed action\n");
				for (int i = 0; i < observers.size(); i++)
					observers[i]->longPress();
				printf("End Switch long pressed action\n");
			}
		}
		if( pcnt_get_counter_value(PCNT_UNIT_0, &r_enc_count) != ESP_OK )
				printf("Error get counter\n");
		if( r_enc_count != old_cnt )
		{
			// printf("Rotary counter %d\n", r_enc_count );
			int diff = r_enc_count - old_cnt;
			pcnt_counter_clear(PCNT_UNIT_0);
			if( diff < 0 ) {
				printf("Rotary up %d times\n",abs(diff) );
				for (int i = 0; i < observers.size(); i++)
					observers[i]->up( abs(diff) );
			}
			else if( diff > 0 ) {
				printf("Rotary down %d times\n", abs(diff) );
				for (int i = 0; i < observers.size(); i++)
					observers[i]->down( abs(diff) );
			}
		}
		vTaskDelay(20 / portTICK_PERIOD_MS);
	}
}
