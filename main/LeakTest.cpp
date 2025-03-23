#include "LeakTest.h"
#include "ESPRotary.h"
#include "logdef.h"

#define LOOPS 150

void LeakTest::start( PressureSensor* bmpBA, PressureSensor* bmpTE, AirspeedSensor *asSensor ) {
	ESP_LOGI(FNAME,"Starting Leak test");
	display->clear();
	display->writeText( 1, "** Leak Test **");
	float sba=0;   // Start values
	float ste=0;
	float sspeed = 0;
	float bad=0;
	float ted=0;
	float speedd=0;
	bool failed = false;
	for( int i=0; i<24; i++ ){  // 180
		char buf[40];
		float ba=0;
		float te=0;
		float speed=0;
		int loops_run=0;
		for( int j=0; j< LOOPS; j++ ) {
			bool ok=false;
			float s=0;
			if( asSensor )
				s = asSensor->readPascal(5,ok);
			if( ok ){
				speed +=s;
				loops_run++;
			}
			bool ok1, ok2;
			ba += bmpBA->readPressure(ok1);
			te += bmpTE->readPressure(ok2);
			delay( 33 );
		}
		ba = ba/LOOPS;
		te = te/LOOPS;
		if( loops_run )
			speed = speed/loops_run;
		if( i==0 ) {
			sba = ba;
			ste = te;
			sspeed = speed;
		}
		sprintf(buf, "ST P: %3.2f hPa   ", ba);
		display->writeText( 2, buf);
		sprintf(buf, "TE P: %3.2f hPa   ", te);
		display->writeText( 3, buf);
		sprintf(buf, "PI P: %3.2f Pa    ", speed);
		display->writeText( 4, buf);
		if( i==0 ) {
			ESP_LOGI(FNAME,"ST %3.2f  TE %3.2f  PI %3.2f", ba,te,speed);
		}
		if( i>=1 ) {
			bad = 100*(ba-sba)/sba;
			ted = 100*(te-ste)/ste;
			speedd = 100*(speed-sspeed)/sspeed;
			sprintf(buf, "ST delta: %2.3f %%   ", bad );
			display->writeText( 5, buf);
			ESP_LOGI(FNAME,"%s", buf);
			sprintf(buf, "TE delta: %2.3f %%   ", ted );
			display->writeText( 6, buf);
			ESP_LOGI(FNAME,"%s", buf);
			sprintf(buf, "PI delta: %2.2f %%   ", speedd );
			display->writeText( 7, buf);
			ESP_LOGI(FNAME,"%s", buf);
		}
		sprintf(buf, "Seconds: %d", (i*5)+5 );
		display->writeText( 8, buf );
		// once we have exceeded, we can already stop here
		if( (abs(bad) > 0.1) || (abs(ted) > 0.1) || ( (sspeed > 10.0) && (abs(speedd) > (1.0) ) ) ){
			failed = true;
			break;
		}
	}
	if( failed ) {
		display->writeText( 9, "Test FAILED" );
		ESP_LOGI(FNAME,"FAILED");
	}
	else {
		display->writeText( 9, "Test PASSED" );
		ESP_LOGI(FNAME,"PASSED");
	}
	while( !Rotary->readSwitch() ) {
		delay(100);
	}
	display->clear();
}

