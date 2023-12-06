/*
 * ESPAudio.h
 *
 *  Created on: Jan 13, 2018
 *      Author: iltis
 */

#ifndef MAIN_ESPAUDIO_H_
#define MAIN_ESPAUDIO_H_

#include "driver/gpio.h"
#include "driver/dac.h"
#include "Setup.h"
#include "soc/rtc.h"
#include "spline.h"
#include "Poti.h"

typedef enum e_audio_alarm_type { AUDIO_ALARM_OFF, AUDIO_ALARM_STALL, AUDIO_ALARM_FLARM_1, AUDIO_ALARM_FLARM_2, AUDIO_ALARM_FLARM_3, AUDIO_ALARM_GEAR } e_audio_alarm_type_t;

extern Poti *DigitalPoti;

class Audio {
public:
	Audio();
	virtual ~Audio(){};

	static void begin( dac_channel_t ch=DAC_CHANNEL_1 );
	static void restart();
	static void startAudio();

	static void setValues( float te, float s2fd );
	static void setFrequency( float f );

	static void setup();
	static void incVolume( int steps );
	static void decVolume( int steps );
	static void setVolume( int vol );

	static void alarm( bool enable, int volume=100, e_audio_alarm_type_t alarmType=AUDIO_ALARM_STALL );
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
	static void modtask(void* arg );
    static bool calcS2Fmode();
    static bool inDeadBand( float te );
	static bool lookup( float f, int& div, int &step );
	static void enableAmplifier( bool enable );  // frue ON, false OFF
	static uint16_t equal_volume( uint16_t volume );
	static void  calculateFrequency();
	static void writeWiper( uint16_t volume );

	static dac_channel_t _ch;
	static float _te;
	static bool _s2f_mode;
	static uint8_t _tonemode;
	static float _high_tone_var;
	static bool _testmode;
	static bool sound;
    static float _range;
    static uint16_t *p_wiper;
    static uint16_t wiper;
    static uint16_t wiper_s2f;
    static uint16_t cur_wiper;
    static float maxf;
    static float minf;
    static int prev_div;
    static int prev_step;
    static bool deadband_active;
    static bool disable_amp;
    static float exponent_max;
    static float prev_aud_fact;
    static int scale;
    static int prev_scale;
    static bool hightone;
    static bool _alarm_mode;
    static int  defaultDelay;
    static uint16_t _vol_back;
    static uint16_t _vol_back_s2f;
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
    static int _step;
    static bool dac_enable;
    static bool amplifier_enable;
    static bool _haveCAT5171;
    static tk::spline *equalizerSpline;
};


#endif /* MAIN_ESPAUDIO_H_ */
