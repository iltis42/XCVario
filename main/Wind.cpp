/*
 * Wind.cpp
 *
 *  Created on: Mar 21, 2021
 *      Author: gittest2
 */


//

#include "Wind.h"
#include "math.h"
#define DEG_TO_RAD(x) (x*M_PI/180.0)


float Wind::windspeed( float tas, float gs, float truehead, float gndhead ){
	drift( truehead, gndhead );
	return( sqrt( tas*tas + gs*gs + 2*tas*gs*cos( DEG_TO_RAD( _drift ))) );
}

float Wind::winddir( float tas, float windspeed, float truehead, float gndhead ){
	return(  asinf( tas / ( windspeed/ sin( _drift ))) );
}

Wind::Wind() {
	// TODO Auto-generated constructor stub
	_drift = 0;

}

Wind::~Wind() {
	// TODO Auto-generated destructor stub
}

