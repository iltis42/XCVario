#pragma once

/*
 * Methods for Athmosphere Model used in Aviation
 *
 *
 */

namespace Atmosphere {

float TAS(float ias, float baro, float temp);
float TAS2(float ias, float altitude, float temp);
float CAS(float dp);
float IAS(float tas, float alti, float temp);
float pascal2kmh(float pascal);
float kmh2pascal(float kmh);
float calcAltitude(float seaLevelPressure, float pressure);
inline float calcAltitudeISA(float pressure) { return calcAltitude(1013.25f, pressure); }
float calcPressure(float seaLevelPressure, float alti);
inline float calcPressureISA(float alti) { return calcPressure(1013.25f, alti); }
float calcQNHPressure(float pressure, float altitude);

}; // namespace Atmosphere
