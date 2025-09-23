/*
 * ESPAudio.h
 *
 *  Created on: Jan 13, 2018
 *      Author: iltis
 */

#pragma once

#include "protocol/ClockIntf.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/dac_continuous.h>

#include <cinttypes>

class Poti;
class TestSequence;

typedef enum e_audio_alarm_type { AUDIO_ALARM_OFF, AUDIO_ALARM_STALL, AUDIO_ALARM_FLARM_1, AUDIO_ALARM_FLARM_2, AUDIO_ALARM_FLARM_3, AUDIO_ALARM_GEAR } e_audio_alarm_type_t;

class Audio : public Clock_I
{
	friend class TestSequence;
public:
	Audio();
	virtual ~Audio();

	bool begin( int16_t ch=0 ); // general initialisations and equalizer setup
	bool isOk() const { return _dac_inited; }
    void stop(); // terminate any sound output
	void setup(); // setup of member variables like range and more depending on settings
	void mute(); // mute the audio
	void unmute();  // unmutes the tone generator
	void startAudio(); // starts cyclic task to control the tone generator
	void setValues( float te, float s2fd ); // this provides vario TE or S2F delta to the tone-generator via member _te; 10 km/h S2F too much creates same sound as 1 m/s climb
	void setVolume( float vol );    // vol: 0.0 .. 100.0 (%) sets the variable 'speaker_volume', which enables a smooth adjustment of volume without crackling
	void evaluateChopping();  // determines if chopping is to be done depending on setting chopping_mode for S2F and climb mode and sets member _chopping
	void alarm( bool enable, float volume=100, e_audio_alarm_type_t alarmType=AUDIO_ALARM_STALL );  // outputs various alarm tones according to alarmType and volume
	void soundCheck();   // performs a self-test
	bool haveCAT5171() const { return _haveCAT5171; }; // returns if this type of poti has been detected
	bool tick() override;  // calculates TE and S2F delta depending on mode e.g. netto mode

private:
	void setFrequency( float f );   // sets the two dividers divisor/step of the HW generator to adjust the tone frequency according to 'f' in Hz. Below 800 Hz a lookup table is used
	static void dactask_starter(void* arg);                           // start task to control HW generator
	void dactask();                                                   // task for control of HW generator
	void calcS2Fmode( bool recalc=false );                            // S2F mode
	bool inDeadBand( float te );                                      // check if mute is needed within deadband
	// bool lookup( float f, int& div, int &step );  // depending on f [Hz], lookup divisor and step in table
	void enableAmplifier(bool enable); // true ON, false OFF
	void calculateFrequency();    // determine frequency to be generated depending on TE value and tone mode (ILEC, dual tone or normal)
	void writeVolume( float volume ); // set digital poti

	bool _s2f_mode_back = false;
	unsigned long next_scedule = 0;

	uint8_t _tonemode = 0;
	float vario_mode_volume = 63.f;
	float s2f_mode_volume = 63.f;
	float speaker_volume = 63.f;
	float current_volume = 63.f;
	dac_continuous_handle_t _dac_chan = nullptr;
    dac_continuous_config_t _dac_cfg;
    Poti *_poti = nullptr;

	bool _chopping = false;
	float _range = 5.f;
	float _high_tone_var;
	bool _s2f_mode = false;
	bool deadband_active = false;
	bool hightone = false;
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

	volatile bool _test_done = false;
    bool _mute = true;
	bool _dac_inited = false;
	bool amp_is_on = false;
	bool _haveCAT5171 = false;
	TaskHandle_t dactid = nullptr;
};

extern Audio *AUDIO;
