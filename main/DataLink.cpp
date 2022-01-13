#include "DataLink.h"
#include <stdint.h>
#include <endian.h>
#include <string.h>
#include <logdef.h>
#include "UbloxGNSSdecode.h"
#include "Router.h"
#include "SString.h"
#include "Serial.h"
#include "Flarm.h"

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
const uint8_t NMEA_CR = '\r';  // 13 0d
const uint8_t NMEA_LF = '\n';  // 10 0a

DataLink::DataLink(){
	state = GET_NMEA_UBX_SYNC;
	pos = 0;
	chkA = 0; // checksum variables UBX
	chkB = 0;
	chkBuf = 0;
	nmeaFound = false;
	ubxFound = false;
}

void DataLink::process( char *packet, int len, int port ) {
	// process every frame byte through state machine
	// ESP_LOGI(FNAME,"S%d: RX len: %d bytes", port, len );
	// ESP_LOG_BUFFER_HEXDUMP(FNAME,packet, len, ESP_LOG_INFO);
	ubxFound = false;
	nmeaFound = false;
	if( Flarm::bincom ){  // neither UBX nor NMEA, for Flarm bincom protocol transparent forward
		// ESP_LOGI(FNAME, "Port S%1d: Flarm::bincom = true", port);
		routeSerialData(packet, len, port, false );
	}else{
		for (int i = 0; i < len; i++) {
			parse_NMEA_UBX(packet[i], port, i == len-1);
		}
	}
}

void DataLink::addChk(const char c) {
	chkA += c;
	chkB += chkA;
}

void DataLink::routeSerialData( char *data, int len, int port, bool nmea ){
	SString tx;
	tx.set( data, len );
	if( port == 1 ){
		Router::forwardMsg( tx, s1_rx_q, nmea );
		Router::routeS1();
	}
	else if( port == 2 ){
		Router::forwardMsg( tx, s2_rx_q, nmea  );
		Router::routeS2();
	}
}


void DataLink::processNMEA( char * buffer, int len, int port ){
	// ESP_LOGI(FNAME, "Port S%1d: processNMEA, frame: %s", port, buffer );
	Flarm::parsePFLAX( buffer, port );  // datalink relevant as this changes protocol to flarm bincom
	routeSerialData(buffer, len, port, true );
}

void DataLink::parse_NMEA_UBX( char c, int port, bool last ){
	// ESP_LOGI(FNAME, "Port S%1d: char=%c pos=%d  state=%d", port, c, pos, state );
	switch(state) {
	case GET_NMEA_UBX_SYNC:
		switch(c) {
		case NMEA_START1:
		case NMEA_START2:
			pos = 0;
			framebuffer[pos] = c;
			pos++;
			state = GET_NMEA_STREAM;
			// ESP_LOGI(FNAME, "Port S%1d: NMEA Start at %d", port, pos);
			break;
		case UBX_SYNC1:
			chkA = 0;
			chkB = 0;
			pos = 0;
			state = GET_UBX_SYNC2;
			// ESP_LOGI(FNAME, "Port S%1d: UBX Start at %d", port, pos );
			break;
		}
		break;

		case GET_NMEA_STREAM:
			if ((c < NMEA_MIN || c > NMEA_MAX) && (c != NMEA_CR && c != NMEA_LF)) {
				ESP_LOGE(FNAME, "Port S%1d: Invalid NMEA character, restart", port);
				state = GET_NMEA_UBX_SYNC;
				break;
			}
			if (pos >= sizeof(framebuffer) - 1) {
				ESP_LOGE(FNAME, "Port S%1d NMEA buffer not large enough, restart", port);
				pos = 0;
				state = GET_NMEA_UBX_SYNC;
			}
			framebuffer[pos] = c;
			pos++;
			if ( (c == NMEA_LF) || (last && (c == NMEA_CR) ) ) { // catch case when CR is last char, e.g. commands from BT
				framebuffer[pos] = 0;  // framebuffer is zero terminated
				processNMEA( framebuffer, pos, port );
				state = GET_NMEA_UBX_SYNC;
				pos = 0;
			}
			break;

		case GET_UBX_SYNC2:
			if (c != UBX_SYNC2) {
				ESP_LOGW(FNAME, "Port S%1d Unexpected UBX SYN2 at %d", port, pos);
				state = GET_NMEA_UBX_SYNC;
				break;
			}
			// ESP_LOGI(FNAME,"Port S%d UBX SYNC2 received at %d", port, pos );
			state = GET_UBX_CLASS;
			break;

		case GET_UBX_CLASS:
			if (c != UBX_CLASS) {
				ESP_LOGW(FNAME, "Port S%1d Unexpected UBX class at %d", port, pos );
				state = GET_NMEA_UBX_SYNC;
				break;
			}
			addChk(c);
			state = GET_UBX_ID;
			break;

		case GET_UBX_ID:
			if (c != UBX_ID) {
				ESP_LOGW(FNAME, "Port S%1d Unexpected UBX ID at %d", port, pos );
				state = GET_NMEA_UBX_SYNC;
				break;
			}
			addChk(c);
			state = GET_UBX_LENGTH1;
			break;

		case GET_UBX_LENGTH1:
			addChk(c);
			state = GET_UBX_LENGTH2;
			break;

		case GET_UBX_LENGTH2:
			addChk(c);
			state = GET_UBX_PAYLOAD;
			break;

		case GET_UBX_PAYLOAD:
			addChk(c);
			framebuffer[pos] = c;
			if (pos == PVT_PAYLOAD_SIZE - 1) {
				state = GET_UBX_CHKA;
			}
			pos++;
			break;

		case GET_UBX_CHKA:
			chkBuf = c;
			state = GET_UBX_CHKB;
			break;

		case GET_UBX_CHKB:
			if (chkBuf == chkA && c == chkB) {
				UbloxGnssDecoder::processNavPvtFrame( framebuffer, port );  // drop to decoder for local usage
				pos = 0;
			} else {
				ESP_LOGE(FNAME, "Port S%1d: UBX checksum does not match", port);
			}
			state = GET_NMEA_UBX_SYNC;
			break;
	}
}
