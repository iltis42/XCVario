/*
 * BatVoltage.h
 *
 *  Created on: Mar 18, 2018
 *      Author: iltis
 */

#ifndef MAIN_BATVOLTAGE_H_
#define MAIN_BATVOLTAGE_H_

#include "esp_adc_cal.h"
#include "SetupCMD.h"

//Use adc2_vref_to_gpio() to obtain a better estimate
#define DEFAULT_VREF    1100

class BatVoltage {
public:
	BatVoltage( SetupCMD *setup );
	virtual ~BatVoltage();

	//  ADC1_CHANNEL_7,     /*!< ADC1 channel 7 is GPIO35 */
	//  ADC1_CHANNEL_6,     /*!< ADC1 channel 6 is GPIO34 */
	void begin(   adc1_channel_t battery=ADC1_CHANNEL_7, adc1_channel_t reference=ADC1_CHANNEL_6 );
	float getBatVoltage( bool init=false );
private:
	float _battery;
	const float _correct = (22.0+1.2)/1200;   // reverse mV after voltage divider 22K/1.2K to Volt
	adc1_channel_t _battery_ch;
	adc1_channel_t _reference_ch;
    long _adc_reference;
    esp_adc_cal_characteristics_t *adc_chars;
    SetupCMD *_setup;
};


#endif /* MAIN_BATVOLTAGE_H_ */
