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
#include <map>
#include <logdef.h>
#include "Switch.h"
#include "I2Cbus.hpp"
#include "sensor.h"
#include "SetupNG.h"


static TaskHandle_t dactid = NULL;

uint8_t Audio::_tonemode;
float Audio::vario_mode_volume;
float Audio::s2f_mode_volume;
float Audio::speaker_volume;
float Audio::current_volume;
dac_channel_t Audio::_ch;

bool  Audio::_chopping = false;
float Audio::_range = 5.0;
float Audio::_high_tone_var;
bool  Audio::_s2f_mode = false;
bool  Audio::sound=false;
bool  Audio::_testmode=false;
bool  Audio::deadband_active = false;
bool  Audio::disable_amp = false;
bool  Audio::hightone = false;
int   Audio::_delay=100;
int   Audio::mtick = 0;
bool  Audio::_alarm_mode=false;
bool  Audio::_s2f_mode_back = false;
unsigned long Audio::next_scedule=0;

float Audio::_vol_back_vario = 0;
float Audio::_vol_back_s2f = 0;
float Audio::maxf;
float Audio::minf;
float Audio::current_frequency;
float Audio::_te = 0;
float Audio::inv_exp_max = 0.5;
float Audio::prev_aud_fact = 0;

int   Audio::prev_div = 0;
int   Audio::prev_step = 0;
int   Audio::scale = 0;
int   Audio::dac_scale = -1;
int   Audio::_tonemode_back = 0;
int   Audio::tick = 0;
int   Audio::volume_change=0;
bool  Audio::dac_enabled=false;
bool  Audio::amplifier_enabled=false;
bool  Audio::_haveCAT5171=false;

int   Audio::old_tick = 0;
int   Audio::silent_ticks = 0;   // keep track of how long in sink or deadband
int   Audio::chop_style;
int   Audio::sink_style;
bool  Audio::long_silence;
bool  Audio::scheduled;

const int clk_8m_div = 7;    // RTC 8M clock divider (division is by clk_8m_div+1, i.e. 0 means 8MHz frequency)
const float freq_step = RTC_FAST_CLK_FREQ_APPROX / (65536 * 8 );  // div = 0x07
typedef struct lookup {  uint8_t div; uint8_t step; } t_lookup_entry;
//typedef struct volume {  uint16_t vol; uint8_t scale; uint8_t wiper; } t_scale_wip;

Poti *DigitalPoti;

Audio::Audio( ) {
	_ch = DAC_CHANNEL_1;
	_te = 0.0;
	_testmode = false;
	_range = 5.0;
	_s2f_mode = false;
	vario_mode_volume = 63;
	s2f_mode_volume = 63;
	current_volume = 63;
	speaker_volume = 63;
}

PROGMEM std::map<uint16_t, t_lookup_entry> lftab{
	{ 18,{ 6,1}},   { 21,{5,1} },	{ 25,{4,1} },	{ 32,{3,1} },	{ 37,{6,2} },	{ 43,{5,2} },	{ 51,{4,2} },	{ 55,{6,3} },
	{ 64,{5,3}},    { 74,{6,4} },	{ 77,{4,3} },	{ 86,{5,4} },	{ 92,{6,5} },	{ 97,{3,3} },	{ 103,{4,4} },	{ 108,{5,5} },
	{ 111,{6,6}},	{ 129,{6,7} },	{ 148,{6,8} },	{ 151,{5,7} },	{ 155,{4,6} },	{ 162,{3,5} },	{ 166,{6,9} },	{ 172,{5,8} },
	{ 181,{4,7}},	{ 185,{6,10} },	{ 194,{5,9} },	{ 203,{6,11} },	{ 207,{4,8} },	{ 216,{5,10} },	{ 222,{6,12} },	{ 226,{3,7} },
	{ 233,{4,9}},	{ 237,{5,11} },	{ 240,{6,13} },	{ 259,{6,14} },	{ 277,{6,15} },	{ 281,{5,13} },	{ 285,{4,11} },	{ 291,{3,9} },
	{ 296,{6,16}},	{ 302,{5,14} },	{ 311,{4,12} },	{ 314,{6,17} },	{ 324,{5,15} },	{ 333,{6,18} },	{ 337,{4,13} },	{ 345,{5,16} },
	{ 352,{6,19}},	{ 356,{3,11} },	{ 363,{4,14} },	{ 367,{5,17} },	{ 370,{6,20} },	{ 389,{6,21} },	{ 407,{6,22} },	{ 410,{5,19} },
	{ 415,{4,16}},	{ 421,{3,13} },	{ 426,{6,23} },	{ 432,{5,20} },	{ 440,{4,17} },	{ 444,{6,24} },	{ 453,{5,21} },	{ 463,{6,25} },
	{ 466,{4,18}},	{ 475,{5,22} },	{ 481,{6,26} },	{ 486,{3,15} },	{ 492,{4,19} },	{ 497,{5,23} },	{ 500,{6,27} },	{ 518,{6,28} },
	{ 537,{6,29}},	{ 540,{5,25} },	{ 544,{4,21} },	{ 551,{3,17} },	{ 555,{6,30} },	{ 562,{5,26} },	{ 570,{4,22} },	{ 574,{6,31} },
	{ 583,{5,27}},	{ 592,{6,32} },	{ 596,{4,23} },	{ 605,{5,28} },	{ 611,{6,33} },	{ 616,{3,19} },	{ 622,{4,24} },	{ 626,{5,29} },
	{ 629,{6,34}},	{ 648,{6,35} },	{ 667,{6,36} },	{ 670,{5,31} },	{ 674,{4,26} },	{ 680,{3,21} },	{ 685,{6,37} },	{ 691,{5,32} },
	{ 700,{4,27}},	{ 704,{6,38} },	{ 713,{5,33} },	{ 722,{6,39} },	{ 726,{4,28} },	{ 734,{5,34} },	{ 741,{6,40} },	{ 745,{3,23} },
	{ 752,{4,29}},	{ 756,{5,35} },	{ 759,{6,41} },	{ 778,{6,42} },	{ 796,{6,43} },	{ 799,{5,37} },	{ 804,{4,31} },	{ 810,{3,25} },
	{ 815,{6,44}},	{ 821,{5,38} },	{ 830,{4,32} },	{ 833,{6,45} },	{ 843,{5,39} },	{ 852,{6,46} },	{ 856,{4,33} },	{ 864,{5,40} },
	{ 870,{6,47}},	{ 875,{3,27} },	{ 881,{4,34} },	{ 886,{5,41} },	{ 889,{6,48} },	{ 907,{6,49} },	{ 926,{6,50} },	{ 929,{5,43} },
	{ 933,{4,36}},	{ 940,{3,29} },	{ 944,{6,51} },	{ 951,{5,44} },	{ 959,{4,37} },	{ 963,{6,52} },	{ 972,{5,45} },	{ 982,{6,53} },
	{ 985,{4,38}},	{ 994,{5,46} },	{ 1000,{6,54} }
};

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

// 4 Ohms Type
static const std::vector<double> F1{   50,  175, 350, 542, 885, 1236,  1380, 2100, 3000, 4000, 10000    };
static const std::vector<double> VOL2{ 0.1, 0.2, 0.3, 1.2, 0.8,  1.5,   2.1,  1.7,  1.5,  1.4,   1.4    };

// 8 Ohms Type
static const std::vector<double> F2{   50,  175, 350, 700, 885, 1120,  1380, 1450, 1600, 2000, 2100, 2300, 2600,  10000    };
static const std::vector<double> VOL1{ 0.1, 0.2, 0.3, 2.3, 0.6,  2.1,   2.2,  1.2, 0.04,  1.0,  1.0,  1.0, 1.0,    0.3    };

// External Speaker
static const std::vector<double> F3{   50,  175, 490,  700, 1000, 1380, 2100, 2400, 3000, 4000, 10000   };
static const std::vector<double> VOL3{ 1.3, 1.2, 0.9, 0.20,  1.2,  2.1,  1.8,  1.3,  1.9,  2.0,   2.0   };

tk::spline *Audio::equalizerSpline = 0;

void Audio::begin( dac_channel_t ch  )
{
	ESP_LOGI(FNAME,"Audio::begin");
	Switch::begin( GPIO_NUM_12 );
	_ch = ch;
	setup();
	//restart();
	_testmode = true;
	if( audio_equalizer.get() == AUDIO_EQ_LS4 )
		equalizerSpline  = new tk::spline(F1,VOL1, tk::spline::cspline_hermite );
	else if( audio_equalizer.get() == AUDIO_EQ_LS8 )
		equalizerSpline  = new tk::spline(F2,VOL2, tk::spline::cspline_hermite );
	else if( audio_equalizer.get() == AUDIO_EQ_LSEXT )
		equalizerSpline  = new tk::spline(F3,VOL3, tk::spline::cspline_hermite );
	delay(10);
}

float Audio::equal_volume( float volume ){
	float freq_resp = ( 1 - (((current_frequency-minf) / (maxf-minf)) * (frequency_response.get()/100.0) ) );
	float new_vol = volume * freq_resp;
	if( equalizerSpline )
		new_vol = new_vol * (float)(*equalizerSpline)( (double)current_frequency );
	if( new_vol >= max_volume.get() )
		new_vol = max_volume.get();
	if( new_vol <= 0 )
		new_vol = 0;
	// ESP_LOGI(FNAME,"Vol: %d Scaled: %f  F: %.0f spline: %.3f", volume, new_vol, current_frequency, (float)(*equalizerSpline)( (double)current_frequency ));
	//ESP_LOGI(FNAME,"equal_volume(): %.2f -> %.2f", volume, new_vol);
	return new_vol;
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
		if( DigitalPoti->haveDevice() ){
			ESP_LOGI(FNAME,"CAT5171 digital Poti found");
			_haveCAT5171 = true;
		}
		else{
			ESP_LOGW(FNAME,"NO digital Poti found !");
			return false;
		}
	}
	else
	{
		ESP_LOGI(FNAME,"MCP4018 digital Poti found");
	}
	float setvolume = default_volume.get();
	speaker_volume = vario_mode_volume = s2f_mode_volume = setvolume;
	ESP_LOGI(FNAME,"default volume: %f", speaker_volume );
	_alarm_mode = true;
	writeVolume( 50.0 );
	float getvolume;
	bool ret = DigitalPoti->readVolume( getvolume );  // gets 49.8049...
	ESP_LOGI(FNAME,"writeVolume(50) then readvolume() got: %f", getvolume );
	if( ret == false ) {
		ESP_LOGI(FNAME,"readWiper returned error");
		return false;
	}
	if( (int)getvolume != 49 )
	{
		ESP_LOGI(FNAME,"readWiper returned wrong setting set=%f get=%f", setvolume, getvolume );
		ret = false;
	}
	else
		ret = true;
	bool fadein=false;
	_alarm_mode = false;
	//	while(1){    // uncomment for continuous self test
	ESP_LOGI(FNAME,"Min F %f, Max F %f", minf, maxf );
	for( float f=minf; f<maxf*1.25; f=f*1.03){
		ESP_LOGV(FNAME,"f=%f",f);
		setFrequency( f );
		current_frequency = f;
		if( !fadein ){
			writeVolume( 0 );
			//enableAmplifier( true );
			dacEnable();
			fade_to( speaker_volume );
			fadein = true;
		}
		writeVolume( setvolume );
		delay(20);
		esp_task_wdt_reset();
	}
	//	}
	delay(200);
	ESP_LOGI(FNAME, "selfTest wiper: %d", 0 );
	writeVolume( 0 );
	dacDisable();
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
	if( scale != dac_scale ) {
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
		dac_scale = scale;
		//ESP_LOGI(FNAME,"DAC scale -> %d", scale);
	}
}

void Audio::alarm( bool enable, float volume, e_audio_alarm_type_t style ){  // non blocking
	if( enable && !_alarm_mode ){ // Begin of alarm
		_vol_back_s2f = s2f_mode_volume;
		_vol_back_vario = vario_mode_volume;
		_s2f_mode_back = _s2f_mode;
		_s2f_mode = false;
		_tonemode_back = _tonemode;
		dacDisable();
		dac_scale_set(_ch, 2 );   // normal loudness (RICO ticks use scale=1)
		_alarm_mode=true;
		enableAmplifier( true );
	}
	if( !enable && _alarm_mode ){ // End of alarm
		_alarm_mode = false;
		vario_mode_volume = _vol_back_vario;
		s2f_mode_volume = _vol_back_s2f;
		_tonemode = _tonemode_back;
		_s2f_mode = _s2f_mode_back;    // S2F mode from before the alarm
		calcS2Fmode(true);             // may update the S2F mode
		writeVolume( speaker_volume );
	}
	if( enable ) {  // tune alarm
		// ESP_LOGI(FNAME,"Alarm sound enable volume: %f style: %d", volume, style );
		if( style == AUDIO_ALARM_STALL ){
			_te = 3.0;
			_tonemode = ATM_DUAL_TONE;
		}
		else if( style == AUDIO_ALARM_FLARM_1 ){  // lowest
			_te = 3.0;
			_tonemode = ATM_SINGLE_TONE;
		}
		else if( style == AUDIO_ALARM_FLARM_2 ){
			_te = 4.0;
			_tonemode = ATM_SINGLE_TONE;
		}
		else if( style == AUDIO_ALARM_FLARM_3 ){ // highest
			_te = 5.0;
			_tonemode = ATM_SINGLE_TONE;
		}
		else if( style == AUDIO_ALARM_GEAR ){
			_te = 2.0;
			_tonemode = ATM_DUAL_TONE;
		}
		else if( style == AUDIO_ALARM_OFF ){
			volume = 0;
		}
		else
			ESP_LOGE(FNAME,"Error, wrong alarm style %d", style );
		calculateFrequency();
		speaker_volume = volume;
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

void Audio::setVolume( float vol ) {
	if( vol > max_volume.get() )
		vol = max_volume.get();
	volume_change = (vol != speaker_volume) ? 100 : 0;
	speaker_volume = vol;
	// also copy the new volume into the cruise-mode specific variables so that
	// calcS2Fmode() will later restore the correct volume when mode changes:
	if( audio_split_vol.get() ) {
		if( _s2f_mode )
			s2f_mode_volume = speaker_volume;
		else
			vario_mode_volume = speaker_volume;
		ESP_LOGI(FNAME, "setvolume() to %f, _s2f_mode %d", speaker_volume, _s2f_mode );
	} else {
		// copy to both variables in case audio_split_vol enabled later
		s2f_mode_volume = speaker_volume;
		vario_mode_volume = speaker_volume;
		ESP_LOGI(FNAME, "setvolume() to %f, mono mode", speaker_volume );
	}
}

void Audio::startAudio(){
	ESP_LOGI(FNAME,"startAudio");
	_testmode = false;
	evaluateChopping();
	speaker_volume = vario_mode_volume;
	xTaskCreatePinnedToCore(&dactask, "dactask", 2400, NULL, 16, &dactid, 0);
}

void Audio::calcS2Fmode( bool recalc ){
	if( _alarm_mode )
		return;
	bool mode = Switch::getCruiseState();
	if( mode != _s2f_mode ){
		ESP_LOGI(FNAME, "S2Fmode changed to %d", mode );
		_s2f_mode = mode;             // do this first, as...
		recalc = true;
	}
	if( recalc ){
		calculateFrequency();         // this needs the new _stf_mode
		if( _s2f_mode )
			speaker_volume = s2f_mode_volume;
		else
			speaker_volume = vario_mode_volume;
		if ( speaker_volume != audio_volume.get() ) {  // due to audio_split_vol
			ESP_LOGI(FNAME, "... changing volume %f -> %f",
			     audio_volume.get(), speaker_volume );
//			audio_volume.set( speaker_volume );  // this too needs _stf_mode
			// this is to keep the current volume shown (or implied) in the menus
			// in step with the actual speaker volume, in case volume is changed
			// after the CruiseState has changed.  Calling audio_volume.set() here
			// will call the action change_volume() which calls Audio::setVolume()
			// which will write it back into speaker_volume and the mode-specific
			// variable, and will also set volume_change=100, but all that is OK.
// But wait: there is a way to avoid that circularity!
// Calling .set() like this does (potentially) sync but skips the action function:
			audio_volume.set( speaker_volume, true, false );
		}
	}
}

void  Audio::evaluateChopping(){
	// re-coded for clarity:
	_chopping = true;   // default
	if (dual_tone.get() == ATM_RICO_TONE)  // overrides chopping_mode
		return;
	int chop_mode = chopping_mode.get();
	if( chop_mode == VARIO_CHOP ) {
		if( _s2f_mode && cruise_audio_mode.get() != AUDIO_VARIO )
			_chopping = false;
	}
	else if( chop_mode == S2F_CHOP ) {
		if( ! _s2f_mode )
			_chopping = false;
	}
	else if( chop_mode == NO_CHOP )
		_chopping = false;
}

void  Audio::calculateFrequency(){
	float max_var;
	if ( _te > 0 )
		max_var = (maxf - center_freq.get()) * 2;
	else
		max_var = center_freq.get() - minf;
	float range = _range;
	if( _s2f_mode && (cruise_audio_mode.get() == AUDIO_S2F) )
		range = 5.0;
	float mult = std::pow( (abs(_te)/range)+1, audio_factor.get());
	if( audio_factor.get() != prev_aud_fact ) {
		inv_exp_max  = 1.0/std::pow( 2, audio_factor.get());
		prev_aud_fact = audio_factor.get();
	}
	current_frequency = center_freq.get() + ((mult*_te)/range )  * (max_var*inv_exp_max);
	if( hightone && (_tonemode == ATM_DUAL_TONE ) )
		setFrequency( current_frequency*_high_tone_var );
	else
		setFrequency( current_frequency );
	// ESP_LOGI(FNAME, "New Freq: (%0.1f) TE:%0.2f exp_fac:%0.1f", current_frequency, _te, mult );
}

void Audio::writeVolume( float volume ){
	//ESP_LOGI(FNAME, "writeVolume() %.2f", volume);
	if( _alarm_mode )
		DigitalPoti->writeVolume( volume );  // max volume
	else
		DigitalPoti->writeVolume( equal_volume(volume) ); // take care frequency response
	current_volume = volume;
}

void Audio::dactask(void* arg )
{
	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();
		tick++;
		Switch::tick();    // we hook switch sceduling here to save extra task
		// Chopping or dual tone modulation
		chop_style = chopping_style.get();
		sink_style = audio_mute_sink.get();
		long_silence = false;
		scheduled = false;
		if( millis() > next_scedule ){
			scheduled = true;
			if ( _te > 0 ){
				mtick++;
				if ( (mtick & 1) )     // always true right after mtick==0
					hightone = true;
				else
					hightone = false;
				float f = _range;
				if( _s2f_mode && (cruise_audio_mode.get() == AUDIO_S2F) )
					f = 5.0;
				if ( _tonemode == ATM_RICO_TONE ) {
					f = f / (4.0*f/(3.0+audio_factor.get()) + 6.0*_te);
					  // lower exponent results in more frequent ticks in weak lift
				} else {
					if (dac_scale != 2)
						dac_scale_set(_ch, 2 );  // do it here in case of two-tone mode
					// Originally: f = 1+9*(_te/_range);
					// Mathematically equivalent: (since used below to multiply not divide)
					f = f / (f + 9.0*_te);
				}
				float period_ms = 1000.0 * f;
				if ( hightone ){  // duration of break (or second tone)
					if ( _tonemode == ATM_RICO_TONE )
						_delay = int(period_ms * 0.7);   // roughly 1000 - 100 mS
					else
						_delay = int(period_ms * 0.1)+40;   // 1Hz: 140 mS; 10Hz: 50 mS
					//ESP_LOGI(FNAME, "dactask: te: %4.2f break: %d  period: %d", _te, _delay, (int)period_ms);
				}
				else{
					if ( _tonemode == ATM_RICO_TONE ) {
						if ( chop_style == AUDIO_CHOP_HARD ) {
							_delay = 9;
							dac_scale_set(_ch, 1 );   // 6 dB louder than normal
						} else  {
							_delay = 24;
							dac_scale_set(_ch, 2 );
						}
					} else {
						_delay = int(period_ms * 0.9)-40;
						  // duration of main tone 1Hz: 860 mS; 10Hz: 50 mS
					}
					//ESP_LOGI(FNAME, "dactask: te: %4.2f tone: %d  period: %d", _te, _delay, (int)period_ms);
				}
				//if (mtick < 5)
				//	ESP_LOGI(FNAME, "scheduled: _delay=%d hightone=%d mtick=%d", _delay, hightone, mtick);

			} else {  // _te < 0
				hightone = false;    // possible continous tone
				mtick = 0;           // first chopping will be to hightone (silence)
				_delay = 100;
			}

			// Frequency Control
			if( !audio_variable_frequency.get() )
				calculateFrequency();
			next_scedule = millis()+_delay;
		}
//		if( audio_variable_frequency.get() )
//			calculateFrequency();

		// Amplifier and Volume control
		//   If RICO, audio ticks are too short to wait until a later task tick,
		//   so always process after delays scheduled above,
		//   otherwise process every fourth tick:
		if( !_testmode && ( !(tick&0x7) || scheduled ) ) {
			// ESP_LOGI(FNAME, "sound dactask tick:%d wiper:%d  te:%f db:%d", tick, (*p_wiper), _te, inDeadBand(_te) );
			int ticks = tick - old_tick;
			old_tick = tick;

			// moved here to call calculateFrequency() less often (every 40 ms is enough):
			if( audio_variable_frequency.get() )
				calculateFrequency();

			if( !(tick&0x3F) ){     // every 320 ms
				calcS2Fmode(false);     // if mode changed, affects volume and frequency
				yield();
			}

			int shutdownamp = amplifier_shutdown.get();

			if( inDeadBand(_te) ) {
				deadband_active = true;
				sound = false;
				mtick = 0;  // (even if _te>0) - first chopping will be to hightone (silence)
				if (shutdownamp == AMP_SHUTDOWN)  // immediately
					disable_amp = true;
			}
			else{
				deadband_active = false;
				sound = true;
				disable_amp = false;
				if( _tonemode == ATM_RICO_TONE || (_chopping && _tonemode == ATM_SINGLE_TONE)){
					if( hightone )
						sound = false;
				}
			}
			if( sound ) {
				if( !_alarm_mode ) {
					// Optionally disable vario audio when in sink
					if( _te < 0 ) {
						if( sink_style == SINK_OFF ) {
							sound = false;
							if (shutdownamp == AMP_SHUTDOWN)  // immediately
								disable_amp = true;
						}
						else if (sink_style == SINK_LOUD)
							dac_scale_set(_ch, 1 );
						else if (sink_style == SINK_SOFT)
							dac_scale_set(_ch, 3 );
						else if (dac_scale != 2)
							dac_scale_set(_ch, 2 );
					}
					// Optionally disable vario audio when in setup menu
					else if( audio_mute_menu.get() && gflags.inSetup ) {
						sound = false;
						disable_amp = true;   // immediately
					}
					// Optionally disable vario audio generally
					else if( audio_mute_gen.get() != AUDIO_ON ) {
						sound = false;
						disable_amp = true;
					}
				} else if( audio_mute_gen.get() == AUDIO_OFF ) {
					// Optionally mute alarms too
					sound = false;
					disable_amp = true;
				}
			}
			//ESP_LOGI(FNAME, "sound %d, ht %d, te %2.1f vc:%d cw:%d ", sound, hightone, _te, volume_change, cur_wiper );

			if( sound ){
				if (scheduled && mtick > 0 && mtick < 3)
					ESP_LOGI(FNAME, "sound, mtick %d, cur_vol %.2f, te %2.1f",
					    mtick, current_volume, _te );
				long_silence = false;
				silent_ticks = 0;
				disable_amp = false;

				// In lift, amplifier usually enabled during silence before tick.
				// But below the deadband need to enable here for continuous tone.
				// Also in 2-tone mode need to enable the amplifier.  If mtick
				// is odd then amplifier will be enabled later on the next mtick.
				// If enabling amplifier here then do need the 180 ms delay.

				// on mtick==0 or any even-numbered mtick (non-hightone):
				if ( (mtick & 1) == 0 )
					enableAmplifier( true );

				if( volume_change || chop_style == AUDIO_CHOP_SOFT ){
					// Fade in volume (or smooth over volume changes)
					dacEnable();
					if( current_volume != speaker_volume ){
						fade_to( speaker_volume );
						volume_change = 0;
						yield();
					}
				}
				else {  // HARD chopping modes
					if( current_volume != speaker_volume ) {
						//ESP_LOGI(FNAME, "hard, volume: %f", speaker_volume );
						writeVolume( speaker_volume );
					}
					dacEnable();
				}

				if( !(tick&0x1F) ){
					if( current_volume != speaker_volume ){
						ESP_LOGI(FNAME, "periodic during sound, readVolume %.2f -> %.2f",
						       current_volume, speaker_volume );
					}
					writeVolume( speaker_volume );  // <<< why necessary?
				}
			}

			else{  // if not sound
				//if ( scheduled && mtick > 0 && mtick < 3 )
				//	ESP_LOGI(FNAME, "no sound, mtick %d, te %2.1f", mtick, _te );
				silent_ticks += ticks;
				if (silent_ticks > 500)      // silence has lasted 5 sec
					long_silence = true;
				if (long_silence)
					disable_amp = true;

				if( mtick <= 1 || chop_style == AUDIO_CHOP_SOFT ){
					// fade out if entering deadband, before first beep, or soft mode
					if( current_volume != 0 ){
						fade_to( 0 );
					}
				}
				// if hard chopping leave the volume as-is, dacDisable() is enough
				dacDisable();

				// Avoid first RICO tick after long silence becoming a beep during the
				// delay(180), by turning the amplifier on in the preceding silent period.
				// In non-RICO too try and turn on amplifier ahead of time (delay as needed).
				// If we are here (no sound) that means mode is not two-tone.
				if ( mtick == 1 ) {
					//ESP_LOGI(FNAME, "calling enableAmplifier, _delay=%d", _delay );
					enableAmplifier( true, _delay );
				} else if( disable_amp ) {
					enableAmplifier( false );
				}
			}
		}
		// ESP_LOGI(FNAME, "Audio delay %d", _delay );
		if( uxTaskGetStackHighWaterMark( dactid ) < 256 )
			ESP_LOGW(FNAME,"Warning Audio dac task stack low: %d bytes", uxTaskGetStackHighWaterMark( dactid ) );
		vTaskDelayUntil(&xLastWakeTime, 5/portTICK_PERIOD_MS);
//		if( volume_change )
//			volume_change--;
	}
}



bool Audio::inDeadBand( float te )
{
	if( _alarm_mode )
		return false;
	float dbp=0.0;
	float dbn=0.0;
	if( _s2f_mode && (cruise_audio_mode.get() == AUDIO_S2F)) {
		dbp = s2f_deadband.get()/10;
		dbn = s2f_deadband_neg.get()/10;
	}else{
		dbp = deadband.get();
		dbn = deadband_neg.get();
	}
//	if( ((int)( dbp*100 ) == 0) && ((int)( dbn*100 ) == 0)  ){
//		return false;
//	}
// - not needed, since the same happens within the checks below:
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
	minf = center_freq.get() / tone_var.get();
	restart();
	dacDisable();
}

void Audio::restart()
{
	//ESP_LOGD(FNAME,"Audio::restart");
	dacDisable();
	dac_cosine_enable(_ch);
	dac_offset_set(_ch, 0 );
	dac_invert_set(_ch, 2 );    // invert MSB to get sine waveform
	dac_scale_set(_ch, 2 );
	enableAmplifier( true );
	dacEnable();
}

void Audio::shutdown(){
	dacDisable();
	// shut down amplifier chip
	gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );   // use pullup 1 == SOUND 0 == SILENCE
	gpio_set_level(GPIO_NUM_19, 0 );
	speaker_volume = 0;
	audio_volume.set( 0 );
	ESP_LOGI(FNAME,"shutdown alarm volume set 0");
}

void Audio::boot(){
	restart();
}

void Audio::enableAmplifier( bool enable, int silence_ms )
{
	// ESP_LOGI(FNAME,"Audio::enableAmplifier( %d )", (int)enable );
	// enable Audio
	if( enable )
	{
		if( !amplifier_enabled ){
			gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );
			gpio_set_level(GPIO_NUM_19, 1 );    // use pullup 1 == SOUND 0 == SILENCE
			// The silence before first beep or tick (mtick==1) cancels the
			// need for delay, amplifier will be ready for the first tick.
			// But if in sink falling below the deadband, need the delay.
			if ( silence_ms < 180 ) {
				ESP_LOGI(FNAME,"enabled Amplifier, 180 ms delay");
				delay(180);  // amplifier startup time ~175mS according to datasheet Fig. 21
			} else {
				ESP_LOGI(FNAME,"enabled Amplifier, skipping 180 ms delay");
			}
			// do NOT call dacEnable() here - allow doing this in silence.
			amplifier_enabled = true;
			//ESP_LOGI(FNAME,"enabled Amplifier");
		}
	}
	else {
		if( amplifier_enabled ){
			if( amplifier_shutdown.get() != AMP_STAY_ON ){
				gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );   // use pullup 1 == SOUND 0 == SILENCE
				gpio_set_level(GPIO_NUM_19, 0 );
				dacDisable();
				amplifier_enabled = false;
				ESP_LOGI(FNAME,"disabled Amplifier");
			}
		}
	}
}

void Audio::dacEnable(){
	if( !dac_enabled ){
		// ESP_LOGI(FNAME,"Audio::dacEnable");
		dac_output_enable(_ch);
		dac_enabled = true;
	}
}

void Audio::dacDisable(){
	if( dac_enabled ){
		// ESP_LOGI(FNAME,"Audio::dacDisable");
		dac_output_disable(_ch);
		dac_enabled = false;
	}
}

// Fade in or out, full or partial, smaller relative changes on the loud end.
// This version uses up to 8 steps, but often skips some steps, and, 
// most of the change happens in 4 steps.
void Audio::fade_to( float target_volume ){
	static const float fade_factor[7] = {0.10, 0.20, 0.38, 0.56, 0.71, 0.83, 0.92};
	float start_volume = current_volume;
	if (target_volume < start_volume) {  // fade-out
		for( int i=6; i>=0; i-- ) {
			float volume = start_volume * fade_factor[i];
			if (volume <  target_volume)  break;
			if (volume <  3)  break;
			writeVolume( volume );
			delay(1);
		}
	} else {  // fade-in
		float threshold = start_volume / target_volume;
		for( int i=0; i<7; i++ ) {
			if (fade_factor[i] < threshold)  continue;
			float volume = target_volume * fade_factor[i];
			writeVolume( volume );
			delay(1);
		}
	}
	writeVolume( target_volume );
	if( target_volume == 0 )
		dacDisable();
}

bool Audio::lookup( float f, int& div, int &step ){
	int fi = (int)(f + 0.5);
	std::map<uint16_t,t_lookup_entry>::iterator low;
	low = lftab.lower_bound(fi);
	if (low != lftab.end()) {
		div =  low->second.div;
		step = low->second.step;
		// ESP_LOGI(FNAME, "found div:%d step:%d for F:%d", div, step, fi );
		return true;
	}
	ESP_LOGW(FNAME,"F: %d not found in map", fi );
	return false;
}

