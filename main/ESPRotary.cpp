
#include "ESPRotary.h"

#include "SetupNG.h"
#include "sensor.h"
#include "logdef.h"

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <driver/gpio.h>

#include <cstdio>
#include <cstring>
#include <stack>
#include <algorithm>

// the global access to the rotary knob
ESPRotary *Rotary = nullptr;

constexpr uint64_t DEBOUNCE_TIME = 100;         // us debounce threshold

static QueueHandle_t buttonQueue;
static TaskHandle_t pid = NULL;
static std::stack<RotaryObserver *> observers;

// The button portion of the rotary ISR
void IRAM_ATTR button_isr_handler(void* arg)
{
	ESPRotary *knob = static_cast<ESPRotary*>(arg);
	static uint64_t lastPressTime = 0;
	uint64_t currentTime = esp_timer_get_time();
	// Ignore interrupts occurring within debounce time
	if ((currentTime - lastPressTime) < DEBOUNCE_TIME) {
		return;
	}

	// We have not woken a task at the start of the ISR.
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// A valid edge detected
	esp_timer_stop(knob->lp_timer);
	lastPressTime = currentTime;
	int buttonState = gpio_get_level(knob->getSw());
	knob->state = buttonState == 0;

	if (buttonState == 0 ) {
		// Button pressed (active LOW)
		esp_timer_start_once(knob->lp_timer, knob->lp_duration);
	}
	else{ // Button released
		if( knob->hold ) {
			knob->hold = false;
			int releaseEvent = BUTTON_RELEASED;
			xQueueSendFromISR(buttonQueue, &releaseEvent, &xHigherPriorityTaskWoken);
		}
		else {
			int pressType = SHORT_PRESS;
			xQueueSendFromISR(buttonQueue, &pressType, &xHigherPriorityTaskWoken);
		}
	}
	if( xHigherPriorityTaskWoken ) {
		portYIELD_FROM_ISR ();
	}
}
// hold time-out watch dog time (task context)
void IRAM_ATTR longpress_timeout(void* arg)
{
	static_cast<ESPRotary*>(arg)->hold = true;
	int pressType = LONG_PRESS;
	xQueueSend(buttonQueue, &pressType, 0); // task context
}

// The totary portion ISR, PCNT event callback function
static int IRAM_ATTR get_increment(int delta_t_us)
{
    if (delta_t_us > 100000) return 1;
    if (delta_t_us > 50000)  return 2;
    return 3;
}
static int pulse_time;
static bool IRAM_ATTR pcnt_event_handler(pcnt_unit_handle_t unit, const pcnt_watch_event_data_t *edata, void *user_ctx)
{
	static uint64_t lastPulseTime = 0;
	uint64_t currentTime = esp_timer_get_time();

	pulse_time = int(currentTime-lastPulseTime);
	int evt = (get_increment(pulse_time) * sign(edata->watch_point_value)) << 4;
	BaseType_t high_task_wakeup = pdFALSE;
	xQueueSendFromISR((QueueHandle_t)user_ctx, &evt, &high_task_wakeup);
	lastPulseTime = currentTime;
	return high_task_wakeup;
}

// Observer task
void ObserverTask( void *arg )
{
	ESPRotary &knob = *static_cast<ESPRotary*>(arg);
	while( true ) {
		// handle button events
		int event;
		if (xQueueReceive(buttonQueue, &event, pdMS_TO_TICKS(100)) == pdTRUE) {
			if (event == SHORT_PRESS) {
				ESP_LOGI(FNAME,"Button short press detected");
				knob.sendPress();
			} else if (event == LONG_PRESS) {
				ESP_LOGI(FNAME,"Button long press detected");
				knob.sendLongPress();
			}else if (event == BUTTON_RELEASED) {
				ESP_LOGI(FNAME, "Button released");
				knob.sendRelease();
			} else {
				int step = reinterpret_cast<KnobEvent&>(event).RotaryEvent;
				ESP_LOGD(FNAME, "Rotation step %d, time %dus", step, pulse_time);
				knob.sendRot(step);
			}
		}

		if( uxTaskGetStackHighWaterMark( pid ) < 256 ) {
			ESP_LOGW(FNAME,"Warning rotary task stack low: %d bytes", uxTaskGetStackHighWaterMark( pid ) );
		}
	}
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
	gpio_set_direction(dt, GPIO_MODE_INPUT);
	gpio_set_direction(clk, GPIO_MODE_INPUT);
	gpio_pullup_en(dt);
	gpio_pullup_en(clk);

	// Init pulse counter unit
	pcnt_unit_config_t unit_config = {
		.low_limit = -1, // enforce an event trigger on every tick and an auto reset of the pulse counter
		.high_limit = 1,
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
		.max_glitch_ns = 2000 // Ignore pulses shorter than
	};
	ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

	// Enable event watching on high/low limits
	ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, 1));
	ESP_ERROR_CHECK(pcnt_unit_add_watch_point(pcnt_unit, -1));

	// Register event callback
	buttonQueue = xQueueCreate(10, sizeof(int));
	pcnt_event_callbacks_t cbs = {
		.on_reach = pcnt_event_handler
	};
	ESP_ERROR_CHECK(pcnt_unit_register_event_callbacks(pcnt_unit, &cbs, buttonQueue));


	// Enable & start PCNT
	pcnt_unit_enable(pcnt_unit);
	pcnt_unit_clear_count(pcnt_unit);
	pcnt_unit_start(pcnt_unit);

	xTaskCreate(&ObserverTask, "knob", 5096, this, 14, &pid);

	gpio_config_t io_conf = {
			.pin_bit_mask = (1ULL << sw),
			.mode = GPIO_MODE_INPUT,
			.pull_up_en = GPIO_PULLUP_ENABLE,
			.pull_down_en = GPIO_PULLDOWN_DISABLE,
			.intr_type = GPIO_INTR_ANYEDGE
	};
	gpio_config(&io_conf);
	gpio_install_isr_service(0);
	gpio_isr_handler_add(sw, button_isr_handler, this);

	// Create long press timer
	esp_timer_create_args_t timer_args = {
        .callback = (esp_timer_cb_t)longpress_timeout,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "long_press",
        .skip_unhandled_events = true,
    };
    esp_timer_create(&timer_args, &lp_timer);
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

void ESPRotary::sendRot( int diff ) const
{
	// ESP_LOGI(FNAME,"Rotary action");
	if (!observers.empty()) {
		auto obs = observers.top();
		float step = pow(obs->getRotDynamic(), abs(diff)-1) * sign(diff);
		ESP_LOGI(FNAME, "Rotation step %.2f, time %d us", step, pulse_time);
		obs->rot( int(step) );
		xQueueReset(buttonQueue); // rince queueed up events due to intermediate polling
	}
}

void ESPRotary::sendPress() const
{
	// ESP_LOGI(FNAME,"Pressed action");
	if (!observers.empty()) {
		observers.top()->press();
		xQueueReset(buttonQueue);
	}
}

void ESPRotary::sendRelease() const
{
	// ESP_LOGI(FNAME,"Release action");
	if (!observers.empty()) {
		observers.top()->release();
		xQueueReset(buttonQueue);
	}
}

void ESPRotary::sendLongPress() const
{
	// ESP_LOGI(FNAME,"Long pressed action");
	if (!observers.empty()) {
		observers.top()->longPress();
		xQueueReset(buttonQueue);
	}
}

void ESPRotary::sendEscape() const
{
	// ESP_LOGI(FNAME,"Rotary up action");
	if (!observers.empty()) {
		observers.top()->escape();
		xQueueReset(buttonQueue);
	}
}



