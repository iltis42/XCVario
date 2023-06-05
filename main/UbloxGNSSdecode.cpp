/**
 *
 * UbloxGNSSdecode.cpp
 * This software parses a data stream to extract UBX NAV-PVT frame information. Key informationtion can be accessed with getGNSSData() using gnss_data_t type.
 * This software also captures NMEA sentences and return them through the exchange buffer so that they can be further processed by the calling function.
 * Aurelien & Jean-Luc Derouineau 2022
 *
 */

#include <stdint.h>
#include <endian.h>
#include <string.h>
#include <esp_log.h>
#include <logdef.h>
#include "UbloxGNSSdecode.h"

gnss_data_t UbloxGnssDecoder::GNSS_DATA[3];

void UbloxGnssDecoder::processNavPvtFrame( char *ubx_frame, int port ) {
	nav_pvt_payload_t *payload = (nav_pvt_payload_t *)ubx_frame;
	// nav fix must be 3D or 3D diff to accept new data
	GNSS_DATA[port].fix = payload->fixType;
	if ( payload->fixType != 3 && payload->fixType != 4 ) return;

	GNSS_DATA[port].coordinates.latitude = payload->lat * 1e-7f;
	GNSS_DATA[port].coordinates.longitude = payload->lon * 1e-7f;
	GNSS_DATA[port].coordinates.altitude = payload->hMSL * 0.001f;
	GNSS_DATA[port].speed.ground = payload->gSpeed * 0.001f;
	GNSS_DATA[port].speed.x = payload->velN * 0.001f;
	GNSS_DATA[port].speed.y = payload->velE * 0.001f;
	GNSS_DATA[port].speed.z = payload->velD * 0.001f;
	GNSS_DATA[port].route = payload->heading * 1e-5f;
	GNSS_DATA[port].date = payload->day;
	GNSS_DATA[port].time = payload->iTOW * 0.001f;
 	GNSS_DATA[port].numSV = payload->numSV;		
	ESP_LOGI(FNAME,"GPS epoch: %d, lat:%f, lon:%f ", payload->iTOW, GNSS_DATA[port].coordinates.latitude, GNSS_DATA[port].coordinates.longitude );
}

