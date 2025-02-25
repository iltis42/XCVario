/*
 * AnalogInput.cpp
 *
 *  Created on: Mar 18, 2018
 *      Author: iltis
 */

#include "AnalogInput.h"
#include "sensor.h"
#include "protocol/Clock.h"
#include "logdefnone.h"


#define DIODE_VOLTAGE_DROP 0        // New Vario now measures behind Diode

// ADC_ATTEN_DB_0 No : Input attenumation, ADC can measure up to approx. 800 mV.
// ADC_ATTEN_DB_2_5  : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1100 mV.
// ADC_ATTEN_DB_6    : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 1350 mV.
// ADC_ATTEN_DB_11   : The input voltage of ADC will be attenuated, extending the range of measurement to up to approx. 2600 mV.

//  ADC_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO_NUM_35 */
//  ADC_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO_NUM_34 */
//  ADC_CHANNEL_2,     /*!< ADC2 channel 2 is GPIO_NUM_2 */

#define DEFAULT_VREF    1100

// multiplier - Uin(Umeasured) := ? ; including 1/1000, because adc measures in mV
AnalogInput::AnalogInput( float multiplier, adc_atten_t attenuation, adc_channel_t ch, adc_unit_t unit, bool calibration ) :
	Clock_I(10),
	_unit(unit),
	_adc_ch(ch),
	_attenuation(attenuation),
	_correct(multiplier * ((100.0 + factory_volt_adjust.get()) / 100.0)),
	_cal(calibration)
{
	if ( _correct == 0.f ) {
		_correct = 1.f;
	}
	for( int i=0; i<RAWBUF; i++ ) {
		raw[i] = 0;
	}

}

void AnalogInput::begin()
{
	ESP_LOGI(FNAME,"begin() unit: %d ch:%d cal:%d att: %d", _unit, _adc_ch, _cal, _attenuation  );

	if( _unit == ADC_UNIT_1 ) {
		adc1_config_width(ADC_WIDTH_BIT_12);
		adc1_config_channel_atten((adc1_channel_t)_adc_ch,_attenuation);
	}
	else if( _unit == ADC_UNIT_2 ) {
		adc2_config_channel_atten((adc2_channel_t)_adc_ch,_attenuation);
	}else {
		 ESP_LOGE(FNAME,"ADC unit %d illegal", _unit );
	}

	if ( _cal ) {
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
	}

	Clock::start(this);
}


unsigned int AnalogInput::getRaw( int loops ) {
	int adc = 0;
	for( int i=0; i<RAWBUF; i++ ) {
		adc += raw[i];
	}
	ESP_LOGI(FNAME,"ADC raw :%d ch :%d", adc/RAWBUF, _adc_ch );
	return adc/RAWBUF;
}
bool AnalogInput::tick()
{
	constexpr int BATCH = 5;
	int adc = 0;
	for( int i=0; i<BATCH; i++ ) {
		int rawadc;
		if( _unit == ADC_UNIT_1 )
			rawadc =  adc1_get_raw((adc1_channel_t)_adc_ch);
		else if( _unit == ADC_UNIT_2 )
			adc2_get_raw((adc2_channel_t)_adc_ch, ADC_WIDTH_BIT_12, &rawadc );
		adc += rawadc;
	}
	raw[rawidx++] = adc/BATCH;
	rawidx = rawidx % RAWBUF;
	return false;
}

float AnalogInput::get( bool nofilter, int loops ){
	int adc = getRaw();
	int voltage;
	
	if( _cal ) {
		voltage = esp_adc_cal_raw_to_voltage(adc, &adc_chars);
	}
	else {
		voltage = adc;
	}
	float corrected = _correct * voltage +  DIODE_VOLTAGE_DROP;
	ESP_LOGI(FNAME,"ADC raw unit:%d ch:%d raw %d cal:%d  corr: %f", _unit, _adc_ch, adc, voltage, corrected );

	if( nofilter )
		_value = corrected;
	else
		_value += ( corrected - _value ) * 0.35;

	return _value;
}


