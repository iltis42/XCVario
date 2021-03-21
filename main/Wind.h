/*
 * Wind.h
 *
 *  Created on: Mar 21, 2021
 *      Author: gittest2
 */

#ifndef MAIN_WIND_H_
#define MAIN_WIND_H_

class Wind {
public:
	Wind();
	virtual ~Wind();
	inline void drift( float truehead, float gndhead ) { _drift = truehead - gndhead; };
	float winddir( float tas, float windspeed, float truehead, float gndhead );
	float windspeed( float tas, float gs, float truehead, float gndhead );
	float _drift;
};

#endif /* MAIN_WIND_H_ */
