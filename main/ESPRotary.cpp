
#include "ESPRotary.h"

#include "setup/SetupNG.h"
#include "sensor.h"
#include "protocol/Clock.h"
#include "logdefnone.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <freertos/portmacro.h>
#include <driver/gpio.h>

#include <cstdio>
#include <cstring>
#include <stack>
#include <algorithm>

// the global access to the rotary knob
ESPRotary *Rotary = nullptr;

static std::stack<RotaryObserver *> observers;

// clock tick timer (task context)
bool IRAM_ATTR ESPRotary::tick()
{
	// called every 10msec
	int gotEvent;
	bool buttonRead = gpio_get_level(getSw()) == 0; // button pressed (active LOW)
	debounceCount = (buttonRead == lastButtonRead) ? (debounceCount+1) : 0;
	lastButtonRead = buttonRead;
	if ( holdCount > 0 ) {
		holdCount++;
		if ( holdCount > lp_duration ) {
			gotEvent = LONG_PRESS;
			holdCount = -1; // go for a "release"
			xQueueSend(buttonQueue, &gotEvent, 0);
		}
	}
	if ( debounceCount < 2 || (buttonRead == state) ) {
		return false;
	}

	// A valid edge detected
	state = buttonRead;
	if (state) {
		holdCount = 1; // start counting
	}
	else { // Button released
		if( holdCount < 0 ) {
			gotEvent = BUTTON_RELEASED;
			xQueueSend(buttonQueue, &gotEvent, 0);
		}
		else {
			gotEvent = SHORT_PRESS;
			xQueueSend(buttonQueue, &gotEvent, 0);
		}
		holdCount = 0; // stop counting
	}

	return false;
}

// The rotary portion ISR, PCNT event callback function
static int IRAM_ATTR get_step(int delta_t_us)
{
    if (delta_t_us > 100000) return 1;
    if (delta_t_us > 50000)  return 2;
    return 3;
}
static int pulse_time;
static bool IRAM_ATTR pcnt_event_handler(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
	static uint64_t lastPulseTime = 0;
	static int wp_value = 1;
	uint64_t currentTime = esp_timer_get_time();

	pulse_time = int(currentTime-lastPulseTime);
	int step = get_step(pulse_time);
	if ( step == 1 ) {
		wp_value = sign(edata->watch_point_value);
	}
		//else suppress all of a sudden changes in rotational direction
	int evt = (step * wp_value) << 4;
	BaseType_t high_task_wakeup = pdFALSE;
	xQueueSendFromISR((QueueHandle_t)user_ctx, &evt, &high_task_wakeup);
	lastPulseTime = currentTime;
	return high_task_wakeup;
}

// Observer registration
void RotaryObserver::attach() {
	// ESP_LOGI(FNAME,"Attach obs: %p", obs );
	observers.push(this);
}
void RotaryObserver::detach() {
	// ESP_LOGI(FNAME,"Detach obs: %p", obs );
	if ( observers.empty() ) {
		ESP_LOGW(FNAME,"Hoppla observer stack empty on detach");
		return;
	}
	observers.pop();
	_rot_dynamic = 2.0;
}

// The rotary knob
ESPRotary::ESPRotary(gpio_num_t aclk, gpio_num_t adt, gpio_num_t asw) :
	Clock_I(1),
	clk(aclk),
	dt(adt),
	sw(asw)
{
	// Init. the button early
	gpio_set_direction(sw, GPIO_MODE_INPUT);
	gpio_pullup_en(sw); // Button
	// Rotary Encoder
	gpio_set_direction(clk, GPIO_MODE_INPUT);
	gpio_set_direction(dt, GPIO_MODE_INPUT);
	gpio_pulldown_en(clk);
	gpio_pulldown_en(dt);

	buttonQueue = xQueueCreate(10, sizeof(int));
}

ESPRotary::~ESPRotary()
{
	vQueueDelete(buttonQueue);
	Clock::stop(this);
}

void ESPRotary::begin()
{
	// Init pulse counter unit
	int increment = rotary_inc.get() + 1;
	ESP_LOGI(FNAME, "new inc %d", increment);
	pcnt_unit_config_t unit_config = {
		.low_limit = -increment, // enforce an event trigger on every tick and an auto reset of the pulse counter
		.high_limit = increment,
		.intr_priority = 0,
		.flags = {}
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

	// Decide on pulse counter hardware revision 
	ESP_ERROR_CHECK(updateRotDir());

	// Init direction sense channel
	ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan, PCNT_CHANNEL_LEVEL_ACTION_INVERSE, PCNT_CHANNEL_LEVEL_ACTION_KEEP));

	pcnt_glitch_filter_config_t filter_config = {
		.max_glitch_ns = 1000 // Ignore pulses shorter than
	};
	ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

	// Enable event watching on high/low limits
	ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, increment));
	ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, -increment));

	// Register event callback
	pcnt_event_callbacks_t cbs = {
		.on_reach = pcnt_event_handler
	};
	ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, buttonQueue));

	// Enable & start PCNT
	pcnt_unit_enable(pcnt_unit);
	pcnt_unit_clear_count(pcnt_unit);
	pcnt_unit_start(pcnt_unit);

	// start tick polling
	Clock::start(this);
}

void ESPRotary::stop()
{
	if ( pcnt_unit ) {
		pcnt_unit_stop(pcnt_unit);
		pcnt_unit_disable(pcnt_unit);
		pcnt_del_channel(pcnt_chan);
		pcnt_chan = nullptr;
		pcnt_del_unit(pcnt_unit);
		pcnt_unit = nullptr;
	}
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
	return pcnt_channel_set_edge_action(pcnt_chan, PCNT_CHANNEL_EDGE_ACTION_HOLD, rot_action);
}

// Increment setup is stored with values 0,1,2 and here used with values 1,2,3
void ESPRotary::updateIncrement(int inc)
{
	inc += 1;
	if ( inc > 0 && inc < 4 && inc != increment ) {
		ESP_LOGI(FNAME, "Update rot inc %d->%d", increment, inc);
		increment = inc;
		stop();
		vTaskDelay(pdMS_TO_TICKS(10));
		begin();
	}
	
}

void ESPRotary::sendRot( int diff ) const
{
	// ESP_LOGI(FNAME,"Rotary action");
	if (!observers.empty()) {
		auto obs = observers.top();
		float step = pow(obs->getRotDynamic(), abs(diff)-1) * sign(diff);
		ESP_LOGI(FNAME, "Rotation step %.2f, time %d us", step, pulse_time);
		obs->rot( int(step) );
	}
}

void ESPRotary::sendPress() const
{
	// ESP_LOGI(FNAME,"Pressed action");
	if (!observers.empty()) {
		observers.top()->press();
	}
}

void ESPRotary::sendRelease() const
{
	// ESP_LOGI(FNAME,"Release action");
	if (!observers.empty()) {
		observers.top()->release();
	}
}

void ESPRotary::sendLongPress() const
{
	// ESP_LOGI(FNAME,"Long pressed action");
	if (!observers.empty()) {
		observers.top()->longPress();
	}
}

void ESPRotary::sendEscape() const
{
	// ESP_LOGI(FNAME,"Rotary up action");
	if (!observers.empty()) {
		observers.top()->escape();
	}
}

// In case an event processing routine is asking itself for button status
bool ESPRotary::readSwitch() const
{
	// return true for any button event in the queue, except a release
	int event;
	while (xQueueReceive(buttonQueue, &event, 0) == pdTRUE) {
		if (event == SHORT_PRESS
			|| event == LONG_PRESS) {
			xQueueReset(buttonQueue);
			return true;
		}
	}
	return false;
}
