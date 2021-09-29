#include "AverageVario.h"
#include "Setup.h"
#include "Protocols.h"
#include "sensor.h"

float AverageVario::averageClimbSec;
float AverageVario::averageClimb;
float AverageVario::avClimb100MSec[10];
float AverageVario::avClimbSec[60];
float AverageVario::avClimbMin[300];
int AverageVario::avindex100MSec;
int AverageVario::avindexSec;
int AverageVario::avindexMin;
int AverageVario::samples;

float AverageVario::readAvgClimb() {
	return average_climb.get();
}

void AverageVario::begin(){
	samples = 0;
	averageClimb = 0.0;
	averageClimbSec = 0.0;
	avindexSec = 0;
	avindexMin = 0;
	avindex100MSec = 0;
	for( int i=0; i<300; i++ )
		avClimbMin[i] = 0.0;
	for( int i=0; i<60; i++ )
		avClimbSec[i] = 0.0;
	for( int i=0; i<10; i++ )
		avClimb100MSec[i] = 0.0;
}

void AverageVario::recalcAvgClimb() {
	float ac = 0;
	int ns=1;
	for( int i=avindexMin, j=(int)(core_climb_history.get() * 60 )-1; i>=0 && j>=0; i--, j-- ) {
		// ESP_LOGI(FNAME,"MST pM= %2.2f  %d", avClimbMin[i], i  );
		if( avClimbMin[i] > core_climb_min.get() ) {
			ac += avClimbMin[i];
			ns++;
		}
	}
	float ac_sec = 0;
	int nss=0; // number of relevant samples per second
	// Look also to second values counted in last second
	for( int i=0; i<60; i++ ) {
		// ESP_LOGI(FNAME,"MST pM= %2.2f  %d", avClimbSec[i], i  );
		if( avClimbSec[i] > core_climb_min.get() ) {
			ac_sec += avClimbSec[i];
			nss++;
		}
	}
	if( nss ) {
		ac += ac_sec/nss;
		ns++;
	}
	if( ns )
		averageClimb = ac/ns;
	// ESP_LOGI(FNAME,"AVGsec:%2.2f  AVG:%2.2f", ac_sec, averageClimb );
	if( (int)(averageClimb*10) != (int)(average_climb.get()*10) ){
		average_climb.set( averageClimb );
	}
}


void AverageVario::newSample( float te ){  // to be called every 0.1 Second (100 mS)
	samples++;
	if( te > 0 )
		avClimb100MSec[avindex100MSec] = te;
	else
		avClimb100MSec[avindex100MSec] = 0;
	avindex100MSec++;
	if( avindex100MSec >= 10 ) {  // 0..9
		// every second
		avindex100MSec = 0;
		float ac=0;
		for( int i=0; i<10; i++ ){
			if( avClimb100MSec[i] > 0 )
				ac += avClimb100MSec[i];
		}
		avClimbSec[avindexSec] = ac/10;
		// ESP_LOGI(FNAME,"- MST pSEC= %2.2f %d", avClimbSec[avindexSec], avindexSec );
		avindexSec++;
		// every minute, or what is setup in mean climb period
		if( avindexSec >= 60 ) { // 0..59
			avindexSec = 0;
			ac=0;
			for( int i=0; i<60; i++ ) {
				ac +=  avClimbSec[i];
				avClimbSec[i] = 0;
			}
			avClimbMin[avindexMin] = ac/60;
			// ESP_LOGI(FNAME,"new MST pM= %2.2f", avClimbMin[avindexMin] );
			avindexMin++;
			if( avindexMin >= 300 ) { // overflowing, drop last h
				for( int i=60; i<300; i++ ) {
					avClimbMin[i-60] = avClimbMin[i]; // move by one h
				}
				avindexMin = 240; // one h free again
			}
		}
	}
}


