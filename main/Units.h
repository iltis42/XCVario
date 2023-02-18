#ifndef UNITS_H
#define UNITS_H

#include "logdef.h"
#include "Setup.h"
#pragma once

class Units {
public:
	static float Airspeed( float as ){
		if( ias_unit.get() == SPEED_UNIT_KMH ) // km/h
			return( as );
		else if( ias_unit.get() == SPEED_UNIT_MPH ) // mph
			return( as*0.621371 );
		else if( ias_unit.get() == SPEED_UNIT_KNOTS ) // knots
			return( as*0.539957 );
		else
			ESP_LOGE(FNAME,"Wrong unit for AS");
		return 0;
	};

	static float Distance( float d ){
		if( dst_unit.get() == DST_UNIT_KM ) // km/h
			return( d );
		else if( dst_unit.get() == DST_UNIT_FT ) // ft
			return( d*3.28084 );
		else if( dst_unit.get() == DST_UNIT_MILES ) // mi
			return( d*0.621371 );
		else
			ESP_LOGE(FNAME,"Wrong unit for Distance");
		return d;
	};


	static int AirspeedRounded( float as ){
        float ret = 0;
		if( ias_unit.get() == SPEED_UNIT_KMH ) { // km/h
			ret = as; }
		else if( ias_unit.get() == SPEED_UNIT_MPH ) { // mph
			ret = as*0.621371; }
		else if( ias_unit.get() == SPEED_UNIT_KNOTS ) { // knots
			ret = as*0.539957; }
		else {
			ESP_LOGE(FNAME,"Wrong unit for AS"); }
		return (int)roundf(ret);
	};

	static float kmh2knots( float kmh ){
			return( kmh / 1.852 );
	};

	static float kmh2ms( float kmh ){
			return( kmh * 0.277778 );
	};

	static float ms2kmh( float ms ){
			return( ms * 3.6 );
	};

	static float knots2kmh( float knots ){
				return( knots * 1.852 );
	};

	static float Airspeed2Kmh( float as ){
		if( ias_unit.get() == SPEED_UNIT_KMH ) // km/h
			return( as );
		else if( ias_unit.get() == SPEED_UNIT_MPH ) // mph
			return( as/0.621371 );
		else if( ias_unit.get() == SPEED_UNIT_KNOTS ) // knots
			return( as/0.539957 );
		else
			ESP_LOGE(FNAME,"Wrong unit for AS");
		return 0;
	};

    static float ActualWingloadCorrection( float v ) {
        return v * sqrt( 100.0/(ballast.get() + 100.0) );  // ballast is in percent overweight
    }

	static float TemperatureUnit( float t ){
		if( temperature_unit.get() == T_CELCIUS ) // °C
			return( t );
		if( temperature_unit.get() == T_FAHRENHEIT ) // °F
			return( (t * 1.8) + 32 );
		if( temperature_unit.get() == T_KELVIN ) // °K
			return( t +  273.15 );
		else{
			return( t ); // default °C
		}
	}

	static const char* TemperatureUnitStr( int idx = -1 ){
		if( idx == -1 )
			idx = temperature_unit.get();
		if( idx == T_FAHRENHEIT ) // °F
			return "°F";
		if( idx == T_KELVIN ) // °F
			return "°K";
		return "°C"; // default °C
	}

	static const char * AirspeedUnitStr( int u = -1 ){
		if( u == -1 )
			u=ias_unit.get();
		if( u == SPEED_UNIT_KMH ) // km/h
			return("kmh");
		if( u == SPEED_UNIT_MPH ) // mph
			return("mph");
		if( u == SPEED_UNIT_KNOTS ) // knots
			return("kt");
		else
			ESP_LOGE(FNAME,"Wrong unit for airspeed");
		return "none";
	};

	static const char* AirspeedModeStr(){
		if ( airspeed_mode.get() == MODE_IAS ) {
			return "IAS";
		}
		else if ( airspeed_mode.get() == MODE_TAS ) {
			return "TAS";
		}
		else if ( airspeed_mode.get() == MODE_CAS ) {
			return "CAS";
		}
		else if( airspeed_mode.get() == MODE_SLIP ) {
			return "SLIP";
		}
		else{
			return "nan";
		}
	}

	static float Vario( const float te ){   // standard is m/s
		if( vario_unit.get() == VARIO_UNIT_MS )
			return( te );
		else if(  vario_unit.get() == VARIO_UNIT_FPM )
			return( te*1.9685 );
		else if( vario_unit.get() == VARIO_UNIT_KNOTS )
			return( te*1.94384 );         // knots
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};

	static float Qnh( float qnh ){   // standard is hPa
		if( qnh_unit.get() == QNH_HPA )
			return( qnh );
		else if(  qnh_unit.get() == QNH_INHG )
			return( hPa2inHg( qnh ) );
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};

	static int QnhRounded( float qnh ){   // standard is hPa
		float qnh_value = qnh;
		if( qnh_unit.get() == QNH_INHG ) {
			qnh_value = hPa2inHg(qnh);
		}
		return (int)(qnh_value+0.5);
	};

	static float hPa2inHg( float hpa ){   // standard is m/s
		return( hpa * 0.02952998597817832 );
	};
	static float inHg2hPa( float inhg ){   // standard is m/s
		return( inhg / 0.02952998597817832 );
	};

	static float knots2ms( float knots ){   // if we got it in knots
			return( knots/1.94384 );
	};

	static float ms2knots( float knots ){   // if we got it in knots
			return( knots*1.94384 );
	};

	static float ms2mph( float ms ){   // if we got it in knots
				return( ms*2.23694 );
	};

	static float ms2fpm( float ms ){   // if we got it in knots
					return( ms*196.85 );
	};

	static float Vario2ms( float var ){
		if( vario_unit.get() == VARIO_UNIT_MS )
			return( var );
		else if(  vario_unit.get() == VARIO_UNIT_FPM )
			return( var/196.85 );
		else if( vario_unit.get() == VARIO_UNIT_KNOTS )
			return( var/1.94384 );         // knots
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};

	static float mcval2knots( float mc ){   // returns MC, stored according to vario setting, in knots
		if( vario_unit.get() == VARIO_UNIT_MS )             // mc is in m/s
			return( mc*1.94384 );
		else if(  vario_unit.get() == VARIO_UNIT_FPM )       // mc is stored in feet per minute
			return( mc * 0.00987472 );
		else if( vario_unit.get() == VARIO_UNIT_KNOTS )        // knots we already have
			return( mc );
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};


	static const char * VarioUnit(){
		if( vario_unit.get() == VARIO_UNIT_MS )
			return("m/s");
		else if( vario_unit.get() == VARIO_UNIT_FPM )
			return("cf/m");
		else if( vario_unit.get() == VARIO_UNIT_KNOTS )
			return("kt");
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return "nan";
	};

	static const char * QnhUnit( int unit = -1 ){
		if( unit == -1 )
			unit = qnh_unit.get();
		if( unit == QNH_HPA )
			return("hPa");
		else if( unit == QNH_INHG )
			return("inHg");
		else
			ESP_LOGE(FNAME,"Wrong unit for QNH");
		return "nan";
	};


	static const char * VarioUnitLong( int unit = -1 ){
		int u = unit;
		if( u == -1 )
			u=vario_unit.get();
		if( u == VARIO_UNIT_MS )
			return("m/s");
		else if( u == VARIO_UNIT_FPM )
			return("x 100ft/m");
		else if( u == VARIO_UNIT_KNOTS )
			return("knots");
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return "nan";
	};



	static float Altitude( float alt, int unit = -1 ){
		int u = unit;
		if( u == -1 )
			u = alt_unit.get();
		if( u == ALT_UNIT_METER )  //m
			return( alt );
		else if( u == ALT_UNIT_FT ) //feet
			return( alt*3.28084 );
		else if( u == ALT_UNIT_FL ) //FL
			return( alt*0.0328084 );
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return 0;
	};

	static float meters2feet( float m ){
			return( m*3.28084 );
	};

	static float feet2meters( float f ){
			return( f/3.28084 );
	};

	static float meters2FL( float m ){
			return( m*0.0328084 );
	};

	static const char * AltitudeUnit( int unit = -1 ){
		int u=unit;
		if( u == -1 )
			u=alt_unit.get();
		if( u == ALT_UNIT_METER )  //m
			return( "m" );
		else if( u == ALT_UNIT_FT ) //feet
			return( "ft" );
		else if( u == ALT_UNIT_FL ) //FL
			return( "FL" );
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude %d", u );
		return "nan";
	};

	static const char * AltitudeUnitMeterOrFeet( int unit = -1 ){
		int u=unit;
		if( u == -1 )
			u=alt_unit.get();
		if( u == ALT_UNIT_METER )  //m
			return( "m" );
		else if( u == ALT_UNIT_FT || u == ALT_UNIT_FL ) //feet
			return( "ft" );
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude %d", u );
		return "nan";
	};

	static const char * DistanceUnit( int unit = -1 ){
		int u=unit;
		if( u == -1 )
			u=dst_unit.get();
		if( u == DST_UNIT_KM )      // kilometers km
			return( "km" );
		else if( u == DST_UNIT_FT ) // hundreds feet cf
			return( "cf" );
		else if( u == DST_UNIT_MILES ) // Miles mi
			return( "mi" );
		else
			ESP_LOGE(FNAME,"Wrong unit for distance %d", u );
		return "nan";
	};

	static float value( float val, e_unit_type_t u ){
		switch( u ){
		case UNIT_NONE:
			return val;
		case UNIT_TEMPERATURE:
			return TemperatureUnit( val );
		case UNIT_ALT:
			return Altitude( val );
		case UNIT_SPEED:
			return Airspeed( val );
		case UNIT_VARIO:
			return Vario( val );
		case UNIT_QNH:
			return Qnh( val );
		default:
			return val;
		}
	};

	static const char * unit( e_unit_type_t u ){
		switch( u ){
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
	};

};

#endif
