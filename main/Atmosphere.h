/*
 * Methods for Athmosphere Model used in Aviation
 *
 *
 */
#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H


// With density of water from: http://www.csgnetwork.com/waterinformation.html
// @ 22.8 degree: 0.997585
// earth gravity: 9.0807 m/s^2
// and standard ICAO air density with 1.225 kg/m3 there is:
// V(km/h) = sqrt(2*( <mmH2O> * 0.997585 * 9.807  )/1.225) * 3.6

//   Speed
// mmH2O m/s	km/h
// 100	 40,0	143,9
// 105          147,4
// 110	 41,9	150,9
// 116	 43,0	155,0
// 120	 43,8	157,6
// 130	 45,6	164,0
// 140	 47,3	170,2

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
