/*
 * ESPAudio.h
 *
 *  Created on: Jan 13, 2018
 *      Author: iltis
 */

#pragma once

#include <driver/dac_continuous.h>

#include <cinttypes>
#include <cmath>

class Poti;
class TestSequence;

enum e_audio_sound_type
{
    AUDIO_NO_SOUND,
    AUDIO_VARIO_SOUND,
    AUDIO_CHECK_SOUND,
    AUDIO_FAIL_SOUND,
    AUDIO_CMD_CIRCLE_OUT,
    AUDIO_CMD_CIRCLE_IN,
    AUDIO_DING,
    AUDIO_HORIZ_GUST,
    AUDIO_WIND_CHANGE,
    AUDIO_FLAP_FORWARD,
    AUDIO_FLAP_BACK,
    AUDIO_ALARMS,
    AUDIO_ALARM_STALL = AUDIO_ALARMS, // first alarm sound, they require a volume raise
    AUDIO_ALARM_GLOAD,
    AUDIO_ALARM_GEAR,
    AUDIO_ALARM_FLARM,
    AUDIO_ALARM_FCODE
};

class Audio
{
    friend class TestSequence;

public:
    Audio();
    virtual ~Audio();

    void applySetup();                          // incorporate setup changes
    void initVarioVoice();                      // start vario sound for the very first time
    bool isUp() const { return _dac_chan != nullptr; }
    bool isPotiUp() const { return _poti != nullptr; }

    void startSound(uint16_t alarmType, bool overlay = false); // outputs various alarm sounds according to alarmType
    static uint16_t encFlarmParam(e_audio_sound_type sound_id, uint8_t alevel, uint8_t side, uint8_t alt_diff);

    // system wide the only point to set audio volume !!!
    void setVolume(float vol, bool sync = true); // vol: 0.0 .. 100.0 logarythmic scale
    void updateAudioMode();                     // call on cruise mode change
    void updateTone();                          // call after sensor update
    void mute();                                // mute the sound entirely
    void unmute();                              // unmutes
    bool haveCAT5171() const;
    void dump();

private:
    friend void pin_audio_irq(void *arg);
    void dacInit();
    bool startAudio(int16_t ch = 0); // initialisations and self-test, starts task driving the sequencer
    void stopAudio();                // terminate any sound output
    bool inDeadBand(float a) const { return (a > _deadband_n && a < _deadband_p); }
    void calculateFrequency(float a);       // determine frequency to be generated depending on TE value
    void writeVolume(float volume);

    static void dactask_starter(void *arg); // start task to control HW generator
    void dactask();                         // task for control of HW generator

    float vario_mode_volume;
    float s2f_mode_volume;
    float speaker_volume;
    int16_t _channel;
    dac_continuous_handle_t _dac_chan = nullptr;
    dac_continuous_config_t _dac_cfg;
    Poti *_poti = nullptr;

    float _range = 5.f;
    float _deadband_p;
    float _deadband_n;
    bool _alarm_mode = false;
    float maxf;
    float minf;
    float _exponent_max = 2;

    bool _terminate = true;
};

extern Audio *AUDIO;
