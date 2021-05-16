// Array representation of binary file ding.wav
//
//

#include <stdlib.h>
#include "esp_system.h"
#include "esp_task_wdt.h"
#include <string>
#include "sdkconfig.h"
#include "esp_system.h"

#include <driver/gpio.h>
#include <driver/dac.h>
#include <stddef.h>
#include <stdio.h>

#include "ESPAudio.h"
#include "ding.h"
#include "hi.h"
#include "sound.h"
#include "driver/timer.h"
#include <logdef.h>

int Sound::pos = 40;
bool Sound::ready=false;
MCP4018 *Sound::_poti;
e_sound Sound::sound;
intr_handle_t s_timer_handle;

// #define sound1 ding_co_wav
// #define sound1 hi_wav


void Sound::timer_isr(void* arg)
{
	TIMERG0.int_clr_timers.t0 = 1;
	TIMERG0.hw_timer[0].config.alarm_en = 1;
	int len = 0;
	if(sound == DING)
		len = ding_co_wav_len;
	else if(sound == HI)
		len = hi_wav_len;
	if( pos < len ) {
		if( sound == DING )
			dac_output_voltage(DAC_CHANNEL_1,ding_co_wav[pos++]);
		else if( sound == HI )
			dac_output_voltage(DAC_CHANNEL_1,hi_wav[pos++]);
	}
	else
		ready = true;
}

void Sound::timerInitialise (int timer_period_us)
{
	timer_config_t config = {
 			.alarm_en = TIMER_ALARM_EN,				//Alarm Enable
			.counter_en = TIMER_PAUSE,			//If the counter is enabled it will start incrementing / decrementing immediately after calling timer_init()
			.intr_type = TIMER_INTR_LEVEL,	//Is interrupt is triggered on timer’s alarm (timer_intr_mode_t)
			.counter_dir = TIMER_COUNT_UP,	//Does counter increment or decrement (timer_count_dir_t)
			.auto_reload = TIMER_AUTORELOAD_EN,			//If counter should auto_reload a specific initial value on the timer’s alarm, or continue incrementing or decrementing.
			.divider = 80   				//Divisor of the incoming 80 MHz (12.5nS) APB_CLK clock. E.g. 80 = 1uS per timer tick
	};

	timer_init(TIMER_GROUP_0, TIMER_0, &config);
	timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0);
	timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, timer_period_us);
	timer_enable_intr(TIMER_GROUP_0, TIMER_0);
	// timer_set_auto_reload(TIMER_GROUP_0, TIMER_0, TIMER_AUTORELOAD_EN );
	timer_isr_register(TIMER_GROUP_0, TIMER_0, &timer_isr, NULL, 0, &s_timer_handle);
	timer_start(TIMER_GROUP_0, TIMER_0);
}


void Sound::playSound( e_sound a_sound, bool end ){
	pos = 40;
	ready = false;
	sound = a_sound;
	ESP_LOGI(FNAME,"Start play sound");
	Audio::setTestmode(true);
	uint16_t volume;
	_poti->readWiper( volume );
	_poti->writeWiper( 50 );
	dac_output_enable(DAC_CHANNEL_1);
	dac_output_voltage(DAC_CHANNEL_1,127);
	sleep(0.05);
	timerInitialise(15);
	while( !ready ) {
		sleep(0.5);
		// ESP_LOGI(FNAME,"playing %d", pos);
	}
	timer_disable_intr(TIMER_GROUP_0, TIMER_0);
	ESP_LOGI(FNAME,"play Sound end");
	if( end ) {
		_poti->writeWiper( volume );
	   Audio::restart();
	}
	ESP_LOGI(FNAME,"play Sound task end");
}


