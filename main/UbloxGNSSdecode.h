/**
  *
  * UbloxGNSSdecode.h 
  * header for decoding UBX messages from Ublox GPS
  *
  */

#ifndef UbloxGNSSdecode_H_
#define UbloxGNSSdecode_H_

#include "SString.h"
#include "types.h"

typedef struct _gnss_data {
	struct {
		float latitude;
		float longitude;
		float altitude;
	} coordinates;
	struct {
		float ground;
		float x;
		float y;
		float z;
	} speed;
	int date;
	float time;
	int fix;
	int numSV;
} gnss_data_t;


// NAV_PVT structure definition
typedef struct _nav_pvt_payload { // offset in UBX frame:
	uint32_t iTOW;         // 00: GPS time of week of the navigation epoch (ms)
	uint16_t year;         // 04: Year (UTC)
	uint8_t month;         // 06: Month, range 1..12 (UTC)
	uint8_t day;           // 07: Day of month, range 1..31 (UTC)
	uint8_t hour;          // 08: Hour of day, range 0..23 (UTC)
	uint8_t minute;        // 09: Minute of hour, range 0..59 (UTC)
	uint8_t second;        // 10: Seconds of minute, range 0..60 (UTC)
	uint8_t valid;         // 11: Validity Flags (see graphic below)
	uint32_t tAcc;         // 12: Time accuracy estimate (UTC) (ns)
	int32_t nano;          // 16: Fraction of second, range -1e9 .. 1e9 (UTC) (ns)
	uint8_t fixType;       // 20: GNSSfix Type, range 0..5
	uint8_t flags;         // 21: Fix Status Flags
	uint8_t flags2;        // 22: Additional flags
	uint8_t numSV;         // 23: Number of satellites used in Nav Solution
	int32_t lon;           // 24: Longitude (deg)
	int32_t lat;           // 28: Latitude (deg)
	int32_t height;        // 32: Height above Ellipsoid (mm)
	int32_t hMSL;          // 36: Height above mean sea level (mm)
	uint32_t hAcc;         // 40: Horizontal Accuracy Estimate (mm)
	uint32_t vAcc;         // 44: Vertical Accuracy Estimate (mm)
	int32_t velN;          // 48: NED north velocity (mm/s)
	int32_t velE;          // 52: NED east velocity (mm/s)
	int32_t velD;          // 56: NED down velocity (mm/s)
	int32_t gSpeed;        // 60: Ground Speed (2-D) (mm/s)
	int32_t heading;       // 64: Heading of motion 2-D (deg)
	uint32_t sAcc;         // 68: Speed Accuracy Estimate
	uint32_t headingAcc;   // 72: Heading Accuracy Estimate
	uint16_t pDOP;         // 76: Position dilution of precision
	uint16_t flags3;       // 78: Additional flags
	uint32_t reserved0;    // 80: Reserved
	int32_t headVeh;       // 84: Heading of vehicue 2-D
	int16_t magDec; 	   // 88: Magnetic declination
	uint16_t magAcc;       // 90: Magnetic declination accuracy
} nav_pvt_payload_t;

#define PVT_PAYLOAD_SIZE  sizeof(nav_pvt_payload_t)

class UbloxGnssDecoder {
public:
	static void processNavPvtFrame( char *ubx_frame, int port );
	static const gnss_data_t *getGNSSData( int port ) { return &GNSS_DATA[port]; };

private:
	static gnss_data_t GNSS_DATA[3];   // port maybe 0,1,2  S1 = 1, S2 = 2,  USB = 0
};


#endif /* UbloxGNSSdecode_H_ */
