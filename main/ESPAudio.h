/*
 * ESPAudio.h
 *
 *  Created on: Jan 13, 2018
 *      Author: iltis
 */

#pragma once

#include "driver/dac.h"
#include "spline.h"

typedef enum e_audio_alarm_type { AUDIO_ALARM_OFF, AUDIO_ALARM_STALL, AUDIO_ALARM_FLARM_1, AUDIO_ALARM_FLARM_2, AUDIO_ALARM_FLARM_3, AUDIO_ALARM_GEAR } e_audio_alarm_type_t;

class Poti;
extern Poti *DigitalPoti;

class Audio {
public:
	Audio();
	virtual ~Audio(){};

	static void begin( dac_channel_t ch=DAC_CHAN_0 );
	static void restart();
	static void startAudio();

	static void setValues( float te, float s2fd );
	static void setFrequency( float f );

	static void setup();
	static void setVolume( float vol );

	static void alarm( bool enable, float volume=100, e_audio_alarm_type_t alarmType=AUDIO_ALARM_STALL );
	static bool selfTest();
	static inline void setTestmode( bool mode ) { _testmode = mode; }
    static void shutdown();  // frue ON, false OFF
    static void evaluateChopping();
    static void dacEnable();
    static void dacDisable();
    static inline bool haveCAT5171(){ return _haveCAT5171; };
    static void boot();

private:
	static void dac_cosine_enable(dac_channel_t channel, bool enable=true);
	static void dac_frequency_set(int clk_8m_div, int frequency_step);
	static void dac_scale_set(dac_channel_t channel, int scale);
	static void dac_offset_set(dac_channel_t channel, int offset);
	static void dac_invert_set(dac_channel_t channel, int invert);
	static void dactask(void* arg);
    static void calcS2Fmode( bool recalc=false );
    static bool inDeadBand( float te );
	static bool lookup( float f, int& div, int &step );
	static void enableAmplifier( bool enable );  // frue ON, false OFF
	static float equal_volume( float volume );
	static void  calculateFrequency();
	static void writeVolume( float volume );

	static dac_channel_t _ch;
	static float _te;
	static bool _s2f_mode;
	static uint8_t _tonemode;
	static float _high_tone_var;
	static bool _testmode;
	static bool sound;
    static float _range;
    static float speaker_volume;
    static float vario_mode_volume;
    static float s2f_mode_volume;
    static float current_volume;
    static float maxf;
    static float minf;
    static int prev_div;
    static int prev_step;
    static bool deadband_active;
    static float exponent_max;
    static float prev_aud_fact;
    static int scale;
    static int prev_scale;
    static bool hightone;
    static bool _alarm_mode;
    static int  defaultDelay;
    static float _vol_back_vario;
    static float _vol_back_s2f;
    static bool  _s2f_mode_back;
    static int   _tonemode_back;
    static int tick;
    static int tickmod;
    static int volume_change;
    static bool _chopping;
    static int _delay;
    static unsigned long next_scedule;
    static int mtick;
    static float current_frequency;
    static bool dac_enable;
    static bool amplifier_enable;
    static bool _haveCAT5171;
    static tk::spline *equalizerSpline;
};

