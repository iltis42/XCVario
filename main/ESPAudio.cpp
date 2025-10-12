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
#include "logdefnone.h"

#include <freertos/queue.h>
#include <esp_system.h>

#include <cstdio>
#include <cstring>
#include <cstdlib>

//#define AUDIO_DEBUG 1

// forward
struct TONE;
struct DURATION;
struct VOICECONF;

// the global access to the audio functions
Audio *AUDIO = nullptr;

constexpr unsigned NIMBLE_AUDIO = 8; // frequency update every 80msec
constexpr unsigned SLOW_AUDIO = 16; // frequency update every 160msec

// Internal queue to handle audio sequences and multi voice
static QueueHandle_t AudioQueue = nullptr;
static bool request_synch_start = false;

// DAC setup
constexpr const int SAMPLE_RATE = 88200;    // Hz
constexpr const int BUF_LEN     = 2048;     // DMA buffer per push -> ~11,6msec chunks
constexpr const dac_continuous_channel_mode_t DAC_MODE = DAC_CHANNEL_MODE_ALTER; // interleaved for 2 channels
// samples per channel
constexpr const int BUF_SAMPLES = BUF_LEN/(DAC_MODE == DAC_CHANNEL_MODE_ALTER ? 4 : 2);
constexpr const int TABLE_SIZE  = 128;
constexpr const int TABLE_BITS  = 7;        // log2((double)TABLE_SIZE);
constexpr const int DAC_CENTER  = 127;      // base to all wave table design
constexpr const int DAC_HLF_AMPL = 64;

// predefined sine wave table
const std::array<int8_t, TABLE_SIZE> sine_table = {
    0, 3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 32, 35, 37, 40, 42, 45, 47, 49, 51, 53, 54, 56, 57, 58, 60, 61, 61, 62,
    63, 63, 63, 63, 63, 63, 63, 62, 61, 61, 60, 58, 57, 56, 54, 53, 51, 49, 47, 45, 42, 40, 37, 35, 32, 30, 27, 24,
    21, 18, 15, 12, 9, 6, 3, 0, -3, -6, -9, -12, -15, -18, -21, -24, -27, -30, -32, -35, -37, -40, -42, -45, -47,
    -49, -51, -53, -54, -56, -57, -58, -60, -61, -61, -62, -63, -63, -63, -63, -63, -63, -63, -62, -61, -61, -60,
    -58, -57, -56, -54, -53, -51, -49, -47, -45, -42, -40, -37, -35, -32, -30, -27, -24, -21, -18, -15, -12, -9, -6, -3 };

const std::array<int8_t, TABLE_SIZE> sawtooth_table = {
       0,   36,   43,   63,   53,   58,   52,   55,   51,   53,   49,   50,   47,   48,   46,   46, 
      44,   44,   42,   42,   40,   40,   39,   39,   37,   37,   35,   35,   33,   33,   31,   31, 
      29,   29,   28,   27,   26,   25,   24,   23,   22,   22,   20,   20,   18,   18,   17,   16, 
      15,   14,   13,   12,   11,   10,    9,    8,    7,    7,    6,    5,    4,    3,    2,    1, 
       0,   -1,   -2,   -3,   -4,   -5,   -6,   -7,   -7,   -8,   -9,  -10,  -11,  -12,  -13,  -14, 
     -15,  -16,  -17,  -18,  -18,  -20,  -20,  -22,  -22,  -23,  -24,  -25,  -26,  -27,  -28,  -29, 
     -29,  -31,  -31,  -33,  -33,  -35,  -35,  -37,  -37,  -39,  -39,  -40,  -40,  -42,  -42,  -44, 
     -44,  -46,  -46,  -48,  -47,  -50,  -49,  -53,  -51,  -55,  -52,  -58,  -53,  -63,  -43,  -36 };
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
    VLOAD_DONE = 0,
    REQUEST_SOUND,
    START_SOUND,
    ENDOFF_SOUND,
    ADD_SOUND,
    DO_VARIO
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
    void load(const VOICECONF *vc, const TONE *t);
    void fastLoad(uint8_t i);
    void reset();
    static constexpr uint32_t calcStep(float f) {
        return (uint32_t)((f * (1ULL << 32)) / SAMPLE_RATE); }
    void setFrequencyAndGain(float f);
    void setFrequency(float f);
    float getFrequency() const;
    static inline constexpr uint16_t CountFromMs(int ms) {
        return (ms * SAMPLE_RATE) / (1000 * BUF_SAMPLES); }
    void setCount(int ms) {
        count = std::max(CountFromMs(ms), (uint16_t)1); }
    static inline constexpr uint16_t CountFromSamples(int32_t spls) {
        return spls / BUF_SAMPLES;
    }
    void setCountFromSamples(int32_t spls) {
        count = CountFromSamples(spls);
    }
    static constexpr int16_t getMaxGain(float f) {
        return (f <= 600.f) ? 128 : 128 - 47 * (f - 600.f) / 1500.0; } // -4dB at 1500Hz above 600Hz
    void setMaxGain() {
        gain_target = (step <= 58434929) ? 128 : 128 - (uint16_t)(((uint32_t)47 * (step - 58434929)) / 146087322); }
    void setGain(uint16_t g) {
        gain_target = g; }
    void start() {
        phase = 0; gain = 0; active = true; }
    void stop() { gain_target = 0; }
    void dump();
    // r/w attributes for one note (one voice) at the DMA ISR
    uint32_t phase;
    uint32_t step;
    const int8_t *table;
    uint8_t shift;
    uint16_t gain;
    uint16_t gain_adjust;
    uint16_t gain_target;
    uint16_t fade_count;
    uint16_t count;
    bool active = false;
    const TONE* seq; // tone sequence precompiled into a step sequence
};
void VOICECMD::init(uint8_t table_id)
{
    phase = 0;
    table = sound_table[table_id].table;
    shift = 32 - sound_table[table_id].bits;
    gain = 0;
    gain_adjust = 128; // no adjustment
    gain_target = 128;
    fade_count = 1;
    count = 0;
    seq = nullptr;
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
    constexpr TONE(float f) : step(VOICECMD::calcStep(f)) {}
    constexpr void setStep(float f) { step = VOICECMD::calcStep(f); }
    uint32_t step;
};
struct DURATION {
    DURATION() = delete;
    constexpr DURATION(int d) : duration(calcSamples(d)) {}
    static constexpr int32_t calcSamples(int ms)  { return (ms * SAMPLE_RATE) / 1000; }
    constexpr void setSamples(int ms) { duration = calcSamples(ms); }
    int32_t duration;
};
struct VOICECONF {
    uint8_t table_id;
    uint8_t gain_adjust; // 128 = no adjustment
};
constexpr int MAX_VOICES = 4;
// the SOUND is the key data structure to store sounds in flash
// it also does work with the default vario ESP_LOGI(FNAME, "tim012 %d, %sound that stays in ram
struct SOUND {
    const DURATION* timeseq;    // time sequence for the tones in msec, terminated with a 0
                                // nr defined here have to be found in the tone sequence
    const TONE* toneseq[MAX_VOICES]; // nr of seq found here need to have a vconf
    const VOICECONF* vconf;
    const int8_t repetitions; // -1 = infinite
};
struct DMACMD {
    void init();
    void loadSound(const SOUND* s);
    void resetSound();
    void dump();
    static inline constexpr uint32_t calcNrSamples(unsigned ms) {
        return ((uint32_t)(ms) * SAMPLE_RATE) / 1000; }
    int32_t counter;    // number of samples to output for the next tone
    const DURATION* timeseq; // sample precision timing for sequence, terminated with a 0
    uint8_t idx;        // current index in the sound sequence
    uint8_t voicecount; // nr of voices used for this sequence
    int8_t  repcount;   // remaining repetitions
    VOICECMD voice[MAX_VOICES];
};
static __attribute__((aligned(4))) DMACMD dma_cmd;
// No tone
const std::array<DURATION, 2> no_tone_tim = {{ {0}, {0} }};
// Vario tone
static std::array<DURATION, 3> vario_tim = {{ {100}, {50}, {0} }};
static std::array<TONE, 3> vario_seq = {{ {500.0}, {0.}, {0.} }};
static std::array<TONE, 3> vario_extra = {{ { 500.0*pow(2.0, 3.) }, { 0.}, {0.} }};
const std::array<VOICECONF, 2> vario_vconf = {{ {0, 220}, {0, 21} }};
const SOUND VarioSound = { vario_tim.data(), { vario_seq.data(), vario_extra.data(), nullptr, nullptr }, vario_vconf.data(), -1 };

// Flarm alarms
const std::array<DURATION, 3> flarm1_tim = {{ {100}, {55}, {0} }};
const std::array<TONE, 2> flarm1_seq = {{ {1100.0}, {0.} }};
const std::array<TONE, 2> flarm1_extra = {{ {1100.0*pow(2.0, 4./12.)}, {0.} }};
const std::array<VOICECONF, 2> flarm1_vconf = {{ {0, 128}, {1, 128} }};
const SOUND Flarm1 = { flarm1_tim.data(), { flarm1_seq.data(), flarm1_extra.data(), nullptr, nullptr }, flarm1_vconf.data(), 2 };
const std::array<DURATION, 3> flarm2_tim = {{ {70}, {52}, {0} }};
const std::array<TONE, 2> flarm2_seq = {{ {1100.0}, {0.} }};
const std::array<TONE, 2> flarm2_extra = {{ {1100.0*pow(2.0, 3./12.)}, {0.} }};
const SOUND Flarm2 = { flarm2_tim.data(), { flarm2_seq.data(), flarm2_extra.data(), nullptr, nullptr }, flarm1_vconf.data(), 4 };
const std::array<DURATION, 3> flarm3_tim = {{ {50}, {50}, {0} }};
const std::array<TONE, 2> flarm3_seq = {{ {1100.0}, {0.} }};
const std::array<TONE, 2> flarm3_extra = {{ {1100.0*pow(2.0, 2./12.)}, {0.} }};
const SOUND Flarm3 = { flarm3_tim.data(), { flarm3_seq.data(), flarm3_extra.data(), nullptr, nullptr }, flarm1_vconf.data(), 6 };

// Center aid sounds
const std::array<DURATION, 2> turn_tim = {{ {1000}, {0} }};
const std::array<TONE, 2> turn_seq = {{ {888.0}, {0.} }};
const std::array<TONE, 2> stretch_seq = {{ {888.0*2./3.}, {0.} }};
const std::array<TONE, 2> turnmore_seq = {{ {888.0*5./4.}, {0.} }};
const std::array<VOICECONF, 2> turn_vconf = {{ {1, 128}, {1, 128} }};
const SOUND TurnOut = { turn_tim.data(), { nullptr, nullptr, turn_seq.data(), stretch_seq.data() }, turn_vconf.data(), 0 };
const SOUND TurnIn  = { turn_tim.data(), { nullptr, nullptr, turn_seq.data(), turnmore_seq.data() }, turn_vconf.data(), 0 };

// Stall warning
const std::array<DURATION, 29> stall_tim = {{ {150}, {65}, {110}, {50}, {134}, {30},  {150}, {65}, {110}, {50}, {134}, {30}, 
    {60}, {60}, {60}, {60}, {60}, {60}, {60}, {60}, {60}, {60}, {60}, {60}, {60}, {60}, {60}, {60}, {0} }};
const std::array<TONE, 29> stall_seq1 = {{ {0}, {3150}, {446}, {354}, {330}, {0}, {0}, {3150}, {446}, {354}, {330}, {0}, 
    {1600}, {913}, {1600}, {913}, {1600}, {913}, {1600}, {913}, {1600}, {913}, {1600}, {913}, {1600}, {913}, {1600}, {913}, {0} }};
const std::array<TONE, 29> stall_seq2 = {{ {0}, {470}, {1318}, {728}, {662}, {0}, {0}, {470}, {1318}, {728}, {662}, {0},
    {300}, {145}, {300}, {145}, {300}, {145}, {300}, {145}, {300}, {145}, {300}, {145}, {300}, {145}, {300}, {145}, {0} }};
const std::array<TONE, 29> stall_seq3 = {{ {0}, {1290}, {0}, {1227}, {1255}, {0}, {0}, {1290}, {0}, {1227}, {1255}, {0},
    {1000}, {1604}, {1000}, {1604}, {1000}, {1604}, {1000}, {1604}, {1000}, {1604}, {1000}, {1604}, {1000}, {1604}, {1000}, {1604}, {0} }};
const std::array<VOICECONF, 3> stell_vconf = {{ {0, 200}, {0, 40}, {1, 20} }};
const SOUND StallWarn = { stall_tim.data(), { stall_seq1.data(), stall_seq2.data(), stall_seq3.data(), nullptr }, stell_vconf.data(), 3 };

// list of sounds
const std::array<const SOUND*, 10> sound_list = { { &VarioSound, &TurnOut, &TurnIn, &TurnOut, &TurnIn, &Flarm1, &Flarm2, &Flarm3, &StallWarn, &Flarm1 } };

// To call from ISR context
void IRAM_ATTR VOICECMD::fastLoad(uint8_t idx) {
    uint32_t s = seq[idx].step;
    active = true; // enforce always a fade-out
    if ( s != 0 ) {
        step = s;
        setMaxGain();
        gain_target = (gain_target * gain_adjust) >> 7; // scale with gain adjust
    }
    else {
        gain_target = 0;
    }
    fade_count = (step > 0) ? (4UL * (1ULL << 32) / 128UL) / step : 1;
    if (fade_count == 0) {
        fade_count = 1;
    }
}
void VOICECMD::reset()
{
    seq = nullptr;
    gain_target = 0;
    fade_count = (step > 0) ? (4UL * (1ULL << 32) / 128UL) / step : 1;
    if (fade_count == 0) {
        fade_count = 1;
    }
}
void VOICECMD::load(const VOICECONF *vc, const TONE *t) {
    // prepare the tone type (table)
    phase = 0;
    table = sound_table[vc->table_id].table;
    shift = 32 - sound_table[vc->table_id].bits;
    // tone sequence steps, by reference
    seq = t;
    gain_adjust = vc->gain_adjust;
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
    timeseq = nullptr;
    counter = 0;
}
// Only call in synch with DAC DMA ISR event
void DMACMD::loadSound(const SOUND *s)
{
    idx = 0;
    repcount = s->repetitions;
    uint8_t vc_tmp = 0;
    for (int j = 0; j < MAX_VOICES; j++) {
        if ( s->toneseq[j] ) {
            vc_tmp++;
            voice[j].load(&s->vconf[j], s->toneseq[j]);
        }
        else {
            voice[j].reset();
        }
    }
    // compile the timing sequence into nr samples
    timeseq = (const DURATION*)(s->timeseq);
    counter = timeseq[0].duration; // kick sound sequence
    voicecount = vc_tmp;
}
void DMACMD::resetSound() {
    idx = 0;
    repcount = 0;
    for (int j = 0; j < MAX_VOICES; j++) {
        voice[j].reset();
    }
    timeseq = no_tone_tim.data();
    voicecount = 0;
}
void VOICECMD::dump() {
#if defined(AUDIO_DEBUG)
    ESP_LOGI(FNAME, "ac %d st %u c %u", active, (unsigned)step, (unsigned)count);
    ESP_LOGI(FNAME, "gain %d %d>%d (f%d)", (int)gain_adjust, (int)gain, (int)gain_target, (int)fade_count);
    if ( seq ) { ESP_LOGI(FNAME, "seq %u %u %u", (unsigned)seq[0].step, (unsigned)seq[1].step, (unsigned)seq[2].step); }
#endif
}
void DMACMD::dump() {
#if defined(AUDIO_DEBUG)
    ESP_LOGI(FNAME, "dmacmd dump");
    ESP_LOGI(FNAME, "idx %d rep %d vc %d cnt %u", (int)idx, (int)repcount, (int)voicecount, (int)counter);
    // if ( timeseq ) ESP_LOGI(FNAME, "tim012 %d, %d, %d", (int)timeseq[0].duration, (int)timeseq[1].duration, (int)timeseq[2].duration);
    for (int j = 0; j < MAX_VOICES; j++) {
        if ( voice[j].seq ) {
            ESP_LOGI(FNAME, "Voice %d", j);
            voice[j].dump();
        }
    }
#endif
}

#if defined(AUDIO_DEBUG)
// Benchmark
int64_t total_us = 0, busy_irs = 0;
int irq_counter = 0;
#endif

// DMA callback
static bool IRAM_ATTR dacdma_done(dac_continuous_handle_t h, const dac_event_data_t *e, void *user_data)
{
    static int32_t reload_count = 0;
    uint16_t fade = 0;
    DMACMD& cmd = *(DMACMD *)user_data;
    VOICECMD *vl = (VOICECMD *)cmd.voice;
    uint8_t *buf = (uint8_t *)e->buf;
    AudioEvent ev(VLOAD_DONE, 0);

    if ( request_synch_start ) {
        request_synch_start = false;
        ev.cmd = REQUEST_SOUND;
    }

#if defined(AUDIO_DEBUG)
    int64_t t_busy0 = esp_timer_get_time(); // benchmark
    irq_counter++;
#endif

    // on the fly counting of active voices
    int numVoices = 0;
    for (int j = 0; j < MAX_VOICES; j++) {
        if ( vl[j].active ) numVoices++;
    }
    if (cmd.counter < reload_count) {
        // reload the frequencies, making the variometer more responsive (even w/o sync to voice cycle)
        for (int j = 0; j < cmd.voicecount; j++) {
            vl[j].fastLoad(cmd.idx);
        }
        reload_count = 0; // once
    }
    numVoices = std::max(1, numVoices - 1);

    // multi tone
    // 1 or 2 channels, each 16 bit
    constexpr int INC_SHIFT = (DAC_MODE == DAC_CHANNEL_MODE_ALTER ? 2 : 1); // corresponding to INC
    int i = 0;
    int z = (cmd.counter > 0 && cmd.counter < (e->buf_size>>INC_SHIFT)) ? 2 : 1; // divide loop
    int loop_end = (z==1) ? (e->buf_size>>INC_SHIFT) : cmd.counter;
    for (; z>0; z--) {

        // decrement the sample counter by the loop iterations
        cmd.counter -= loop_end-i;

        if (numVoices == 0)
        {
            // silent
            memset(buf+i, DAC_CENTER, (loop_end-i)<<INC_SHIFT);
        }
        else {
            // fuse all voices
            for (; i < loop_end; i++) {
                int sum = 0;
                fade++;
                for (int j = 0; j < MAX_VOICES; j++) {
                    // all voices
                    VOICECMD& v = vl[j];
                    if (!v.active) continue; // a silent voice
                    if ( v.gain != v.gain_target ) {
                        // fade in/out
                        if ( (fade % v.fade_count) == 0 ) {
                            if ( v.gain < v.gain_target ) v.gain++;
                            else                          v.gain--;
                            if ( v.gain == 0 ) {
                                v.active = false;
                                numVoices = std::max(1, numVoices - 1);
                            }
                        }
                    }
                    // fix phase-accumulator frequency generation
                    sum += ((int)v.table[v.phase >> v.shift] * v.gain) >> 7;
                    v.phase += v.step;
                }
                sum /= numVoices;
                if ( sum >  DAC_HLF_AMPL ) sum =  DAC_HLF_AMPL;
                if ( sum < -DAC_HLF_AMPL ) sum = -DAC_HLF_AMPL;
                sum += DAC_CENTER;

                // buf[i<<INC_SHIFT]   = 0; // one sample
                buf[(i<<INC_SHIFT)+1] = (uint8_t)sum;
            }
        }

        if ( cmd.voicecount == 0 || cmd.counter > 0 ) break; // not the end of the tone, DMA buffer is complete

        // move to the next tone in sequence
        cmd.idx++;
        if ( cmd.timeseq[cmd.idx].duration <= 0 ) {

            if ( cmd.repcount >= 0 ) {
                cmd.repcount--; // restart another cycle
                if ( cmd.repcount < 0 ) {
                    // end of sound sequence
                    ev.cmd = ENDOFF_SOUND;
                    numVoices = 0;
                }
            }
            cmd.idx = 0;
        }
        // the end+1 time slice needs to be zero, as well as the n+1 steps need tp be zero
        cmd.counter = cmd.timeseq[cmd.idx].duration;
        // prepare an optional extra voice reload for signal periods longer than 300msec
        reload_count = (cmd.counter > DMACMD::calcNrSamples(300)) ? cmd.counter/2 : 0;
        loop_end = e->buf_size >> INC_SHIFT; // finish the remaining DMA buffer
        for (int j = 0; j < cmd.voicecount; j++) {
            vl[j].fastLoad(cmd.idx);
        }
    }

    BaseType_t high_task_wakeup = pdFALSE;
    if ( ev.cmd != VLOAD_DONE ) {
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

#if defined(AUDIO_DEBUG)
    // benchmark
    int64_t t_busy1 = esp_timer_get_time();
    busy_irs += (t_busy1 - t_busy0);
#endif

    return high_task_wakeup;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Audio class implementation
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Audio::Audio()
{
    dma_cmd.init();
    if ( ! AudioQueue ) {
        AudioQueue = xQueueCreate(10, sizeof(uint16_t));
    }
}
Audio::~Audio()
{
    if (_dac_chan)
    {
        stopAudio();
    }

    if (_poti)
    {
        delete _poti;
        _poti = nullptr;
    }
    _terminate = true;
}

void Audio::dacInit()
{
	ESP_LOGI(FNAME, "Starting audio on core %d", xPortGetCoreID());

    _dac_cfg = {
        .chan_mask = (dac_channel_mask_t)BIT(_channel), // which channel to enable
        .desc_num = 2,
        .buf_size = BUF_LEN,
        .freq_hz = SAMPLE_RATE,
        .offset = 0,
        .clk_src = DAC_DIGI_CLK_SRC_DEFAULT,
        .chan_mode = DAC_MODE,
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
}
void pin_audio_irq( void *arg ) {
	AUDIO->dacInit(); // core does inherit towards the dac irq resources
	xTaskNotifyGive((TaskHandle_t)arg);
	vTaskDelete(NULL);
}
bool Audio::startAudio(int16_t ch)
{
	ESP_LOGI(FNAME,"start Audio");
	if ( ! S2FSWITCH ) {
		S2FSWITCH = new S2fSwitch( GPIO_NUM_12 );
	}
    // switch to amplifier on GPIO19
    gpio_set_direction(GPIO_NUM_19, GPIO_MODE_OUTPUT ); // use pullup 1 == SOUND 0 == SILENCE
    mute();

	updateSetup();
    updateAudioMode();

    _channel = ch;
    TaskHandle_t my_task = xTaskGetCurrentTaskHandle();
	xTaskCreatePinnedToCore(pin_audio_irq, "pinaudio", 4096, (void*)my_task, 10, NULL, 1);
	ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    if ( ! _poti ) {
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
    }

    if (_poti && _dac_chan) {
        unmute();
        ESP_ERROR_CHECK(dac_continuous_start_async_writing(_dac_chan));

        // set the volume from nvs
        float setvolume = default_volume.get();
        speaker_volume = vario_mode_volume = s2f_mode_volume = setvolume;
        setVolume(setvolume);

        xTaskCreate(&dactask_starter, "dactask", 2400, this, 22, nullptr);

        return true;
    }
    return false;
}
void Audio::stopAudio() {
    mute();
    ESP_ERROR_CHECK(dac_continuous_stop_async_writing(_dac_chan));
    ESP_ERROR_CHECK(dac_continuous_disable(_dac_chan));
    ESP_ERROR_CHECK(dac_continuous_del_channels(_dac_chan));
    _dac_chan = nullptr;
    _terminate = true; // kill task
}

void Audio::startVarioVoice()
{
    while (!_test_done)
    {
        vTaskDelay(100);
    }; // wait until sound check finished

    ESP_LOGI(FNAME, "load vario sound");
    // load the vario sound
    unmute();
    writeVolume(speaker_volume);
    dma_cmd.loadSound(&VarioSound);
}
void Audio::stopVarioVoice()
{
    dma_cmd.resetSound();
}

// do the sound check, when the audio task is not yet running
class TestSequence : public Clock_I
{
public:
    TestSequence(float freq, float maxfreq) : Clock_I(15), f(freq), maxf(maxfreq)
    {
        dma_cmd.voice[0].setFrequencyAndGain(f);
        dma_cmd.voice[0].start();
        AUDIO->writeVolume(35.);
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
    ESP_LOGI(FNAME, "Audio soundCheck");
    new TestSequence(633, maxf * 1.25);
}

// kick some sound sequence, non blocking
void Audio::alarm(e_audio_alarm_type style)
{
    if ( style > AUDIO_NO_ALARM && style < sound_list.size() && ! _alarm_mode && _dac_chan) {
        AudioEvent ev(ADD_SOUND, style); // overlay sound
        if ( style >= AUDIO_ALARM_FLARM_1 ) {
            ev.cmd = START_SOUND; // start an alarm sound 
            _alarm_mode = true;
        }
        
        xQueueSend(AudioQueue, &ev, 0);
    }
}

// [%] - in a logarithmic db sense
void Audio::setVolume(float vol, bool sync) {
    if ( _alarm_mode ) {
        return; // no volume change during alarm
    }
    audio_volume.setCheckRange(vol, sync, false);
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
		// ESP_LOGI(FNAME, "setvolume() to %f, joint mode", speaker_volume );
	}
    writeVolume(speaker_volume);
}

// call when audio setup changes
void Audio::updateSetup()
{
	ESP_LOGD(FNAME,"Audio::setup");
	if( audio_range.get() == AUDIO_RANGE_5_MS ) {
		_range = 5.0;
	} else if( audio_range.get() == AUDIO_RANGE_10_MS ) {
		_range = 10.0;
	} else {
		_range = scale_range.get();
	}
    _exponent_max  = std::pow( 2, audio_factor.get());

	maxf = center_freq.get() * tone_var.get();
	minf = center_freq.get() / tone_var.get();
    ESP_LOGI(FNAME,"min/max freq. %.1f/%.1f", minf, maxf);
    VCMode.updateCache(); // force re- evaluation of cruise and audio mode
    updateAudioMode();
}

// call when cruise mode changes, setup changes
void Audio::updateAudioMode()
{
    // adjust dead band doe S2F
    // deadband also used to implement audio mute options
    if (VCMode.audioIsVario()) {
        _deadband_p = deadband.get();
        _deadband_n = audio_mute_sink.get() ? -_range-1 : deadband_neg.get();
    }
    else {
        _deadband_p = s2f_deadband.get()/10;
        _deadband_n = s2f_deadband_neg.get()/10;
    }
    if (std::abs(_deadband_p-_deadband_n) < 0.02) {
        _deadband_n = 0.01;
        _deadband_p = -0.01;
    }
    ESP_LOGI(FNAME, "Deaband %f/%f", _deadband_p, _deadband_n);

    // chopping is caches from VCMode
    ESP_LOGI(FNAME, "Vario chopping mode %d", VCMode.audioIsChopping());

    // set volume according s2f mode, need to be the last action here last
    setVolume(VCMode.getCMode() ? s2f_mode_volume : vario_mode_volume);
}

void Audio::updateTone()
{
    if ( audio_mute_gen.get() == AUDIO_ON ) {
        AudioEvent ev(DO_VARIO, 0); // update vario sound
        xQueueSend(AudioQueue, &ev, 0);
    }
}

// unmute/mute the default voice
void Audio::unmute()
{
    ESP_LOGI(FNAME, "enable amplifier");
    gpio_set_level(GPIO_NUM_19, 1);
}
void Audio::mute()
{
    ESP_LOGI(FNAME, "disable amplifier");
    gpio_set_level(GPIO_NUM_19, 0 );
}

bool Audio::haveCAT5171() const
{
    if ( _poti ) {
        return _poti->getType() == POTI_CAT5171;
    }
    return false;
}

void Audio::dump() {
    dma_cmd.dump();
}

void  Audio::calculateFrequency(float val) {
	float max_var = (val > 0) ? ((maxf-center_freq.get()) * 2) : (center_freq.get()-minf);
	float range = (VCMode.audioIsVario()) ? _range : 5.0;
    float mult = std::pow( (abs(val)/range)+1, audio_factor.get());
	float freq = center_freq.get() + ((mult*val)/range )  * (max_var/_exponent_max);
    if ( val > 0 ) {
        // durations
        float tmp = 1000. / (1+9*(val/range));
        // duration of main tone 1Hz: 900 ms; 10Hz: 50 ms
        int duration = (int)(tmp * 0.9) - 40;
        vario_tim[0].setSamples(duration);

        // duration of break (or second tone)
        duration = (int)(tmp * 0.1) + 40; // 1Hz: 100 ms; 10Hz: 50 ms
        vario_tim[1].setSamples(duration);
        // ESP_LOGI(FNAME, "d0 %d d1 %d", (int)vario_tim[0].duration, (int)vario_tim[1].duration );
    }
    else {
        vario_tim[0].setSamples(50);
        vario_tim[1].setSamples(50);
    }
    if ( inDeadBand(val) || speaker_volume < 1.0 ) {
        vario_seq[0].step = vario_extra[0].step = vario_seq[1].step = vario_extra[1].step = 0;
    }
    else {
        // frequencies
        vario_seq[0].setStep(freq);
        vario_extra[0].setStep(freq * 5.);
        if ( VCMode.audioIsChopping() && val > 0 ) {
            if ( dual_tone.get() ) vario_seq[1].setStep(freq * _high_tone_var);
            else vario_seq[1].step = vario_extra[1].step = 0;
        }
        else {
            vario_seq[1].step = vario_seq[0].step;
            vario_extra[1].step = vario_extra[0].step;
        }
    }

    // ESP_LOGI(FNAME, "New Freq: (%0.1f) TE:%0.2f exp_fac:%0.1f", freq, a, mult );
}

// set the digital volume poti, 0..100%
void Audio::writeVolume(float volume) {
	// ESP_LOGI(FNAME, "set volume: %f/%f", volume, vol);
    if (_poti) {
        _poti->softSetVolume(volume);
    }
}

void Audio::dactask_starter(void *arg)
{
	static_cast<Audio*>(arg)->dactask();
}

void Audio::dactask()
{
#if defined(AUDIO_DEBUG)
	int64_t t0 = esp_timer_get_time();
#endif
    SOUND *alarm = nullptr; // preempting the vario sound
    const TONE* next_tone[MAX_VOICES]; // added to the vario sound
    const DURATION* next_time = nullptr; // current sequence time line
    uint8_t curr_idx[MAX_VOICES] = {0};
    uint8_t repetitions[MAX_VOICES] = {0};

    _terminate = false;
    xQueueReset(AudioQueue);

	while(1)
    {
		AudioEvent event;
        if (xQueueReceive(AudioQueue, &event, pdMS_TO_TICKS(2000)) == pdTRUE)
        {
			// Process audio events
            if ( event.cmd == START_SOUND ) {
                if ( event.param < sound_list.size() ) {
                    alarm = (SOUND *)sound_list[event.param];
                    ESP_LOGI(FNAME, "Start sound %d", event.param );
                    request_synch_start = true;
                }
            }
            else if ( event.cmd == REQUEST_SOUND ) {
                dma_cmd.loadSound(alarm);
                // raise volume +20%, but assure at least 60%
                float alarm_vol = std::min(speaker_volume+20.f, 100.f);
                alarm_vol = std::max(alarm_vol, 60.f);
                writeVolume(alarm_vol);
            }
            else if ( event.cmd == ADD_SOUND ) {
                // Add sound to the queue, no synchronized start required
                const SOUND* snd = sound_list[event.param];
                next_time = snd->timeseq;
                ESP_LOGI(FNAME, "Overlay sound %d", event.param );
                for ( int i = 2; i < MAX_VOICES; i++ ) {
                    next_tone[i] = snd->toneseq[i];
                    if ( next_tone[i] ) {
                        dma_cmd.voice[i].load(&(snd->vconf[i-2]), next_tone[i]);
                        dma_cmd.voice[i].setCountFromSamples(next_time[0].duration);
                        repetitions[i] = snd->repetitions;
                        curr_idx[i] = 0;
                        ESP_LOGI(FNAME, "Add %d: step %u gain %u count %u", i, (unsigned)dma_cmd.voice[i].step, 
                            (unsigned)dma_cmd.voice[i].gain_target, (unsigned)dma_cmd.voice[i].count );
                    }
                }
            }
            else if ( event.cmd == VLOAD_DONE ) {
                uint8_t vid = event.param;
                ESP_LOGI(FNAME, "Event voice done %d", vid);
                            
                curr_idx[vid]++;
                if ( next_tone[vid][curr_idx[vid]].step == 0 ) {
                    if ( repetitions[vid] > 0 ) {
                        repetitions[vid]--;
                        curr_idx[vid] = 0; // restart this voice
                    }
                    else {
                        next_tone[vid] = nullptr; // end of this voice
                        ESP_LOGI(FNAME, "End of v-load");
                    }
                }
                if ( next_tone[vid] ) {
                    dma_cmd.voice[vid].fastLoad(curr_idx[vid]);
                    dma_cmd.voice[vid].setCountFromSamples(next_time[curr_idx[vid]].duration);
                    // ESP_LOGI(FNAME, "Voice %d: step %u dur %u", vid, (unsigned)next_tone[vid][curr_idx[vid]].step, (unsigned)next_tone[vid][curr_idx[vid]].duration );
                }
            }
            else if ( event.cmd == ENDOFF_SOUND ) {
                writeVolume(speaker_volume);
                alarm = nullptr;
                _alarm_mode = false;
                ESP_LOGI(FNAME, "End of sound");
                if (audio_mute_gen.get() == AUDIO_ON) {
                    dma_cmd.loadSound(&VarioSound);
                }
                else {
                    dma_cmd.resetSound();
                }
            }
            else if ( event.cmd == DO_VARIO && ! _alarm_mode) {
                // update vario sound
                // pull the intput value from vario indicator, or speed respectively
                float max = _range;
                float audio_value;
                if( VCMode.audioIsVario() ) {
                    // vario is the parameter for audio
                    audio_value = te_vario.get();
                    if ( VCMode.isNetto() ) {
                        audio_value -= polar_sink;
                    }
                    if ( VCMode.getVMode() == CruiseMode::MODE_REL_NETTO ) {
                        audio_value += Speed2Fly.circlingSink( ias.get() );
                    }
                }
                else {
                    // speed to fly is the parameter for audio
                    // map s2f_delta to -5..+5, instead of heaving another set of min/max variables.
                    audio_value = -s2f_delta/10.0;
                    max = 5.0; // +/- 50km/h range
                }
                if( audio_value > max ) {
                    audio_value = max;
                } else if( audio_value < -max ) {
                    audio_value = -max;
                }

                calculateFrequency(audio_value);
            }
		}

#if defined(AUDIO_DEBUG)
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
#endif

        if (_terminate) {
            break;
        }
	}
    vTaskDelete(NULL);
}


