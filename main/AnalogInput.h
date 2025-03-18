/*
 * AnalogInput.h
 *
 *  Created on: Mar 18, 2018
 *      Author: iltis
 */

#pragma once

#include "protocol/ClockIntf.h"

#include <driver/adc.h>
#include <esp_adc_cal.h>



class AnalogInput final : public Clock_I {
public:
	explicit AnalogInput( float multiplier, adc_atten_t attenuation, adc_channel_t ch, adc_unit_t unit=ADC_UNIT_1, bool calibration=true );
	virtual ~AnalogInput() = default;

	void begin();
	float get( bool nofilter=false, int loops=64 );
	unsigned int getRaw( int loops=32 );
	void redoAdjust();

    // Clock tick callback
    bool tick() override;

private:
	static constexpr int RAWBUF = 5;
	const adc_unit_t _unit;
	const adc_channel_t _adc_ch;
	float _multiplier;
	const adc_atten_t _attenuation;
	uint16_t raw[RAWBUF];
	int rawidx = 0;
	float _correct;   // reverse mV after voltage divider 22K/1.2K to Volt
	float _value;
	esp_adc_cal_characteristics_t adc_chars;
    bool _cal;
};



