#ifndef UNITS_H
#define UNITS_H

#include "logdef.h"
#include "Setup.h"

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
        return v * sqrt( 100.0/( ballast.get() + 100.0) );
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

	static const char* TemperatureUnitStr( int idx ){
		if( idx == T_FAHRENHEIT ) // °F
			return "\xb0""F";
		if( idx == T_KELVIN ) // °F
			return "\xb0""K";
		return "\xb0""C"; // default °C
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
		} else {
			return "TAS";
		}
	}

	static float Vario( float te ){   // standard is m/s
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

	static float Qnh( float qnh ){   // standard is m/s
		if( qnh_unit.get() == QNH_HPA )
			return( qnh );
		else if(  qnh_unit.get() == QNH_INHG )
			return( inHg2hPa( qnh ) );
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};

	static float QnhRaw( float qnh ){   // standard is hPa
		if( qnh_unit.get() == QNH_HPA )
			return( qnh );
		else if(  qnh_unit.get() == QNH_INHG )
			return( hPa2inHg( qnh ) );
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};

	static void recalculateQnh(){
		ESP_LOGI(FNAME,"recalculateQnh");
		if( qnh_unit.get() == QNH_HPA ){
			if( QNH.get() < 500 ){  // convert to hPa
				QNH.set( inHg2hPa( QNH.get() ));
				ESP_LOGI(FNAME,"Converted QNH to hPa %.2f", QNH.get() );
			}
		}
		else if( qnh_unit.get() == QNH_INHG ){
			if( QNH.get() > 500 ){ // convert to inHG
				QNH.set( hPa2inHg( QNH.get() ));
				ESP_LOGI(FNAME,"Converted QNH to inHg %.2f", QNH.get() );
			}
		}
	}

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
			return("cft/m");
		else if( vario_unit.get() == VARIO_UNIT_KNOTS )
			return("kt");
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return "nan";
	};

	static const char * QnhUnit( int unit ){
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
		if( u == 0 )
			return("m/s");
		else if( u == 1 )
			return("x 100ft/m");
		else if( u == 2 )
			return("knots");
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return "nan";
	};

	static float Altitude( float alt ){
		if( alt_unit.get() == 0 )  //m
			return( alt );
		else if( alt_unit.get() == 1 ) //feet
			return( alt*3.28084 );
		else if( alt_unit.get() == 2 ) //FL
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

	static const char * AltitudeUnit(){
		if( alt_unit.get() == 0 )  //m
			return( "m" );
		else if( alt_unit.get() == 1 ) //feet
			return( "ft" );
		else if( alt_unit.get() == 2 ) //FL
			return( "FL" );
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return "nan";
	};

};

#endif
