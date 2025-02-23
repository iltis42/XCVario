#include "ESPRotary.h"

#include "Setup.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <logdef.h>


#include <cstdio>
#include <cstring>
#include <list>
#include <algorithm>

#define ROTARY_SINGLE_INC 0
#define ROTARY_DOUBLE_INC 1

// the global access to the rotary knob
ESPRotary *Rotary = nullptr;

static TaskHandle_t pid = NULL;
static std::list<RotaryObserver *> observers;

// Observer task
void ObserverTask( void *args )
{
	ESPRotary &knob = *static_cast<ESPRotary*>(args);
	static int old_cnt = 0;
	int enc_count;
	static int timer = 0;
	static bool released;
	static bool longPressed;
	static bool pressed;

	while( 1 ) {
		int button = gpio_get_level(knob.sw);
		if( button == 0 ) {
			// Push button is being pressed
			timer++;
			released = false;
			pressed = false;
			if( timer > 20/5 ) {  // > 400 mS
				if( !longPressed ){
					longPressed = true;
					knob.longPress();
					knob.release();

				}
			}
		}
		else {
			// Push button is being released
			if( !released ){
				// ESP_LOGI(FNAME,"timer=%d", timer );
				longPressed = false;
				if( timer < 20/5 ){  // > 400 mS
					if( !pressed ){
						pressed = true;
						knob.press();
						knob.release();
					}
				}
				timer = 0;
				released = true;
				vTaskDelay(pdMS_TO_TICKS(20));
			}
		}
		if( pcnt_unit_get_count(knob.pcnt_unit, &enc_count) != ESP_OK ) {
			ESP_LOGE(FNAME,"Error get counter");
		}
		
		if( abs( enc_count - old_cnt) > rotary_inc.get() )
		{
			// pcnt_counter_clear(PCNT_UNIT_0);
			// ESP_LOGI(FNAME,"Rotary counter %d", enc_count);
			int diff = (enc_count) - old_cnt;
			diff = diff / ( rotary_inc.get()+1 );
			// ESP_LOGI(FNAME,"Rotary diff %d", diff );
			old_cnt = enc_count;
			if( diff < 0 ) {
				// ESP_LOGI(FNAME,"Rotary down %d times",abs(diff) );
				knob.down( abs(diff) );
			}
			else {
				// ESP_LOGI(FNAME,"Rotary up %d times", abs(diff) );
				knob.up( abs(diff) );
			}
		}
		if( uxTaskGetStackHighWaterMark( pid ) < 256 )
			ESP_LOGW(FNAME,"Warning rotary task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid ) );
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

// Observer registration
void RotaryObserver::attach(RotaryObserver *obs) {
	// ESP_LOGI(FNAME,"Attach obs: %p", obs );
	observers.push_back(obs);
}
void RotaryObserver::detach(RotaryObserver *obs) {
	// ESP_LOGI(FNAME,"Detach obs: %p", obs );
	auto it = std::find(observers.begin(), observers.end(), obs);
	if ( it != observers.end() ) {
		observers.erase(it);
	}
}

// The rotary knob
ESPRotary::ESPRotary(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw) :
	clk(aclk),
	dt(adt),
	sw(asw)
{
	// Init. the button early
	gpio_set_direction(sw, GPIO_MODE_INPUT);
	gpio_pullup_en(sw); // Rotary Encoder Button
}

void ESPRotary::begin()
{
	gpio_set_direction(sw, GPIO_MODE_INPUT);
	gpio_set_direction(dt, GPIO_MODE_INPUT);
	gpio_set_direction(clk, GPIO_MODE_INPUT);
	gpio_pullup_en(sw); // Rotary Encoder Button
	gpio_pullup_en(dt);
	gpio_pullup_en(clk);

	// Init pulse counter unit
	pcnt_unit_config_t unit_config = {
		.low_limit = -32000,
		.high_limit = 32000,
		.intr_priority = 0,
		.flags = {0}
	};
	ESP_ERROR_CHECK(pcnt_new_unit(&unit_config, &pcnt_unit));

	pcnt_chan_config_t chan_config = {
		.edge_gpio_num = clk, // pulse
		.level_gpio_num = dt, // direction
		.flags = {
			.invert_edge_input = 0,
			.invert_level_input = 0,
			.virt_edge_io_level = 0,
			.virt_level_io_level = 0,
			.io_loop_back = 0,
		},
	};
	ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_config, &pcnt_chan));

	// Decide on rotary hardware direction sense
	ESP_ERROR_CHECK(updateRotDir());
	ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_INVERSE, PCNT_CHANNEL_LEVEL_ACTION_KEEP));

	pcnt_glitch_filter_config_t filter_config = {
		.max_glitch_ns = 2000 // Ignore pulses shorter than 2000 ns
	};
	ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

	pcnt_unit_enable(pcnt_unit);
	pcnt_unit_clear_count(pcnt_unit);
	pcnt_unit_start(pcnt_unit);

	xTaskCreate(&ObserverTask, "knob", 5096, this, 14, &pid);
}

esp_err_t ESPRotary::updateRotDir()
{
	pcnt_channel_edge_action_t rot_action = PCNT_CHANNEL_EDGE_ACTION_INCREASE;
	if( hardwareRevision.get() <= XCVARIO_21 ) {
		rot_action = PCNT_CHANNEL_EDGE_ACTION_DECREASE;
	}
	if ( rotary_dir.get() == 1 ) {
		// flip the knob sens
		rot_action = (rot_action == PCNT_CHANNEL_EDGE_ACTION_INCREASE) ? PCNT_CHANNEL_EDGE_ACTION_DECREASE : PCNT_CHANNEL_EDGE_ACTION_INCREASE;
	}
	return pcnt_channel_set_edge_action(pcnt_chan, rot_action, PCNT_CHANNEL_EDGE_ACTION_HOLD);
}

void ESPRotary::up( int diff ){
	// ESP_LOGI(FNAME,"Rotary down action");
	for (auto &observer : observers)
		observer->up( diff );   // tbd, dito
}

void ESPRotary::down( int diff ){
	// ESP_LOGI(FNAME,"Rotary up action");
	for (auto &observer : observers)
		observer->down( diff );
}

void ESPRotary::press(){
	// ESP_LOGI(FNAME,"Pressed action");
	for (auto &observer : observers)
		observer->press();
	// ESP_LOGI(FNAME,"End pressed action");

}

void ESPRotary::release(){
	// ESP_LOGI(FNAME,"Release action");
	for (auto &observer : observers)
		observer->release();
	// ESP_LOGI(FNAME,"End switch release action");
}

void ESPRotary::longPress(){
	// ESP_LOGI(FNAME,"Long pressed action");
	for (auto &observer : observers)
		observer->longPress();
	// ESP_LOGI(FNAME,"End long pressed action");
}

void ESPRotary::escape(){
	// ESP_LOGI(FNAME,"Rotary up action");
	for (auto &observer : observers)
		observer->escape();
}

bool ESPRotary::readSwitch() {
	return gpio_get_level(sw) == 0;
}



