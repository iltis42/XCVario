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
	static inline void setVolume( int vol ) {wiper = vol;};

	static void alarm( bool enable );
	static bool selfTest();
	static inline void setTestmode( bool mode ) { _testmode = mode; }

private:
	static void dac_cosine_enable(dac_channel_t channel, bool enable=true);
	static void dac_frequency_set(int clk_8m_div, int frequency_step);
	static void dac_scale_set(dac_channel_t channel, int scale);
	static void dac_offset_set(dac_channel_t channel, int offset);
	static void dac_invert_set(dac_channel_t channel, int invert);
	static void dactask(void* arg);
	static void modtask(void* arg );
    static void enableAmplifier( bool enable );  // frue ON, false OFF
    static void calcS2Fmode();
    static bool inDeadBand( float te );
	static bool lookup( float f, int& div, int &step );
	static bool volumeScale( int vol, int& scale, int &wiper );

	static dac_channel_t _ch;
	static float _te;
	static bool _s2f_mode;
	static uint8_t _tonemode;
	static uint8_t _chopping_mode;
	static float _high_tone_var;
	static bool _testmode;
	static bool sound_on;
    static float _range;
    static uint16_t wiper;
    static uint16_t cur_wiper;
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
    static float _vol_back;
    static bool  _s2f_mode_back;
    static int   _tonemode_back;
    static int tick;
    static int tickmod;

};


#endif /* MAIN_ESPAUDIO_H_ */
