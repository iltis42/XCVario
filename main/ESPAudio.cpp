/* DAC Cosine Generator Example
   This example code is in the Public Domain (or CC0 licensed, at your option.)
   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
 */

#include "ESPAudio.h"

#include "driver/dac_continuous.h"
#include "mcp4018.h"
#include "cat5171.h"
#include "S2fSwitch.h"
#include "I2Cbus.hpp"
#include "spline.h"
#include "setup/CruiseMode.h"
#include "protocol/Clock.h"
#include "sensor.h"
#include "logdef.h"

#include <esp_system.h>
#include <esp_task_wdt.h>

#include <soc/rtc.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>


Audio *AUDIO = nullptr;

constexpr const int SAMPLE_RATE = 44100; // Hz
constexpr const int BUF_LEN     = 1300;  // DMA buffer per push
constexpr const int TABLE_SIZE  = 256;
constexpr const int RAMP_SIZE   = 64;
constexpr const int TABLE_BITS  = 8; // log2((double)TABLE_SIZE);
constexpr const int DAC_CENTER  = 127;
constexpr const int DAC_HLF_AMPL = 32;

struct dma_cmd {
    uint32_t phase;
    uint32_t step;
	int16_t gain;
    uint8_t cmd;
};


// static std::array<int8_t, TABLE_SIZE> sine_table;
const std::array<int8_t, TABLE_SIZE> sine_table = {
    // triangular
    // 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 15, 16, 16, 
    // 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 30, 30, 31, 
    // 31, 32, 31, 31, 30, 30, 29, 29, 28, 28, 27, 27, 26, 26, 25, 25, 24, 24, 23, 23, 22, 22, 21, 21, 20, 20, 19, 19, 18, 
    // 18, 17, 17, 16, 16, 15, 15, 14, 14, 13, 13, 12, 12, 11, 11, 10, 10, 9, 9, 8, 8, 7, 7, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 
    // 1, 1, 0, 0, 0, -1, -1, -2, -2, -3, -3, -4, -4, -5, -5, -6, -6, -7, -7, -8, -8, -9, -9, -10, -10, -11, -11, -12, -12, 
    // -13, -13, -14, -14, -15, -15, -16, -16, -17, -17, -18, -18, -19, -19, -20, -20, -21, -21, -22, -22, -23, -23, -24, 
    // -24, -25, -25, -26, -26, -27, -27, -28, -28, -29, -29, -30, -30, -31, -31, -32, -31, -31, -30, -30, -29, -29, -28, 
    // -28, -27, -27, -26, -26, -25, -25, -24, -24, -23, -23, -22, -22, -21, -21, -20, -20, -19, -19, -18, -18, -17, -17, 
    // -16, -16, -15, -15, -14, -14, -13, -13, -12, -12, -11, -11, -10, -10, -9, -9, -8, -8, -7, -7, -6, -6, -5, -5, -4, 
    // -4, -3, -3, -2, -2, -1, -1, 0};
    0, 1, 2, 2, 3, 4, 5, 5, 6, 7, 8, 8, 9, 10, 10, 11, 12, 13, 13, 14, 15, 15, 16, 17, 17, 18, 18, 19, 20, 20, 21, 21, 22, 
    22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 27, 28, 28, 28, 29, 29, 29, 29, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 
    31, 31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 30, 30, 30, 29, 29, 29, 29, 28, 28, 28, 27, 27, 27, 26, 26, 25, 25, 24, 24, 
    23, 23, 22, 22, 21, 21, 20, 20, 19, 18, 18, 17, 17, 16, 15, 15, 14, 13, 13, 12, 11, 10, 10, 9, 8, 8, 7, 6, 5, 5, 4, 3, 
    2, 2, 1, 0, -1, -2, -2, -3, -4, -5, -5, -6, -7, -8, -8, -9, -10, -10, -11, -12, -13, -13, -14, -15, -15, -16, -17, -17, 
    -18, -18, -19, -20, -20, -21, -21, -22, -22, -23, -23, -24, -24, -25, -25, -26, -26, -27, -27, -27, -28, -28, -28, -29, 
    -29, -29, -29, -30, -30, -30, -30, -30, -31, -31, -31, -31, -31, -31, -31, -31, -31, -31, -31, -31, -31, -31, -31, -30, 
    -30, -30, -30, -30, -29, -29, -29, -29, -28, -28, -28, -27, -27, -27, -26, -26, -25, -25, -24, -24, -23, -23, -22, -22, 
    -21, -21, -20, -20, -19, -18, -18, -17, -17, -16, -15, -15, -14, -13, -13, -12, -11, -10, -10, -9, -8, -8, -7, -6, -5, 
    -5, -4, -3, -2, -2, -1};
static volatile dma_cmd* voice_list[4] = {nullptr, nullptr, nullptr, nullptr};
static __attribute__((aligned(4))) volatile dma_cmd default_voice;
// static int8_t hann_ramp[RAMP_SIZE];
const std::array<int8_t, RAMP_SIZE> hann_ramp = {
    32, 31, 31, 31, 31, 31, 31, 31, 30, 30, 30, 29, 29, 28, 28, 27, 27, 26, 25, 25, 24, 24, 23, 22, 21, 21, 20, 19, 18, 17, 
    17, 16, 15, 14, 14, 13, 12, 11, 10, 10, 9, 8, 7, 7, 6, 6, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0};

// static void init_sine_table() {
//     for (int i = 0; i < TABLE_SIZE; i++) {
//         sine_table[i] = (int8_t)((DAC_HLF_AMPL-.25) * sin(2. * M_PI * i / TABLE_SIZE));
//         printf("%4d, ", sine_table[i]);
//     }
// }
// static void init_triangle_table() {
//     for (int i = 0; i < TABLE_SIZE; i++) {
//         // normalized position in [0,1)
//         float t = (float)i / TABLE_SIZE;
//         // triangle wave in range [-1, 1]
//         float tri = 4.0f * fabsf(t + 0.25 - roundf(t + 0.25f)) - 1.0f;
//         // scale to DAC amplitude
//         sine_table[i] = (int8_t)(DAC_HLF_AMPL * tri);
//         printf("%d, ", sine_table[i]);
//     }
// }

// static void init_hann_ramp() {
//     for (int n = 0; n < RAMP_SIZE; n++) {
//         // hann_ramp[n] = 1. - sin((M_PI * n) / (2.0f * RAMP_SIZE));
//         hann_ramp[n] = (uint8_t)(0.5 * (1.0 + cos(M_PI * n / (RAMP_SIZE-1))) * DAC_HLF_AMPL);
//         printf("%d, ", hann_ramp[n]);
//     }
// }

// Benchmark
int64_t total_us = 0, busy_irs = 0;
int irq_counter = 0;


// DMA callback
static bool IRAM_ATTR dacdma_done(dac_continuous_handle_t h, const dac_event_data_t *e, void *user_data)
{
    dma_cmd **list = (dma_cmd **)user_data;
    uint8_t *buf = (uint8_t *)e->buf;

    int64_t t_busy0 = esp_timer_get_time();
	irq_counter++;

    int i = 0;
    if (list[0] != nullptr)
    {
        uint32_t phase = list[0]->phase;
        uint32_t step = list[0]->step;
        int gain = list[0]->gain;
        if (list[0]->cmd == 0)
        {
            memset(buf, DAC_CENTER, e->buf_size);
        }
        else if (list[0]->cmd == 1)
        {
            // phase in
            i = e->buf_size - 2*RAMP_SIZE;
            memset(buf, DAC_CENTER, i);
            int revidx = RAMP_SIZE - 1;
            for (; i < e->buf_size; i+=2)
            {
                int idx = phase >> (32 - TABLE_BITS);
                buf[i] = (((int)hann_ramp[revidx--] * sine_table[idx] * gain) >> 12) + DAC_CENTER;
                buf[i+1] = buf[i];
                phase += step;
            }
            list[0]->phase = phase;
            list[0]->cmd = 2;
        }
        else if (list[0]->cmd == 2)
        {
            for (; i < e->buf_size; i+=2)
            {
                buf[i] = (((int)sine_table[phase >> (32 - TABLE_BITS)]) * gain >> 7) + DAC_CENTER;
                buf[i+1] = buf[i];
                phase += step;
            }
            list[0]->phase = phase;
        }
        else if (list[0]->cmd == 3)
        {
            // phase out
            for (int hannidx=0; hannidx<RAMP_SIZE; hannidx++, i+=2)
            {
                int idx = phase >> (32 - TABLE_BITS);
                buf[i] = (((int)hann_ramp[hannidx] * sine_table[idx] * gain) >> 12) + DAC_CENTER;
                buf[i+1] = buf[i];
                phase += step;
            }
            memset(buf + i, DAC_CENTER, e->buf_size - i);
            list[0]->phase = 0;
            list[0]->cmd = 0;
        }
    }

    // if ( list[1] > 0 ) {
    // for (int i = 0; i < e->buf_size; i++) {
    //     int32_t mix =0;
    //     for ( int voice=0; voice<all; voice++ ) {
    //         uint32_t *phase = &list[voice*2];
    //         mix += sine_table[*phase >> (32 - TABLE_BITS)];
    //         *phase += list[voice*2+1];
    //     }
    //     mix /= all;
    //     buf[i] = (uint8_t)(mix + 128);
    // }
    // for (int i = 0; i < e->buf_size; i++) {
    //     uint16_t idx = phase >> (32 - TABLE_BITS);
    //     phase += step;
    //     buf[i] = sine_table[idx];
    // }
    //     int32_t mix;
    //     for (int i = 0; i < e->buf_size; i++) {
    //         uint16_t idx = phase >> (32 - TABLE_BITS);
    //         mix = sine_table[idx];
    //         buf[i] = mix + DAC_CENTER;
    //         phase += step;
    //     }
    // }
    // if ( i < e->buf_size ) {
    //     memset(buf+i, DAC_CENTER, e->buf_size-i);
    // }

    int64_t t_busy1 = esp_timer_get_time();
    busy_irs += (t_busy1 - t_busy0);

    return false;
}

Audio::Audio() :
	Clock_I(8) // every 80msec
{
	// init_sine_table();
    // init_triangle_table();
    // init_hann_ramp();
    voice_list[0] = &default_voice;
}

Audio::~Audio()
{
    if ( _dac_chan ) {
        mute();
        ESP_ERROR_CHECK(dac_continuous_stop_async_writing(_dac_chan));
        ESP_ERROR_CHECK(dac_continuous_disable(_dac_chan));
        ESP_ERROR_CHECK(dac_continuous_del_channels(_dac_chan));
    }

	Clock::stop(this);
	if ( _poti ) {
		delete _poti;
		_poti = nullptr;
	}
	if ( dactid ) {
		vTaskDelete(dactid);
		dactid = nullptr;
	}
}

bool Audio::begin( int16_t ch  )
{
	ESP_LOGI(FNAME,"begin");
	if ( ! S2FSWITCH ) {
		S2FSWITCH = new S2fSwitch( GPIO_NUM_12 );
	}
	setup();
	ESP_LOGI(FNAME, "Starting Audio on core %d", xPortGetCoreID());

    _dac_cfg = {
        .chan_mask = (dac_channel_mask_t)BIT(ch), // which channel to enable
        .desc_num = 3,
        .buf_size = BUF_LEN,
        .freq_hz = SAMPLE_RATE,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .chan_mode = DAC_CHANNEL_MODE_SIMUL,
    };

    esp_err_t err = dac_continuous_new_channels(&_dac_cfg, &_dac_chan);
    // register callback
    dac_event_callbacks_t callbacks = {
        .on_convert_done = dacdma_done,
        .on_stop = nullptr
    };
    err |= dac_continuous_register_event_callback(_dac_chan, &callbacks, voice_list);

    err |= dac_continuous_enable(_dac_chan);
    ESP_LOGI(FNAME, "DAC initialized success, DAC DMA is ready");

    ESP_LOGI(FNAME,"Find digital poti");
	_poti = new MCP4018(&i2c1);
	_poti->begin();
    if (_poti->haveDevice()) {
        ESP_LOGI(FNAME, "MCP4018 digital Poti found");
    } else {
        ESP_LOGI(FNAME, "Try CAT5171 digital Poti");
        delete _poti;
        _poti = new CAT5171(&i2c1);
        _poti->begin();
        if (_poti->haveDevice()) {
            ESP_LOGI(FNAME, "CAT5171 digital Poti found");
            _haveCAT5171 = true;
        } else {
            ESP_LOGW(FNAME, "NO digital Poti found !");
            delete _poti;
            _poti = nullptr;
        }
    }
    if ( _poti ) {
        _alarm_mode = true;
        if( _poti->writeWiper( 5, true ) ) {
            ESP_LOGI(FNAME,"writeWiper(5) returned Ok");
        } else {
            ESP_LOGI(FNAME,"readWiper returned error");
        }
        _alarm_mode = false;
    }
    err |= _poti == nullptr;

    if ( err == ESP_OK ) {
        enableAmplifier(true);
        ESP_ERROR_CHECK(dac_continuous_start_async_writing(_dac_chan));
        Clock::start(this);
		_dac_inited = true;
        return true;
    }
    return false;
}

void Audio::stop() {
    mute();
    vTaskDelay(pdMS_TO_TICKS(10)); // no cracks ..
    enableAmplifier(false);
    ESP_ERROR_CHECK(dac_continuous_stop_async_writing(_dac_chan));
}

// do the sound check, when the audio task is not yet running
class TestSequence : public Clock_I
{
public:
	TestSequence(float freq, float maxfreq) : Clock_I(3), f(freq), maxf(maxfreq) {
		AUDIO->setFrequency(f);
		AUDIO->writeVolume(20.);
	    AUDIO->unmute();
		Clock::start(this);
	}
	~TestSequence() {
		AUDIO->_test_done = true;
	}
	bool tick() override {
		// Example: log a message and return false
		ESP_LOGI(FNAME, "f=%f", f);
		if ( f<maxf ) {
			AUDIO->setFrequency(f);
			f *= 1.03;
			return false;
		}
		else {
		    AUDIO->mute();
			delete this;
			return true;
		}
	}
private:
	float f;
	float maxf;
};
void Audio::soundCheck() {
    if ( ! _dac_inited ) {
        return;
    }
    ESP_LOGI(FNAME,"Audio::selfTest");
	new TestSequence(minf, maxf*1.25);
}


void Audio::setFrequency( float f ){
    default_voice.step = (uint32_t)((f * (1ULL << 32)) / SAMPLE_RATE);
	default_voice.gain = (f<=center_freq.get()) ? 128 : 128 - 38 *(f-center_freq.get()) / 1500.0; // -3dB at 1500Hz above center

    // ESP_LOGI(FNAME,"freq set %f step %u phase %u", f, (unsigned)default_voice.step, (unsigned)default_voice.phase);
}

void Audio::alarm( bool enable, float volume, e_audio_alarm_type_t style ){  // non blocking
	if( enable && !_alarm_mode ){ // Begin of alarm
		_vol_back_s2f = s2f_mode_volume;
		_vol_back_vario = vario_mode_volume;
		_s2f_mode_back = _s2f_mode;
		_s2f_mode = false;
		_tonemode_back = _tonemode;
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
		else {
			ESP_LOGE(FNAME,"Error, wrong alarm style %d", style );
		}
		calculateFrequency();
		speaker_volume = volume;
	}
}

// [%]
void Audio::setVolume( float vol ) {
	speaker_volume = vol;
	// also copy the new volume into the cruise-mode specific variables so that
	// calcS2Fmode() will later restore the correct volume when mode changes:
	if( audio_split_vol.get() ) {
		if( _s2f_mode )
			s2f_mode_volume = speaker_volume;
		else
			vario_mode_volume = speaker_volume;
		ESP_LOGI(FNAME, "setvolume() to %f, for %s", speaker_volume, _s2f_mode ? "s2f" : "vario" );
	} else {
		// copy to both variables in case audio_split_vol enabled later
		s2f_mode_volume = speaker_volume;
		vario_mode_volume = speaker_volume;
		ESP_LOGI(FNAME, "setvolume() to %f, joint mode", speaker_volume );
	}
}

void Audio::startAudio(){
    if ( ! _dac_inited ) {
        return;
    }
	ESP_LOGI(FNAME,"startAudio");
	evaluateChopping();
	while ( ! _test_done ) ; // wait until sound check finished
	float setvolume = default_volume.get();
	speaker_volume = vario_mode_volume = s2f_mode_volume = setvolume;
	writeVolume( setvolume );
	xTaskCreate(&dactask_starter, "dactask", 2400, this, 24, &dactid);
}

void Audio::calcS2Fmode( bool recalc ){
	if( _alarm_mode )
		return;

	bool mode = VCMode.getCMode();
	if( mode != _s2f_mode ){
		ESP_LOGI(FNAME, "S2Fmode changed to %d", mode );
		_s2f_mode = mode;             // do this first, as...
		recalc = true;
		evaluateChopping();
	}
	if( recalc ){
		calculateFrequency();         // this needs the new _stf_mode
		if( _s2f_mode )
			speaker_volume = s2f_mode_volume;
		else
			speaker_volume = vario_mode_volume;
		if ( speaker_volume != audio_volume.get() ) {  // due to audio_split_vol
			ESP_LOGI(FNAME, "... changing volume %f -> %f", audio_volume.get(), speaker_volume );

			audio_volume.set( speaker_volume );  // this too needs _stf_mode
			// this is to keep the current volume shown (or implied) in the menus
			// in step with the actual speaker volume, in case volume is changed
			// after the CruiseState has changed.  Calling audio_volume.set() here
			// will call the action change_volume() which calls Audio::setVolume()
			// which will write it back into speaker_volume and the mode-specific
			// variable, but all that is OK.
		}
	}
}

void  Audio::evaluateChopping(){
	if(
			(chopping_mode.get() == BOTH_CHOP) ||
			(_s2f_mode && (chopping_mode.get() == S2F_CHOP) ) ||
			( _s2f_mode && (cruise_audio_mode.get() == AUDIO_VARIO) && (chopping_mode.get() != NO_CHOP) ) ||
			(!_s2f_mode && (chopping_mode.get() == VARIO_CHOP) )
	) {
		_chopping = true;
	} else {
		_chopping = false;
	}
}

void  Audio::calculateFrequency(){
	float max_var = center_freq.get()-minf;
	if ( _te > 0 )
		max_var = (maxf-center_freq.get()) * 2;
	float range = _range;
	if( _s2f_mode && (cruise_audio_mode.get() == AUDIO_S2F) )
		range = 5.0;
	float mult = std::pow( (abs(_te)/range)+1, audio_factor.get());
	if( audio_factor.get() != prev_aud_fact ) {
		exponent_max  = std::pow( 2, audio_factor.get());
		prev_aud_fact = audio_factor.get();
	}
	current_frequency = center_freq.get() + ((mult*_te)/range )  * (max_var/exponent_max);

    if (hightone && (_tonemode == ATM_DUAL_TONE))
    {
        default_voice.setFrequency(current_frequency * _high_tone_var);
    }
    else {
        default_voice.setFrequency(current_frequency);
    }
	// ESP_LOGI(FNAME, "New Freq: (%0.1f) TE:%0.2f exp_fac:%0.1f", current_frequency, _te, mult );
}

void Audio::writeVolume( float volume ){
	ESP_LOGI(FNAME, "set volume: %f", volume);
    if ( _poti ) {
        // if( _alarm_mode )
        //  ...
        _poti->writeVolume( volume );
        current_volume = volume;
    }
}

void Audio::dactask_starter(void *arg)
{
	static_cast<Audio*>(arg)->dactask();
}

void Audio::dactask()
{
	int tick = 0;
    bool sound = false;
    int delay = 100;
	int64_t t0 = esp_timer_get_time();
	esp_task_wdt_add(NULL);

	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();
		tick++;

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
					delay = int(period_ms * 0.1)+40;  // 1Hz: 100 mS; 10Hz: 50 mS
					// ESP_LOGI(FNAME, "Break: %d period:%4.2f %4.1f", _delay, period_ms, f );
				}
				else {  // duration of main tone 1Hz: 900 mS; 10Hz: 50 mS
					delay = int(period_ms * 0.9)-40;
					// ESP_LOGI(FNAME, "Tone: %d period:%4.2f %4.1f", _delay, period_ms, f );
				}
			}
			else{
				delay = 100;
				hightone = false;
			}
			// Frequency Control
			if( !audio_variable_frequency.get() ) {
				calculateFrequency();
			}
			next_scedule = millis()+delay;
		}

        // Amplifier and Volume control
		if( !(tick%4) ) {
			// ESP_LOGI(FNAME, "sound dactask tick:%d volume:%f  te:%f db:%d", tick, speaker_volume, _te, inDeadBand(_te) );

			// continuous variable tone
			if( audio_variable_frequency.get() ) {
				calculateFrequency();
			}

			if( !(tick%10) ){
				calcS2Fmode(false);     // if mode changed, affects volume and frequency
			}

			if( inDeadBand(_te) ){
				deadband_active = true;
				sound = false;
				// ESP_LOGI(FNAME,"Audio in DeadBand true");
			}
			else{
				deadband_active = false;
				sound = true;
				if( _tonemode == ATM_SINGLE_TONE ){
					if( hightone )
						if( _chopping ) {
							sound = false;
						}
				}
				// ESP_LOGI(FNAME,"Audio in DeadBand false");
			}
			if( sound ) {
				if( !_alarm_mode ) {
					// Optionally disable vario audio when in Sink
					if( audio_mute_sink.get() && _te < 0 ) {
						sound = false;
					// Optionally disable vario audio generally
					} else if( audio_mute_gen.get() != AUDIO_ON ) {
						sound = false;
					}
				} else if( audio_mute_gen.get() == AUDIO_OFF ) {
					// Optionally mute alarms too
					sound = false;
				}
			}
			//ESP_LOGI(FNAME, "sound %d, ht %d, te %2.1f cw:%f ", sound, hightone, _te, current_volume );
			if ( sound && current_volume > 0) {
                unmute();
            }
            else {
                mute();
            }

		}
		if ( current_volume != speaker_volume ) {
            current_volume = speaker_volume;
            writeVolume( current_volume );
		}
		// ESP_LOGI(FNAME, "Audio delay %d", _delay );
		if( uxTaskGetStackHighWaterMark( dactid ) < 256 ) {
			ESP_LOGW(FNAME,"Warning Audio dac task stack low: %d bytes", uxTaskGetStackHighWaterMark( dactid ) );
		}
		if ( !(tick%1000) ) {
			int64_t t1 = esp_timer_get_time();
            total_us += (t1 - t0);
            float load = (float)(busy_irs) * 1000.0 / total_us;
            ESP_LOGI(FNAME, "DAC load: %.1fmsec/sec (x%d; Fs=%dHz)", load, irq_counter/10, SAMPLE_RATE);
			irq_counter = 0;
			busy_irs = 0;
			total_us = 0;
            t0 = t1;
		}
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
		esp_task_wdt_reset();
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
	if( audio_range.get() == AUDIO_RANGE_5_MS )
		_range = 5.0;
	else if( audio_range.get() == AUDIO_RANGE_10_MS )
		_range = 10.0;
	else
		_range = scale_range.get();
	_tonemode = dual_tone.get();
	_high_tone_var = powf(2.0, 4./12.0); // major third

	maxf = center_freq.get() * tone_var.get();
	minf = center_freq.get() / tone_var.get();
    ESP_LOGI(FNAME,"min/max freq. %.1f/%.1f", minf, maxf);
	current_frequency = (minf+maxf)/2.;
}

void Audio::unmute()
{
    if ( !_mute ) return;

    default_voice.phase = 0;
    default_voice.cmd = 1; // phase in

    // ESP_LOGI(FNAME,"unmute");
    // ESP_ERROR_CHECK(dac_continuous_start_async_writing(_dac_chan));
    _mute = false;
}

void Audio::mute(){
    if ( _mute ) return;

    // ESP_ERROR_CHECK(dac_continuous_stop_async_writing(_dac_chan));
    default_voice.cmd = 3; // phase out
    _mute = true;
	// ESP_LOGI(FNAME,"mute");
}

void Audio::enableAmplifier( bool enable )
{
	// ESP_LOGI(FNAME,"Audio::enableAmplifier( %d )", (int)enable );
	// enable Audio
	if( enable )
	{
		if( !amp_is_on ){
			ESP_LOGI(FNAME,"Audio::enableAmplifier");
			gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );   // use pullup 1 == SOUND 0 == SILENCE
			gpio_set_level(GPIO_NUM_19, 1 );
			amp_is_on = true;
			vTaskDelay(pdMS_TO_TICKS(180));  // amplifier startup time ~175mS according to datasheet Fig. 21
		}
	}
	else {
		if( amp_is_on ){
            ESP_LOGI(FNAME,"Audio::disableAmplifier");
            gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT );   // use pullup 1 == SOUND 0 == SILENCE
            gpio_set_level(GPIO_NUM_19, 0 );
            amp_is_on = false;
		}
	}
}

bool Audio::tick()
{
	polar_sink = Speed2Fly.sink( ias.get() );
	float netto = te_vario.get() - polar_sink;
	as2f = Speed2Fly.speed( netto, !VCMode.getCMode() );
	s2f_ideal.set(static_cast<int>(std::round(as2f)));
	s2f_delta = s2f_delta + ((as2f - ias.get()) - s2f_delta)* (1/(s2f_delay.get()*10)); // low pass damping moved to the correct place
	// ESP_LOGI( FNAME, "te: %f, polar_sink: %f, netto %f, s2f: %f  delta: %f", aTES2F, polar_sink, netto, as2f, s2f_delta );
	float tmp = te_vario.get();
	if ( VCMode.isNetto() ) {
		tmp -= polar_sink;
	}
	if ( VCMode.getVMode() == CruiseMode::MODE_REL_NETTO ) {
		tmp += Speed2Fly.circlingSink( ias.get() );
	}
	setValues( tmp, s2f_delta );
	return false;
}
