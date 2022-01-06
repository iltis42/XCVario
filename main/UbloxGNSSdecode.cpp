/**
  *
  * UbloxGNSSdecode.cpp 
  * This software parses a data stream to extract UBX NAV-PVT frame information. Key informationtion can be accessed with getGNSSData() using gnss_data_t type.
  * This software also captures NMEA sentences and return them through the exchange buffer so that they can be further processed by the calling function.
  * This feature allows to mix UBX and NMEA sentences on both S1 and S2 ports
  * Aurelien & Jean-Luc Derouineau 2022
  *
  */

#include <stdint.h>
#include <endian.h>
#include <string.h>

#include <esp_log.h>
#include <logdef.h>

#include "UbloxGNSSdecode.h"

// UBX SYNC
const uint8_t UBX_SYNC1 = 0xb5;
const uint8_t UBX_SYNC2 = 0x62;
// UBX NAV PVT CLASS/ID
const uint8_t UBX_CLASS = 0x01;
const uint8_t UBX_ID = 0x07;
// NMEA SYNC
const uint8_t NMEA_START1 = '$';
const uint8_t NMEA_START2 = '!';
// NMEA stream
const uint8_t NMEA_MIN = 0x20;
const uint8_t NMEA_MAX = 0x7e;
// NMEA End of Line
const uint8_t NMEA_CR = '\r';
const uint8_t NMEA_LF = '\n';

// NAV_PVT structure definition
struct nav_pvt_payload_t { // offset in UBX frame:
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
	int16_t magDec; 		   // 88: Magnetic declination
	uint16_t magAcc;       // 90: Magnetic declination accuracy
};

const size_t PVT_PAYLOAD_SIZE = sizeof(struct nav_pvt_payload_t);

// state machine definition 
enum state_t {
	GET_NMEA_UBX_SYNC,

	GET_NMEA_STREAM,
	GOT_NMEA_FRAME,

	GET_UBX_SYNC2,
	GET_UBX_CLASS,
	GET_UBX_ID,
	GET_UBX_LENGTH1,
	GET_UBX_LENGTH2,
	GET_UBX_PAYLOAD,
	GET_UBX_CHKA,
	GET_UBX_CHKB
};

// UBX NMEA class decoder declaration
class UbloxGnssDecoder::Impl {
	friend class UbloxGnssDecoder;

	const uint8_t portId;

	char nmeaBuf[128]; // NMEA standard max size is 82 byte including termination cr lf but provision is make to allow 128 byte in case of proprietary sentences.
	char chkA = 0; // checksum variables
	char chkB = 0;
	char chkBuf = 0;
	uint8_t pos = 0; // pointer for NMEA or UBX payload
	struct nav_pvt_payload_t payload;
	enum state_t state = GET_NMEA_UBX_SYNC;
	struct gnss_data_t GNSS_DATA{};

	// process all byte from frame (buffer) method declaration
	bool processGNSS(SString& frame);
	// state machine to parse NMEA and UBX sentences method declaration
	void parse_NMEA_UBX(const char c);
	// allocate NAV PVT data to GNSS_DATA method declaration
	void processNavPvtFrame();
	// checksum method declaration
	void addChk(const char c);

	Impl(const uint8_t portId);
};

UbloxGnssDecoder::Impl::Impl(const uint8_t portId) : portId(portId) {
}

bool UbloxGnssDecoder::Impl::processGNSS(SString& frame) {
	const char* cf = frame.c_str();
	// process every frame byte through state machine
	for (size_t i = 0; i < frame.length(); i++) {
		this->parse_NMEA_UBX(cf[i]);
		// if NMEA sentence send it back to Router through frame
		if (this->state == GOT_NMEA_FRAME) {
		frame.set(this->nmeaBuf, pos);
		this->state = GET_NMEA_UBX_SYNC;
		return true;
		}
  	}
	return false;
}

void UbloxGnssDecoder::Impl::parse_NMEA_UBX(const char c) {
	switch(this->state) {
	case GET_NMEA_UBX_SYNC:
		switch(c) {
		case NMEA_START1:
		case NMEA_START2:
			this->pos = 0;
			this->nmeaBuf[pos] = c;
			this->pos++;
			this->state = GET_NMEA_STREAM;
			break;
		case UBX_SYNC1:
			this->chkA = 0;
			this->chkB = 0;
			this->pos = 0;
			this->state = GET_UBX_SYNC2;
			break;
		}
		break;

	case GET_NMEA_STREAM:
		if ((c < NMEA_MIN || c > NMEA_MAX) && (c != NMEA_CR && c != NMEA_LF)) {
		ESP_LOGE(FNAME, "Port S%1d: Invalid NMEA character", this->portId);
		this->state = GET_NMEA_UBX_SYNC;
		break;
		}
		if (pos == sizeof(nmeaBuf) - 1) {
		ESP_LOGE(FNAME, "Port S%1d NMEA buffer not large enough", this->portId);
		this->state = GET_NMEA_UBX_SYNC;
		}
		this->nmeaBuf[pos] = c;
		this->pos++;
		if (c == NMEA_LF) {
		this->nmeaBuf[pos] = 0;
		this->state = GOT_NMEA_FRAME;
		}
		break;

	case GOT_NMEA_FRAME:
		break;

	case GET_UBX_SYNC2:
		if (c != UBX_SYNC2) {
		this->state = GET_NMEA_UBX_SYNC;
		break;
		}
		this->state = GET_UBX_CLASS;
		break;

	case GET_UBX_CLASS:
		if (c != UBX_CLASS) {
		ESP_LOGW(FNAME, "Port S%1d Unexpected UBX class", this->portId);
		this->state = GET_NMEA_UBX_SYNC;
		break;
		}
		this->addChk(c);
		this->state = GET_UBX_ID;
		break;

	case GET_UBX_ID:
		if (c != UBX_ID) {
		ESP_LOGW(FNAME, "Port S%1d Unexpected UBX ID", this->portId);
		this->state = GET_NMEA_UBX_SYNC;
		break;
		}
		this->addChk(c);
		this->state = GET_UBX_LENGTH1;
		break;

	case GET_UBX_LENGTH1:
		this->addChk(c);
		this->state = GET_UBX_LENGTH2;
		break;

	case GET_UBX_LENGTH2:
		this->addChk(c);
		this->state = GET_UBX_PAYLOAD;
		break;

	case GET_UBX_PAYLOAD:
		this->addChk(c);
		((char*)(&this->payload))[this->pos] = c;
		if (this->pos == PVT_PAYLOAD_SIZE - 1) {
		this->state = GET_UBX_CHKA;
		}
		this->pos++;
		break;

	case GET_UBX_CHKA:
		this->chkBuf = c;
		this->state = GET_UBX_CHKB;
		break;

	case GET_UBX_CHKB:
		if (this->chkBuf == this->chkA && c == this->chkB) {
		this->processNavPvtFrame();
		} else {
		ESP_LOGE(FNAME, "Port S%1d: Checksum does not match", this->portId);
		}
		this->state = GET_NMEA_UBX_SYNC;
		break;
	}
}

void UbloxGnssDecoder::Impl::processNavPvtFrame() {
	// nav fix must be 3D or 3D diff to accept new data
	this->GNSS_DATA.fix = this->payload.fixType;
	if ( this->payload.fixType != 3 && this->payload.fixType != 4 ) return;

	this->GNSS_DATA.coordinates.latitude = this->payload.lat * 1e-7f;
	this->GNSS_DATA.coordinates.longitude = this->payload.lon * 1e-7f;
	this->GNSS_DATA.coordinates.altitude = this->payload.hMSL * 0.001f;
	this->GNSS_DATA.speed.ground = this->payload.gSpeed * 0.001f;
	this->GNSS_DATA.speed.x = this->payload.velN * 0.001f;
	this->GNSS_DATA.speed.y = this->payload.velE * 0.001f;
	this->GNSS_DATA.speed.z = this->payload.velD * 0.001f;
	this->GNSS_DATA.date = this->payload.day;
	this->GNSS_DATA.time = this->payload.iTOW * 0.001f;
}

void UbloxGnssDecoder::Impl::addChk(const char c) {
	this->chkA += c;
	this->chkB += chkA;
}

UbloxGnssDecoder::UbloxGnssDecoder(const uint8_t portId) : pImpl(new UbloxGnssDecoder::Impl(portId)) {
}

UbloxGnssDecoder::~UbloxGnssDecoder() {
}

bool UbloxGnssDecoder::process(SString& frame) {
	return this->pImpl->processGNSS(frame);
}

const struct gnss_data_t UbloxGnssDecoder::getGNSSData() const {
	return this->pImpl->GNSS_DATA;
}
