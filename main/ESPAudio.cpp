/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "ESPAudio.h"

#include "mcp4018.h"
#include "cat5171.h"
#include "S2fSwitch.h"
#include "I2Cbus.hpp"
#include "setup/CruiseMode.h"
#include "protocol/Clock.h"
#include "sensor.h"
#include "logdef.h"

#include <freertos/queue.h>

#include <esp_system.h>
#include <esp_task_wdt.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>


Audio *AUDIO = nullptr;

// Internal queue to handle audio sequences and multi voice
static QueueHandle_t AudioQueue = nullptr;

// DAC setup
constexpr const int SAMPLE_RATE = 44100; // Hz
constexpr const int BUF_LEN     = 1024;  // DMA buffer per push
constexpr const int TABLE_SIZE  = 128;
constexpr const int TABLE_BITS  = 7; // log2((double)TABLE_SIZE);
constexpr const int DAC_CENTER  = 127;
constexpr const int DAC_HLF_AMPL = 32;

// helper struct for DMA commands
struct DMACMD
{
    void init(int8_t *t, uint8_t b);
    static uint32_t calcStep(float f) { return (uint32_t)((f * (1ULL << 32)) / SAMPLE_RATE); }
    void setFrequencyAndGain(float f);
    void setFrequency(float f);
    float getFrequency() const;
    int16_t getMaxGain(float f) const;
    void setGain(int16_t g) { gain_target = g; }
    inline void start() { phase = 0; gain = 0; active = true; }
    inline void stop() { gain_target = 0; }
    uint32_t phase; 
    uint32_t step;
    int8_t *table;
    uint8_t shift;
	int16_t gain;
	int16_t gain_target;
	uint16_t count;
    bool active = false;
    // DMACMD *next = nullptr;
};
void DMACMD::init(int8_t *t, uint8_t b) {
    phase = 0;
    table = t;
    shift = 32 - b;
    gain = 0;
    gain_target = 128;
    count = 0;
    active = false;
}
void DMACMD::setFrequencyAndGain(float f) {
    step = calcStep(f);
	gain_target = getMaxGain(f);
}
void DMACMD::setFrequency(float f)
{
    step = calcStep(f);
}
float DMACMD::getFrequency() const
{
    return (float)(step * SAMPLE_RATE) / (1ULL << 32);
}
int16_t DMACMD::getMaxGain(float f) const
{
    return (f<=center_freq.get()) ? 128 : 128 - 47 *(f-center_freq.get()) / 1500.0; // -4dB at 1500Hz above center
}

// predefined sine wave table
const std::array<int8_t, TABLE_SIZE> sine_table = {
    0, 2, 4, 5, 7, 8, 10, 11, 13, 14, 15, 17, 18, 19, 20, 21, 23, 24, 24, 25, 26, 27, 28, 28, 29, 29, 
    30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 30, 30, 29, 29, 28, 28, 27, 26, 25, 24, 24, 23, 21, 20, 19, 
    18, 17, 15, 14, 13, 11, 10, 9, 7, 5, 4, 2, 0, 0, -2, -4, -6, -7, -8, -10, -11, -13, -14, -15, -16, 
    -18, -19, -20, -21, -23, -24, -24, -25, -26, -27, -28, -28, -29, -29, -30, -30, -31, -31, -31, -31, 
    -31, -31, -31, -30, -30, -30, -30, -29, -28, -28, -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, 
    -17, -15, -14, -12, -11, -10, -8, -7, -5, -4, -2, -1 };
const std::array<int8_t, TABLE_SIZE> sawtooth_table = {
    0,   18,   21,   31,   26,   29,   26,   27,   25,   26,   24,   25,   23,   24,   22,   23, 
    22,   22,   21,   21,   20,   20,   19,   19,   18,   18,   17,   17,   16,   16,   15,   15, 
    14,   14,   14,   13,   13,   12,   12,   12,   11,   11,   10,   10,    9,    9,    8,    8, 
    7,    7,    6,    6,    5,    5,    5,    4,    4,    3,    3,    2,    2,    1,    1,    0, 
    0,    0,   -1,   -1,   -2,   -2,   -3,   -3,   -4,   -4,   -5,   -5,   -5,   -6,   -6,   -7, 
    -7,   -8,   -8,   -9,   -9,  -10,  -10,  -11,  -11,  -12,  -12,  -12,  -13,  -13,  -14,  -14, 
    -14,  -15,  -15,  -16,  -16,  -17,  -17,  -18,  -18,  -19,  -19,  -20,  -20,  -21,  -21,  -22, 
    -22,  -23,  -22,  -24,  -23,  -25,  -24,  -26,  -25,  -27,  -26,  -29,  -26,  -31,  -21,  -18 };

constexpr int MAX_VOICES = 4;
static __attribute__((aligned(4))) DMACMD voice_list[MAX_VOICES];
static constexpr DMACMD& default_voice = voice_list[0];
static constexpr DMACMD& second_voice = voice_list[1];
static constexpr DMACMD& third_voice = voice_list[2];
static constexpr DMACMD& fourth_voice = voice_list[3];

// Sequence control

// start a sequence of DMACMD commands
void alertSequence(int id) {
    // Kick a predefined sequence


}

// Overlay one or more voices onto the default voice
void setVoice(int idx, int duration_ms, float frequency, int8_t *table, int size_bits) {
    DMACMD &v = voice_list[idx];
    v.init(table, size_bits);
    if ( frequency > 0 ) {
        v.setFrequencyAndGain(frequency);
    } else {
        v.step = default_voice.step * -frequency;
    }
    v.count = std::max((duration_ms * SAMPLE_RATE) / (1000 * BUF_LEN/2), 1);
    v.start();
}

// predefined voice control sequences
// alarm 1: 1400Hz & 2097Hz
// DMACMD  alarm1_5 = {0, 0, (int8_t *)sine_table.data(), 32-TABLE_BITS, 0, 7, CMD_SILENT, nullptr};
// DMACMD  alarm1_4 = {0, 204291425, (int8_t *)sine_table.data(), 32-TABLE_BITS, 128, 0, CMD_SING, (DMACMD*)&alarm1_5};
// DMACMD  alarm1_3 = {0, 136348168, (int8_t *)sine_table.data(), 32-TABLE_BITS, 128, 11, 0x10 | CMD_SING, (DMACMD*)&alarm1_4};
// DMACMD  alarm1_2 = {0, 0, (int8_t *)sine_table.data(), 32-TABLE_BITS, 0, 7, CMD_SILENT, (DMACMD*)&alarm1_3};
// DMACMD  alarm1_1 = {0, 204291425, (int8_t *)sine_table.data(), 32-TABLE_BITS, 128, 0, CMD_SING, (DMACMD*)&alarm1_2};
// DMACMD  alarm1   = {0, 136348168, (int8_t *)sine_table.data(), 32-TABLE_BITS, 128, 11, 0x10 | CMD_SING, (DMACMD*)&alarm1_1};


// Benchmark
int64_t total_us = 0, busy_irs = 0;
int irq_counter = 0;


// DMA callback
static bool IRAM_ATTR dacdma_done(dac_continuous_handle_t h, const dac_event_data_t *e, void *user_data)
{
    DMACMD *vl = (DMACMD *)user_data;
    uint8_t *buf = (uint8_t *)e->buf;

    int64_t t_busy0 = esp_timer_get_time(); // benchmark
    irq_counter++;

    int numVoices = 0;
    for (int j = 0; j < MAX_VOICES; j++) {
        if ( vl[j].active ) numVoices++;
    }
    numVoices = std::max(1, numVoices - 1);

    if (numVoices == 0)
    {
        // silent
        memset(buf, DAC_CENTER, e->buf_size);
    }
    else
    {
        // multi tone
        for (int i=0; i < e->buf_size; i += 2) {
            int sum = 0;
            for (int j = 0; j < MAX_VOICES; j++) {
                DMACMD& v = vl[j];
                if (!v.active) continue;
                if ( v.gain != v.gain_target ) {
                    if ( v.gain < v.gain_target ) v.gain++;
                    else                          v.gain--;
                    if ( v.gain == 0 ) {
                        v.active = false;
                        numVoices = std::max(1, numVoices - 1);
                    }
                }
                sum += ((int)v.table[v.phase >> v.shift] * v.gain) >> 7;
                v.phase += v.step;
            }
            sum /= numVoices;
            if ( sum >  DAC_HLF_AMPL ) sum =  DAC_HLF_AMPL;
            if ( sum < -DAC_HLF_AMPL ) sum = -DAC_HLF_AMPL;
            sum += DAC_CENTER;

            buf[i]   = (int8_t)sum;
            buf[i+1] = (int8_t)sum;
        }
    }
        
    for (int j = 0; j < MAX_VOICES; j++) {
        if ( vl[j].count > 0 ) {
            vl[j].count--;
            if ( vl[j].count == 0 ) vl[j].stop();
        }
    }


    // benchmark
    int64_t t_busy1 = esp_timer_get_time();
    busy_irs += (t_busy1 - t_busy0);

    return false;
}

Audio::Audio() :
	Clock_I(8) // every 80msec
{
    default_voice.init((int8_t*)sine_table.data(), TABLE_BITS);
    second_voice.init((int8_t*)sine_table.data(), TABLE_BITS);
    third_voice.init((int8_t*)sine_table.data(), TABLE_BITS);
    fourth_voice.init((int8_t*)sine_table.data(), TABLE_BITS);

    if ( ! AudioQueue ) {
        AudioQueue = xQueueCreate(5, sizeof(uint32_t));
    }
}

Audio::~Audio()
{
    if ( _dac_chan ) {
        mute();
        vTaskDelay(pdMS_TO_TICKS(10)); // no cracks ..
        enableAmplifier(false);
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
    vTaskDelay(pdMS_TO_TICKS(2)); // no cracks ..
    enableAmplifier(false);
    ESP_ERROR_CHECK(dac_continuous_stop_async_writing(_dac_chan));
}

// do the sound check, when the audio task is not yet running
class TestSequence : public Clock_I
{
public:
    TestSequence(float freq, float maxfreq) : Clock_I(15), f(freq), maxf(maxfreq)
    {
        default_voice.setFrequencyAndGain(f);
        default_voice.start();
        AUDIO->writeVolume(25.);
        AUDIO->unmute();
        Clock::start(this);
    }
    ~TestSequence()
    {
        AUDIO->_test_done = true;
    }
    bool tick() override
    {
        i++;
        float freq = f * powf(2.0, i / 12.0); // half tone up
        ESP_LOGI(FNAME, "iter %d %f", i, freq);
        if (i < 4) // major third
        {
            default_voice.setFrequencyAndGain(freq);
        }
        else if (i < 8) // fifth
        {
            second_voice.setFrequencyAndGain(f);
            if ( ! second_voice.active ) {
                second_voice.start();
            }
            default_voice.setFrequencyAndGain(freq);
            ESP_LOGI(FNAME, "s=%f", f);
        }
        else if (i < 12) // octave - 1
        {
            default_voice.setFrequencyAndGain(freq);
            freq = f * pow(2.0, (4 - (i-8)) / 12.0);
            second_voice.setFrequencyAndGain(freq);
            ESP_LOGI(FNAME, "s=%f", freq);
        }
        else if (i < 13) // octave
        {
            default_voice.setFrequencyAndGain(freq);
            second_voice.setFrequencyAndGain(f);
            ESP_LOGI(FNAME, "s=%f", f);
            freq = f * pow(2.0, 4./12.0);
            third_voice.setFrequency(freq);
            third_voice.start();
            ESP_LOGI(FNAME, "t=%f", freq);
            freq = f * pow(2.0, 7./12.0);
            fourth_voice.setFrequency(freq);
            fourth_voice.start();
            ESP_LOGI(FNAME, "f=%f", freq);
        }
        else if (i < 17)
        {
            second_voice.setFrequency(f/2.);
            second_voice.setGain(200);
        }
        else if (i < 21) {
        }
        else
        {
            default_voice.stop();
            second_voice.stop();
            third_voice.stop();
            fourth_voice.stop();
            AUDIO->mute();
            delete this;
            return true;
        }
        return false;
    }

private:
    float f;
    float maxf;
    int i = 0;
};
void Audio::soundCheck()
{
    if (!_dac_inited)
    {
        return;
    }
    ESP_LOGI(FNAME, "Audio::selfTest");
    new TestSequence(633, maxf * 1.25);
}

void Audio::alarm( bool enable, float volume, e_audio_alarm_type_t style ){  // non blocking
	if( enable && !_alarm_mode ){ // Begin of alarm
		_vol_back_s2f = s2f_mode_volume;
		_vol_back_vario = vario_mode_volume;
		_s2f_mode_back = _s2f_mode;
		_s2f_mode = false;
		_tonemode_back = _tonemode;
		_alarm_mode=true;
		// enableAmplifier( true );
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
			// startSequence(&alarm1);
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
void Audio::setVolume(float vol) {
    audio_volume.setCheckRange(vol, false, false);
	speaker_volume = audio_volume.get();
    if( audio_split_vol.get() ) {
		if( VCMode.getCMode() ) {
			s2f_mode_volume = speaker_volume;
		} else {
			vario_mode_volume = speaker_volume;
		}
		ESP_LOGI(FNAME, "setvolume() to %f, for %s", speaker_volume, VCMode.getCMode() ? "s2f" : "vario" );
	} else {
		// copy to both variables in case audio_split_vol enabled later
		s2f_mode_volume = speaker_volume;
		vario_mode_volume = speaker_volume;
		ESP_LOGI(FNAME, "setvolume() to %f, joint mode", speaker_volume );
	}
    writeVolume(speaker_volume);
}


void Audio::startAudio(){
    if ( ! _dac_inited ) {
        return;
    }
	while ( ! _test_done ) { vTaskDelay(100); }; // wait until sound check finished
	ESP_LOGI(FNAME,"startAudio");
    // default_voice.next = &second_voice;
	evaluateChopping();
	float setvolume = default_volume.get();
	speaker_volume = vario_mode_volume = s2f_mode_volume = setvolume;
	writeVolume( setvolume );
	xTaskCreate(&dactask_starter, "dactask", 2400, this, 24, &dactid);
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

void Audio::indicateCenter(bool open)
{
    const float frq1 = 880.0;
    setVoice(1, 1000, frq1, (int8_t*)sawtooth_table.data(), TABLE_BITS);
    // a quint down or a major third up
    setVoice(2, 1000, frq1* (open ? 2./3. : 5./4.), (int8_t*)sawtooth_table.data(), TABLE_BITS);
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
        default_voice.setFrequencyAndGain(current_frequency * _high_tone_var);
    }
    else {
        default_voice.setFrequencyAndGain(current_frequency);
    }
	// ESP_LOGI(FNAME, "New Freq: (%0.1f) TE:%0.2f exp_fac:%0.1f", current_frequency, _te, mult );
}

void Audio::writeVolume(float volume) {
	ESP_LOGI(FNAME, "set volume: %f", volume);
    if (_poti) {
        // if( _alarm_mode )
        //  ...
        _poti->writeVolume(volume);
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

    xQueueReset(AudioQueue);

	esp_task_wdt_add(NULL);

	while(1){
		TickType_t xLastWakeTime = xTaskGetTickCount();
		tick++;

		// uint32_t event;
        // if (xQueueReceive(AudioQueue, &event, pdMS_TO_TICKS(delay)) == pdTRUE)
        // {
		// 	// Process audio events
		// }
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

        // ISR benchmark
		if ( !(tick%1000) ) {
			int64_t t1 = esp_timer_get_time();
            total_us += (t1 - t0);
            float load = (float)(busy_irs) / 10000.0;
            ESP_LOGI(FNAME, "DAC load: %.1fmsec/sec (x%d; Fs=%dHz)", load, irq_counter/10, SAMPLE_RATE);
			irq_counter = 0;
			busy_irs = 0;
			total_us = 0;
            t0 = t1;
		}

        if( uxTaskGetStackHighWaterMark( dactid ) < 256 ) {
			ESP_LOGW(FNAME,"Warning Audio dac task stack low: %d bytes", uxTaskGetStackHighWaterMark( dactid ) );
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

	maxf = center_freq.get() * tone_var.get();
	minf = center_freq.get() / tone_var.get();
    ESP_LOGI(FNAME,"min/max freq. %.1f/%.1f", minf, maxf);
	current_frequency = (minf+maxf)/2.;
}

// unmute/mute the default voice
void Audio::unmute()
{
    if ( ! default_voice.active ) {
        default_voice.start(); //setCmd(CMD_SING);
    }

    // ESP_LOGI(FNAME,"unmute");
    // ESP_ERROR_CHECK(dac_continuous_start_async_writing(_dac_chan));
    _mute = false;
}
void Audio::mute(){
    // ESP_ERROR_CHECK(dac_continuous_stop_async_writing(_dac_chan));
    default_voice.stop();
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
