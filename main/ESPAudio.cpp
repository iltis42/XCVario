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

#include <cstdio>
#include <cstring>
#include <cstdlib>

// forward
struct TONE;

// the global access to the audio functions
Audio *AUDIO = nullptr;

// Internal queue to handle audio sequences and multi voice
static QueueHandle_t AudioQueue = nullptr;
static bool request_synch_start = false;

// DAC setup
constexpr const int SAMPLE_RATE = 44100; // Hz
constexpr const int BUF_LEN     = 1024;  // DMA buffer per push -> ~15msec chunks
constexpr const int BUF_BITMASK = (BUF_LEN/2 - 1);
constexpr const int BUF_BITS    = 9;     // log2(BUF_LEN/2)
constexpr const int TABLE_SIZE  = 128;
constexpr const int TABLE_BITS  = 7; // log2((double)TABLE_SIZE);
constexpr const int DAC_CENTER  = 127;
constexpr const int DAC_HLF_AMPL = 32;
// sequencer max number of items
constexpr const int MAX_SEQ_LEN = 12;

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
struct SNDTBL {
	const int8_t *table;
	const uint8_t bits;
};
const std::array<SNDTBL, 2> sound_table = {{
	{ (int8_t *)sine_table.data(), TABLE_BITS },
	{ (int8_t *)sawtooth_table.data(), TABLE_BITS }
}};

// helper for DMA command stream and sequencer
enum {
    VOICE_DONE = 0,
    REQUEST_SOUND,
    START_SOUND,
    ENDOFF_SOUND,
    ADD_SOUND
};
union AudioEvent {
    struct {
        uint8_t cmd;
        uint8_t param; // can be voice id, sound id, or volume
    };
    uint16_t raw = 0; // access the packed 32-bit value

    constexpr AudioEvent() = default;
    constexpr AudioEvent(uint32_t d) : raw(d) {}
    constexpr AudioEvent(uint8_t c, uint8_t v)
        : cmd(c), param(v) {}
};
struct VOICECMD
{
    void init(uint8_t table_id);
    void load(const TONE *t);
    void fastLoad(uint8_t i);
    static constexpr uint32_t calcStep(float f) {
        return (uint32_t)((f * (1ULL << 32)) / SAMPLE_RATE); }
    void setFrequencyAndGain(float f);
    void setFrequency(float f);
    float getFrequency() const;
    void setCount(int ms) {
        count = std::max((ms * SAMPLE_RATE) / (1000 * BUF_LEN/2), 1); }
    static constexpr int16_t getMaxGain(float f) {
        return (f <= 600.f) ? 128 : 128 - 47 * (f - 600.f) / 1500.0; } // -4dB at 1500Hz above 600Hz
    void setMaxGain() {
        gain_target = (step <= 58434929) ? 128 : 128 - (int16_t)(((uint32_t)47 * (step - 58434929)) / 146087322); }
    void setGain(int16_t g) {
        gain_target = g; }
    inline void start() {
        phase = 0; gain = 0; active = true; }
    inline void stop() { gain_target = 0; }
    // r/w attributes for one note (one voice) at the DMA ISR
    uint32_t phase;
    uint32_t step;
    const int8_t *table;
    uint8_t shift;
    int16_t gain;
    int16_t gain_adjust;
    int16_t gain_target;
    uint16_t count;
    bool active = false;
    uint32_t seq[MAX_SEQ_LEN+1]; // tone sequence precompiled into a step sequence
};
void VOICECMD::init(uint8_t table_id)
{
    phase = 0;
    table = sound_table[table_id].table;
    shift = 32 - sound_table[table_id].bits;
    gain = 0;
    gain_adjust = 128; // no adjustment
    gain_target = 128;
    count = 0;
    seq[0] = 0;
    active = false;
}
void VOICECMD::setFrequencyAndGain(float f)
{
    step = calcStep(f);
    gain_target = ((int)getMaxGain(f) * gain_adjust) >> 7;
}
void VOICECMD::setFrequency(float f)
{
    step = calcStep(f);
}
float VOICECMD::getFrequency() const
{
    return (float)(step * SAMPLE_RATE) / (1ULL << 32);
}

// Sequence control
struct TONE {
    TONE() = delete;
    // as first item in a sequence it is interpreted as table index, gain, and sequence length
    constexpr TONE(uint16_t t, uint16_t g, uint16_t l) : step(t<<16 | g), duration(l) {}
    // as following items it is read as frequency step and duration
    constexpr TONE(float f, uint16_t d) : step(VOICECMD::calcStep(f)), duration(d) {}
    void setFrequency(float f) { step = VOICECMD::calcStep(f); }
    uint32_t getSamples() const { return (uint32_t)((duration * SAMPLE_RATE) / 1000); }
    // only valid access on firste elements of a predefined sequence
    uint16_t getTableId() const { return step >> 16; }
    uint16_t getGain() const { return step & 0xffff; } // normalized to 128
    uint16_t getLength() const { return duration; } // number of tones in sequence
    void setLength(uint16_t l) { duration = l; }
    // attributes
    uint32_t step;
    uint16_t duration; // in msec
};
struct VOICECONF {
    uint8_t table_id;
    uint8_t gain_adjust; // 128 = no adjustment
};
constexpr int MAX_VOICES = 4;
struct SOUND {
    bool hasV0() const { return toneseq[0] != nullptr; }
    // uint32_t* timeseq; // time sequence for the tones
    const TONE* toneseq[MAX_VOICES];
    const uint8_t repetitions; // 0 = infinite
};
struct DMACMD {
    void init();
    void loadSound(SOUND* s);
    static constexpr int32_t calcNrSamples(int ms) {
        return ((int32_t)(ms) * SAMPLE_RATE) / 1000; }
    int32_t counter;    // number of samples to output for the next tone
    int32_t timeseq[MAX_SEQ_LEN+1]; // sample precision timing for sequence, terminated with a 0
    uint8_t idx;        // current index in the sound sequence
    uint8_t voicecount; // nr of voices used for this sequence
    int8_t  repcount;   // remaining repetitions
    VOICECMD voice[MAX_VOICES];
};
static __attribute__((aligned(4))) DMACMD dma_cmd;
// Vario tone
// static std::array<uint32_t, 3> vario_tim = {{ 100, 50, 0 }};
const std::array<TONE, 3> vario_seq = {{ { 0, 217, 2 }, { 500.0, 100 }, { 0., 50 } }};
const std::array<TONE, 3> vario_extra = {{ { 1, 38, 2 }, { 500.0*pow(2.0, 3./2.), 100 }, { 0., 50 } }};
static SOUND VarioSound = { { vario_seq.data(), nullptr, nullptr, nullptr }, 0 };

// Flarm alarms
const std::array<TONE, 3> flarm1_seq = {{ { 0, 128, 2 }, { 1100.0, 100 }, { 0., 50 } }};
const std::array<TONE, 3> flarm1_extra = {{ { 0, 128, 2 }, { 1100.0*pow(2.0, 4./12.), 100 }, { 0, 50 } }};
const SOUND Flarm1 = { { flarm1_seq.data(), flarm1_extra.data(), nullptr, nullptr }, 2 };
const std::array<TONE, 3> flarm2_seq = {{ { 0, 128, 2 }, { 1100.0, 70 }, { 0., 50 } }};
const std::array<TONE, 3> flarm2_extra = {{ { 0, 128, 2 }, { 1100.0*pow(2.0, 3./12.), 70 }, { 0., 50 } }};
static SOUND Flarm2 = { { flarm2_seq.data(), flarm2_extra.data(), nullptr, nullptr }, 4 };
const std::array<TONE, 3> flarm3_seq = {{ { 0, 128, 2 }, { 1100.0, 50 }, { 0., 50 } }};
const std::array<TONE, 3> flarm3_extra = {{ { 0, 128, 2 }, { 1100.0*pow(2.0, 2./12.), 50 }, { 0., 50 } }};
static SOUND Flarm3 = { { flarm3_seq.data(), flarm3_extra.data(), nullptr, nullptr }, 6 };

// Center aid sounds
static std::array<uint32_t, 3> turn_tim = {{ 1000, 0 }};
const std::array<TONE, 2> turn_seq = {{ { 0, 128, 2 }, { 888.0, 1000 } }};
const std::array<TONE, 2> stretch_seq = {{ { 1, 128, 2 }, { 888.0*2./3., 1000 } }};
const std::array<TONE, 2> turnmore_seq = {{ { 1, 128, 2 }, { 888.0*5./4., 1000 } }};
const SOUND TurnOut = { { nullptr, nullptr, turn_seq.data(), stretch_seq.data() }, 0 };
const SOUND TurnIn  = { { nullptr, nullptr, turn_seq.data(), turnmore_seq.data() }, 0 };
// list of sounds
const std::array<const SOUND*, 10> sound_list = { { &VarioSound, &TurnOut, &TurnIn, &TurnOut, &TurnIn, &Flarm1, &Flarm2, &Flarm3, &Flarm1, &Flarm1 } };

// To call from ISR context
void IRAM_ATTR VOICECMD::fastLoad(uint8_t idx) {
    uint32_t s = seq[idx];
    active = s != 0;
    if ( active ) {
        step = s;
        setMaxGain();
        gain_target = (gain_target * gain_adjust) >> 7; // scale with gain adjust
    }
    else {
        gain_target = 0;
    }
}
void VOICECMD::load(const TONE *t) {
    // prepare the tone type (table)
    phase = 0;
    table = sound_table[t->getTableId()].table;
    shift = 32 - sound_table[t->getTableId()].bits;
    // compile the tone sequence into steps
    int len = t->getLength();
    for (int i=0; i < len; i++) {
        seq[i] = t[i+1].step;
    }
    seq[len] = 0; // option to fade out a fraction of the DMA buffer length
    // prepare the very first dma irq
    fastLoad(0);
    gain = 0; // phase in
    count = 0; // need to be set only for simple timing
}
void DMACMD::init() {
    idx = 0;
    repcount = 0;
    voicecount = 0;
    for ( int i=0; i<MAX_VOICES; i++ ) {
        voice[i].init(0);
    }
    timeseq[0] = 0;
    counter = 0;
}
// Only call in synch with DAC DMA ISR event
void DMACMD::loadSound(SOUND *s)
{
    idx = 0;
    repcount = s->repetitions;
    voicecount = 0;
    for (int j = 0; j < MAX_VOICES; j++) {
        if ( s->toneseq[j] ) {
            voicecount++;
            voice[j].load(s->toneseq[j]);
        }
        else {
            voice[j].active = false;
        }
    }
    // compile the timing sequence into nr samples
    const TONE* t = s->toneseq[0];
    int i = 0;
    for (; i < t->getLength(); i++) {
        timeseq[i] = t[i+1].getSamples();
    }
    timeseq[i] = 0; // terminate sequence
    counter = timeseq[0]; // kick sound sequence
}

// Benchmark
int64_t total_us = 0, busy_irs = 0;
int irq_counter = 0;

// DMA callback
static bool IRAM_ATTR dacdma_done(dac_continuous_handle_t h, const dac_event_data_t *e, void *user_data)
{
    DMACMD& cmd = *(DMACMD *)user_data;
    VOICECMD *vl = (VOICECMD *)cmd.voice;
    uint8_t *buf = (uint8_t *)e->buf;
    AudioEvent ev(VOICE_DONE, 0);
    if ( request_synch_start ) {
        request_synch_start = false;
        ev.cmd = REQUEST_SOUND;
    }

    int64_t t_busy0 = esp_timer_get_time(); // benchmark
    irq_counter++;

    // on the fly counting of active voices
    int numVoices = 0;
    for (int j = 0; j < MAX_VOICES; j++) {
        if ( vl[j].active ) numVoices++;
    }

    if (numVoices == 0)
    {
        // silent
        memset(buf, DAC_CENTER, e->buf_size);
    }
    else
    {
        // multi tone 
        // numVoices = std::max(1, numVoices - 1);
        int i = 0;
        int z = (cmd.counter > 0 && cmd.counter < e->buf_size/2); // divide loop
        int loop_end = (z==0) ? e->buf_size : (cmd.counter * 2);
        for (; z>=0; z--) {
        
            for (int j = 0; j < cmd.voicecount; j++) {
                vl[j].fastLoad(cmd.idx);
            }

            // fuse all voices
            for (; i < loop_end; i += 2) {
                int sum = 0;
                for (int j = 0; j < MAX_VOICES; j++) {
                    // all voices
                    VOICECMD& v = vl[j];
                    if (!v.active) continue; // a silent voice
                    if ( v.gain != v.gain_target ) { // phase-in/out
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

                buf[i]   = (uint8_t)sum; // one sample
                buf[i+1] = (uint8_t)sum;
            }

            // handle the sample precise timing
            cmd.counter -= e->buf_size/2;
            
            if ( loop_end == e->buf_size ) break; // no split, DMA buffer is complete

            // cmd.counter is now < 0
            // move to the next tone in sequence
            cmd.idx++;
            if ( cmd.timeseq[cmd.idx] == 0 ) {
                
                if ( cmd.repcount > 0 ) {
                    cmd.repcount--; // restart another cycle
                    if ( cmd.repcount == 0 ) {
                        // end of sound sequence
                        ev.cmd = ENDOFF_SOUND;
                    }
                }
                cmd.idx = 0;
            }
            cmd.counter += cmd.timeseq[cmd.idx];
            loop_end = e->buf_size; // finish the remaining DMA buffer
        }
    }

    BaseType_t high_task_wakeup = pdFALSE;
    if ( ev.cmd != VOICE_DONE ) {
        xQueueSendFromISR(AudioQueue, &ev, &high_task_wakeup);
    }
    else {
        // handle voice sequence switching, timing and stopping voices
        for (int j = 0; j < MAX_VOICES; j++)
        {
            VOICECMD& v = vl[j];
            if (v.count > 0)
            {
                v.count--;
                if (v.count == 0)
                {
                    // request the audio task to feed more
                    ev.param = j;
                    BaseType_t w = pdFALSE;
                    xQueueSendFromISR(AudioQueue, &ev, &w);
                    if (w == pdTRUE) {
                        high_task_wakeup = pdTRUE;
                    }
                    vl[j].stop();
                }
            }
        }
    }

    // benchmark
    int64_t t_busy1 = esp_timer_get_time();
    busy_irs += (t_busy1 - t_busy0);

    return high_task_wakeup;
}

Audio::Audio() :
	Clock_I(8) // every 80msec
{
    dma_cmd.init();
    if ( ! AudioQueue ) {
        AudioQueue = xQueueCreate(10, sizeof(uint16_t));
    }
}

Audio::~Audio()
{
    if ( _dac_chan ) {
        stop();
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
    // switch to amplifier on GPIO19
    gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT ); // use pullup 1 == SOUND 0 == SILENCE

	updateSetup();
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
    err |= dac_continuous_register_event_callback(_dac_chan, &callbacks, &dma_cmd);

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
        if( _poti->writeWiper( 5, true ) ) {
            ESP_LOGI(FNAME,"writeWiper(5) returned Ok");
        } else {
            ESP_LOGI(FNAME,"readWiper returned error");
        }
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
        dma_cmd.voice[0].setFrequencyAndGain(f);
        dma_cmd.voice[0].start();
        AUDIO->writeVolume(25.);
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
            dma_cmd.voice[0].setFrequencyAndGain(freq);
        }
        else if (i < 8) // fifth
        {
            dma_cmd.voice[1].setFrequencyAndGain(f);
            if ( ! dma_cmd.voice[1].active ) {
                dma_cmd.voice[1].start();
            }
            dma_cmd.voice[0].setFrequencyAndGain(freq);
            ESP_LOGI(FNAME, "s=%f", f);
        }
        else if (i < 12) // octave - 1
        {
            dma_cmd.voice[0].setFrequencyAndGain(freq);
            freq = f * pow(2.0, (4 - (i-8)) / 12.0);
            dma_cmd.voice[1].setFrequencyAndGain(freq);
            ESP_LOGI(FNAME, "s=%f", freq);
        }
        else if (i < 17) // octave
        {
            if ( ! dma_cmd.voice[2].active ) {
                dma_cmd.voice[0].setFrequencyAndGain(freq);
                dma_cmd.voice[1].setFrequencyAndGain(f);
                ESP_LOGI(FNAME, "s=%f", f);
                freq = f * pow(2.0, 4./12.0);
                dma_cmd.voice[2].setFrequency(freq);
                dma_cmd.voice[2].start();
                ESP_LOGI(FNAME, "t=%f", freq);
                freq = f * pow(2.0, 7./12.0);
                dma_cmd.voice[3].setFrequency(freq);
                dma_cmd.voice[3].start();
                ESP_LOGI(FNAME, "f=%f", freq);
            }
        }
        else if (i < 21)
        {
            dma_cmd.voice[1].setFrequency(f/2.);
            dma_cmd.voice[1].setGain(200);
        }
        else
        {
            for ( int i=0; i<MAX_VOICES; i++ ) {
                dma_cmd.voice[i].stop();
            }
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

// non blocking
void Audio::alarm(e_audio_alarm_type style)
{
    if ( style > AUDIO_NO_ALARM && style < sound_list.size() && ! _alarm_mode) {
        AudioEvent ev(ADD_SOUND, style); // play complete sound
        if ( style >= AUDIO_ALARM_FLARM_1 ) {
            ev.cmd = START_SOUND; // start an alarm sound 
        }
        
        xQueueSend(AudioQueue, &ev, 0);
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

	// set the volume from nvs
   	float setvolume = default_volume.get();
	speaker_volume = vario_mode_volume = s2f_mode_volume = setvolume;
	setVolume(setvolume);

	xTaskCreate(&dactask_starter, "dactask", 2400, this, 22, &dactid);
}


void  Audio::calculateFrequency(){
	float max_var = (_audio_value > 0) ? ((maxf-center_freq.get()) * 2) : (center_freq.get()-minf);
	float range = (VCMode.audioIsVario()) ? _range : 5.0;
    float mult = std::pow( (abs(_audio_value)/range)+1, audio_factor.get());
	float freq = center_freq.get() + ((mult*_audio_value)/range )  * (max_var/_exponent_max);
    // frequencies
    dma_cmd.voice[0].seq[0] = VOICECMD::calcStep(freq);
    dma_cmd.voice[1].seq[0] = VOICECMD::calcStep(freq * 3.);
    if ( dual_tone.get() ) dma_cmd.voice[0].seq[1] = VOICECMD::calcStep(freq * _high_tone_var);
    if ( VCMode.audioIsChopping() && _audio_value > 0 ) {
        // durations
        float tmp = 1000. / (1+9*(_audio_value/range));
        // duration of main tone 1Hz: 900 ms; 10Hz: 50 ms
        int duration = (int)(tmp * 0.9) - 40;
        dma_cmd.timeseq[0] = DMACMD::calcNrSamples(duration);

        // duration of break (or second tone)
        bool needs_kick = dma_cmd.timeseq[1] == 0;
        int duration1 = (int)(tmp * 0.1) + 40; // 1Hz: 100 ms; 10Hz: 50 ms
        dma_cmd.timeseq[1] = DMACMD::calcNrSamples(duration1);
        ESP_LOGI(FNAME, "ChTiming %d/%d", duration, duration1);

        // kick the sample counting if it was stopped
        if ( needs_kick ) {
            dma_cmd.idx = 0;
            // start sample counting
            dma_cmd.counter = dma_cmd.timeseq[0];
        }
    }
    else {
        // continuous sound
        dma_cmd.timeseq[1] = 0; // disable chopping
        dma_cmd.idx = 0;
        dma_cmd.counter = 0; // infinite
    }

    // ESP_LOGI(FNAME, "New Freq: (%0.1f) TE:%0.2f exp_fac:%0.1f", freq, _audio_value, mult );
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
	int64_t t0 = esp_timer_get_time();
    SOUND *current_sound = nullptr;
    const TONE *next_tone[MAX_VOICES];
    uint8_t curr_idx[MAX_VOICES] = {0};
    uint8_t repetitions[MAX_VOICES] = {0};
    bool raise_volume = false;

    xQueueReset(AudioQueue);

    // load the vario sound
    dma_cmd.loadSound(&VarioSound);

	while(1){
		// TickType_t xLastWakeTime = xTaskGetTickCount();

		AudioEvent event;
        if (xQueueReceive(AudioQueue, &event, pdMS_TO_TICKS(2000)) == pdTRUE)
        {
			// Process audio events
            if ( event.cmd == START_SOUND ) {
                if ( event.param < sound_list.size() ) {
                    current_sound = (SOUND *)sound_list[event.param];
                    ESP_LOGI(FNAME, "Start sound %d", event.param );
                    request_synch_start = true;
                }
            }
            else if ( event.cmd == REQUEST_SOUND ) {
                dma_cmd.loadSound(current_sound);
                raise_volume = (next_tone[0] != nullptr); // alarm sound on default voice
            }
            else if ( event.cmd == ADD_SOUND ) {
                // Add sound to the queue, no synchronized start required
                current_sound = (SOUND *)sound_list[event.param];
                ESP_LOGI(FNAME, "Overlay sound %d", event.param );
                for ( int i = 2; i < MAX_VOICES; i++ ) {
                    next_tone[i] = current_sound->toneseq[i];
                    if ( next_tone[i] ) {
                        dma_cmd.voice[i].load(next_tone[i]);
                        dma_cmd.voice[i].setCount(next_tone[i][1].duration);
                        repetitions[i] = current_sound->repetitions;
                        curr_idx[i] = 1;
                        ESP_LOGI(FNAME, "Add %d: step %u d %u count %u", i, (unsigned)dma_cmd.voice[i].step, (unsigned)next_tone[i][1].duration, (unsigned)dma_cmd.voice[i].count );

                    }
                }

            }
            else if ( event.cmd == VOICE_DONE ) {
                uint8_t vid = event.param;
                ESP_LOGI(FNAME, "Event voice done %d", vid);
                            
                curr_idx[vid]++;
                if ( curr_idx[vid] > next_tone[vid]->getLength() ) {
                    if ( repetitions[vid] > 0 ) {
                        repetitions[vid]--;
                        curr_idx[vid] = 1; // restart this voice
                    }
                    else {
                        next_tone[vid] = nullptr; // end of this voice
                        current_sound = nullptr;
                        // _alarm_mode = false;
                        // writeVolume(speaker_volume);
                        ESP_LOGI(FNAME, "End of sound");
                    }
                }
                if ( next_tone[vid] ) {
                    dma_cmd.voice[vid].fastLoad(curr_idx[vid]);
                    dma_cmd.voice[vid].setCount(next_tone[vid][curr_idx[vid]].duration);

                    ESP_LOGI(FNAME, "Voice %d: step %u dur %u", vid, (unsigned)next_tone[vid][curr_idx[vid]].step, (unsigned)next_tone[vid][curr_idx[vid]].duration );
                }
                if ( raise_volume && (vid == 0) ) {
                    raise_volume = false;
                    writeVolume(80);
                }
            
            }
            // continue; // event handling done
		}
        
        if ( ! _alarm_mode ) {
            // normal vario sound operation
            ESP_LOGI(FNAME, "No event time out");
        
        }

        // ISR benchmark
        int64_t t1 = esp_timer_get_time();
        total_us = t1 - t0;
        if ( total_us > 10000000 ) { // every 10 seconds
            float load = (float)(busy_irs) * 1000.0 / total_us;
            ESP_LOGI(FNAME, "DAC load: %.1fmsec/sec (x%d; Fs=%dHz)", load, irq_counter/10, SAMPLE_RATE);
            irq_counter = 0;
            busy_irs = 0;
            total_us = 0;
            t0 = t1;
        }

        if( uxTaskGetStackHighWaterMark( dactid ) < 256 ) {
			ESP_LOGW(FNAME,"Warning Audio dac task stack low: %d bytes", uxTaskGetStackHighWaterMark( dactid ) );
		}

	}
}

bool Audio::inDeadBand()
{
    if (_audio_value > _deadband_n && _audio_value < _deadband_p) {
        return true;
    }
    return false;
}

void Audio::updateSetup()
{
	ESP_LOGD(FNAME,"Audio::setup");
	_audio_value = 0.0;
	if( audio_range.get() == AUDIO_RANGE_5_MS ) {
		_range = 5.0;
	} else if( audio_range.get() == AUDIO_RANGE_10_MS ) {
		_range = 10.0;
	} else {
		_range = scale_range.get();
	}
    _exponent_max  = std::pow( 2, audio_factor.get());

    if ( ! dual_tone.get() ) dma_cmd.voice[0].seq[1] = 0;
    
	maxf = center_freq.get() * tone_var.get();
	minf = center_freq.get() / tone_var.get();
    ESP_LOGI(FNAME,"min/max freq. %.1f/%.1f", minf, maxf);
}

// called from VCMode when mode changes
void Audio::updateAudioMode()
{
    if (VCMode.audioIsVario()) {
        _deadband_p = deadband.get();
        _deadband_n = (audio_mute_sink.get()) ? -_range-1 : deadband_neg.get();
    }
    else {
        _deadband_p = s2f_deadband.get() / 10;
        _deadband_n = s2f_deadband_neg.get() / 10;
    }
    if ((_deadband_p-_deadband_n) < 0.02) {
        _deadband_n = 0.01;
        _deadband_p = -0.01;
    }
    ESP_LOGI(FNAME, "Deaband %f/%f", _deadband_p, _deadband_n);
    ESP_LOGI(FNAME, "Vario chopping mode %d", VCMode.audioIsChopping());
    tick(); // extra tick to make changes promptly effective
    setVolume(VCMode.getCMode() ? s2f_mode_volume : vario_mode_volume); // change volume last
}

// unmute/mute the default voice
void Audio::unmute()
{
    if ( ! dma_cmd.voice[0].active ) {
        dma_cmd.voice[0].start();
    }
    if ( ! dma_cmd.voice[1].active ) {
        dma_cmd.voice[1].start();
    }
}
void Audio::mute()
{
    dma_cmd.voice[0].stop();
    dma_cmd.voice[1].stop();
}

void Audio::enableAmplifier( bool enable )
{
	// ESP_LOGI(FNAME,"Audio::enableAmplifier( %d )", (int)enable );
	// enable Audio
	if( enable )
	{
		if( !amp_is_on ){
			ESP_LOGI(FNAME, "enable amplifier");
			
			gpio_set_level(GPIO_NUM_19, 1 );
			amp_is_on = true;
            // amplifier startup time ~175ms according to datasheet Fig. 21
			vTaskDelay(pdMS_TO_TICKS(180));
		}
	}
	else {
		if( amp_is_on ){
            ESP_LOGI(FNAME, "disable amplifier");
            gpio_set_level(GPIO_NUM_19, 0 );
            amp_is_on = false;
		}
	}
}

bool Audio::tick()
{
    static int damper = 0; 
    if ( ! audio_variable_frequency.get() && (damper++ & 1) ) return false; // fixme -> change clock properly

    polar_sink = Speed2Fly.sink( ias.get() ); // fixme variable scope !!!!
	float netto = te_vario.get() - polar_sink;
	as2f = Speed2Fly.speed( netto, !VCMode.getCMode() ); // fixme variable scope !!!!
    
	s2f_ideal.set(static_cast<int>(std::round(as2f))); // fixme place code better
    // low pass damping moved to the correct place
    s2f_delta = s2f_delta + ((as2f - ias.get()) - s2f_delta)* (1/(s2f_delay.get()*10)); // fixme variable scope
    // ESP_LOGI( FNAME, "te: %f, polar_sink: %f, netto %f, s2f: %f  delta: %f", aTES2F, polar_sink, netto, as2f, s2f_delta );

    // pull the intput value from vario indicator, or speed respectively
  	float max = _range;
    if( VCMode.audioIsVario() ) {
        // vario is the parameter for audio
        float tmp = te_vario.get();
        if ( VCMode.isNetto() ) {
            tmp -= polar_sink;
        }
        if ( VCMode.getVMode() == CruiseMode::MODE_REL_NETTO ) {
            tmp += Speed2Fly.circlingSink( ias.get() );
        }
        _audio_value = tmp;
    }
    else {
        // speed to fly is the parameter for audio
        // map s2f_delta to -5..+5, instead of heaving another set of min/max variables.
        _audio_value = -s2f_delta/10.0;
        max = 5.0; // +/- 50km/h range
    }
    if( _audio_value > max ) {
        _audio_value = max;
    } else if( _audio_value < -max ) {
        _audio_value = -max;
    }

    if ( inDeadBand() ) {
        mute();
    }
    else {
        // if( audio_variable_frequency.get() ) {
        calculateFrequency();
        unmute();
    }
	return false;
}
