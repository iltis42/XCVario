/*
 * ESPAudio.h
 *
 *  Created on: Jan 13, 2018
 *      Author: iltis
 */

#pragma once

#include "protocol/ClockIntf.h"
#include "spline.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include <driver/dac.h>

typedef enum e_audio_alarm_type { AUDIO_ALARM_OFF, AUDIO_ALARM_STALL, AUDIO_ALARM_FLARM_1, AUDIO_ALARM_FLARM_2, AUDIO_ALARM_FLARM_3, AUDIO_ALARM_GEAR } e_audio_alarm_type_t;

class Audio : public Clock_I
{
public:
	Audio();
	virtual ~Audio();

	void begin( dac_channel_t ch=DAC_CHAN_0 );
	void unmute();
	void startAudio();

	void setValues( float te, float s2fd );
	void setFrequency( float f );
	void setup();
	void setVolume( float vol );
	void alarm( bool enable, float volume=100, e_audio_alarm_type_t alarmType=AUDIO_ALARM_STALL );
	bool selfTest();
	inline void setTestmode( bool mode ) { _testmode = mode; }
	void mute();
	void evaluateChopping();
	void dacEnable();
	void dacDisable();
	inline bool haveCAT5171(){ return _haveCAT5171; };

	bool tick() override;

private:
	void dac_cosine_enable(dac_channel_t channel, bool enable=true);
	void dac_frequency_set(int clk_8m_div, int frequency_step);
	void dac_scale_set(dac_channel_t channel, int scale);
	void dac_offset_set(dac_channel_t channel, int offset);
	void dac_invert_set(dac_channel_t channel, int invert);
	static void dactask_starter(void* arg);
	void dactask();
	void calcS2Fmode( bool recalc=false );
	bool inDeadBand( float te );
	bool lookup( float f, int& div, int &step );
	void enableAmplifier(bool enable); // frue ON, false OFF
	float equal_volume( float volume );
	void  calculateFrequency();
	void writeVolume( float volume );
	// static void doAudio();
    
	bool _s2f_mode_back = false;
	unsigned long next_scedule = 0;

	uint8_t _tonemode = 0;
	float vario_mode_volume = 63.f;
	float s2f_mode_volume = 63.f;
	float speaker_volume = 63.f;
	float current_volume = 63.f;
	dac_channel_t _ch;

	bool _chopping = false;
	float _range = 5.f;
	float _high_tone_var;
	bool _s2f_mode = false;
	bool sound = false;
	bool _testmode = false;
	bool deadband_active = false;
	bool hightone = false;
	int _delay = 100;
	int mtick = 0;
	bool _alarm_mode = false;
	float _vol_back_vario = 0;
	float _vol_back_s2f = 0;
	float maxf;
	float minf;
	float current_frequency;
	float _te = 0.f;
	float exponent_max = 2;
	float prev_aud_fact = 0;

	int prev_div = 0;
	int prev_step = 0;
	int scale = 0;
	int prev_scale = -1;
	int _tonemode_back = 0;
	int volume_change = 0;

	bool dac_enable = false;
	bool amplifier_enable = false;
	bool _haveCAT5171 = false;
	tk::spline *equalizerSpline = nullptr;
	TaskHandle_t dactid = nullptr;
};

extern Audio *AUDIO;

