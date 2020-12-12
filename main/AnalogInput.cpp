/*
 * AnalogInput.cpp
 *
 *  Created on: Mar 18, 2018
 *      Author: iltis
 */



#include "driver/adc.h"
#include "AnalogInput.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc_cal.h"
#include <logdef.h>

#define DIODE_VOLTAGE_DROP 0        // New Vario now measures behind Diode

// ADC_ATTEN_DB_0 No : Input attenumation, ADC can measure up to approx. 800 mV.
// ADC_ATTEN_DB_2_5  : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1100 mV.
// ADC_ATTEN_DB_6    : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1350 mV.
// ADC_ATTEN_DB_11   : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 2600 mV.


AnalogInput::AnalogInput( float multiplier, adc_atten_t attenuation ) {
  _adc_ch = ADC1_CHANNEL_MAX;
  adc_chars = 0;
  _correct = multiplier;
  _attenuation = attenuation;
}

esp_adc_cal_characteristics_t adc_cal;

esp_adc_cal_characteristics_t adc_cal2;

void AnalogInput::begin( adc1_channel_t ch ) {
	_adc_ch = ch;
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ch,_attenuation);

	adc_chars = (esp_adc_cal_characteristics_t *) &adc_cal;
	esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, _attenuation, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
	//Check type of calibration value used to characterize ADC
	if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
		ESP_LOGI(FNAME,"ESP32 ADC eFuse Vref");
	} else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
		ESP_LOGI(FNAME,"ESP32 ADC Two Point");
	} else {
		ESP_LOGI(FNAME,"ESP32 ADC Default");
	}
	get( true );
}

float AnalogInput::get( bool nofilter, int loops ){
	int adc = 0;
	for( int i=0; i<loops; i++ ) {
	   adc += adc1_get_raw( _adc_ch );
	}
	adc = adc/64;
	uint32_t voltage = esp_adc_cal_raw_to_voltage( adc, adc_chars);
    // ESP_LOGI(FNAME,"Voltage %d", voltage);

	float current = (float)voltage * _correct * ( (100.0 + factory_volt_adjust.get()) / 100.0 ) +  DIODE_VOLTAGE_DROP;
	if( nofilter )
		_value = current;
	else
		_value += ( current - _value ) * 0.2;
	return _value;
}

AnalogInput::~AnalogInput() {

}

void AnalogInput2::begin( adc2_channel_t ch ){
	_adc_ch = ch;
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc2_config_channel_atten(ch,_attenuation);
	adc_chars = (esp_adc_cal_characteristics_t *) &adc_cal2;
};


