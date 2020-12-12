/*
 * AnalogInput.h
 *
 *  Created on: Mar 18, 2018
 *      Author: iltis
 */

#ifndef MAIN_BATVOLTAGE_H_
#define MAIN_BATVOLTAGE_H_

#include "esp_adc_cal.h"
#include "Setup.h"

//Use adc2_vref_to_gpio() to obtain a better estimate
#define DEFAULT_VREF    1100

class AnalogInput {
public:
	AnalogInput( float multiplier=0 , adc_atten_t attenuation=ADC_ATTEN_DB_0 );
	virtual ~AnalogInput();

	//  ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO_NUM_35 */
	//  ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO_NUM_34 */
	//  ADC1_CHANNEL_2,     /*!< ADC2 channel 2 is GPIO_NUM_2 */
	// https://randomnerdtutorials.com/esp32-adc-analog-read-arduino-ide/
	void begin( adc1_channel_t ch=ADC1_CHANNEL_7 );
	float get( bool nofilter=false, int loops=64 );
	inline int getRaw() { return( adc1_get_raw( _adc_ch ) ); };

private:
	float _value;
	adc_atten_t _attenuation;
	float _correct;   // reverse mV after voltage divider 22K/1.2K to Volt
	adc1_channel_t _adc_ch;
    esp_adc_cal_characteristics_t *adc_chars;
};

class AnalogInput2 {
public:
	AnalogInput2( adc_atten_t attenuation=ADC_ATTEN_DB_0 ) {
		_adc_ch = ADC2_CHANNEL_MAX;
		adc_chars = 0;
		_attenuation = attenuation;
	};
	virtual ~AnalogInput2() {};

	void begin( adc2_channel_t ch=ADC2_CHANNEL_2 );
	inline int getRaw( int loops=64, float filter=0.2 ){
		int ret = 0;
		for( int i=0; i<loops; i++ ){
			int raw;
			adc2_get_raw( _adc_ch, ADC_WIDTH_BIT_12, &raw );
			ret+=raw;
		}

		if( filter > 0 )
			_value += ( (ret/64) - _value ) * filter;
		else
			_value = ret/64;
		return _value;
	};

private:
	adc_atten_t _attenuation;
	adc2_channel_t _adc_ch;
    esp_adc_cal_characteristics_t *adc_chars;
    float _value;
};



#endif /* MAIN_BATVOLTAGE_H_ */
