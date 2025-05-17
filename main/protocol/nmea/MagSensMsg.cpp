

#include "MagSensMsg.h"

#include "protocol/nmea_util.h"
#include "comm/Messages.h"
#include "comm/DeviceMgr.h"
#include "protocol/MagSensBin.h"
#include "Compass.h"
#include "QMCMagCAN.h"

#include "sensor.h"
#include "setup/SetupNG.h"
#include "Version.h"
#include "logdef.h"

#include <string>

// update packet counter, assuming there will be only one magsense at a time
static int Conf_Pack_Nr = 0;


// MagSensMsg NMEA protocol is just a simple one. Those queries are supported:
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
//
// - Stream data:
//   $PMMD, <stream_type>, <data>\r\n

dl_action_t MagSensMsg::magsensVersion(NmeaPlugin *plg)
{
    //$PMSV, <release_number>, <build_dateandtime>\r\n
    ESP_LOGI(FNAME,"PMS version");
    ProtocolState *sm = plg->getNMEA().getSM();

    int mag_release;
    char mag_build[40];
    sscanf(sm->_frame.c_str()+6, "%d, %s", &mag_release, mag_build);
    ESP_LOGI(FNAME,"R: %d B: %s", mag_release, mag_build);

    return NOACTION;
}

dl_action_t MagSensMsg::magsensConfirmation(NmeaPlugin *plg)
{
    // $PMSC, <enum>\r\n
    ProtocolState *sm = plg->getNMEA().getSM();

    Conf_Pack_Nr = std::stoi(NMEA::extractWord(sm->_frame, sm->_word_start[0]));
    ESP_LOGI(FNAME,"PMS pack conf %d", Conf_Pack_Nr);

    return NOACTION;
}

dl_action_t MagSensMsg::streamData(NmeaPlugin *plg)
{
    // $PMMD,r/c,x,y,z\r\n
    ProtocolState *sm = plg->getNMEA().getSM();
    const std::vector<int> *word = &sm->_word_start;

    if ( word->size() != 4 ) {
        ESP_LOGI(FNAME,"PMMD stream data incomplete");
        return NOACTION;
    }
    ESP_LOGI(FNAME,"PMMD stream data %s", sm->_frame.c_str());
    if ( sm->_frame.at(6) == 'r' ) {
        if ( MagSensBin::Can_Mag ) {
            t_magn_axes tmp;
            tmp.x = atoi(sm->_frame.c_str() + word->at(1));
            tmp.y = atoi(sm->_frame.c_str() + word->at(2)); 
            tmp.z = atoi(sm->_frame.c_str() + word->at(3));
            MagSensBin::Can_Mag->fromCAN(&tmp);
        }
    }
    else if ( sm->_frame.at(6) == 'c' ) {
        // later
    }
    return NOACTION;
}





// NMEA plugin table
const ParserEntry MagSensMsg::_pt[] = {
    { Key("PMSV"), MagSensMsg::magsensVersion },
    { Key("PMSC"), MagSensMsg::magsensConfirmation },
    { Key("PMMD"), MagSensMsg::streamData },
    {}
};


// Transmitter 
bool NmeaPrtcl::sendHello()
{
    Message* msg = newMessage();
    ESP_LOGI(FNAME, "Hello MagSens %d/%d", msg->target_id, msg->port);
    msg->buffer = "$PMSH\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::sendCalibration()
{
    return false; // todo
}
bool NmeaPrtcl::startStream(int choice)
{
    Message* msg = newMessage();
    msg->buffer = "$PMSSr\r\n";
    if ( choice == 1 ) {
        msg->buffer[4] = 'c';
    }
    return DEV::Send(msg);
}
bool NmeaPrtcl::killStream()
{
    // block until mag stream is down
    MagSensBin *prtc = static_cast<MagSensBin*>(DEVMAN->getProtocol(MAGSENS_DEV, MAGSENSBIN_P));
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
bool NmeaPrtcl::prepareUpdate(int len, int pack)
{
    Message* msg = newMessage();
    msg->buffer = "$PMSU, " + std::to_string(len) + ", " + std::to_string(pack);
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    return DEV::Send(msg);
}

bool NmeaPrtcl::firmwarePacket(const char *buf, int len)
{
    Message *msg = newMessage();
    if ( msg->buffer.max_size() < len ) {
        msg->buffer.reserve(len);
    }
    msg->buffer.assign(buf, len);
    // reset confirmation
    Conf_Pack_Nr = -1;
    return DEV::Send(msg);
}

int NmeaPrtcl::waitConfirmation()
{
    // block until a confirmation or timeout received
    int try_times = 150;
    do {
        vTaskDelay(pdMS_TO_TICKS(10));
        if ( Conf_Pack_Nr > 0 ) {
            break;
        }
    }
    while ( --try_times > 0 );

    return Conf_Pack_Nr;
}

