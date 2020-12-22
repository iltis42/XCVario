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
#include <vector>
#include <logdef.h>
#include "Switch.h"
#include "I2Cbus.hpp"
#include "sensor.h"


uint8_t Audio::_tonemode;
uint8_t Audio::_chopping_mode;
uint16_t Audio::wiper;
uint16_t Audio::cur_wiper;
dac_channel_t Audio::_ch;

float Audio::_range = 5.0;
float Audio::_high_tone_var;
bool  Audio::_s2f_mode = false;
bool  Audio::sound_on=false;
bool  Audio::_testmode=false;
bool  Audio::deadband_active = false;
bool  Audio::hightone = false;
bool  Audio::_alarm_mode=false;
bool  Audio::_s2f_mode_back = false;

float Audio::_vol_back = 0;
float Audio::maxf = 2000;
float Audio::minf = 250;
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
int   Audio::tickmod  = 0;

const int clk_8m_div = 7;    // RTC 8M clock divider (division is by clk_8m_div+1, i.e. 0 means 8MHz frequency)
const float freq_step = RTC_FAST_CLK_FREQ_APPROX / (65536 * 8 );  // div = 0x07
typedef struct lookup {  uint16_t f; uint8_t div; uint8_t step; } t_lookup_entry;
typedef struct volume {  uint16_t vol; uint8_t scale; uint8_t wiper; } t_scale_wip;

MCP4018 Poti;


Audio::Audio( ) {
	_ch = DAC_CHANNEL_1;
	_te = 0.0;
	_testmode = false;
	_range = 5.0;
	_s2f_mode = false;
	wiper = 63;
	cur_wiper = 63;
	sound_on = false;
}

// Table to lookup settings audio generator setting divider and step covering frequencies up to 800 Hz
PROGMEM std::vector<t_scale_wip> scaletab{
	{ 0,2,0 },	    { 1,2,1 },	    { 2,2,4 },	    { 3,2,7 },	    { 4,2,10 },	    { 5,2,13 },	    { 6,2,16 },	    { 7,2,19 },
	{ 8,2,22 },	    { 9,2,25 },	    { 10,2,28 },    { 11,2,31 },	{ 12,2,34 },	{ 13,2,37 },	{ 14,2,40 },	{ 15,2,43 },
	{ 16,2,46 },	{ 17,2,49 },	{ 18,2,52 },	{ 19,2,55 },	{ 20,2,58 },	{ 21,2,61 },	{ 22,2,64 },	{ 23,2,67 },
	{ 24,2,70 },	{ 25,2,73 },	{ 26,2,76 },	{ 27,2,79 },	{ 28,2,82 },	{ 29,2,85 },	{ 30,2,88 },	{ 31,2,91 },
	{ 32,2,94 },	{ 33,2,97 },	{ 34,2,100 },	{ 35,2,103 },	{ 36,2,106 },	{ 37,2,109 },	{ 38,2,112 },	{ 39,2,115 },
	{ 40,2,118 },	{ 41,2,121 },	{ 42,2,124 },	{ 43,2,127 },	{ 44,1,86 },	{ 45,1,88 },	{ 46,1,90 },	{ 47,1,92 },
	{ 48,1,94 },	{ 49,1,96 },	{ 50,1,98 },	{ 51,1,100 },	{ 52,1,102 },	{ 53,1,104 },	{ 54,1,106 },	{ 55,1,108 },
	{ 56,1,110 },	{ 57,1,112 },	{ 58,1,114 },	{ 59,1,116 },	{ 60,1,118 },	{ 61,1,120 },	{ 62,1,122 },	{ 63,1,124 },
	{ 64,1,126 },	{ 65,1,127 },	{ 66,0,65 },	{ 67,0,66 },	{ 68,0,67 },	{ 69,0,68 },	{ 70,0,69 },	{ 71,0,70 },
	{ 72,0,71 },	{ 73,0,72 },	{ 74,0,73 },	{ 75,0,74 },	{ 76,0,75 },	{ 77,0,76 },	{ 78,0,77 },	{ 79,0,78 },
	{ 80,0,79 },	{ 81,0,80 },	{ 82,0,81 },	{ 83,0,82 },	{ 84,0,83 },	{ 85,0,84 },	{ 86,0,85 },	{ 87,0,86 },
	{ 88,0,87 },	{ 89,0,88 },	{ 90,0,89 },	{ 91,0,90 },	{ 92,0,91 },	{ 93,0,92 },	{ 94,0,93 },	{ 95,0,94 },
	{ 96,0,95 },	{ 97,0,96 },	{ 98,0,97 },	{ 99,0,98 },	{ 100,0,99 },	{ 101,0,100 },	{ 102,0,101 },	{ 103,0,102 },
	{ 104,0,103 },	{ 105,0,104 },	{ 106,0,105 },	{ 107,0,106 },	{ 108,0,107 },	{ 109,0,108 },	{ 110,0,109 },	{ 111,0,110 },
	{ 112,0,111 },	{ 113,0,112 },	{ 114,0,113 },	{ 115,0,114 },	{ 116,0,115 },	{ 117,0,116 },	{ 118,0,117 },	{ 119,0,118 },
	{ 120,0,119 },	{ 121,0,120 },	{ 122,0,121 },	{ 123,0,122 },	{ 124,0,123 },	{ 125,0,124 },	{ 126,0,125 },	{ 127,0,126 },
	{ 128,0,127 }
};


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

bool Audio::selfTest(){
	ESP_LOGI(FNAME,"Audio::selfTest");
	uint16_t getwiper;
	uint16_t setwiper = ((default_volume.get() * 100.0) / 128) -1 ;
	ESP_LOGI(FNAME, "selfTest wiper: %d", wiper );
	Poti.haveDevice();
	Poti.writeWiper( setwiper );
	bool ret = Poti.readWiper( getwiper );
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

	dac_output_enable(_ch);
	for( float f=261.62; f<1046.51; f=f*1.03){
		ESP_LOGV(FNAME,"f=%f",f);
		setFrequency( f );
		delay(30);
		esp_task_wdt_reset();
	}

	delay(200);
	ESP_LOGI(FNAME, "selfTest wiper: %d", 0 );
	Poti.writeWiper( 0 );
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



void Audio::alarm( bool enable ){  // non blocking
	if( enable ) {
		_s2f_mode_back = _s2f_mode;
		_s2f_mode = false;
		setValues( 3.0, 0  );
		_alarm_mode=true;
		_tonemode_back = _tonemode;
		_tonemode = ATM_DUAL_TONE;
		_vol_back = wiper;
		wiper = 100;
		defaultDelay = 125;
	}
	else
	{
		_s2f_mode = _s2f_mode_back;
		defaultDelay = 500;
		_alarm_mode=false;
		_tonemode = _tonemode_back;
		wiper = _vol_back;
		Poti.writeWiper( _vol_back );
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


//  modulation frequency
void Audio::modtask(void* arg )
{
	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();
		tickmod++;
		hightone = false;
		if ( _te > 0 )
			if ( (tickmod & 1) == 1 )
				hightone = true;
		int delay = 100;
		if ( !hightone )
		{
			int delaydelta = 0;
			if (_te > 0.05 ){
				if( _s2f_mode )
					delaydelta = defaultDelay * 0.8 *(_te/5.0);
				else
					delaydelta = defaultDelay * 0.8 *(_te/_range);
			}
			delay = int( defaultDelay - delaydelta + 0.5 );
		}
		// ESP_LOGI(FNAME,"delay:%d  ht:%d", delay, hightone);
		vTaskDelayUntil(&xLastWakeTime, delay/portTICK_PERIOD_MS);
	}
}

void Audio::incVolume( int steps ) {
	steps = int( 1+ ( (float)wiper/16.0 ))*steps;
	while( steps && (wiper > 0) ){
		wiper--;
		steps--;
	}
	ESP_LOGI(FNAME,"inc volume, wiper: %d", wiper );
}

void Audio::decVolume( int steps ) {
	steps = int( 1+ ( (float)wiper/16.0 ))*steps;
	while( steps && (wiper < 127) ){
		wiper++;
		steps--;
	}
	ESP_LOGI(FNAME,"dec volume, wiper: %d", wiper );
}

void Audio::startAudio(){
	_testmode = false;
	xTaskCreate(modtask, "modtask", 1024*2, NULL, 31, NULL);
	xTaskCreate(dactask, "dactask", 1024*2, NULL, 30, NULL);
}

void Audio::calcS2Fmode(){
	if( !_alarm_mode ) {
		switch( audio_mode.get() ) {
		case 0: // Vario
			_s2f_mode = false;
			break;
		case 1: // S2F
			_s2f_mode = true;
			break;
		case 2: // Switch
			_s2f_mode = Switch::cruiseMode(false);
			break;
		case 3: // Auto
			if( Switch::cruiseMode() )
				_s2f_mode = true;
			else
				_s2f_mode = false;
			break;
		}
	}
}

void Audio::dactask(void* arg )
{
	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();
		tick++;
		Switch::tick();    // we hook switch sceduling here to save extra task
		if( !_testmode ) {
			if( !(tick%20) )
				calcS2Fmode();
			bool sound=true;
			if( (inDeadBand(_te) || (wiper == 0 )) && !_testmode ){
				if( !deadband_active ) {
					ESP_LOGD(FNAME,"Audio in DeadBand true");
					enableAmplifier(false);
					deadband_active = true;
				}
				sound = false;
			}
			else if( !inDeadBand(_te) || wiper > 0 ){
				if( deadband_active ) {
					ESP_LOGD(FNAME,"Audio in DeadBand false");
					enableAmplifier(true);
					deadband_active = false;
				}
				if( hightone && (_tonemode == ATM_SINGLE_TONE) ){
					if( (_chopping_mode == BOTH_CHOP) ||
							(_s2f_mode && (_chopping_mode == S2F_CHOP)) ||	(!_s2f_mode && (_chopping_mode == VARIO_CHOP)) ) {
						sound = false;
					}
				}
			}
			ESP_LOGV(FNAME, "sound dactask %d", tick );
			if( sound ){
				ESP_LOGV(FNAME, "have sound");
				if( !sound_on  || (cur_wiper != wiper) ) {
					if( !sound_on ) {
						for( int i=1; i<=8; i++ ) {
							int nw=(wiper/8) * i;
							Poti.writeWiper( nw );
							delayMicroseconds( 5 );
							// ESP_LOGI(FNAME, "fade in sound, wiper: %d", nw);
						}
					}
					Poti.writeWiper( wiper );
					// ESP_LOGI(FNAME, "sound on, set wiper: %d", wiper );
					cur_wiper = wiper;
					sound_on = true;
				}
				float max = minf;
				if ( _te > 0 )
					max = maxf;
				float range = _range;
				if( _s2f_mode )
					range = 5.0;
				float mult = std::pow( (abs(_te)/range)+1, audio_factor.get());
				if( audio_factor.get() != prev_aud_fact ) {
					exponent_max  = std::pow( 2, audio_factor.get());
					prev_aud_fact = audio_factor.get();
				}
				float f = center_freq.get() + ((mult*_te)/range )  * (max/exponent_max);
				ESP_LOGV(FNAME, "New Freq: (%0.1f) TE:%0.2f exp_fac:%0.1f multi:%0.3f", f, _te, audio_factor.get(), mult );
				if( hightone && (_tonemode == ATM_DUAL_TONE ) )
					setFrequency( f*_high_tone_var );
				else
					setFrequency( f );
			}else{
				if( sound_on ) {
					if( cur_wiper > 1 ) {  // turn off gracefully sound
						for( int i=8; i>=1; i-- ) {
							int nw=(wiper/8) * i;
							Poti.writeWiper(nw);  // fade out volume
							delayMicroseconds( 5 );
							// ESP_LOGI(FNAME, "fade out sound, set wiper: %d", nw );
						}
						Poti.writeWiper( 1 );
						cur_wiper = 1;
					}
					sound_on = false;
				}
			}
			// assert( heap_caps_check_integrity_all(true) == true );
		}
		vTaskDelayUntil(&xLastWakeTime, 20/portTICK_PERIOD_MS);
	}
}

bool Audio::inDeadBand( float te )
{
	float dbp;
	float dbn;
	if( _s2f_mode ) {
		dbp = s2f_deadband.get()/10;
		dbn = s2f_deadband_neg.get()/10;
	}else{
		dbp = deadband.get();
		dbn = deadband_neg.get();
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
	_chopping_mode = chopping_mode.get();

	maxf = center_freq.get() * tone_var.get();
	minf = center_freq.get() - ( center_freq.get() / tone_var.get() );
}

void Audio::restart()
{
	ESP_LOGD(FNAME,"Audio::restart");
	dac_output_disable(_ch);
	dac_cosine_enable(_ch);
	dac_offset_set(_ch, 0 );
	dac_invert_set(_ch, 2 );    // invert MSB to get sine waveform
	dac_scale_set(_ch, 2 );
	enableAmplifier( true );
	ESP_LOGV(FNAME, "restart wiper: %d", wiper );
	Poti.writeWiper( wiper );
	delay( 10 );
	dac_output_enable(_ch);
}

void Audio::begin( dac_channel_t ch  )
{
	ESP_LOGI(FNAME,"Audio::begin");
	Switch::begin( GPIO_NUM_12 );
	Poti.setBus( &i2c );
	Poti.begin();
	setup();
	_ch = ch;
	wiper = (uint16_t)( ( (default_volume.get() * 100.0) / 128) -1 );
	restart();
	_testmode = true;
	delay(10);
}

void Audio::enableAmplifier( bool enable )
{
	ESP_LOGV(FNAME,"Audio::enableAmplifier( %d )", (int)enable );
	// enable Audio
	if( enable )
	{
		gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );   // use pullup 1 == SOUND 0 == SILENCE
		gpio_set_level(GPIO_NUM_19, 1 );
	}
	else {
		gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );   // use pullup 1 == SOUND 0 == SILENCE
		gpio_set_level(GPIO_NUM_19, 0 );
	}
}

bool Audio::volumeScale( int volume, int& scale, int &wiper ) {
	if( volume < 128 ){
		scale = scaletab[ volume ].scale;
		wiper   = scaletab[ volume ].wiper;
		ESP_LOGI(FNAME,"volumeScale v:%d s:%d w:%d", volume, scale, wiper );
		return true;
	}
	return false;
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

