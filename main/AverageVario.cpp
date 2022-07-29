#include "AverageVario.h"
#include "Setup.h"
#include "Protocols.h"
#include "sensor.h"

float AverageVario::averageClimbSec;
float AverageVario::averageClimb;
std::array<float, 10> AverageVario::avClimb100MSec;
std::array<float, 60> AverageVario::avClimbSec;
std::list<float> AverageVario::avClimbMin;
int AverageVario::avindex100MSec;
int AverageVario::avindexSec;
int AverageVario::samples;

float AverageVario::readAvgClimb() {
	return average_climb.get();
}

void AverageVario::begin(){
	samples = 0;
	averageClimb = 0.0;
	averageClimbSec = 0.0;
	avindexSec = 0;
	avindex100MSec = 0;
	avClimbMin.resize( core_climb_history.get() );
}

void AverageVario::recalcAvgClimb() {
	float ac = 0;
	int ns=0;
	for( auto it=avClimbMin.begin(); it != avClimbMin.end(); it++ ) {
		// ESP_LOGI(FNAME,"MST pM= %2.2f", *it  );
		ac += *it;
		ns++;
	}
	if( ns )
		averageClimb = ac/ns;
	// ESP_LOGI(FNAME,"AVGsec:%2.2f  AVG:%2.2f", ac_sec, averageClimb );
	if( (int)(averageClimb*100) != (int)(average_climb.get()*100) ){
		average_climb.set( averageClimb );
	}
}


void AverageVario::newSample( float te ){  // to be called every 0.1 Second (100 mS)
	samples++;
	float te_positive = 0.0;
	if( te > 0 )
		te_positive = te;
	avClimb100MSec[avindex100MSec] = te_positive;
	avindex100MSec++;
	if( avindex100MSec >= 10 ) {  // 0..9
		// every second sum up all 100mS samples, then store in second
		avindex100MSec = 0;
		float acms=0.0;
		for( int i=0; i<10; i++ ){
			acms += avClimb100MSec[i];
		}
		acms = acms/10.0;
		if( acms > core_climb_min.get() ){  // if climb average in one second is above core climb rate
			avClimbSec[avindexSec] = acms;
			// ESP_LOGI(FNAME,"- MST pSEC= %2.2f %d", avClimbSec[avindexSec], avindexSec );
			avindexSec++;
			// every minute, or what is setup in mean climb period
			if( avindexSec >= 60 ) { // 0..59
				avindexSec = 0;
				float acs=0.0;
				for( int i=0; i<60; i++ ) {
					acs +=  avClimbSec[i];
				}
				acs = acs/60.0;
				avClimbMin.push_back( acs ); // push new element to end of FIFO
				// ESP_LOGI(FNAME,"new MST pM= %2.2f", ac );
				if( avClimbMin.size() >= core_climb_history.get() ) { // overflowing, drop last element
					avClimbMin.pop_front(); // remove oldest element from FIFO
				}
			}
		}
	}
}


