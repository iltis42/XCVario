#include "sensor.h"
#include "Setup.h"
#include "Units.h"
#include "Flap.h"


#define ZERO_INDEX 4
#define NUMBER_POS 9

AnalogInput * Flap::sensorAdc = 0;
float Flap::lever=-1;
int   Flap::leverold=-2;
int   Flap::senspos[NUMBER_POS];
int   Flap::flapSpeeds[NUMBER_POS];
Ucglib_ILI9341_18x240x320_HWSPI* Flap::ucg;
bool Flap::surroundingBox = false;
int Flap::optPosOldY;
int Flap::sensorOldY;

#define NUMPOS  (int)( flap_pos_max.get() +1 - flap_neg_max.get() )
#define MINPOS  flap_neg_max.get()
#define MAXPOS  flap_pos_max.get()


void Flap::drawSmallBar( int ypos, int xpos, float wkf ){
	ucg->setFont(ucg_font_profont22_mr );
	int	lfh = ucg->getFontAscent()+4;
	int lfw = ucg->getStrWidth( "+2" );
	int top = ypos-lfh/2;
	if( !surroundingBox ) {
		ucg->drawFrame(xpos-5, top-3, lfw+4, 2*lfh);
		int tri = ypos+lfh/2-3;
		ucg->drawTriangle( xpos-10, tri-5,  xpos-10,tri+5, xpos-5, tri );
		surroundingBox = true;
	}
	ucg->setClipRange( xpos-2, top-2, lfw, 2*lfh-2 );
	for( int wk=int(wkf-1); wk<=int(wkf+1) && wk<=2; wk++ ){
		char position[6];
		if(wk<-2)
			continue;
		if( wk == 0 )
			sprintf( position,"% d", wk);
		else
			sprintf( position,"%+d", wk);
		int y=top+(lfh+4)*(5-(wk+2))+(int)((wkf-2)*(lfh+4));
		ucg->setPrintPos(xpos-2, y );
		ucg->setColor(COLOR_WHITE);
		ucg->printf(position);
		if( wk != -2 ) {
			ucg->drawHLine(xpos-5, y+3, lfw+4 );
		}
	}
	ucg->undoClipRange();
}

void Flap::drawLever( int xpos, int ypos, int oldypos ){
	ucg->setColor(COLOR_BLACK);
	ucg->drawBox( xpos-25, oldypos-4, 19, 8 );
	ucg->drawBox( xpos-6, oldypos-2, 4, 4 );

	ucg->setColor(COLOR_WHITE);  // left upper x,y and w,h
	ucg->drawBox( xpos-25, ypos-4, 19, 8 );
	ucg->drawBox( xpos-6, ypos-2, 4, 4 );
}

void Flap::drawBigBar( int ypos, int xpos, float wkf, float wksens ){
	ucg->setFont(ucg_font_profont22_mr );
	ucg->setFontPosCenter();
	int lfh = ucg->getFontAscent()+10;  // a bit place around number
	int lfw = ucg->getStrWidth( "+2" );
	int size = NUMPOS*lfh;
	// draw Frame around and a triangle
	if( !surroundingBox ) {
		for( int wk=MINPOS; wk<=MAXPOS; wk++ ){
			char position[6];
			if( wk == 0 )
				sprintf( position,"% d", wk);
			else
				sprintf( position,"%+d", wk);
			int y= ypos + lfh*wk;  // negative WK eq. lower position
			ucg->setPrintPos(xpos+2, y);
			ucg->setColor(COLOR_WHITE);
			// print digit
			ucg->printf(position);
			// Frame around digit
			ucg->drawFrame(xpos-2, y-(lfh/2), lfw+6, lfh );
		}
		surroundingBox = true;
	}
	ESP_LOGI(FNAME,"np: %d size: %d",  NUMPOS, size );
	int yclip = ypos+MINPOS*lfh-(lfh/2);
	ucg->setClipRange( xpos-15, yclip, 15, size );
	// now draw the numbers
	int y = ypos + (int)((wkf)*(lfh) + 0.5 );

	int ys = ypos + (int)(( wksens )*(lfh) + 0.5 );
	if( optPosOldY != y || ( (sensorOldY != ys) )) {  // redraw on change or when wklever is near
		ucg->setColor(COLOR_BLACK);
		ucg->drawTriangle( xpos-15,optPosOldY-5,  xpos-15,optPosOldY+5,  xpos-2,optPosOldY );
		ucg->setColor(COLOR_GREEN);
		ucg->drawTriangle( xpos-15,y-5,       xpos-15,y+5,       xpos-2,y );
		optPosOldY = y;
	}
	if( flap_sensor.get() ) {
		if( sensorOldY != ys ) {
			ESP_LOGI(FNAME,"wk lever redraw, old=%d", sensorOldY );
			drawLever( xpos, ys, sensorOldY );
			sensorOldY = ys;
		}
	}
	ucg->setFontPosBottom();
	ucg->undoClipRange();
}

#define DISCRAD 3
#define BOXLEN  12
#define FLAPLEN 14

void Flap::drawWingSymbol( int ypos, int xpos, int wk, int wkalt ){
	ucg->setColor( COLOR_WHITE );
	ucg->drawDisc( xpos, ypos, DISCRAD, UCG_DRAW_ALL );
	ucg->drawBox( xpos, ypos-DISCRAD, BOXLEN, DISCRAD*2+1  );
	ucg->setColor( COLOR_BLACK );
	ucg->drawTriangle( xpos+DISCRAD+BOXLEN-2, ypos-DISCRAD,
			xpos+DISCRAD+BOXLEN-2, ypos+DISCRAD+1,
			xpos+DISCRAD+BOXLEN-2+FLAPLEN, ypos+wkalt*4 );
	ucg->setColor( COLOR_RED );
	ucg->drawTriangle( xpos+DISCRAD+BOXLEN-2, ypos-DISCRAD,
			xpos+DISCRAD+BOXLEN-2, ypos+DISCRAD+1,
			xpos+DISCRAD+BOXLEN-2+FLAPLEN, ypos+wk*4 );
}

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

void  Flap::init( Ucglib_ILI9341_18x240x320_HWSPI *theUcg ){
	ucg = theUcg;

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

