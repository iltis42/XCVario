/* DAC Cosine Generator Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include <esp_log.h>
#include "sdkconfig.h"
#include "esp_task_wdt.h"

#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"
#include <string>

#include "driver/dac.h"
#include "ESPAudio.h"
#include "sensor.h"


float ESPAudio::_range = 5.0;

ESPAudio::ESPAudio( ) {
	_ch = DAC_CHANNEL_1;
	_center = 440;
	_te = 0.0;
	_s2fd = 0.0;
	_variation = 3.0;
	_testmode = false;
	_button = GPIO_NUM_0;
	_deadband = 0;
	_mute = false;
	_test_ms = 0;
	_dead_mute = true;
	_setup = 0;
	_deadband = 0;
	_deadband_neg = 0;
	_disable = false;
	_old_ms = 0;
	_range = 5.0;
	_s2f_mode = false;
}

ESPAudio::~ESPAudio() {

}

/* Declare global sine waveform parameters
 * so they may be then accessed and changed from debugger
 * over an JTAG interface
 */
const int clk_8m_div = 7;    // RTC 8M clock divider (division is by clk_8m_div+1, i.e. 0 means 8MHz frequency)
const int offset = 0;              // leave it default / 0 = no any offset
const int invert = 2;          // invert MSB to get sine waveform


/*
 * Enable cosine waveform generator on a DAC channel
 */
void ESPAudio::dac_cosine_enable(dac_channel_t channel, bool enable)
{
	// Enable tone generator common to both channels
	SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL1_REG, SENS_SW_TONE_EN);
	switch(channel) {
	case DAC_CHANNEL_1:
		// Enable / connect tone tone generator on / to this channel
		if( enable )
			SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN1_M);
		else
			CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN1_M);
		// Invert MSB, otherwise part of waveform will have inverted
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV1, 2, SENS_DAC_INV1_S);
		break;
	case DAC_CHANNEL_2:
		if( enable )
			SET_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN2_M);
		else
			CLEAR_PERI_REG_MASK(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_CW_EN2_M);
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV2, 2, SENS_DAC_INV2_S);
		break;
	default :
		printf("Channel %d\n", channel);
	}
}


/*
 * Set frequency of internal CW generator common to both DAC channels
 *
 * clk_8m_div: 0b000 - 0b111
 * frequency_step: range 0x0001 - 0xFFFF
 *
 */
void ESPAudio::dac_frequency_set(int clk_8m_div, int frequency_step)
{
	REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, clk_8m_div);
	SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL1_REG, SENS_SW_FSTEP, frequency_step, SENS_SW_FSTEP_S);
}


/*
 * Scale output of a DAC channel using two bit pattern:
 *
 * - 00: no scale
 * - 01: scale to 1/2
 * - 10: scale to 1/4
 * - 11: scale to 1/8
 *
 */
void ESPAudio::dac_scale_set(dac_channel_t channel, int scale)
{
	switch(channel) {
	case DAC_CHANNEL_1:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_SCALE1, scale, SENS_DAC_SCALE1_S);
		break;
	case DAC_CHANNEL_2:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_SCALE2, scale, SENS_DAC_SCALE2_S);
		break;
	default :
		printf("Channel %d\n", channel);
	}
}


/*
 * Offset output of a DAC channel
 *
 * Range 0x00 - 0xFF
 *
 */
void ESPAudio::dac_offset_set(dac_channel_t channel, int offset)
{
	switch(channel) {
	case DAC_CHANNEL_1:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_DC1, offset, SENS_DAC_DC1_S);
		break;
	case DAC_CHANNEL_2:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_DC2, offset, SENS_DAC_DC2_S);
		break;
	default :
		printf("Channel %d\n", channel);
	}
}


/*
 * Invert output pattern of a DAC channel
 *
 * - 00: does not invert any bits,
 * - 01: inverts all bits,
 * - 10: inverts MSB,
 * - 11: inverts all bits except for MSB
 *
 */
void ESPAudio::dac_invert_set(dac_channel_t channel, int invert)
{
	printf("DAC invert: channel %d, invert: %d\n", channel, invert);
	switch(channel) {
	case DAC_CHANNEL_1:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV1, invert, SENS_DAC_INV1_S);
		break;
	case DAC_CHANNEL_2:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV2, invert, SENS_DAC_INV2_S);
		break;
	default :
		printf("Channel %d\n", channel);
	}
}

/*
 * Main task that let you test CW parameters in action
 *
 *
  freq = dig_clk_rtc_freq x SENS_SAR_SW_FSTEP / 65536


  frequency = ( (RTC_FAST_CLK_FREQ_APPROX / (1 + clk_8m_div) ) *  frequency_step) / 65536;
  frequency = ( (RTC_FAST_CLK_FREQ_APPROX / (1 + clk_8m_div) ) *  frequency_step) / 65536;
   (RTC_FAST_CLK_FREQ_APPROX / (1 + clk_8m_div) ) *  frequency_step = frequency * 65536
   frequency_step =  (frequency * 65536) / ( RTC_FAST_CLK_FREQ_APPROX / (1 + clk_8m_div) )
   frequency_step =  (frequency * 65536) * (1 + clk_8m_div) ) / RTC_FAST_CLK_FREQ_APPROX

  RTC_FAST_CLK_FREQ_APPROX = 8500000

  frequency_step =  (frequency * 65536) * (1 + clk_8m_div) ) / 8500000

  div: 4 bit =  0x0F or 15

  div = 4:
  frequency_step =  ( frequency * 65536  * 5 ) / 8500000
  frequency_step =  frequency  / 25.93
  200 = 8
  500 = 19
  2000 = 77

  div = 7:
  frequency_step =  frequency  / 16.21
  100 = 6
  500 = 30
  2000 = 123


   RTC_FAST_CLK_FREQ_APPROX / (65536  * 8 )

 */

/*
const float delta = 1.05946309436;
sounds strange, better chop tone
 */

const float freq_step = RTC_FAST_CLK_FREQ_APPROX / (65536 * 8 );  // div = 0x07
long int tick = 0;
long int tickmod  = 0;

//  modulation frequency

bool hightone = false;

const int del = 500;

void ESPAudio::modtask(void* arg )
{
	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();
		tickmod++;
		hightone = false;
		float te = Audio.getTE();


		if ( te > 0 )
			if ( (tickmod & 1) == 1 )
				hightone = true;
		int delay = 100;
		if ( !hightone )
		{
			int delaydelta = 0;
			if (te > 0.05 )
				delaydelta = del * 0.8 *(te/_range);
			delay = int( del - delaydelta + 0.5 );
		}
		vTaskDelayUntil(&xLastWakeTime, delay/portTICK_PERIOD_MS);
	}
}


void ESPAudio::dactask(void* arg )
{
	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();

		tick++;
		float te = Audio.getTE();

		float max;
		if ( te > 0 )
			max = Audio.getCenter() * Audio.getVariation();
		else
			max = Audio.getCenter() / Audio.getVariation();
		float f = (float)Audio.getCenter() + ( (te/_range ) * max );

		int step = int( (f/freq_step ) + 0.5);
		if( Audio.inDeadBand(te) || Audio.getMute() || hightone  ){
			step = int( (5/freq_step ) + 0.5);
			// printf("Audio OFF\n");
		}
		else{
			// printf("Audio ON\n");
		}

		Audio.dac_frequency_set(clk_8m_div, step);
		// printf("TE %f  frequency: %.0f Hz\n", te, f );
		vTaskDelayUntil(&xLastWakeTime, 20/portTICK_PERIOD_MS);
	}
}

bool ESPAudio::inDeadBand( float te )
{
   if( te > 0 ) {
	   if( te < _deadband )
		   return true;
   }
   else {
  	   if( te > _deadband_neg )
  		   return true;
   }
   return false;
}

void ESPAudio::setValues( float te, float s2fd, bool fromtest )
{
	if ( !fromtest ){
		if ( _testmode )
			return;
	}
	if( _s2f_mode )
		te = -s2fd/10.0;
	if( te > _range )
		_te = _range;
	else if( te < -_range )
		_te = -_range;
	else
		_te = te;
}

/*
 * Generate a sine waveform on both DAC channels:
 *
 * _ch - GPIO25
 * DAC_CHANNEL_2 - GPIO26
 *
 * Connect scope to both GPIO25 and GPIO26
 * to observe the waveform changes
 * in response to the parameter change
 */

void ESPAudio::setup()
{
	_center = _setup->get()->_center_freq;
	_te = 0.0;
	_variation = _setup->get()->_tone_var;
	_testmode = false;
	_deadband = _setup->get()->_deadband;
	_deadband_neg = _setup->get()->_deadband_neg;
	_dead_mute = true;
	_mute = false;
	if( _setup->get()->_audio_range == 0 )
		_range = 5.0;
	else
	_range = _setup->get()->_range;
}

void ESPAudio::begin( dac_channel_t ch, gpio_num_t button, Setup *asetup )
{
	_setup = asetup;
	setup();
    mute();
	_ch = ch;
	_button = button;
	dac_cosine_enable(_ch);
	dac_output_enable(_ch);
	dac_offset_set(_ch, offset);
	dac_invert_set(_ch, 2 );
	_testmode = false;

	_dead_mute = true;
	gpio_set_direction(_button, GPIO_MODE_INPUT);
	gpio_set_pull_mode(_button, GPIO_PULLUP_ONLY);

	xTaskCreate(modtask, "modtask", 1024*4, NULL, 10, NULL);
	xTaskCreate(dactask, "dactask", 1024*4, NULL, 10, NULL);
	dac_scale_set(_ch, 0 );
}

void ESPAudio::disable( bool disable ) {
	_disable = disable;
	if( _disable )
	{
		mute( true );
		vTaskDelay(100 / portTICK_PERIOD_MS);
		Audio.dac_cosine_enable(Audio.getCh(), false);
	}
	else
	{
		Audio.dac_cosine_enable(Audio.getCh(), true );
		vTaskDelay(100 / portTICK_PERIOD_MS);
		mute( false );
	}
};

void ESPAudio::test( float to, float from )
{
	Audio.setTestmode( true );
	if( to > from){
		for( float tef=from; tef < to; tef=tef+0.05 ){
			Audio.setValues( tef, 0, true );
			vTaskDelay(50 / portTICK_PERIOD_MS);
		}
	}
	else{
		for( float tef=from; tef > to; tef=tef-0.05 ){
			Audio.setValues( tef, 0, true );
			vTaskDelay(50 / portTICK_PERIOD_MS);
		}
	}
}
ESPAudio Audio;
