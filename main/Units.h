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

	static inline float kmh2knots( float kmh ){
			return( kmh*0.539957 );
	};

	static inline float Airspeed2Kmh( float as ){
		if( ias_unit.get() == 0 ) // km/h
			return( as );
		else if( ias_unit.get() == 1 ) // mph
			return( as/0.621371 );
		else if( ias_unit.get() == 2 ) // knots
			return( as/0.539957 );
		else
			ESP_LOGE(FNAME,"Wrong unit for AS");
		return 0;
	};

	static inline const char * AirspeedUnit( int unit = -1 ){
		int u = unit;
		if( u == -1 )
			u=ias_unit.get();
		if( u == 0 ) // km/h
			return("kmh");
		if( u == 1 ) // mph
			return("mph");
		if( u == 2 ) // knots
			return("kt");
		else
			ESP_LOGE(FNAME,"Wrong unit for airspeed");
		return "nan";
	};

	static inline float Vario( float te ){   // standard is m/s
		if( vario_unit.get() == 0 )
			return( te );
		else if(  vario_unit.get() == 1 )
			return( te*196.85 );
		else if( vario_unit.get() == 2 )
			return( te*1.94384 );         // knots
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};

	static inline float knots2ms( float knots ){   // if we got it in knots
			return( knots/1.94384 );
	};

	static inline float ms2knots( float knots ){   // if we got it in knots
			return( knots*1.94384 );
	};

	static inline float Vario2ms( float var ){
		if( vario_unit.get() == 0 )
			return( var );
		else if(  vario_unit.get() == 1 )
			return( var/196.85 );
		else if( vario_unit.get() == 2 )
			return( var/1.94384 );         // knots
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};

	static inline float mcval2knots( float mc ){   // returns MC, stored according to vario setting, in knots
		if( vario_unit.get() == 0 )             // mc is in m/s
			return( mc*1.94384 );
		else if(  vario_unit.get() == 1 )       // mc is stored in feet per minute
			return( mc * 0.00987472 );
		else if( vario_unit.get() == 2 )        // knots we already have
			return( mc );
		else
			ESP_LOGE(FNAME,"Wrong unit for Vario");
		return 0;
	};


	static inline const char * VarioUnit(){
		if( vario_unit.get() == 0 )
			return("m/s");
		else if( vario_unit.get() == 1 )
			return("cft/m");
		else if( vario_unit.get() == 2 )
			return("kt");
		else
			ESP_LOGE(FNAME,"Wrong unit for altitude");
		return "nan";
	};
	static inline const char * VarioUnitLong( int unit = -1 ){
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

	static inline float Altitude( float alt ){
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

	static inline float meters2feet( float m ){
			return( m*3.28084 );
	};

	static inline const char * AltitudeUnit(){
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
