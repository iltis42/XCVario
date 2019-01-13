/*
 * ESPAudio.h
 *
 *  Created on: Jan 13, 2018
 *      Author: iltis
 */

#ifndef MAIN_ESPAUDIO_H_
#define MAIN_ESPAUDIO_H_


#include "driver/dac.h"
#include "SetupCMD.h"

class ESPAudio {
public:
	ESPAudio();
	virtual ~ESPAudio();
	void begin( dac_channel_t ch=DAC_CHANNEL_1, gpio_num_t button=GPIO_NUM_0, SetupCMD *setup=0 );
	void setTE( float te, bool fromtest=false );
	void test( float to, float from );
	void mute( bool mt=true ) { _mute = mt; };
	void disable( bool disable=true );
	bool getDisable() { return _disable; }
	void setup();

private:
	void dac_cosine_enable(dac_channel_t channel, bool enable=true);
	void dac_frequency_set(int clk_8m_div, int frequency_step);
	void dac_scale_set(dac_channel_t channel, int scale);
	void dac_offset_set(dac_channel_t channel, int offset);
	void dac_invert_set(dac_channel_t channel, int invert);
	static void dactask(void* arg);
	static void modtask(void* arg );
	dac_channel_t _ch;
	dac_channel_t getCh() { return _ch; };
	float getTE() { return _te; };
	float getCenter() { return _center; };
	float getVariation() { return _variation; };
	float getDeadBand() { return _deadband; };
	int getMute() { return _mute; };
	void setTestmode( bool mode ) { _testmode = mode; }
	gpio_num_t getButton() { return _button; }
	bool inDeadBand( float te );
	bool getDeadMute() { return _dead_mute; }
	void setDeadMute( bool mt ) {
		printf("sdm %d\n", mt );
		_dead_mute = mt; }
	void setMute( bool mt ) { _mute = mt; };
	float _te;
	float _center;
	float _deadband;
	float _deadband_neg;
	float _variation;  // max = center * variation,  min = center / variation
	bool _testmode;
    static float _range;
	gpio_num_t _button;
	bool _mute;
	bool _disable;
	bool _dead_mute;
	float _test_ms;
	float _old_ms;
	SetupCMD *_setup;
};



extern ESPAudio Audio;

#endif /* MAIN_ESPAUDIO_H_ */
