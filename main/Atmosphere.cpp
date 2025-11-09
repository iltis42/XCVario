#include "Atmosphere.h"

#include <cmath>

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

// Standard Atmosphere Equations:
//
// p0 = air pressure at sea level
//
// ℎ = altitude above sea level
// 
// 𝑇 = 288,15𝐾
//
// T0 = 288,15K
//
// 𝑔 = 9,80665 𝑚/s^2
//
// 𝑀 = 0,0289644 𝑘𝑔/𝑚𝑜𝑙
//
// 𝑅 = 8,3144598 𝐽/(𝑚𝑜𝑙⋅𝐾)
//
// Exponent:
// E = R⋅L/g⋅M ≈ 5,255
//
// p = p0​⋅(1−​L⋅h/T0​)^E
//

float Atmosphere::TAS(float ias, float baro, float temp) {
     return (ias * sqrt(1.225 / (baro * 100.0 / (287.058 * (273.15 + temp)))));
}

// TAS=IAS/sqrt( 288.15/(T+273.15) * (P/1013.25) )
float Atmosphere::TAS2(float ias, float altitude, float temp) {
    return (ias / sqrt(288.15 / (temp + 273.15) * (Atmosphere::calcPressureISA(altitude) / 1013.25)));
}

// TAS=IAS/sqrt( 288.15/(T+273.15) * (P/1013.25) )
float Atmosphere::CAS(float dp) {
    return (1225.0 * sqrt(5.0 * (pow((dp / 101325.0) + 1.0, (2.0 / 7)) - 1.0)));
}

float Atmosphere::IAS(float tas, float alti, float temp) {
    return (tas / sqrt(1.225 / (Atmosphere::calcPressureISA(alti) * 100.0 / (287.058 * (273.15 + temp)))));
}

float Atmosphere::pascal2kmh(float pascal) {
    return sqrt(2 * pascal / 1.225) * 3.6;
}

float Atmosphere::kmh2pascal(float kmh) {
    return ((kmh / 3.6) * (kmh / 3.6)) * 1.225 / 2.;
}

float Atmosphere::calcAltitude(float SeaLevel_Pres, float pressure) {
    return (44330.0 * (1.0 - pow((double)(pressure) / SeaLevel_Pres, (1.0 / 5.255))));
}

// respect temp gradient 6.5 K / km
float Atmosphere::calcPressure(float seaLevelPressure, float altitude) {
    return (seaLevelPressure * pow((1.0 - ((double)(altitude) / 44330.76923)), 5.255));
}

float Atmosphere::calcQNHPressure(float pressure, float altitude) {
    return pressure / pow((1.0 - (6.5 * (double)(altitude) / 288150.0)), 5.255);
}
