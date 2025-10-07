/*
 * ESPAudio.h
 *
 *  Created on: Jan 13, 2018
 *      Author: iltis
 */

#pragma once

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/dac_continuous.h>

#include <cinttypes>
#include <cmath>

class Poti;
class TestSequence;

enum e_audio_alarm_type
{
    AUDIO_NO_ALARM = -1,
    AUDIO_VARIO_SOUND,
    AUDIO_CMD_CIRCLE_OUT,
    AUDIO_CMD_CIRCLE_IN,
    AUDIO_HORIZ_GUST,
    AUDIO_WIND_CHANGE,
    AUDIO_ALARM_FLARM_1,
    AUDIO_ALARM_FLARM_2,
    AUDIO_ALARM_FLARM_3,
    AUDIO_ALARM_STALL,
    AUDIO_ALARM_GEAR
};

class Audio
{
	friend class TestSequence;
public:
	Audio();
	virtual ~Audio();

	bool begin( int16_t ch=0 ); 				// initialisations and self-test
	bool isOk() const { return _dac_inited; }
    void stop(); 								// terminate any sound output
	void updateSetup(); 						// incorporate setup changes
	void updateAudioMode(); 					// call on cruise mode change
	void updateTone(); 							// call after sensor update
	void mute(); 								// mute the vario voice
	void unmute();  							// unmutes the vario voice
	void startAudio(); 							// starts task driving the sequencer
	// system wide the only point to set audio volume !!!
	void setVolume(float vol, bool sync = true); // vol: 0.0 .. 100.0 logarythmic scale
	void alarm(e_audio_alarm_type alarmType);  	// outputs various alarm sounds according to alarmType
	void soundCheck();   						// audible check of the audio
	bool haveCAT5171() const { return _haveCAT5171; };
	void dump();

private:
	static void dactask_starter(void* arg); 	// start task to control HW generator
	void dactask();                           	// task for control of HW generator
	bool inDeadBand();
	void enableAmplifier(bool enable);
	void calculateFrequency();    				// determine frequency to be generated depending on TE value
	void writeVolume( float volume ); // fixme

	unsigned long next_scedule = 0;

	float vario_mode_volume;
	float s2f_mode_volume;
	float speaker_volume;
	float current_volume;
	dac_continuous_handle_t _dac_chan = nullptr;
    dac_continuous_config_t _dac_cfg;
    Poti *_poti = nullptr;

	float _range = 5.f;
	float _deadband_p;
    float _deadband_n;
	static constexpr const float _high_tone_var = powf(2.0, 2./12.0); // major prime up
	bool _alarm_mode = false;
	float maxf;
	float minf;
	float _audio_value = 0.f;
	float _exponent_max = 2;

	volatile bool _test_done = false;
	bool _dac_inited = false;
	bool amp_is_on = false;
	bool _haveCAT5171 = false;
	TaskHandle_t dactid = nullptr;
};

extern Audio *AUDIO;
