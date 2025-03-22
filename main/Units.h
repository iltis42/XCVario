#pragma once

#include "SetupNG.h"

class Units
{
public:
	static float Airspeed(float as);
	static float Distance(float d);
	static int AirspeedRounded(float as);
	static float kmh2knots(float kmh);
	static float kmh2ms(float kmh);
	static float ms2kmh(float ms);
	static float knots2kmh(float knots);
	static float Airspeed2Kmh(float as);
	static float ActualWingloadCorrection(float v);
	static float TemperatureUnit(float t);
	static const char* TemperatureUnitStr(int idx = -1);
	static const char* AirspeedUnitStr(int u = -1);
	static const char* AirspeedModeStr();
	static float Vario(const float te);
	static float Qnh(float qnh);
	static int QnhRounded(float qnh);
	static float hPa2inHg(float hpa);
	static float inHg2hPa(float inhg);
	static float knots2ms(float knots);
	static float ms2knots(float knots);
	static float ms2mph(float ms);
	static float ms2fpm(float ms);
	static float Vario2ms(float var);
	static float mcval2knots(float mc);
	static const char *VarioUnit();
	static const char *QnhUnit(int unit = -1);
	static const char *VarioUnitLong(int unit = -1);
	static float Altitude(float alt, int unit = -1);
	static float meters2feet(float m);
	static float feet2meters(float f);
	static float meters2FL(float m);
	static const char *AltitudeUnit(int unit = -1);
	static const char *AltitudeUnitMeterOrFeet(int unit = -1);
	static const char *DistanceUnit(int unit = -1);
	static float value(float val, e_unit_type_t u);
	static const char *unit(e_unit_type_t u);
};
