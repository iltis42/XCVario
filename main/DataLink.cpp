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
#include "DataMonitor.h"

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

const uint8_t FB_START_FRAME = 0x73;

const uint8_t KRT2_STX_START = 0x02;


DataLink::DataLink(){
	state = GET_NMEA_UBX_SYNC;
	pos = 0;
	chkA = 0; // checksum variables UBX
	chkB = 0;
	chkBuf = 0;
	nmeaFound = false;
	ubxFound = false;
	len = 0;
}

void DataLink::process( const char *packet, int len, int port ) {
	// process every frame byte through state machine
	// ESP_LOGI(FNAME,"Port %d: RX len: %d bytes", port, len );
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

void DataLink::routeSerialData( const char *data, int len, int port, bool nmea ){
	SString tx;
	tx.set( data, len );
	if( port == 1 ){      // S1
		Router::forwardMsg( tx, s1_rx_q, nmea );
		Router::routeS1();
	}
	else if( port == 2 ){  // S2
		Router::forwardMsg( tx, s2_rx_q, nmea  );
		Router::routeS2();
	}
	else if( port == 3 ){  // CAN
		Router::forwardMsg( tx, can_rx_q, nmea  );
		Router::routeCAN();
		DM.monitorString( MON_CAN, DIR_RX, tx.c_str(), false );
		// ESP_LOG_BUFFER_HEXDUMP(FNAME, tx.c_str(), tx.length(), ESP_LOG_INFO);
	}
	else if( port == 7 ){  // Bluetooth
		Router::forwardMsg( tx, bt_rx_q, nmea  );
		Router::routeBT();
		// ESP_LOG_BUFFER_HEXDUMP(FNAME, tx.c_str(), tx.length(), ESP_LOG_INFO);
	}
	else if( port == 8880 ){  // WiFi / XCVario
		Router::forwardMsg( tx, wl_vario_rx_q, nmea  );
		Router::routeWLAN();
		// ESP_LOG_BUFFER_HEXDUMP(FNAME, tx.c_str(), tx.length(), ESP_LOG_INFO);
	}
	else if( port == 8881 ){  // WiFi / Flarm
		Router::forwardMsg( tx, wl_flarm_rx_q, nmea  );
		Router::routeWLAN();
		// ESP_LOG_BUFFER_HEXDUMP(FNAME, tx.c_str(), tx.length(), ESP_LOG_INFO);
	}
	else if( port == 8882 ){  // WiFi / Aux
		Router::forwardMsg( tx, wl_aux_rx_q, nmea  );
		Router::routeWLAN();
		// ESP_LOG_BUFFER_HEXDUMP(FNAME, tx.c_str(), tx.length(), ESP_LOG_INFO);
	}
	else if( port == 8884 ){  // WiFi / Aux
		Router::forwardMsg( tx, can_rx_q, nmea  );
		Router::routeCAN();
		// ESP_LOG_BUFFER_HEXDUMP(FNAME, tx.c_str(), tx.length(), ESP_LOG_INFO);
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
		case KRT2_STX_START:
			pos = 0;
			framebuffer[pos] = c;
			pos++;
			state = GET_KRT2_STX;
			// ESP_LOGI(FNAME, "Port %1d: STX Start at %d", port, pos );
			break;
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
		case FB_START_FRAME:
			chkA = 0;
			pos = 0;
			len = 0;
			state = GET_FB_LEN1;
			framebuffer[pos] = c;
			pos++;
			break;
		}
		break;

		case GET_KRT2_STX:
			framebuffer[pos] = c;
			pos++;
			if( pos > 12 ){ // 0..12 = 13 bytes STX message buffer.
				framebuffer[pos] = 0;  // framebuffer is zero terminated
				// ESP_LOG_BUFFER_HEXDUMP(FNAME, framebuffer, pos+1, ESP_LOG_INFO);
				routeSerialData(framebuffer, pos+1, port, true );
				state = GET_NMEA_UBX_SYNC;
				pos = 0;
			}
			break;

		case GET_FB_LEN1:
			chkA = c;
			state = GET_FB_LEN2;
			framebuffer[pos] = c;
			pos++;
			break;

		case  GET_FB_LEN2:
			len = (chkA + (unsigned char)c*256 );
			ESP_LOGI(FNAME, "got length from flarm bincom: %d", len );
			state = GET_FB_DATA;
			framebuffer[pos] = c;
			pos++;
			break;

		case GET_FB_DATA:
			framebuffer[pos] = c;
			pos++;
			if( pos > len ){
				routeSerialData(framebuffer, len+1, port, true );
				state = GET_NMEA_UBX_SYNC;
			}
			break;

		case GET_NMEA_STREAM:
			if ((c < NMEA_MIN || c > NMEA_MAX) && (c != NMEA_CR && c != NMEA_LF)) {
				ESP_LOGE(FNAME, "Port S%1d: Invalid NMEA character %x, restart, pos: %d, state: %d", port, (int)c, pos, state );
				ESP_LOG_BUFFER_HEXDUMP(FNAME, framebuffer, pos+1, ESP_LOG_INFO);
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
