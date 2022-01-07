/**
  *
  * UbloxGNSSdecode.h 
  * header for decoding UBX messages from Ublox GPS
  *
  */

#ifndef UbloxGNSSdecode_H_
#define UbloxGNSSdecode_H_

#include <memory>

#include "SString.h"

struct gnss_data_t {
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
};

class UbloxGnssDecoder {
public:
  UbloxGnssDecoder(const uint8_t portId);
  ~UbloxGnssDecoder();

  bool process(SString& frame);
  const gnss_data_t getGNSSData() const;

private:
  class Impl;
  const std::unique_ptr<Impl> pImpl;
};

#endif /* UbloxGNSSdecode_H_ */
