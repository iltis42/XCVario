/*
 * AnalogInput.cpp
 *
 *  Created on: Mar 18, 2018
 *      Author: iltis
 */

#include "AnalogInput.h"

#include "sensor.h"
#include "sdkconfig.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/adc.h>
#include <esp_adc_cal.h>
#include <logdef.h>

#define DIODE_VOLTAGE_DROP 0        // New Vario now measures behind Diode

// ADC_ATTEN_DB_0 No : Input attenumation, ADC can measure up to approx. 800 mV.
// ADC_ATTEN_DB_2_5  : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1100 mV.
// ADC_ATTEN_DB_6    : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1350 mV.
// ADC_ATTEN_DB_11   : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 2600 mV.


AnalogInput::AnalogInput( float multiplier, adc_atten_t attenuation, adc_channel_t ch, adc_unit_t unit, bool calibration ) {
  _adc_ch = ch;
  _correct = multiplier;
  _attenuation = attenuation;
  _unit = unit;
  _cal = calibration;
  _value = 0;
}

void AnalogInput::begin() {
	ESP_LOGI(FNAME,"begin() unit: %d ch:%d cal:%d att: %d", _unit, _adc_ch, _cal, _attenuation  );
	adc_power_acquire(); // we need ADC for battery measure and Flaps
	if( _unit == ADC_UNIT_1 ) {
		adc1_config_width(ADC_WIDTH_BIT_12);
		adc1_config_channel_atten((adc1_channel_t)_adc_ch,_attenuation);
	}
	else if( _unit == ADC_UNIT_2 ) {
		adc2_config_channel_atten((adc2_channel_t)_adc_ch,_attenuation);
	}else
		 ESP_LOGE(FNAME,"ADC unit %d illegal", _unit );

	ESP_LOGI(FNAME,"ADC chars: unit:%d atten:%d", _unit, _attenuation );
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(_unit, _attenuation, ADC_WIDTH_BIT_12, DEFAULT_VREF, &adc_chars);
	//Check type of calibration value used to characterize ADC
	if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
		ESP_LOGI(FNAME,"ESP32 ADC eFuse Vref");
	} else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
		ESP_LOGI(FNAME,"ESP32 ADC Two Point");
	} else {
		ESP_LOGI(FNAME,"ESP32 ADC Default");
	}
	get();
}


unsigned int AnalogInput::getRaw( int loops ) {
	int adc = 0;
	for( int i=0; i<loops; i++ ) {
		int raw;
		if( _unit == ADC_UNIT_1 )
			raw =  adc1_get_raw((adc1_channel_t)_adc_ch);
		else if( _unit == ADC_UNIT_2 )
			adc2_get_raw((adc2_channel_t)_adc_ch, ADC_WIDTH_BIT_12, &raw );
		adc += raw;
		// ESP_LOGI(FNAME,"ADC raw :%d ch :%d", raw, _adc_ch );
		if( loops > 1)
			delay(1);
	}
	adc = adc/loops;
	return( adc );
};

float AnalogInput::get( bool nofilter, int loops ){
	int adc = getRaw( loops );
	uint32_t voltage;
	if( _cal )
		voltage = esp_adc_cal_raw_to_voltage(adc, &adc_chars);
	else
		voltage = adc;
	float corrected;
	if( _correct > 0 )
		corrected = (float)voltage * _correct * ( (100.0 + factory_volt_adjust.get()) / 100.0 ) +  DIODE_VOLTAGE_DROP;
	else
		corrected = (float)voltage;
	// ESP_LOGI(FNAME,"ADC raw unit:%d ch:%d raw %d cal:%d  corr: %f", _unit, _adc_ch, adc, voltage, corrected );

	if( nofilter )
		_value = corrected;
	else
		_value += ( corrected - _value ) * 0.2;

	return _value;
}

AnalogInput::~AnalogInput() {

}


