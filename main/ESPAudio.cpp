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
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_task_wdt.h"

#include "soc/rtc_io_reg.h"
#include "soc/rtc_cntl_reg.h"
#include "soc/sens_reg.h"
#include "soc/rtc.h"
#include <string>

#include "ESPAudio.h"
#include "driver/dac.h"
#include "sensor.h"
#include "mcp4018.h"
#include <Arduino.h>
// #include "sound.h"


float ESPAudio::_range = 5.0;
bool ESPAudio::_s2f_mode = false;
uint8_t ESPAudio::_tonemode;
uint8_t ESPAudio::_chopping_mode;
float ESPAudio::_high_tone_var;
dac_channel_t ESPAudio::_ch;
uint16_t ESPAudio::wiper;
uint16_t ESPAudio::cur_wiper;
bool ESPAudio::sound_on;
bool ESPAudio::_testmode;

MCP4018 Poti;


ESPAudio::ESPAudio( ) {
	_ch = DAC_CHANNEL_1;
	_center = 440;
	_te = 0.0;
	_s2fd = 0.0;
	_variation = 3.0;
	_testmode = false;
	_button = GPIO_NUM_0;
	_s2f_deadband = 0;
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
	wiper = 63;
	cur_wiper = 63;
	sound_on = false;
}

ESPAudio::~ESPAudio() {

}



/* Declare global sine waveform parameters
 * so they may be then accessed and changed from debugger
 * over an JTAG interface
 */
const int clk_8m_div = 7;    // RTC 8M clock divider (division is by clk_8m_div+1, i.e. 0 means 8MHz frequency)
const int offset = 0;        // leave it default / 0 = no any offset
const int invert = 2;        // invert MSB to get sine waveform

/*
 * Enable cosine waveform generator on a DAC channel
 */
void ESPAudio::dac_cosine_enable(dac_channel_t channel, bool enable)
{
	// Enable tone generator common to both channels
	printf("ESPAudio::dac_cosine_enable ch: %d\n", channel);
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

bool ESPAudio::selfTest(){
	printf("ESPAudio::selfTest\n");
	uint16_t awiper;
	bool ret = Poti.readWiper( awiper );
	printf("readWiper val=%d ret=%d cur=%d\n", awiper, ret, cur_wiper );
	if( ret == false )
		return false;
	if( awiper != cur_wiper )  // begin sets already cur_wiper
		ret = false;
	else
		ret = true;
	// Tone test, beep 440 Hz 1 second
	_testmode=true;  // disable task for modulation
	Poti.writeWiper( ((_setup->get()->_default_volume * 100.0) / 128) -1 );
	dac_output_enable(_ch);
	for( float f=261.62; f<1046.51; f=f*1.03){
		// printf("f=%f\n",f);
		Audio.dac_frequency_set(clk_8m_div, int(f/freq_step) );
		delay(30);
		esp_task_wdt_reset();
	}
	delay(200);
	Poti.writeWiper( 0 );
	dac_output_disable(_ch);
	_testmode=false;
	return ret;
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

   GPIO_NUM_19 as input programmed will enable the PAM amplifier.
   We do not use enable/disable from amplifier as this creates some pop noise
   Mute will be realized by putting down the wiper from digital poti to zero what works like a charm.


 */

/*
const float delta = 1.05946309436;
sounds strange, better chop tone
 */


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
			if (te > 0.05 ){
				if( _s2f_mode )
					delaydelta = del * 0.8 *(te/5.0);
				else
					delaydelta = del * 0.8 *(te/_range);
			}
			delay = int( del - delaydelta + 0.5 );
		}
		vTaskDelayUntil(&xLastWakeTime, delay/portTICK_PERIOD_MS);
	}
}

void ESPAudio::incVolume( int steps ) {
	steps = int( 1+ ( (float)wiper/8.0 ))*steps;
	printf("steps %d wiper %d\n\n", steps, wiper );
	while( steps && (wiper > 0) ){
		wiper--;
		steps--;
	}
};
void ESPAudio::decVolume( int steps ) {
	steps = int( 1+ ( (float)wiper/8.0 ))*steps;
	while( steps && (wiper < 127) ){
		wiper++;
		steps--;
	}
};

void ESPAudio::setVolume( int vol ) {
	wiper = vol;
};


bool output_enable = false;


void  ESPAudio::adjustVolume(){
	if( cur_wiper != wiper ) {
		printf("*****  SET WIPER=%d\n", wiper );
		if( sound_on ) {
			Poti.writeWiper( wiper );
		}
		cur_wiper = wiper;
	}
}

void ESPAudio::voltask(void* arg )
{
	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();
		adjustVolume();
		vTaskDelayUntil(&xLastWakeTime, 50/portTICK_PERIOD_MS);
	}
}



void ESPAudio::dactask(void* arg )
{
	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();
		tick++;
		if( !_testmode ) {
			float te = Audio.getTE();
			float max;
			if ( te > 0 )
				max = Audio.getCenter() * Audio.getVariation();
			else
				max = Audio.getCenter() / Audio.getVariation();
			float f;
			if( _s2f_mode )
				f = (float)Audio.getCenter() + ( (te/5.0 ) * max );
			else
				f = (float)Audio.getCenter() + ( (te/_range ) * max );
			bool sound=true;
			int step = int( (f/freq_step ) + 0.5);
			if( Audio.inDeadBand(te) || Audio.getMute()  ){
				sound = false;
			}
			else{
				if( hightone && (_tonemode == 1)  ){
					step = int( (f*_high_tone_var/freq_step) + 0.5);
				}
				else if( hightone && (_tonemode == 0) ){
					if( (_chopping_mode == BOTH_CHOP) ||
						(_s2f_mode && (_chopping_mode == S2F_CHOP)) ||
						(!_s2f_mode && (_chopping_mode == VARIO_CHOP)) ) {
						sound = false;
					}
				}
			}

			if( sound ){
				if( !sound_on ) {
					Poti.writeWiper( wiper );
					dac_output_enable(_ch);
					sound_on = true;
				}
			}
			else{
				if( sound_on ) {
						if( cur_wiper != 0 ) {
							Poti.writeWiper( 0 );
						}
						dac_output_disable(_ch);
						sound_on = false;
				}
			}
			// assert( heap_caps_check_integrity_all(true) == true );
			Audio.dac_frequency_set(clk_8m_div, step);
		}
		vTaskDelayUntil(&xLastWakeTime, 20/portTICK_PERIOD_MS);
	}
}

bool ESPAudio::inDeadBand( float te )
{
	float dbp = _deadband;
	float dbn = _deadband_neg;
	if( _s2f_mode ) {
		dbp = _s2f_deadband;
		dbn = -_s2f_deadband;
	}
	if( te > 0 ) {
		if( te < dbp )
			return true;
	}
	else {
		if( te > dbn )
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
	float max = _range;
	if( _s2f_mode ) {
		_te = -s2fd/10.0;
		max = 5.0;
	}
	else {
		_te = te;
	}
	if( _te > max )
		_te = max;
	else if( _te < -max )
		_te = -max;
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
	_s2f_deadband = _setup->get()->_s2f_deadband/10.0;
	_dead_mute = true;
	_mute = false;
	if( _setup->get()->_audio_range == 0 )
		_range = 5.0;
	else
		_range = _setup->get()->_range;
	_tonemode = _setup->get()->_dual_tone;
	_high_tone_var = ((_setup->get()->_high_tone_var + 100.0)/100);
	_chopping_mode = _setup->get()->_chopping_mode;

}

void ESPAudio::restart()
{
	dac_output_disable(_ch);
	dac_cosine_enable(_ch);
	dac_offset_set(_ch, offset);
	dac_invert_set(_ch, 2 );
	dac_scale_set(_ch, 0 );
	dac_output_enable(_ch);
	_testmode = false;
	_dead_mute = true;
	Poti.writeWiper( cur_wiper );
}


void ESPAudio::begin( dac_channel_t ch, gpio_num_t button, Setup *asetup )
{
	_setup = asetup;
	setup();
	mute();
	_ch = ch;
	_button = button;
	restart();

	gpio_set_direction(_button, GPIO_MODE_INPUT);
	gpio_set_pull_mode(_button, GPIO_PULLUP_ONLY);

	xTaskCreate(modtask, "modtask", 1024*2, NULL, 31, NULL);
	xTaskCreate(dactask, "dactask", 1024*2, NULL, 31, NULL);
	xTaskCreate(voltask, "voltask", 1024*2, NULL, 21, NULL);

	Poti.begin(GPIO_NUM_21, GPIO_NUM_22);

	cur_wiper = (uint16_t)( ( (_setup->get()->_default_volume * 100.0) / 128) -1 );
	wiper = cur_wiper;
	Poti.writeWiper( cur_wiper );
	// Enable Audio Amplifiler
	gpio_set_direction(GPIO_NUM_19, GPIO_MODE_INPUT );   // use pullup 1 == SOUND 0 == SILENCE
	gpio_set_pull_mode(GPIO_NUM_19, GPIO_FLOATING);      // ESP32 level too low from PAM enable
	// Sound::setPoti( &Poti );
}



void ESPAudio::disable( bool disable ) {
	_disable = disable;
	if( _disable )
	{
		mute( true );
		delay(100);
		dac_output_disable(_ch);
	}
	else
	{
		dac_output_enable(_ch);
		delay(100);
		mute( false );
	}
};

void ESPAudio::test( float to, float from )
{
	Audio.setTestmode( true );
	if( to > from){
		for( float tef=from; tef < to; tef=tef+0.05 ){
			Audio.setValues( tef, 0, true );
			delay(50);
		}
	}
	else{
		for( float tef=from; tef > to; tef=tef-0.05 ){
			Audio.setValues( tef, 0, true );
			delay(50);
		}
	}
}


ESPAudio Audio;
