/*
 * AnalogInput.cpp
 *
 *  Created on: Mar 18, 2018
 *      Author: iltis
 */

#include "AnalogInput.h"

#include <esp_adc/adc_cali.h>
#include "protocol/Clock.h"
#include "logdefnone.h"


// ADC_ATTEN_DB_0 No : Input attenumation, ADC can measure up to approx. 800 mV.
// ADC_ATTEN_DB_2_5  : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1100 mV.
// ADC_ATTEN_DB_6    : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1350 mV.
// ADC_ATTEN_DB_11   : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 2600 mV.

//  ADC_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO_NUM_35 */
//  ADC_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO_NUM_34 */
//  ADC_CHANNEL_2,     /*!< ADC2 channel 2 is GPIO_NUM_2 */

constexpr const unsigned DEFAULT_VREF = 1100;

adc_oneshot_unit_handle_t AnalogInput::_adc_handle = nullptr;

// multiplier - Uin(Umeasured) := ? ; including 1/1000, because adc measures in mV
AnalogInput::AnalogInput( float multiplier, adc_channel_t ch) :
	Clock_I(7),
	_adc_ch(ch),
	_multiplier(multiplier)
{
	for( int i=0; i<RAWBUF; i++ ) {
		raw[i] = 0;
	}
}

AnalogInput::~AnalogInput()
{
	Clock::stop(this);
}

// can handle only one unit, but multiple channels on it
void AnalogInput::begin(adc_atten_t attenuation, adc_unit_t unit, bool calibration)
{
	ESP_LOGI(FNAME,"begin() unit: %d ch:%d cal:%d att: %d", unit, _adc_ch, calibration, attenuation  );

	if ( ! _adc_handle ) {
		adc_oneshot_unit_init_cfg_t init_config = {
			.unit_id = unit,
			.clk_src = (adc_oneshot_clk_src_t)0,
			.ulp_mode = ADC_ULP_MODE_DISABLE,
		};
		ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &_adc_handle));
	}

	adc_oneshot_chan_cfg_t config = {
        .atten = attenuation,
        .bitwidth = ADC_BITWIDTH_12,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(_adc_handle, _adc_ch, &config));

	if ( calibration ) {
        adc_cali_line_fitting_config_t cali_config = {
            .unit_id = unit,
            .atten = attenuation,
            .bitwidth = ADC_BITWIDTH_12,
			.default_vref = DEFAULT_VREF,
        };
        esp_err_t ret = adc_cali_create_scheme_line_fitting(&cali_config, &_adc_cali);
        if (ret == ESP_OK && _adc_cali) {
			ESP_LOGI(FNAME, "calibration scheme version is %s", "Line Fitting");
        }
	}
	Clock::start(this);
}

void AnalogInput::setAdjust(float adj){
	_adjust_factor = _multiplier * ((100.0 + adj) / 100.0);
}

unsigned int AnalogInput::getRaw() const {
	int adc = 0;
	for( int i=0; i<RAWBUF; i++ ) {
		adc += raw[i];
	}
	ESP_LOGI(FNAME,"ADC raw :%d ch :%d", adc/RAWBUF, _adc_ch );
	return adc/RAWBUF;
}
bool AnalogInput::tick()
{
	constexpr int BATCH = 3;
	int adc = 0;
	for( int i=0; i<BATCH; i++ ) {
		int rawadc = 0;
		adc_oneshot_read(_adc_handle, _adc_ch, &rawadc);
		adc += rawadc;
	}
	raw[rawidx++] = adc/BATCH;
	rawidx = rawidx % RAWBUF;
	return false;
}


float AnalogInput::get(bool damp) {
	int adc = getRaw();
	int voltage;

	if ( _adc_cali ) {
		ESP_LOGI(FNAME,"ADC have cal");
		adc_cali_raw_to_voltage(_adc_cali, adc, &voltage);
	}
	else {
		ESP_LOGI(FNAME,"ADC no cal");
		voltage = adc;
	}
	ESP_LOGI(FNAME,"ADC raw ch:%d raw %d cal-volt: %d  corr-volt: %f, _adjust_factor: %f", _adc_ch, adc, voltage, _adjust_factor * voltage, _adjust_factor);

	if ( damp ) {
		_damped_value += ( voltage - _damped_value ) * 0.35;
		return _adjust_factor * _damped_value;
	}
	else {
		return _adjust_factor * voltage;
	}
}
