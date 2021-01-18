#include "Flap.h"
#include "sensor.h"
#include "Setup.h"
#include "Units.h"



#define ZERO_INDEX 4
#define NUMBER_POS 9

AnalogInput * Flap::sensorAdc = 0;
float Flap::lever=-1;
int   Flap::leverold=-2;
int   Flap::senspos[NUMBER_POS];
int   Flap::flapSpeeds[NUMBER_POS];

void  Flap::initSpeeds(){
	flapSpeeds[0] = Units::Airspeed2Kmh( 280 );
	flapSpeeds[1] = Units::Airspeed2Kmh( flap_minus_3.get() );
	flapSpeeds[2] = Units::Airspeed2Kmh( flap_minus_2.get() );
	flapSpeeds[3] = Units::Airspeed2Kmh( flap_minus_1.get() );
	flapSpeeds[4] = Units::Airspeed2Kmh( flap_0.get() );
	flapSpeeds[5] = Units::Airspeed2Kmh( flap_plus_1.get() );
	flapSpeeds[6] = Units::Airspeed2Kmh( flap_plus_2.get() );
	flapSpeeds[7] = Units::Airspeed2Kmh( 50 );
}

void  Flap::init(){
	if( flap_sensor.get() == FLAP_SENSOR_GPIO_2 ) {
		sensorAdc = new AnalogInput( -1, ADC_ATTEN_DB_0, ADC_CHANNEL_2, ADC_UNIT_2, true );
	}else if( flap_sensor.get() == FLAP_SENSOR_GPIO_34 ) {
		sensorAdc = new AnalogInput( -1, ADC_ATTEN_DB_0, ADC_CHANNEL_6, ADC_UNIT_1, true );
	}
	if( sensorAdc != 0 ) {
		sensorAdc->begin(); // GPIO2 for Wk Sensor)
		delay(10);
		uint32_t read =  sensorAdc->getRaw();
		if( read == 0  || read >= 4096 ) // try GPIO pin 34, series 2021-2
			ESP_LOGI( FNAME, "Flap senor not found or edge value, reading: %d", read);
		else
			ESP_LOGI( FNAME, "ADC2 GPIO 2 looks good, reading: %d", read );
	}
	initSpeeds();
}

float Flap::getLeverPosition( int wks ){
	// ESP_LOGI(FNAME,"getSensorWkPos %d", wks);
	int wk=0;
	int max = ZERO_INDEX +1 + flap_pos_max.get();
	int min = ZERO_INDEX -1 + flap_neg_max.get();
	// ESP_LOGI(FNAME,"getSensorWkPos %d min:%d max:%d", wks, min, max );
	for( int i=min; i<=max; i++ ){
		if( ((senspos[i] < wks) && (wks < senspos[i+1]))  ||
				((senspos[i] > wks) && (wks > senspos[i+1]))	) {
			wk = i;
			break;
		}
	}
	float delta=senspos[wk]-senspos[wk+1];
	float moved=senspos[wk]-wks;
	float relative=moved/delta;
	float wkf =(wk-ZERO_INDEX) + relative;
	// ESP_LOGI(FNAME,"getLeverPos(%d): lever: %1.2f wk: %d relative: %1.1f  N:%d X%d ", wks, wkf, wk, relative, min, max  );
	return wkf;
}

void  Flap::progress(){
	if( sensorAdc ) {
		int wkraw = sensorAdc->getRaw();
		if( wkraw < 4095 && wkraw > 0 ){
			lever = getLeverPosition( wkraw );
			// ESP_LOGI(FNAME,"wk sensor=%1.2f", lever );
		}
		else
			lever = -10;  // off screen to blank
		if( blue_enable.get() == WL_WLAN ) {
			if( leverold != (int)(lever*10) ){
				OV.sendWkChange( lever );   // update secondary vario
				leverold = (int)(lever*10);
			}
		}
	}
}

void  Flap::initSensor(){
	ESP_LOGI(FNAME,"initSensor");
	senspos[ZERO_INDEX-4] = 4095;

	if( (int)flap_neg_max.get() < -2 )
		senspos[ZERO_INDEX-3] = wk_sens_pos_minus_3.get();
	else{
		senspos[ZERO_INDEX-3] = wk_sens_pos_minus_2.get() - ( wk_sens_pos_minus_1.get() - wk_sens_pos_minus_2.get()); // extrapolated
		if( senspos[ZERO_INDEX-3] > 4095 )
			senspos[ZERO_INDEX-3] = 4095;
	}

	if(  (int)flap_neg_max.get() < -1)
		senspos[ZERO_INDEX-2] = wk_sens_pos_minus_2.get();
	else{
		senspos[ZERO_INDEX-2] = wk_sens_pos_minus_1.get() - ( wk_sens_pos_0.get() - wk_sens_pos_minus_1.get()); // extrapolated
		if( senspos[ZERO_INDEX-2] > 4095 )
			senspos[ZERO_INDEX-2] = 4095;
	}

	if( (int)flap_neg_max.get() < 0 )
		senspos[ZERO_INDEX-1] = wk_sens_pos_minus_1.get();
	else
		senspos[ZERO_INDEX-1] = wk_sens_pos_0.get() - ( wk_sens_pos_plus_1.get() - wk_sens_pos_0.get()); // extrapolated

	senspos[ZERO_INDEX] = wk_sens_pos_0.get();

	if( (int)flap_pos_max.get() > 0 )
		senspos[ZERO_INDEX+1] = wk_sens_pos_plus_1.get();
	else
		senspos[ZERO_INDEX+1] = wk_sens_pos_0.get() - ( wk_sens_pos_minus_1.get() - wk_sens_pos_0.get()); // extrapolated pos pole

	if( (int)flap_pos_max.get() > 1 )
		senspos[ZERO_INDEX+2] = wk_sens_pos_plus_2.get();
	else{
		senspos[ZERO_INDEX+2] = wk_sens_pos_plus_1.get() - ( wk_sens_pos_0.get() - wk_sens_pos_plus_1.get()); // extrapolated pos pole
		if( senspos[ZERO_INDEX+2] < 0 )
			senspos[ZERO_INDEX+2] = 0;
	}
	if( (int)flap_pos_max.get() > 2 )
		senspos[ZERO_INDEX+3] = wk_sens_pos_plus_3.get();
	else{
		senspos[ZERO_INDEX+3] = wk_sens_pos_plus_2.get() - ( wk_sens_pos_plus_1.get() - wk_sens_pos_plus_2.get()); // extrapolated pos pole
		if( senspos[ZERO_INDEX+3] < 0 )
			senspos[ZERO_INDEX+3] = 0;
	}
	senspos[ZERO_INDEX+4] = 0;
	for( int i=0; i<=8; i++ ){
		ESP_LOGI(FNAME,"lever: %d  senspos[i]: %d", i-4, senspos[i]  );
	}
}


float Flap::getOptimum( float wks, int wki ){
	// ESP_LOGI(FNAME,"wks:%f min:%f max:%f", wks, minv, maxv );
	float minv = flapSpeeds[wki+4];
	float maxv = flapSpeeds[wki+3];
	if( wks <= maxv && wks >= minv )
		return ((wks-minv)/(maxv-minv));
	else if( wks > maxv )
		return 1;
	else if( wks < minv )
		return 0.5;
	return 0.5;
}


int Flap::getOptimumInt( float wks )
{
	for( int wk=flap_neg_max.get(); wk<=flap_pos_max.get(); wk++ ){
		if( wks <= flapSpeeds[wk+3] && wks >=  flapSpeeds[wk+4] ) {
			return wk;
		}
	}
	if( wks < flapSpeeds[7] ) {
		return 1;
	}
	else if( wks > flapSpeeds[0] ){
		return -2;
	}
	else {
		return 1;
	}
}

