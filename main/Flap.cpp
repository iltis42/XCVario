#include "Flap.h"
#include "sensor.h"
#include "Setup.h"

AnalogInput * Flap::AnalogInWk = 0;
float Flap::wksensor=-1;
int   Flap::wksenspos[9];
int   Flap::wksensorold=-2;

void  Flap::init(){
	if( flap_sensor.get() == FLAP_SENSOR_GPIO_2 ) {
		AnalogInWk = new AnalogInput( -1, ADC_ATTEN_DB_0, ADC_CHANNEL_2, ADC_UNIT_2, true );
	}else if( flap_sensor.get() == FLAP_SENSOR_GPIO_34 ) {
		AnalogInWk = new AnalogInput( -1, ADC_ATTEN_DB_0, ADC_CHANNEL_6, ADC_UNIT_1, true );
	}
	if( AnalogInWk != 0 ) {
		AnalogInWk->begin(); // GPIO2 for Wk Sensor)
		delay(10);
		uint32_t read =  AnalogInWk->getRaw();
		if( read == 0  || read >= 4096 ) // try GPIO pin 34, series 2021-2
			ESP_LOGI( FNAME, "Flap senor not found or edge value, reading: %d", read);
		else
			ESP_LOGI( FNAME, "ADC2 GPIO 2 looks good, reading: %d", read );
	}
}


float Flap::getLeverPosition( int wks ){
	// ESP_LOGI(FNAME,"getSensorWkPos %d", wks);
	int wk=0;
	int min = 3 - flap_pos_max.get();
	int max = 5 - flap_neg_max.get();
	// ESP_LOGI(FNAME,"getSensorWkPos %d min:%d max:%d", wks, min, max );
	for( int i=min; i<=max; i++ ){
		if( ((wksenspos[i] < wks) && (wks < wksenspos[i+1]))  ||
				((wksenspos[i] > wks) && (wks > wksenspos[i+1]))	) {
			wk = i;
			break;
		}
	}
	float delta=wksenspos[wk]-wksenspos[wk+1];
	float moved=wksenspos[wk]-wks;
	float relative=moved/delta;
	float wkf =(wk-4) + relative;
	// ESP_LOGI(FNAME,"return flap: %1.2f wk:%d relative: %f ", wkf, wk, relative  );
	return wkf;
}


void  Flap::progress(){
	if( AnalogInWk ) {
		int wkraw = AnalogInWk->getRaw();
		if( wkraw < 4095 && wkraw > 0 ){
			wksensor = getLeverPosition( wkraw );
			// ESP_LOGI(FNAME,"wk sensor=%1.2f", wksensor );
		}
		else
			wksensor = -10;  // off screen to blank
		if( blue_enable.get() == WL_WLAN ) {
			if( wksensorold != (int)(wksensor*10) ){
				OV.sendWkChange( wksensor );   // update secondary vario
				wksensorold = (int)(wksensor*10);
			}
		}
	}
}

void  Flap::initSensor(){
	ESP_LOGI(FNAME,"initSensor");
	wksenspos[0] = 4095;

	if( (int)flap_neg_max.get() < -2 )
		wksenspos[1] = wk_sens_pos_minus_3.get();
	else{
		wksenspos[1] = wk_sens_pos_minus_2.get() - ( wk_sens_pos_minus_1.get() - wk_sens_pos_minus_2.get()); // extrapolated
		if( wksenspos[1] > 4095 )
			wksenspos[1] = 4095;
	}

	if(  (int)flap_neg_max.get() < -1)
		wksenspos[2] = wk_sens_pos_minus_2.get();
	else{
		wksenspos[2] = wk_sens_pos_minus_1.get() - ( wk_sens_pos_0.get() - wk_sens_pos_minus_1.get()); // extrapolated
		if( wksenspos[2] > 4095 )
			wksenspos[2] = 4095;
	}

	if( (int)flap_neg_max.get() < 0 )
		wksenspos[3] = wk_sens_pos_minus_1.get();
	else
		wksenspos[3] = wk_sens_pos_0.get() - ( wk_sens_pos_plus_1.get() - wk_sens_pos_0.get()); // extrapolated

	wksenspos[4] = wk_sens_pos_0.get();

	if( (int)flap_pos_max.get() > 0 )
		wksenspos[5] = wk_sens_pos_plus_1.get();
	else
		wksenspos[5] = wk_sens_pos_0.get() - ( wk_sens_pos_minus_1.get() - wk_sens_pos_0.get()); // extrapolated pos pole

	if( (int)flap_pos_max.get() > 1 )
		wksenspos[6] = wk_sens_pos_plus_2.get();
	else{
		wksenspos[6] = wk_sens_pos_plus_1.get() - ( wk_sens_pos_0.get() - wk_sens_pos_plus_1.get()); // extrapolated pos pole
		if( wksenspos[6] < 0 )
			wksenspos[6] = 0;
	}
	if( (int)flap_pos_max.get() > 2 )
		wksenspos[7] = wk_sens_pos_plus_3.get();
	else{
		wksenspos[7] = wk_sens_pos_plus_2.get() - ( wk_sens_pos_plus_1.get() - wk_sens_pos_plus_2.get()); // extrapolated pos pole
		if( wksenspos[7] < 0 )
			wksenspos[7] = 0;
	}

	wksenspos[8] = 0;
	for( int i=0; i<=8; i++ ){
		ESP_LOGI(FNAME,"i: %d  wksenspos[i]: %d", i, wksenspos[i]  );
	}

}
