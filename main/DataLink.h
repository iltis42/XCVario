#pragma once

#include "protocols/ProtocolItf.h"
#include "types.h"

class SString;

typedef void (*router_t)(SString &s2);

/**
 *
 * DataLink.cpp
 * This software parses a data stream to extract UBX and NMEA frame information from unspecified data packet
 * Part derived from combined implementation thanks to
 * Aurelien & Jean-Luc Derouineau 2022
 *
 */

// state machine definition
enum state_t {
	GET_NMEA_UBX_SYNC,
	GET_NMEA_STREAM,
	GET_UBX_SYNC2,
	GET_UBX_CLASS,
	GET_UBX_ID,
	GET_UBX_LENGTH1,
	GET_UBX_LENGTH2,
	GET_UBX_PAYLOAD,
	GET_UBX_CHKA,
	GET_UBX_CHKB,
	GET_FB_LEN1,
	GET_FB_LEN2,
	GET_FB_DATA,
	GET_KRT2_CMD,
	GET_KRT2_DATA,
	GET_BECKER_PROTID,
	GET_BECKER_LEN,
	GET_BECKER_DATA
};


class DataLink {
public:
	DataLink();
	void process( const char *packet, int len, int port );

private:
	void parse_NMEA_UBX( char c, int port );
	void processNMEA( char * buffer, int len, int port );
	void addChk(const char c);
	void routeSerialData( const char *data,uint32_t len, int port, bool nmea );
	enum state_t state;
	char framebuffer[128];
	int  pos;
	int  len;
	char chkA; // checksum variables
	char chkB;
	char chkBuf;
	bool nmeaFound;
	bool ubxFound;
	unsigned char krt2_len;
};

/**
 *
 * DataLink
 * This layer handles all inbound unspecific data and precesses it byte per byte to a configured list of 
 * protocol parser for this port.
 *
 */
class DataLinkNT {
public:
	DataLinkNT(router_t route);
	void setProtocol(protocol_nt ptyp);
	void process(const char *packet, const int len);


private:
	// could be also a vector of protocols
	ProtocolItf* _protocol = nullptr;
	// the port's proper queue for the traffic routing
	router_t _router;
};

extern DataLinkNT dl_S1;
extern DataLinkNT dl_S2;
