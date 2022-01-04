/**
  *
  * UbloxGNSSdecode.h 
  * header for decoding UBX messages from Ublox GPS
  *
  */

#ifndef UbloxGNSSdecode_H_
#define UbloxGNSSdecode_H_

#include "SString.h"

struct GNSS_DATA_T {
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
  bool fix;
};

const struct GNSS_DATA_T getGNSSData();

bool processGNSS( SString& frame );

#endif /* UbloxGNSSdecode_H_ */
