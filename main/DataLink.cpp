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

const uint8_t BECKER_START_FRAME = 0xA5;
const uint8_t BECKER_PROTID      = 0x14;

static const char *ANEMOI_IDS = "ADMSWadw";
static const char *ANEMOI_OFF = "";
static const int ANEMOI_LEN[] = {10, 15, 7, 7, 12, 10, 15, 12};
const char *ANEMOI_IDS_PTR = ANEMOI_OFF;

void enable_anemoi(){
	ANEMOI_IDS_PTR = ANEMOI_IDS;
}
void disable_anemoi() {
	ANEMOI_IDS_PTR = ANEMOI_OFF;
}


DataLink::DataLink(){
	state = GET_NMEA_UBX_SYNC;
	pos = 0;
	chkA = 0; // checksum variables UBX
	chkB = 0;
	chkBuf = 0;
	nmeaFound = false;
	ubxFound = false;
	len = 0;
	krt2_len=0;
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
			parse_NMEA_UBX(packet[i], port );
		}
	}
}

void DataLink::addChk(const char c) {
	chkA += c;
	chkB += chkA;
}

void DataLink::routeSerialData( const char *data, uint32_t len, int port, bool nmea ){
	SString tx;
	tx.set( data, len );
	// ESP_LOGI(FNAME, "Port S%1d: len: %d", port, len );
	// ESP_LOG_BUFFER_HEXDUMP(FNAME, tx.c_str(), tx.length(), ESP_LOG_INFO);
	if( port == 1 ){      // S1
		Router::forwardMsg( tx, s1_rx_q, nmea );
		Router::routeS1();
	}
	else if( port == 2 ){  // S2
		Router::forwardMsg( tx, s2_rx_q, nmea  );
		Router::routeS2();
	}
	else if( port == 3 ){  // CAN
		Router::forwardMsg( tx, can_rx_q, nmea );
		Router::routeCAN();
		DM.monitorString( MON_CAN, DIR_RX, tx.c_str(), len);
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

void DataLink::parse_NMEA_UBX( char c, int port ){
	// ESP_LOGI(FNAME, "Port S%1d: char=%c pos=%d  state=%d", port, c, pos, state );
	switch(state) {
	case GET_NMEA_UBX_SYNC:
		switch(c) {
		case KRT2_STX_START:
			pos = 0;
			framebuffer[pos] = c;
			pos++;
			state = GET_KRT2_CMD;
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
		case BECKER_START_FRAME:
			pos = 0;
			len = 0;
			framebuffer[pos] = c;
			pos++;
			state = GET_BECKER_PROTID;
		}
		break;

	case GET_BECKER_PROTID:
		if( c == BECKER_PROTID ){
			framebuffer[pos] = c;
			pos++;
			state = GET_BECKER_LEN;
		}else{
			state = GET_NMEA_UBX_SYNC;
			pos = 0;
		}
		break;

	case GET_BECKER_LEN:
		framebuffer[pos] = c;
		len = c;
		if( len < 64 ){ // we support up to 64 bytes of msg length what should be far enough
			pos++;
			state = GET_BECKER_DATA;
		}else
		{
			state = GET_NMEA_UBX_SYNC;
			pos = 0;
		}
		break;

	case GET_BECKER_DATA:
		framebuffer[pos] = c;
		pos++;
		if( pos > len+5 ){  // including 3 bytes header/protid/len and 2 bytes CRC
			framebuffer[pos] = 0;
			routeSerialData(framebuffer, pos+1, port, true );
			state = GET_NMEA_UBX_SYNC;
			pos = 0;
		}
		break;

	case GET_KRT2_CMD:
		framebuffer[pos] = c;
		pos++;
		if( c == 'U' || c == 'R'  ){
			krt2_len = 12;
			state = GET_KRT2_DATA;
		}
		else if( c == 'A' ){
			krt2_len = 5;
			state = GET_KRT2_DATA;
		}
		else if( c == 'C' || c == 'O' || c == 'o'  ){   // we have a single byte command Swap or Dual Mode
			krt2_len = 1;
			framebuffer[pos] = 0;
			routeSerialData(framebuffer, pos+1, port, true );
			state = GET_NMEA_UBX_SYNC;
			pos = 0;
		}
		else {
			krt2_len = 1;
			state = GET_NMEA_UBX_SYNC;
			pos = 0;
		};
		break;

	case GET_KRT2_DATA: {
		framebuffer[pos] = c;
		pos++;
		if( pos > krt2_len ){ // 0..12 = 13 bytes STX message buffer.
			framebuffer[pos] = 0;  // framebuffer is zero terminated
			// ESP_LOG_BUFFER_HEXDUMP(FNAME, framebuffer, pos+1, ESP_LOG_INFO);
			routeSerialData(framebuffer, pos+1, port, true );
			state = GET_NMEA_UBX_SYNC;
			pos = 0;
		}
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
		if( len > 512 ){
			ESP_LOGW(FNAME, "Odd length from flarm bincom: %d: restart", len );
			state = GET_NMEA_UBX_SYNC;
			break;
		}
		// ESP_LOGI(FNAME, "got length from flarm bincom: %d", len );
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
			// ESP_LOGE(FNAME, "Port S%1d: Invalid NMEA character %x, restart, pos: %d, state: %d", port, (int)c, pos, state );
			// ESP_LOG_BUFFER_HEXDUMP(FNAME, framebuffer, pos+1, ESP_LOG_INFO);
			state = GET_NMEA_UBX_SYNC;
			break;
		}
		if ( pos == 1 && framebuffer[0] == NMEA_START1 ) {
			char *ptr = strchr(ANEMOI_IDS_PTR, c);
			if ( ptr != nullptr ) {
				len = ANEMOI_LEN[int(ptr-ANEMOI_IDS)];
				state = GET_ANEMOI_DATA;
				framebuffer[pos] = c;
				pos++;
				// ESP_LOGE(FNAME, "ANEMOI Start");
				break;
			}
		}
		if (pos >= sizeof(framebuffer) - 1) {
			ESP_LOGE(FNAME, "Port S%1d NMEA buffer not large enough, restart", port);
			pos = 0;
			state = GET_NMEA_UBX_SYNC;
		}
		if ( c == NMEA_CR || c == NMEA_LF ) { // normal case, accordign to NMEA 183 protocol, first CR, then LF as the last char  (<CR><LF> ends the message.)
												// but we accept also a single terminator as not relevant for the data carried        0d  0a
			// make things clean!                                                                                                   \r  \n
			framebuffer[pos] = NMEA_CR;       // append a CR
			pos++;
			framebuffer[pos] = NMEA_LF;       // append a LF
			pos++;
			framebuffer[pos] = 0;  // framebuffer is zero terminated
			// pos++;
			processNMEA( framebuffer, pos, port );
			state = GET_NMEA_UBX_SYNC;
			pos = 0;
		}else{
			framebuffer[pos] = c;
			pos++;
		}

		break;

	case GET_ANEMOI_DATA:
		framebuffer[pos] = c;
		pos++;
		if( pos >= len ) { // including frame and CRC
			if ( strchr("SWw", framebuffer[1]) != nullptr ) {
				// Only process status and wind
				framebuffer[pos] = 0;
				// ESP_LOGI(FNAME, "Port S%1d anemoi %c", port, framebuffer[1]);
				routeSerialData(framebuffer, pos, port, true );
			}
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
