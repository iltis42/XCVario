
#include "Units.h"
#include "logdefnone.h"

float Units::Airspeed(float as)
{
	if (ias_unit.get() == SPEED_UNIT_KMH)
	{ // km/h
		return (as);
	}
	else if (ias_unit.get() == SPEED_UNIT_MPH)
	{ // mph
		return (as * 0.621371);
	}
	else if (ias_unit.get() == SPEED_UNIT_KNOTS)
	{ // knots
		return (as * 0.539957);
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for AS");
	}
	return 0;
}

float Units::Distance(float d)
{ //
	if (dst_unit.get() == DST_UNIT_M)
	{ // meters per default
		return (d);
	}
	else if (dst_unit.get() == DST_UNIT_FT)
	{ // ft
		return (d * 3.28084);
	}
	else if (dst_unit.get() == DST_UNIT_MILES)
	{ // mi
		return (d * 0.000621371);
	}
	else if (dst_unit.get() == DST_UNIT_NAUTICAL_MILES)
	{ // sm
		return (d * 0.000539957);
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for Distance");
	}
	return d;
}

int Units::AirspeedRounded(float as)
{
	float ret = 0;
	if (ias_unit.get() == SPEED_UNIT_KMH)
	{ // km/h
		ret = as;
	}
	else if (ias_unit.get() == SPEED_UNIT_MPH)
	{ // mph
		ret = as * 0.621371;
	}
	else if (ias_unit.get() == SPEED_UNIT_KNOTS)
	{ // knots
		ret = as * 0.539957;
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for AS");
	}
	return (int)roundf(ret);
}

float Units::kmh2knots(float kmh)
{
	return (kmh / 1.852);
}

float Units::kmh2ms(float kmh)
{
	return (kmh * 0.277778);
}

float Units::ms2kmh(float ms)
{
	return (ms * 3.6);
}

float Units::knots2kmh(float knots)
{
	return (knots * 1.852);
}

float Units::Airspeed2Kmh(float as)
{
	if (ias_unit.get() == SPEED_UNIT_KMH)
	{ // km/h
		return (as);
	}
	else if (ias_unit.get() == SPEED_UNIT_MPH)
	{ // mph
		return (as / 0.621371);
	}
	else if (ias_unit.get() == SPEED_UNIT_KNOTS)
	{ // knots
		return (as / 0.539957);
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for AS");
	}
	return 0;
}

float Units::ActualWingloadCorrection(float v)
{
	return v * sqrt(100.0 / (ballast.get() + 100.0)); // ballast is in percent overweight
}

float Units::TemperatureUnit(float t)
{
	if (temperature_unit.get() == T_CELCIUS)
	{ // °C
		return (t);
	}
	else if (temperature_unit.get() == T_FAHRENHEIT)
	{ // °F
		return ((t * 1.8) + 32);
	}
	else if (temperature_unit.get() == T_KELVIN)
	{ // °K
		return (t + 273.15);
	}
	else
	{
		return (t); // default °C
	}
}

const char* Units::TemperatureUnitStr(int idx)
{
	if (idx == -1)
	{
		idx = temperature_unit.get();
	}
	if (idx == T_FAHRENHEIT)
	{ // °F
		return "°F";
	}
	else if (idx == T_KELVIN)
	{ // °F
		return "°K";
	}
	return "°C"; // default °C
}

const char* Units::AirspeedUnitStr(int u)
{
	if (u == -1)
	{
		u = ias_unit.get();
	}
	if (u == SPEED_UNIT_KMH)
	{ // km/h
		return ("kmh");
	}
	else if (u == SPEED_UNIT_MPH)
	{ // mph
		return ("mph");
	}
	else if (u == SPEED_UNIT_KNOTS)
	{ // knots
		return ("kt");
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for airspeed");
	}
	return "none";
}

const char* Units::AirspeedModeStr()
{
	if (airspeed_mode.get() == MODE_IAS)
	{
		return "IAS";
	}
	else if (airspeed_mode.get() == MODE_TAS)
	{
		return "TAS";
	}
	else if (airspeed_mode.get() == MODE_CAS)
	{
		return "CAS";
	}
	else if (airspeed_mode.get() == MODE_SLIP)
	{
		return "SLIP";
	}
	else
	{
		return "nan";
	}
}

float Units::Vario(const float te)
{ // standard is m/s
	if (vario_unit.get() == VARIO_UNIT_MS)
	{
		return (te);
	}
	else if (vario_unit.get() == VARIO_UNIT_FPM)
	{
		return (te * 1.9685);
	}
	else if (vario_unit.get() == VARIO_UNIT_KNOTS)
	{
		return (te * 1.94384); // knots
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for Vario");
	}
	return 0;
}

float Units::Qnh(float qnh)
{ // standard is hPa
	if (qnh_unit.get() == QNH_HPA)
	{
		return (qnh);
	}
	else if (qnh_unit.get() == QNH_INHG)
	{
		return (hPa2inHg(qnh));
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for Vario");
	}
	return 0;
}

int Units::QnhRounded(float qnh)
{ // standard is hPa
	float qnh_value = qnh;
	if (qnh_unit.get() == QNH_INHG)
	{
		qnh_value = hPa2inHg(qnh);
	}
	return (int)(qnh_value + 0.5);
}

float Units::hPa2inHg(float hpa)
{ // standard is m/s
	return (hpa * 0.02952998597817832);
}
float Units::inHg2hPa(float inhg)
{ // standard is m/s
	return (inhg / 0.02952998597817832);
}

float Units::knots2ms(float knots)
{ // if we got it in knots
	return (knots / 1.94384);
}

float Units::ms2knots(float knots)
{ // if we got it in knots
	return (knots * 1.94384);
}

float Units::ms2mph(float ms)
{ // if we got it in knots
	return (ms * 2.23694);
}

float Units::ms2fpm(float ms)
{ // if we got it in knots
	return (ms * 196.85);
}

float Units::Vario2ms(float var)
{
	if (vario_unit.get() == VARIO_UNIT_MS)
	{
		return (var);
	}
	else if (vario_unit.get() == VARIO_UNIT_FPM)
	{
		return (var / 196.85);
	}
	else if (vario_unit.get() == VARIO_UNIT_KNOTS)
	{
		return (var / 1.94384); // knots
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for Vario");
	}
	return 0;
}

float Units::mcval2knots(float mc)
{ // returns MC, stored according to vario setting, in knots
	if (vario_unit.get() == VARIO_UNIT_MS)
	{ // mc is in m/s
		return (mc * 1.94384);
	}
	else if (vario_unit.get() == VARIO_UNIT_FPM)
	{ // mc is stored in feet per minute
		return (mc * 0.00987472);
	}
	else if (vario_unit.get() == VARIO_UNIT_KNOTS)
	{ // knots we already have
		return (mc);
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for Vario");
	}
	return 0;
}

const char* Units::VarioUnit()
{
	if (vario_unit.get() == VARIO_UNIT_MS)
	{
		return ("m/s");
	}
	else if (vario_unit.get() == VARIO_UNIT_FPM)
	{
		return ("ft/m");
	}
	else if (vario_unit.get() == VARIO_UNIT_KNOTS)
	{
		return ("kt");
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for altitude");
	}
	return "nan";
}

const char* Units::QnhUnit(int unit)
{
	if (unit == -1)
	{
		unit = qnh_unit.get();
	}
	if (unit == QNH_HPA)
	{
		return ("hPa");
	}
	else if (unit == QNH_INHG)
	{
		return ("inHg");
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for QNH");
	}
	return "nan";
}

const char* Units::VarioUnitLong(int unit)
{
	int u = unit;
	if (u == -1)
	{
		u = vario_unit.get();
	}
	if (u == VARIO_UNIT_MS)
	{
		return ("m/s");
	}
	else if (u == VARIO_UNIT_FPM)
	{
		return ("x 100ft/m");
	}
	else if (u == VARIO_UNIT_KNOTS)
	{
		return ("knots");
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for altitude");
	}
	return "nan";
}

float Units::Altitude(float alt, int unit)
{
	int u = unit;
	if (u == -1)
	{
		u = alt_unit.get();
	}
	if (u == ALT_UNIT_METER)
	{ // m
		return (alt);
	}
	else if (u == ALT_UNIT_FT)
	{ // feet
		return (alt * 3.28084);
	}
	else if (u == ALT_UNIT_FL)
	{ // FL
		return (alt * 0.0328084);
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for altitude");
	}
	return 0;
}

float Units::meters2feet(float m)
{
	return (m * 3.28084);
}

float Units::feet2meters(float f)
{
	return (f / 3.28084);
}

float Units::meters2FL(float m)
{
	return (m * 0.0328084);
}

const char* Units::AltitudeUnit(int unit)
{
	int u = unit;
	if (u == -1)
	{
		u = alt_unit.get();
	}
	if (u == ALT_UNIT_METER)
	{ // m
		return ("m");
	}
	else if (u == ALT_UNIT_FT)
	{ // feet
		return ("ft");
	}
	else if (u == ALT_UNIT_FL)
	{ // FL
		return ("FL");
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for altitude %d", u);
	}
	return "nan";
}

const char* Units::AltitudeUnitMeterOrFeet(int unit)
{
	int u = unit;
	if (u == -1)
	{
		u = alt_unit.get();
	}
	if (u == ALT_UNIT_METER)
	{ // m
		return ("m");
	}
	else if (u == ALT_UNIT_FT || u == ALT_UNIT_FL)
	{ // feet
		return ("ft");
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for altitude %d", u);
	}
	return "nan";
}

const char* Units::DistanceUnit(int unit)
{
	int u = unit;
	if (u == -1)
	{
		u = dst_unit.get();
	}
	if (u == DST_UNIT_M)
	{ // kilometers km
		return ("m");
	}
	else if (u == DST_UNIT_FT)
	{ // hundreds feet
		return ("ft");
	}
	else if (u == DST_UNIT_MILES)
	{ // Miles mi
		return ("mi");
	}
	else if (u == DST_UNIT_NAUTICAL_MILES)
	{ // nautical miles
		return ("nm");
	}
	else
	{
		ESP_LOGE(FNAME, "Wrong unit for distance %d", u);
	}
	return "nan";
}

float Units::value(float val, e_unit_type_t u)
{
	switch (u)
	{
	case UNIT_NONE:
		return val;
	case UNIT_TEMPERATURE:
		return TemperatureUnit(val);
	case UNIT_ALT:
		return Altitude(val);
	case UNIT_SPEED:
		return Airspeed(val);
	case UNIT_VARIO:
		return Vario(val);
	case UNIT_QNH:
		return Qnh(val);
	default:
		return val;
	}
}

const char* Units::unit(e_unit_type_t u)
{
	switch (u)
	{
	case UNIT_NONE:
		return "";
	case UNIT_TEMPERATURE:
		return TemperatureUnitStr();
	case UNIT_ALT:
		return AltitudeUnit();
	case UNIT_SPEED:
		return AirspeedUnitStr();
	case UNIT_VARIO:
		return VarioUnit();
	case UNIT_QNH:
		return QnhUnit();
	default:
		return "";
	}
}
