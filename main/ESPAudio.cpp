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
#include "cat5171.h"
#include <Arduino.h>
#include <vector>
#include <logdef.h>
#include "Switch.h"
#include "I2Cbus.hpp"
#include "sensor.h"
#include "SetupNG.h"
#include "Poti.h"


TaskHandle_t *dactid;

uint8_t Audio::_tonemode;
uint16_t *Audio::p_wiper;
uint16_t Audio::wiper;
uint16_t Audio::wiper_s2f;
uint16_t Audio::cur_wiper;
dac_channel_t Audio::_ch;

bool  Audio::_chopping = false;
float Audio::_range = 5.0;
float Audio::_high_tone_var;
bool  Audio::_s2f_mode = false;
bool  Audio::sound_enabled=false;
bool  Audio::sound=false;
bool  Audio::_testmode=false;
bool  Audio::deadband_active = false;
bool  Audio::hightone = false;
int   Audio::_delay=100;
int   Audio::mtick = 0;
bool  Audio::_alarm_mode=false;
bool  Audio::_s2f_mode_back = false;
unsigned long Audio::next_scedule=0;

uint16_t Audio::_vol_back = 0;
uint16_t Audio::_vol_back_s2f = 0;
float Audio::maxf = 2000;
float Audio::minf = 250;
float Audio::current_frequency;
float Audio::_te = 0;
float Audio::exponent_max = 2;
float Audio::prev_aud_fact = 0;

int   Audio::prev_div = 0;
int   Audio::prev_step = 0;
int   Audio::scale = 0;
int   Audio::prev_scale = -1;
int   Audio::defaultDelay = 500;
int   Audio::_tonemode_back = 0;
int   Audio::tick = 0;
int   Audio::volume_change=0;
int   Audio::_step = 2;
bool  Audio::prev_alarm=false;
bool  Audio::dac_enable=false;

const int clk_8m_div = 7;    // RTC 8M clock divider (division is by clk_8m_div+1, i.e. 0 means 8MHz frequency)
const float freq_step = RTC_FAST_CLK_FREQ_APPROX / (65536 * 8 );  // div = 0x07
typedef struct lookup {  uint16_t f; uint8_t div; uint8_t step; } t_lookup_entry;
typedef struct volume {  uint16_t vol; uint8_t scale; uint8_t wiper; } t_scale_wip;

#define FADING_STEPS 5  // steps used for fade in/out at chopping
#define FADING_TIME  3  // factor for volume changes fade over smoothing

Poti *DigitalPoti;


Audio::Audio( ) {
	_ch = DAC_CHANNEL_1;
	_te = 0.0;
	_testmode = false;
	_range = 5.0;
	_s2f_mode = false;
	wiper = 63;
	wiper_s2f = 63;
	cur_wiper = 63;
	sound_enabled = false;
}



PROGMEM std::vector<t_lookup_entry> lftab{
	{ 18,6,1 },		{ 21,5,1 },		{ 25,4,1 },		{ 32,3,1 },		{ 37,6,2 },		{ 43,5,2 },		{ 51,4,2 },		{ 55,6,3 },
	{ 64,5,3 },		{ 74,6,4 },		{ 77,4,3 },		{ 86,5,4 },		{ 92,6,5 },		{ 97,3,3 },		{ 103,4,4 },	{ 108,5,5 },
	{ 111,6,6 },	{ 129,6,7 },	{ 148,6,8 },	{ 151,5,7 },	{ 155,4,6 },	{ 162,3,5 },	{ 166,6,9 },	{ 172,5,8 },
	{ 181,4,7 },	{ 185,6,10 },	{ 194,5,9 },	{ 203,6,11 },	{ 207,4,8 },	{ 216,5,10 },	{ 222,6,12 },	{ 226,3,7 },
	{ 233,4,9 },	{ 237,5,11 },	{ 240,6,13 },	{ 259,6,14 },	{ 277,6,15 },	{ 281,5,13 },	{ 285,4,11 },	{ 291,3,9 },
	{ 296,6,16 },	{ 302,5,14 },	{ 311,4,12 },	{ 314,6,17 },	{ 324,5,15 },	{ 333,6,18 },	{ 337,4,13 },	{ 345,5,16 },
	{ 352,6,19 },	{ 356,3,11 },	{ 363,4,14 },	{ 367,5,17 },	{ 370,6,20 },	{ 389,6,21 },	{ 407,6,22 },	{ 410,5,19 },
	{ 415,4,16 },	{ 421,3,13 },	{ 426,6,23 },	{ 432,5,20 },	{ 440,4,17 },	{ 444,6,24 },	{ 453,5,21 },	{ 463,6,25 },
	{ 466,4,18 },	{ 475,5,22 },	{ 481,6,26 },	{ 486,3,15 },	{ 492,4,19 },	{ 497,5,23 },	{ 500,6,27 },	{ 518,6,28 },
	{ 537,6,29 },	{ 540,5,25 },	{ 544,4,21 },	{ 551,3,17 },	{ 555,6,30 },	{ 562,5,26 },	{ 570,4,22 },	{ 574,6,31 },
	{ 583,5,27 },	{ 592,6,32 },	{ 596,4,23 },	{ 605,5,28 },	{ 611,6,33 },	{ 616,3,19 },	{ 622,4,24 },	{ 626,5,29 },
	{ 629,6,34 },	{ 648,6,35 },	{ 667,6,36 },	{ 670,5,31 },	{ 674,4,26 },	{ 680,3,21 },	{ 685,6,37 },	{ 691,5,32 },
	{ 700,4,27 },	{ 704,6,38 },	{ 713,5,33 },	{ 722,6,39 },	{ 726,4,28 },	{ 734,5,34 },	{ 741,6,40 },	{ 745,3,23 },
	{ 752,4,29 },	{ 756,5,35 },	{ 759,6,41 },	{ 778,6,42 },	{ 796,6,43 },	{ 799,5,37 },	{ 804,4,31 },	{ 810,3,25 },
	{ 815,6,44 },	{ 821,5,38 },	{ 830,4,32 },	{ 833,6,45 },	{ 843,5,39 },	{ 852,6,46 },	{ 856,4,33 },	{ 864,5,40 },
	{ 870,6,47 },	{ 875,3,27 },	{ 881,4,34 },	{ 886,5,41 },	{ 889,6,48 },	{ 907,6,49 },	{ 926,6,50 },	{ 929,5,43 },
	{ 933,4,36 },	{ 940,3,29 },	{ 944,6,51 },	{ 951,5,44 },	{ 959,4,37 },	{ 963,6,52 },	{ 972,5,45 },	{ 982,6,53 },
	{ 985,4,38 },	{ 994,5,46 },	{ 1000,6,54 }
};


/* Declare global sine waveform parameters
 * so they may be then accessed and changed from debugger
 * over an JTAG interface
 */

/*
 * Enable cosine waveform generator on a DAC channel
 */
void Audio::dac_cosine_enable(dac_channel_t channel, bool enable)
{
	// Enable tone generator common to both channels
	ESP_LOGD(FNAME,"Audio::dac_cosine_enable ch: %d", channel);
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
		ESP_LOGD(FNAME,"Channel %d", channel);
	}
}


void Audio::begin( dac_channel_t ch  )
{
	ESP_LOGI(FNAME,"Audio::begin");
	Switch::begin( GPIO_NUM_12 );
	setup();
	_ch = ch;
	restart();
	_testmode = true;
	delay(10);
}

bool Audio::selfTest(){
	ESP_LOGI(FNAME,"Audio::selfTest");
	DigitalPoti = new MCP4018();
	DigitalPoti->setBus( &i2c );
	DigitalPoti->begin();
	if( !DigitalPoti->haveDevice() )
	{
		ESP_LOGI(FNAME,"Try CAT5171 digital Poti");
		delete DigitalPoti;
		DigitalPoti = new CAT5171();
		DigitalPoti->setBus( &i2c );
		DigitalPoti->begin();
		if( DigitalPoti->haveDevice() )
			ESP_LOGI(FNAME,"CAT5171 digital Poti found");
		else{
			ESP_LOGW(FNAME,"NO digital Poti found !");
			return false;
		}
	}
	else
	{
		ESP_LOGI(FNAME,"MCP4018 digital Poti found");
	}
	_step = DigitalPoti->getStep();

	uint16_t setwiper = ((default_volume.get() * 100.0) / DigitalPoti->getRange());
	wiper = wiper_s2f = setwiper;
	p_wiper = &wiper;
	ESP_LOGI(FNAME,"default volume/wiper: %d", (*p_wiper) );
	ESP_LOGI(FNAME, "selfTest wiper: %d", wiper );
	DigitalPoti->writeWiper( setwiper );
	uint16_t getwiper;
	bool ret = DigitalPoti->readWiper( getwiper );
	if( ret == false ) {
		ESP_LOGI(FNAME,"readWiper returned error");
		return false;
	}
	if( getwiper != setwiper )  // begin sets already cur_wiperw
	{
		ESP_LOGI(FNAME,"readWiper returned wrong setting set=%d get=%d", setwiper, getwiper );
		ret = false;
	}
	else
		ret = true;

	restart();
	for( float f=261.62; f<1046.51; f=f*1.03){
		ESP_LOGV(FNAME,"f=%f",f);
		current_frequency = f;
		setFrequency( f );
		DigitalPoti->writeWiper( equal_volume( setwiper ) );
		delay(30);
		esp_task_wdt_reset();
	}

	delay(200);
	ESP_LOGI(FNAME, "selfTest wiper: %d", 0 );
	DigitalPoti->writeWiper( 0 );
	setFrequency( 440 );
	_testmode=true;
	return ret;
}

/*

  freq = dig_clk_rtc_freq x SENS_SAR_SW_FSTEP / 65536

  frequency = ( (RTC_FAST_CLK_FREQ_APPROX / (1 + clk_8m_div) ) *  frequency_step) / 65536;

  frequency_step =  (frequency * 65536) * (1 + clk_8m_div) ) / RTC_FAST_CLK_FREQ_APPROX

   RTC_FAST_CLK_FREQ_APPROX = 8500000

   frequency_step =  (frequency * 65536) * (1 + clk_8m_div) ) / 8500000

   GPIO_NUM_19 as input programmed will enable the PAM amplifier.
   We do not use enable/disable from amplifier as this creates some pop noise
   Mute will be realized by putting down the wiper from digital poti to zero what works like a charm.

 */

void Audio::dac_frequency_set(int adiv, int frequency_step)
{
	ESP_LOGD(FNAME,"Audio::dac_frequency_set( div:%d step:%d )", adiv, frequency_step );
	REG_SET_FIELD(RTC_CNTL_CLK_CONF_REG, RTC_CNTL_CK8M_DIV_SEL, adiv);
	SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL1_REG, SENS_SW_FSTEP, frequency_step, SENS_SW_FSTEP_S);
}

void Audio::setFrequency( float f ){
	int step;
	int div;
	if( f < 800.0 ) {
		lookup( f, div, step );
	}
	else {
		step = int( (f/freq_step) + 0.5);
		div = clk_8m_div;
	}
	if ( prev_div != div || prev_step != step ) {
		prev_div = div;
		prev_step = step;
		dac_frequency_set(div, step);
	}
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
void Audio::dac_scale_set(dac_channel_t channel, int scale)
{
	if( scale != prev_scale ) {
		switch(channel) {
		case DAC_CHANNEL_1:
			SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_SCALE1, scale, SENS_DAC_SCALE1_S);
			break;
		case DAC_CHANNEL_2:
			SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_SCALE2, scale, SENS_DAC_SCALE2_S);
			break;
		default :
			ESP_LOGD(FNAME,"Channel %d", channel);
		}
		prev_scale = scale;
	}
}

void Audio::alarm( bool enable, int volume, e_audio_alarm_type_t style ){  // non blocking
	if( enable ) {
		// ESP_LOGI(FNAME,"Alarm sound enable volume: %d style: %d", volume, style );
		if( !prev_alarm ){
			prev_alarm = true;
			_vol_back_s2f = wiper_s2f;
			_vol_back = wiper;
			ESP_LOGI(FNAME,"Alarm volume backup %d", (*p_wiper) );
		}
		enableAmplifier( true );
		_s2f_mode_back = _s2f_mode;
		_s2f_mode = false;
		(*p_wiper) = volume;
		_tonemode_back = _tonemode;
		if( style == AUDIO_ALARM_STALL ){
			_te = 3.0;
			_tonemode = ATM_DUAL_TONE;
			defaultDelay = 125;
		}
		else if( style == AUDIO_ALARM_FLARM_1 ){  // lowest
			_te = 3.0;
			_tonemode = ATM_SINGLE_TONE;
			defaultDelay = 500;
		}
		else if( style == AUDIO_ALARM_FLARM_2 ){
			_te = 4.0;
			_tonemode = ATM_SINGLE_TONE;
			defaultDelay = 400;
		}
		else if( style == AUDIO_ALARM_FLARM_3 ){ // highest
			_te = 5.0;
			_tonemode = ATM_SINGLE_TONE;
			defaultDelay = 300;
		}
		else if( style == AUDIO_ALARM_GEAR ){
			_te = 2.0;
			_tonemode = ATM_DUAL_TONE;
			defaultDelay = 250;
		}
		else if( style == AUDIO_ALARM_OFF ){
			volume = 0;
		}
		else
			ESP_LOGE(FNAME,"Error, wrong alarm style %d", style );

		_alarm_mode=true;
	}
	else
	{
		if( prev_alarm ){
			prev_alarm = false;
			wiper = _vol_back;
			wiper_s2f = _vol_back_s2f;
			ESP_LOGI(FNAME,"Alarm volume restore %d", (*p_wiper) );
		}
		_s2f_mode = _s2f_mode_back;
		defaultDelay = 500;
		_alarm_mode=false;
		_tonemode = _tonemode_back;
	}
}

/*
 * Offset output of a DAC channel
 *
 * Range 0x00 - 0xFF
 *
 */
void Audio::dac_offset_set(dac_channel_t channel, int offset)
{
	switch(channel) {
	case DAC_CHANNEL_1:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_DC1, offset, SENS_DAC_DC1_S);
		break;
	case DAC_CHANNEL_2:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_DC2, offset, SENS_DAC_DC2_S);
		break;
	default :
		ESP_LOGD(FNAME,"Channel %d", channel);
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
void Audio::dac_invert_set(dac_channel_t channel, int invert)
{
	ESP_LOGD(FNAME,"DAC invert: channel %d, invert: %d", channel, invert);
	switch(channel) {
	case DAC_CHANNEL_1:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV1, invert, SENS_DAC_INV1_S);
		break;
	case DAC_CHANNEL_2:
		SET_PERI_REG_BITS(SENS_SAR_DAC_CTRL2_REG, SENS_DAC_INV2, invert, SENS_DAC_INV2_S);
		break;
	default :
		ESP_LOGD(FNAME,"Channel %d", channel);
	}
}


void Audio::setVolume( int vol ) {
	(*p_wiper) = vol;
};

void Audio::decVolume( int steps ) {
	steps = int( 1+ ( (float)(*p_wiper)/16.0 ))*steps;
	while( steps && ((*p_wiper) > 0) ){
		(*p_wiper)--;
		steps--;
	}
	volume_change = 100;
	ESP_LOGI(FNAME,"dec volume, *p_wiper: %d", (*p_wiper) );
}


void Audio::incVolume( int steps ) {
	steps = int( 1+ ( (float)(*p_wiper)/16.0 ))*steps;
	while( steps && ((*p_wiper) <  DigitalPoti->getRange()*(max_volume.get()/100)) ){
		(*p_wiper)++;
		steps--;
	}
	if( (*p_wiper) > DigitalPoti->getRange() )
		(*p_wiper) =  DigitalPoti->getRange();
	volume_change = 100;
	ESP_LOGI(FNAME,"inc volume, *p_wiper: %d", (*p_wiper) );
}

void Audio::startAudio(){
	ESP_LOGI(FNAME,"startAudio");
	_testmode = false;
	evaluateChopping();
	xTaskCreatePinnedToCore(&dactask, "dactask", 2048, NULL, 15, dactid, 0);
}

void Audio::calcS2Fmode(){
	if( !_alarm_mode ) {
		_s2f_mode = Switch::cruiseMode();
		if( _s2f_mode )
			p_wiper = &wiper_s2f;
		else
			p_wiper = &wiper;
	}
}

void  Audio::evaluateChopping(){
	if(
		(chopping_mode.get() == BOTH_CHOP) ||
		(_s2f_mode && (chopping_mode.get() == S2F_CHOP) ) ||
		( _s2f_mode && (cruise_audio_mode.get() == AUDIO_VARIO) && (chopping_mode.get() != NO_CHOP) ) ||
		(!_s2f_mode && (chopping_mode.get() == VARIO_CHOP) )
	)
		_chopping = true;
	else
		_chopping = false;
}

void Audio::dactask(void* arg )
{
	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();
		tick++;
		Switch::tick();    // we hook switch sceduling here to save extra task

		// Chopping or dual tone modulation
		if( millis() > next_scedule ){
			if ( _te > 0 ){
				mtick++;
				if ( (mtick & 1) == 1 )
					hightone = true;
				else
					hightone = false;

				float f=0;
				if( _s2f_mode && (cruise_audio_mode.get() == AUDIO_S2F) )
					f = 1+9*(_te/5.0);
				else
					f = 1+9*(_te/_range);

				float period_ms = 1000.0/f;
				if ( hightone ){  // duration of break (or second tone)
					_delay = int(period_ms * 0.1)+40;  // 1Hz: 100 mS; 10Hz: 50 mS
					// ESP_LOGI(FNAME, "Break: %d period:%4.2f %4.1f", _delay, period_ms, f );
				}
				else{  // duration of main tone 1Hz: 900 mS; 10Hz: 50 mS
					_delay = int(period_ms * 0.9)-40;
					// ESP_LOGI(FNAME, "Tone: %d period:%4.2f %4.1f", _delay, period_ms, f );
				}
			}
			else{
				_delay = 100;
				hightone = false;
			}
			// Frequency Control
			float max = minf;
			if ( _te > 0 )
				max = maxf;
			float range = _range;
			if( _s2f_mode && (cruise_audio_mode.get() == AUDIO_S2F) )
				range = 5.0;
			float mult = std::pow( (abs(_te)/range)+1, audio_factor.get());
			if( audio_factor.get() != prev_aud_fact ) {
				exponent_max  = std::pow( 2, audio_factor.get());
				prev_aud_fact = audio_factor.get();
			}
			float f = center_freq.get() + ((mult*_te)/range )  * (max/exponent_max);
			if( (int)(f/10.0) != int(current_frequency/10.0) ){
				current_frequency = center_freq.get() + ((mult*_te)/range )  * (max/exponent_max);
				DigitalPoti->writeWiper( equal_volume(*p_wiper) );
			}
			if( hightone && (_tonemode == ATM_DUAL_TONE ) )
				setFrequency( current_frequency*_high_tone_var );
			else
				setFrequency( current_frequency );
			// ESP_LOGI(FNAME, "New Freq: (%0.1f) TE:%0.2f exp_fac:%0.1f multi:%0.3f  wiper:%d", f, _te, audio_factor.get(), mult, cur_wiper );

			next_scedule = millis()+_delay;
		}

		// Amplifier and Volume control
		// Optionally disable Sound when in Menu
		if( audio_disable.get() && Menu->isActive() ){
			if( sound_enabled ){
				dacDisable();
				DigitalPoti->writeWiper( 0 );
				cur_wiper = 0;
				sound_enabled = false;
			}
		}
		else
		{
			if( !_testmode && !(tick%2) ) {
				// ESP_LOGI(FNAME, "sound dactask tick:%d wiper:%d  te:%f db:%d", tick, (*p_wiper), _te, inDeadBand(_te) );
				if( !(tick%20) )
					calcS2Fmode();
				if( (inDeadBand(_te) || ((*p_wiper) == 0 )) && !_testmode && !volume_change ){
					if( !deadband_active ) {
						// ESP_LOGI(FNAME,"Audio in DeadBand true");
						enableAmplifier(false);  // optionally power off amplifier if configured
						deadband_active = true;
					}
					sound = false;
					// ESP_LOGI(FNAME,"sound = false");
				}
				else if( !inDeadBand(_te) || (*p_wiper) > 0 || volume_change ){
					if( deadband_active ) {
						// ESP_LOGI(FNAME,"Audio in DeadBand false");
						enableAmplifier(true);
						deadband_active = false;
					}
				}
				if( deadband_active )
					sound = false;
				else{
					if( (hightone && (_tonemode == ATM_SINGLE_TONE)) ){
						if( _chopping )
							sound = false;
					}else
						sound = true;
				}
				// ESP_LOGI(FNAME, "sound %d, ht %d, te %2.1f", sound, hightone, _te );
				if( sound ){
					// ESP_LOGI(FNAME, "have sound");
					if( !sound_enabled || volume_change ) {
						if( chopping_style.get() == AUDIO_CHOP_HARD ){
							dacEnable();
						}
						else{
							if( !sound_enabled ) {
								float volume=3;
								for( int i=0; i<FADING_STEPS && (int)volume <=(*p_wiper); i++ ) {
									// ESP_LOGI(FNAME, "fade in sound, wiper: %d", volume );
									DigitalPoti->writeWiper( equal_volume( (int)volume ) );
									dacEnable();
									cur_wiper = volume;
									volume = volume*2;
									delay(1);
								}
								if(  cur_wiper != (*p_wiper) ){
									// ESP_LOGI(FNAME, "fade in sound, wiper: %d", (*p_wiper)  );
									DigitalPoti->writeWiper( equal_volume( (*p_wiper) ) );
									cur_wiper = (*p_wiper);
								}
								// ESP_LOGI(FNAME, "fade in sound, final wiper: %d", cur_wiper );
							}
						}
						sound_enabled = true;
						// Fade in/out volume changes
						if(  cur_wiper != (*p_wiper) ){
							int delta = 1;
							if( (*p_wiper) > cur_wiper )
								for( int i=cur_wiper; i<(*p_wiper); i+=delta ) {
									DigitalPoti->writeWiper( equal_volume(i) );
									delta = _step+i/FADING_TIME;
									delay(1);
								}else
									for( int i=cur_wiper; i>(*p_wiper); i-=delta ) {
										DigitalPoti->writeWiper( equal_volume(i) );
										delta = _step+i/FADING_TIME;
										delay(1);
									}
							DigitalPoti->writeWiper( equal_volume((*p_wiper)) );
							cur_wiper = (*p_wiper);
							// ESP_LOGI(FNAME, "volume change, new wiper: %d", cur_wiper );
						}
					}
				}
				else{
					if( sound_enabled ) {
						if( chopping_style.get() == AUDIO_CHOP_HARD ){
							dacDisable();
						}else{
							if( cur_wiper > 0 ) {  // turn off gracefully sound
								float volume = (float)(*p_wiper);
								for( int i=0; i<FADING_STEPS && (int)volume >=0; i++ ) {
									// ESP_LOGI(FNAME, "fade out sound, wiper: %d", volume );
									DigitalPoti->writeWiper( equal_volume( (int)volume ) );
									volume = volume*0.65;
								}
								delay(1);
								DigitalPoti->writeWiper( 0 );
								// ESP_LOGI(FNAME, "fade out sound, final wiper: 0" );
								dacDisable();
								cur_wiper = 0;
							}
						}
						sound_enabled = false;
					}
				}
			}
		}
		// ESP_LOGI(FNAME, "Audio delay %d", _delay );
		if( uxTaskGetStackHighWaterMark( dactid ) < 256 )
			ESP_LOGW(FNAME,"Warning Audio dac task stack low: %d bytes", uxTaskGetStackHighWaterMark( dactid ) );
		vTaskDelayUntil(&xLastWakeTime, 10/portTICK_PERIOD_MS);
		if( volume_change )
			volume_change--;
	}
}

uint16_t Audio::equal_volume( uint16_t volume ){
	float fdelta = current_frequency/center_freq.get();
	uint16_t new_vol = (uint16_t)( volume *( 1 - ((fdelta-1.0) * (frequency_response.get()/100.0) ) ));
	if( new_vol >=  DigitalPoti->getRange()*(max_volume.get()/100) )
		new_vol =  DigitalPoti->getRange()*(max_volume.get()/100);
	// ESP_LOGI(FNAME,"Vol: %d Scaled %d  f: %4.0f delta:%2.2f", volume, new_vol, current_frequency, fdelta );
	return new_vol;
}

bool Audio::inDeadBand( float te )
{
	float dbp=0.0;
	float dbn=0.0;
	if( _s2f_mode && (cruise_audio_mode.get() == AUDIO_S2F)) {
		dbp = s2f_deadband.get()/10;
		dbn = s2f_deadband_neg.get()/10;
	}else{
		dbp = deadband.get();
		dbn = deadband_neg.get();
	}
	if( ((int)( dbp*100 ) == 0) && ((int)( dbn*100 ) == 0)  ){
		return false;
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

void Audio::setValues( float te, float s2fd )
{
	float max = _range;
	if( !_alarm_mode ){
		if( _s2f_mode && (cruise_audio_mode.get() == AUDIO_S2F) ) {
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
}

void Audio::setup()
{
	ESP_LOGD(FNAME,"Audio::setup");
	_te = 0.0;
	_testmode = false;
	if( audio_range.get() == AUDIO_RANGE_5_MS )
		_range = 5.0;
	else if( audio_range.get() == AUDIO_RANGE_10_MS )
		_range = 10.0;
	else
		_range = range.get();
	_tonemode = dual_tone.get();
	_high_tone_var = ((high_tone_var.get() + 100.0)/100);

	maxf = center_freq.get() * tone_var.get();
	minf = center_freq.get() - ( center_freq.get() / tone_var.get() );
}

void Audio::restart()
{
	ESP_LOGD(FNAME,"Audio::restart");
	dacDisable();
	dac_cosine_enable(_ch);
	dac_offset_set(_ch, 0 );
	dac_invert_set(_ch, 2 );    // invert MSB to get sine waveform
	dac_scale_set(_ch, 2 );
	enableAmplifier( true );
	delay( 10 );
	dacEnable();
}


void Audio::shutdown(){
	dacDisable();
	gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );   // use pullup 1 == SOUND 0 == SILENCE
	gpio_set_level(GPIO_NUM_19, 0 );
	if( p_wiper != 0)
		(*p_wiper) = 0;
	ESP_LOGI(FNAME,"shutdown alarm volume set 0");
	sound_enabled = false;
}

void Audio::enableAmplifier( bool enable )
{
	ESP_LOGV(FNAME,"Audio::enableAmplifier( %d )", (int)enable );
	// enable Audio
	if( enable )
	{
		gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );   // use pullup 1 == SOUND 0 == SILENCE
		gpio_set_level(GPIO_NUM_19, 1 );
		dacEnable();
	}
	else {
		if( amplifier_shutdown.get() ){
			dacEnable();
			gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );   // use pullup 1 == SOUND 0 == SILENCE
			gpio_set_level(GPIO_NUM_19, 0 );
		}
	}
}

void Audio::dacEnable(){
	if( !dac_enable ){
		dac_output_enable(_ch);
		dac_enable = true;
	}
}
void Audio::dacDisable(){
	if( dac_enable ){
		dac_output_disable(_ch);
		dac_enable = false;
	}
}

bool Audio::lookup( float f, int& div, int &step ){
	int fi = (int)(f + 0.5);
	if( fi < 19  || fi > 1000 ) {
		ESP_LOGW(FNAME,"f out of bounds");
		return false;
	}
	int i = (fi-19)/8;
	if( i >= lftab.size() ) {
		ESP_LOGW(FNAME,"i out of bounds i:%d tab:%d", i, lftab.size() );
		return false;
	}
	if( f  > lftab[i].f ) {
		while( f > lftab[i].f ){
			i++;
		}
	}
	else{
		while( f < lftab[i].f ){
			i--;
		}
	}
	if( abs( fi - lftab[i-1].f ) < abs( fi - lftab[i].f )) {
		div =  lftab[i-1].div;
		step = lftab[i-1].step;
	}
	else {
		div =  lftab[i].div;
		step = lftab[i].step;
	}
	// ESP_LOGD(FNAME, "found at %d d:%d s:%d with f:%d for f%d", i, div, step, lftab[i-1].f, fi );
	return true;
}

