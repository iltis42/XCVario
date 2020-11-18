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
#include <logdef.h>
#include "Setup.h"


gpio_num_t ESPRotary::clk, ESPRotary::dt;
gpio_num_t ESPRotary::sw = GPIO_NUM_0;
std::vector<RotaryObserver *> ESPRotary::observers;

pcnt_config_t ESPRotary::enc;
pcnt_config_t ESPRotary::enc2;
int16_t ESPRotary::r_enc_count  = 0;
int16_t ESPRotary::r_enc2_count = 0;

void ESPRotary::attach(RotaryObserver *obs) {
	observers.push_back(obs);
}

bool ESPRotary::readSwitch() {
	gpio_set_direction(sw,GPIO_MODE_INPUT);
	gpio_pullup_en(sw);
	if( gpio_get_level((gpio_num_t)sw) )
		return false;
	else
		return true;
}

void ESPRotary::begin(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw ) {
	clk = aclk;
	dt = adt;
	sw = asw;

	gpio_set_direction(sw,GPIO_MODE_INPUT);
	gpio_pullup_en(sw); // Rotary Encoder Button
	gpio_pullup_en(dt);
	gpio_pullup_en(clk);

	enc.pulse_gpio_num = clk; //Rotary Encoder Chan A
	enc.ctrl_gpio_num = dt;	 //Rotary Encoder Chan B
	enc.unit = PCNT_UNIT_0;
	enc.channel = PCNT_CHANNEL_0;

	enc.pos_mode = PCNT_COUNT_INC; //Count Only On Rising-Edges
	enc.neg_mode = PCNT_COUNT_DIS;	// Discard Falling-Edge

	enc.lctrl_mode = PCNT_MODE_KEEP;    // Rising A on HIGH B = CW Step
	enc.hctrl_mode = PCNT_MODE_REVERSE; // Rising A on LOW B = CCW Step

	enc.counter_h_lim = 32000;
	enc.counter_l_lim = -32000;

	pcnt_unit_config(&enc);
	pcnt_set_filter_value(PCNT_UNIT_0, 250);  // Filter Runt Pulses
	pcnt_filter_enable(PCNT_UNIT_0);

	pcnt_counter_pause(PCNT_UNIT_0); // Initial PCNT init
	pcnt_counter_clear(PCNT_UNIT_0);
	pcnt_counter_resume(PCNT_UNIT_0);


	if( rotary_inc.get() == 0 ) {   // Single step type, need both counters
		// second encoder for incrementing on each rotary step:
		enc2.pulse_gpio_num = clk; //Rotary Encoder Chan A
		enc2.ctrl_gpio_num = dt;	 //Rotary Encoder Chan B
		enc2.unit = PCNT_UNIT_1;
		enc2.channel = PCNT_CHANNEL_1;

		enc2.pos_mode = PCNT_COUNT_DIS; //Count Only On Rising-Edges
		enc2.neg_mode = PCNT_COUNT_INC;	// Discard Falling-Edge

		enc2.lctrl_mode = PCNT_MODE_REVERSE;    // Rising A on HIGH B = CW Step
		enc2.hctrl_mode = PCNT_MODE_KEEP; // Rising A on LOW B = CCW Step

		enc2.counter_h_lim = 32000;
		enc2.counter_l_lim = -32000;

		pcnt_unit_config(&enc2);
		pcnt_set_filter_value(PCNT_UNIT_1, 250);  // Filter Runt Pulses
		pcnt_filter_enable(PCNT_UNIT_1);

		pcnt_counter_pause(PCNT_UNIT_1); // Initial PCNT init
		pcnt_counter_clear(PCNT_UNIT_1);
		pcnt_counter_resume(PCNT_UNIT_1);
	}

	xTaskCreatePinnedToCore(&ESPRotary::informObservers, "informObservers", 1024*8, NULL, 29, NULL, 0);
}

int16_t old_cnt = 0;
int old_button = RELEASE;

void ESPRotary::informObservers( void * args )
{
	while( 1 ) {
		int button = gpio_get_level((gpio_num_t)sw);
		if( button != old_button )
		{
//			ESP_LOGI(FNAME,"Rotary button:%d", button);
			int event = NONE;
			if( button==0 )
				event = PRESS;
			else
				event = RELEASE;
			old_button=button;
			if( event == RELEASE ){
				ESP_LOGI(FNAME,"Switch released action");
				for (int i = 0; i < observers.size(); i++)
					observers[i]->release();
				ESP_LOGI(FNAME,"End Switch released action");
			}
			else if ( event == PRESS ){
				ESP_LOGI(FNAME,"Switch pressed action");
				for (int i = 0; i < observers.size(); i++)
					observers[i]->press();
				ESP_LOGI(FNAME,"End Switch pressed action");
			}
			else if ( event == LONG_PRESS ){   // tbd
				ESP_LOGI(FNAME,"Switch long pressed action");
				for (int i = 0; i < observers.size(); i++)
					observers[i]->longPress();
				ESP_LOGI(FNAME,"End Switch long pressed action");
			}
		}
		if( pcnt_get_counter_value(PCNT_UNIT_0, &r_enc_count) != ESP_OK ) {
				ESP_LOGE(FNAME,"Error get counter");
		}
		if( pcnt_get_counter_value(PCNT_UNIT_1, &r_enc2_count) != ESP_OK ) {
				ESP_LOGE(FNAME,"Error get counter");
		}

		if( r_enc_count+r_enc2_count != old_cnt )
		{
			// pcnt_counter_clear(PCNT_UNIT_0);
			// ESP_LOGI(FNAME,"Rotary counter %d %d", r_enc_count,  r_enc2_count);
			int diff = (r_enc_count+r_enc2_count) - old_cnt;
			if( rotary_dir.get() == 1 ) // reverse type
				diff = -diff;
			old_cnt = r_enc_count+r_enc2_count;
			if( diff < 0 ) {
				// ESP_LOGI(FNAME,"Rotary up %d times",abs(diff) );
				for (int i = 0; i < observers.size(); i++)
					observers[i]->up( abs(diff) );
			}
			else if( diff > 0 ) {
				// ESP_LOGI(FNAME,"Rotary down %d times", abs(diff) );
				for (int i = 0; i < observers.size(); i++)
					observers[i]->down( abs(diff) );
			}
		}
		vTaskDelay(20 / portTICK_PERIOD_MS);
	}
}
