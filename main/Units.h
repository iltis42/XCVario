#ifndef UNITS_H
#define UNITS_H

#include "logdef.h"
#include "Setup.h"

class Units {
public:
	static inline float Airspeed( float as ){
		if( ias_unit.get() == 0 ) // km/h
			return( as );
		else if( ias_unit.get() == 1 ) // mph
			return( as*0.621371 );
		else if( ias_unit.get() == 2 ) // knots
			return( as*0.539957 );
		else
			ESP_LOGE(FNAME,"Wrong unit for AS");
		return 0;
	};

	static inline const char * AirspeedUnit(){
		if( ias_unit.get() == 0 ) // km/h
			return("km/h");
		if( ias_unit.get() == 1 ) // mph
			return("mph");
		if( ias_unit.get() == 2 ) // knots
			return("kt");
		else
			ESP_LOGE(FNAME,"Wrong unit for airspeed");
		return "nan";
	};

	static inline float Vario( float te ){
		if( vario_unit.get() == 0 )
			return( te );
		else if(  vario_unit.get() == 1 ){
			return( te*196.85 );
		}
		else if( vario_unit.get() == 2 )
			return( te*1.94384 );         // knots
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};

	static inline const char * VarioUnit(){
		if( vario_unit.get() == 0 )
			return("m/s");
		else if( vario_unit.get() == 1 )
			return("ft/m");
		else if( vario_unit.get() == 2 )
			return("kt");
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return "nan";
	};

	static inline float Altitude( float alt ){
		if( alt_unit.get() == 0 ) { //m
			return( alt );
		}
		else if( alt_unit.get() == 1 ){ //feet
			return( alt*3.28084 );
		}
		else if( alt_unit.get() == 2 ){ //FL
			return( alt*0.0328084 );
		}
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return 0;
	};

	static inline const char * AltitudeUnit(){
		if( alt_unit.get() == 0 ) { //m
			return( "m" );
		}
		else if( alt_unit.get() == 1 ){ //feet
			return( "ft" );
		}
		else if( alt_unit.get() == 2 ){ //FL
			return( "FL" );
		}
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return "nan";
	};

};

#endif
