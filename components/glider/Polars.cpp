/*
 * Polars.cpp
 *
 *  Created on: Mar 1, 2019
 *      Author: iltis
 *
 *      Class to deliver a library of default polars
 */

#include "glider/Polars.h"

#include <esp_log.h>

#include <cinttypes>
#include <string_view>

#define FNAME "Polars"

// Format per glider:  { Index, GliderType, Reference Wingload (kg/m2), speed1 (km/h), sink1 (m/s), speed2, sink2, speed3, sink3 , max ballast [liters or kg], wing area [m2] },

static int my_polar = 0;

struct compressed_polar {
	uint16_t index;
	std::string_view type;	// units
	uint16_t wingload;		// 1/100x kg/mxm
	uint8_t  speed1;		// km/h
	int16_t  sink1;			// 1/100x m/s
	uint8_t  speed2;		// km/h
	int16_t  sink2;			// 1/100x m/s
	uint8_t  speed3;		// km/h
	int16_t  sink3;			// 1/100x m/s
	uint8_t  max_ballast;	// 2x kg
	uint16_t wingarea;		// 1/100x mxm
	uint8_t  flags;                 // bit 0 = 0x01 -> has flaps
};


constexpr const compressed_polar polars_default_arr[] = {
#include <PackedPolarTable.inc>
};


t_polar::t_polar(const compressed_polar *cp)
{
	index = cp->index;
	type = cp->type.data();
	wingload = (float)(cp->wingload) / 100.f;
	speed1 = (float)(cp->speed1);
	sink1 = (float)(cp->sink1) / 100.f;
	speed2 = (float)(cp->speed2);
	sink2 = (float)(cp->sink2) / 100.f;
	speed3 = (float)(cp->speed3);
	sink3 = (float)(cp->sink3) / 100.f;
	max_ballast = (float)(cp->max_ballast) * 2.f;
	wingarea = (float)(cp->wingarea) / 100.f;
	flags = (int)(cp->flags);
}


const t_polar Polars::getPolar() {
	return t_polar(&polars_default_arr[my_polar]);
}

int Polars::numPolars() {
	return(  sizeof( polars_default_arr ) / sizeof(compressed_polar) );
}

const char *Polars::getPolarName(int i)
{
    return polars_default_arr[i].type.data();
}

int Polars::getPolarIndex(int i)
{
    return polars_default_arr[i].index;
}

void Polars::extract(int glider_index)
{
	ESP_LOGI( FNAME,"Polars::begin() configured glider index: %d", glider_index);
	for( int p=0; p<numPolars(); p++ ){
		if( polars_default_arr[p].index == glider_index ){
			ESP_LOGI( FNAME,"Found Glider index %d at position %d", glider_index, p );
			my_polar = p;
			break;
		}
	}
}

const char *Polars::getGliderType(int i)
{
    return polars_default_arr[i].type.data();
}

int Polars::getGliderEnumPos()
{
    return my_polar;
}

bool Polars::hasFlaps(int i){
    return polars_default_arr[i].flags & 0x01;
}

