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

#define SOUND_ON 1
#define SOUND_OFF 0

const float freq_step = RTC_FAST_CLK_FREQ_APPROX / (65536 * 8 );  // div = 0x07

class ESPAudio {
public:
	ESPAudio();
	virtual ~ESPAudio();
	void begin( dac_channel_t ch=DAC_CHANNEL_1, gpio_num_t button=GPIO_NUM_0 );
	void restart();
	void startAudio();
	void setValues( float te, float s2fd, float ias, bool fromtest=false );
	void test( float to, float from );
	inline void mute( bool mt=true ) { _mute = mt; };
	void disable( bool disable=true );
	inline bool getDisable() { return _disable; };  // Just store the disable state and return on demand, Audio itself now can play in setup mode
	void setup();
	void incVolume( int steps );
	void decVolume( int steps );
	void setVolume( int vol );
	void alarm( bool enable );
	inline int  getVolume() { return wiper; };
	bool selfTest();
	inline void setTestmode( bool mode ) { _testmode = mode; }

private:
	void dac_cosine_enable(dac_channel_t channel, bool enable=true);
	inline void dac_frequency_set(int clk_8m_div, int frequency_step);
	void dac_scale_set(dac_channel_t channel, int scale);
	void dac_offset_set(dac_channel_t channel, int offset);
	void dac_invert_set(dac_channel_t channel, int invert);
	static void dactask(void* arg);
	static void modtask(void* arg );
    static void enableAmplifier( bool enable );  // frue ON, false OFF
    static void calcS2Fmode();
	bool inDeadBand( float te );
	static bool lookup( float f, int& div, int &step );
	static bool volumeScale( int vol, int& scale, int &wiper );

	static dac_channel_t _ch;
	static float _te;
	static bool _s2f_mode;
	static uint8_t _tonemode;
	static uint8_t _chopping_mode;
	static float _high_tone_var;
	float _s2fd;
	float _center;
	float _variation;  // max = center * variation,  min = center / variation
	static bool _testmode;
	static bool sound_on;
    static float _range;
	gpio_num_t _button;
	bool _mute;
	bool _disable;
	bool _dead_mute;
	float _test_ms;
	float _old_ms;
	static float _ias;
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
    static int scaled_wip;
};

typedef struct lookup {  uint16_t f; uint8_t div; uint8_t step; } t_lookup_entry;
typedef struct volume {  uint16_t vol; uint8_t scale; uint8_t wiper; } t_scale_wip;




extern ESPAudio Audio;

#endif /* MAIN_ESPAUDIO_H_ */
