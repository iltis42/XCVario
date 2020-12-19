/*
 * Methods for Athmosphere Model used in Aviation
 *
 *
 */
#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H


class Atmosphere {
	Atmosphere() {};
	~Atmosphere() {};
public:
	static inline float TAS( float ias, float baro, float temp ) {
		return( ias * sqrt( 1.225 / ( baro*100.0 / (287.058 * (273.15+temp)))));
	};
	static inline float pascal2kmh( float pascal ){
		return sqrt( 2*pascal / 1.225 )*3.6;
	};
	static inline double calcAltitude(double SeaLevel_Pres, double pressure) {
			return ( 44330.0 * (1.0 - pow(pressure / SeaLevel_Pres, (1.0/5.255))) );
	};

};

#endif
