#include "ESPRotary.h"
#include "Setup.h"
#include "RingBufCPP.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include <freertos/queue.h>
#include "freertos/task.h"
#include "string.h"
#include "esp_system.h"
#include <esp32/rom/ets_sys.h>
#include <sys/time.h>
#include <Arduino.h>
#include <logdef.h>
#include <list>
#include <algorithm>
#include "Flarm.h"

gpio_num_t ESPRotary::clk, ESPRotary::dt;
gpio_num_t ESPRotary::sw = GPIO_NUM_0;
std::list<RotaryObserver *> ESPRotary::observers;

pcnt_config_t ESPRotary::enc;
pcnt_config_t ESPRotary::enc2;
int16_t ESPRotary::r_enc_count  = 0;
int16_t ESPRotary::r_enc2_count = 0;
int ESPRotary::timer = 0;
bool ESPRotary::released = true;
bool ESPRotary::pressed = false;
bool ESPRotary::longPressed = false;

#define ROTARY_SINGLE_INC 0
#define ROTARY_DOUBLE_INC 1
static TaskHandle_t *pid;

void ESPRotary::attach(RotaryObserver *obs) {
	observers.push_back(obs);
}
void ESPRotary::detach(RotaryObserver *obs) {
	auto it = std::find(observers.begin(), observers.end(), obs);
	if ( it != observers.end() ) {
		observers.erase(it);
	}
}

bool ESPRotary::readSwitch() {
  if( Flarm::bincom )
    return false;
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

	xTaskCreatePinnedToCore(&ESPRotary::informObservers, "informObservers", 4096, NULL, 18, pid, 0);
}

int16_t old_cnt = 0;
int old_button = RELEASE;

void ESPRotary::sendRelease(){
	ESP_LOGI(FNAME,"Release action");
	if( Flarm::bincom )
	  return;
	for (auto &observer : observers)
		observer->release();
	ESP_LOGI(FNAME,"End switch release action");
}

void ESPRotary::sendPress(){
	ESP_LOGI(FNAME,"Pressed action");
  if( Flarm::bincom )
    return;
	for (auto &observer : observers)
		observer->press();
	ESP_LOGI(FNAME,"End pressed action");

}

void ESPRotary::sendLongPress(){
	ESP_LOGI(FNAME,"Long pressed action");
  if( Flarm::bincom )
    return;
	for (auto &observer : observers)
		observer->longPress();
	ESP_LOGI(FNAME,"End long pressed action");
}

void ESPRotary::informObservers( void * args )
{
	while( 1 ) {
	  if( Flarm::bincom ) {
	    vTaskDelay(20 / portTICK_PERIOD_MS);
	    continue;
	  }
		int button = gpio_get_level((gpio_num_t)sw);
		if( button == 0 ){  // Push button is being pressed
			timer++;
			released = false;
			pressed = false;
			if( timer > 20 ){  // > 400 mS
				if( !longPressed ){
					sendLongPress();
					sendRelease();
					longPressed = true;
				}
			}
		}
		else{   // Push button is being released
			if( !released ){
				ESP_LOGI(FNAME,"timer=%d", timer );
				longPressed = false;
				if( timer < 20 ){  // > 400 mS
					if( !pressed ){
						sendPress();
						sendRelease();
						pressed = true;
					}
				}
				timer = 0;
				released = true;
				delay( 20 );
			}
		}
		if( pcnt_get_counter_value(PCNT_UNIT_0, &r_enc_count) != ESP_OK ) {
			ESP_LOGE(FNAME,"Error get counter");
		}
		if( pcnt_get_counter_value(PCNT_UNIT_1, &r_enc2_count) != ESP_OK ) {
			ESP_LOGE(FNAME,"Error get counter");
		}

		if( abs( r_enc_count+r_enc2_count - old_cnt) > rotary_inc.get() )
		{
			// pcnt_counter_clear(PCNT_UNIT_0);
			// ESP_LOGI(FNAME,"Rotary counter %d %d", r_enc_count,  r_enc2_count);
			int diff = (r_enc_count+r_enc2_count) - old_cnt;
			diff = diff / ( rotary_inc.get()+1 );
			// ESP_LOGI(FNAME,"Rotary diff %d", diff );
			if( hardwareRevision.get() >= 3 ) {
				if( rotary_dir_21.get() == 1 ) // reverse default for 2021 series
					diff = -diff;
			}
			else{
				if( rotary_dir.get() == 1 )    // reverse type default for 2020 series
					diff = -diff;
			}
			old_cnt = r_enc_count+r_enc2_count;
			if( diff < 0 ) {
				// ESP_LOGI(FNAME,"Rotary up %d times",abs(diff) );
				for (auto &observer : observers)
					observer->up( abs(diff) );
			}
			else{
				// ESP_LOGI(FNAME,"Rotary down %d times", abs(diff) );
				for (auto &observer : observers)
					observer->down( abs(diff) );
			}
		}
		if( uxTaskGetStackHighWaterMark( pid ) < 256 )
			ESP_LOGW(FNAME,"Warning rotary task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid ) );
		vTaskDelay(20 / portTICK_PERIOD_MS);
	}
}
