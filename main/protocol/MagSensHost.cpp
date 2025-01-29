

#include "MagSensHost.h"

#include "nmea_util.h"
#include "comm/Messages.h"
#include "comm/DeviceMgr.h"
#include "protocol/MagSensBin.h"

#include "sensor.h"
#include "SetupNG.h"
#include "Version.h"
#include <logdef.h>

#include <string>


// MagSensHost NMEA protocol is just a simple one. Those queries are supported:
// - Hello and version query:
//   $PMSH\r\n
//   response: $PMSV, <release_number>, <build_dateandtime>\r\n
//
// - The stream request: ( to be compatible a raw stream is picked up after 2 minutes time-out)
//   $PMSS<stream_type=r/c/..>\r\n
//
// - Transfer new calibration:
//   $PMSC ... *<CRC>\r\n
//
// - Kill stream request:
//   $PMSK\r\n
//
// - Anounce firmware update:
//   $PMSU,  <length>, <packet_size>*<CRC>\r\n
//   response: $PMSC, <enum>\r\n


datalink_action_t MagSensHost::nextByte(const char c)
{
    int pos = _sm._frame.size() - 1; // c already in the buffer
    ESP_LOGD(FNAME, "state %d, pos %d next char %c", _sm._state, pos, c);

    switch(_sm._state) {
    case START_TOKEN:
        if ( c == '$' ) {
            _sm._state = HEADER;
            ESP_LOGI(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        NMEA::incrCRC(_sm._crc,c);
        if ( pos < 3 ) { break; }
        if ( _sm._frame.substr(1,3) != "PMS" ) { // MagSens  sender id
            break;
        }
        _sm._state = PAYLOAD;
        break;
    case PAYLOAD:
        if ( c == '*' ) {
            _sm._state = CHECK_CRC1; // Expecting a CRC to check
            break;
        }
        if ( c != '\r' && c != '\n' ) {
            ESP_LOGD(FNAME, "Msg PAYLOAD");
            NMEA::incrCRC(_sm._crc,c);
            break;
        }
        _sm._state = COMPLETE;
        break;
    case CHECK_CRC1:
        _crc_buf[0] = c;
        _sm._state = CHECK_CRC2;
        break;
    case CHECK_CRC2:
    {
        _crc_buf[1] = c;
        _crc_buf[2] = '\0';
        char read_crc = (char)strtol(_crc_buf, NULL, 16);
        ESP_LOGD(FNAME, "Msg CRC %s/%x - %x", _crc_buf, read_crc, _sm._crc);
        if ( read_crc != _sm._crc ) {
            _sm._state = START_TOKEN;
            break;
        }
        _sm._state = COMPLETE;
        break;
    }
    case STOP_TOKEN:
    case COMPLETE:
        ESP_LOGD(FNAME, "Msg complete %s", _sm._frame.c_str());
        _sm._state = START_TOKEN;
        switch (_sm._frame[4]) {
            case 'V':
                Version();
                break;
            case 'C':
                Confirmation();
                break;
            default:
                break;
        }
        break;
    default:
        break;
    }
    return NOACTION;
}

void MagSensHost::Version()
{
    //$PMSV, <release_number>, <build_dateandtime>\r\n
    ESP_LOGI(FNAME,"PMS version");

    int mag_release;
    char mag_build[40];
    sscanf(_sm._frame.c_str()+6, "%d, %s", &mag_release, mag_build);
    ESP_LOGI(FNAME,"R: %d B: %s", mag_release, mag_build);
}

void MagSensHost::Confirmation()
{
    // $PMSC, <enum>\r\n

    int pos = 6;
    _conf_pack_nr = std::stoi(NMEA::extractWord(_sm._frame, pos));
    ESP_LOGI(FNAME,"PMS pack conf %d", _conf_pack_nr);
}

// Transmitter 
bool MagSensHost::sendHello()
{
    Message* msg = newMessage();
    msg->buffer = "$PMH\r\n";
    return DEV::Send(msg);
}

bool MagSensHost::sendCalibration()
{
    return false; // todo
}
bool MagSensHost::startStream(int choice)
{
    Message* msg = newMessage();
    msg->buffer = "$PMSr\r\n";
    if ( choice == 1 ) {
        msg->buffer[4] = 'c';
    }
    return DEV::Send(msg);
}
bool MagSensHost::killStream()
{
    // block until mag stream is down
    MagSensBinary *prtc = static_cast<MagSensBinary*>(DEVMAN->getProtocol(MAGSENS_DEV, MAGSENSBIN_P));
    int try_times = 10;
    do {
        Message* msg = newMessage();
        msg->buffer = "$PMSK\r\n";
        DEV::Send(msg);
        delay(205);
        ESP_LOGI(FNAME, "Mag kill %d", try_times);
        if ( prtc && ! prtc->isActive() ) {
            break;
        }
    }
    while ( --try_times > 0 );
    
    return true;
}
bool MagSensHost::prepareUpdate(int len, int pack)
{
    Message* msg = newMessage();
    msg->buffer = "$PMSU, " + std::to_string(len) + ", " + std::to_string(pack);
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    return DEV::Send(msg);
}

int MagSensHost::firmwarePacket(const char *buf, int len)
{
    Message *msg = newMessage();
    if ( msg->buffer.max_size() < len ) {
        msg->buffer.reserve(len);
    }
    msg->buffer.assign(buf, len);
    // reset confirmation
    _conf_pack_nr = -1;
    DEV::Send(msg);

    // block until a confirmation or timeout received
    int try_times = 10;
    do {
        delay(5);
        if ( _conf_pack_nr > 0 ) {
            break;
        }
    }
    while ( --try_times > 0 );

    if ( _conf_pack_nr < 0 ) ESP_LOGI(FNAME, "no confirmation");

    return _conf_pack_nr;
}
