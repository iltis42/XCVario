/*
 * AnalogInput.h
 *
 *  Created on: Mar 18, 2018
 *      Author: iltis
 */

#pragma once

#include "protocol/ClockIntf.h"

#include <esp_adc/adc_oneshot.h>


class AnalogInput final : public Clock_I {
public:
	explicit AnalogInput(float multiplier, adc_channel_t ch);
	virtual ~AnalogInput();

	void begin(adc_atten_t attenuation, adc_unit_t unit=ADC_UNIT_1, bool calibration=true);
	void setAdjust(float adj);
	unsigned int getRaw() const;
 	// create raw readings
    bool tick() override;
	// get a calibrated voltage reading
	float get(bool damp=true);

    

private:
	const adc_channel_t _adc_ch;
	static adc_oneshot_unit_handle_t _adc_handle;
	adc_cali_handle_t _adc_cali = nullptr;
	// buffers
	static constexpr int RAWBUF = 5;
	uint16_t raw[RAWBUF];
	int rawidx = 0;
	float _multiplier;
	float _adjust_factor;   // reverse mV after voltage divider 22K/1.2K to Volt
	float _damped_value = 0.;
};



