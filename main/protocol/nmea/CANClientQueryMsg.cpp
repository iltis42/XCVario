/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "CANClientQueryMsg.h"

#include "protocol/nmea_util.h"
#include "protocol/Clock.h"
#include "comm/DeviceMgr.h"
#include "comm/Messages.h"
#include "setup/SetupCommon.h"

#include "logdefnone.h"

#include <cstring>

// The XCV client registration query message.
//
// - Registration query: Send a request to a potential XCV peer on the bus grabbing the master role,
//      and deleivers the expected registration response. Until a peer responds the client will continue sending a query 
//      every two seconds.
//      The request is sent on a high id to give precedence to all other traffic on the bus. The registration is not time 
//      critical. The registration query is sent on the CAN id=0x7f0. (== b0111 1111 0000, the ones are recessive on CAN and thus
//      most other telegrams will have precedence). The master response is expected on the same id.
//      The request is built with an unique token that consists of an arbitrary sequence of three characters. It is used as identification 
//      of the masters response, because all client registration are happening on the same CAN id.
//      In case multiple peers respond with different port/id information, the information of the first valid response will be used.
//      Second parameter is a string with the letters "XCVCLIENT" to identify the desired protocol to talk, and thus the kind of devcie
//      requesting.
//   $PJPREG, <token>, <protocol type>\r\n
//
// The expected master message/response
// - Registration query accepted: A master respons with the query token, a client id to listen, and a master id to respond messages to.
//   $PJMACC, <token>, <drive id>, <master id>*<CRC>\r\n
//
// - Re-registration broadcast: Triggers a new registration cycle from the client.
//   $PJMLOR\r\n


CANClientQueryMsg::CANClientQueryMsg(NmeaPrtcl &nr) :
    NmeaPlugin(nr, XCVQUERY_P),
    Clock_I(200)
{
    // Create a token from the default id
    const char* buf = SetupCommon::getID();
    int len = strlen(buf);
    strcpy(Q_TOKEN, &buf[len-3]); // last three chars

    Clock::start(this); // register query kicker time-out
}


CANClientQueryMsg::~CANClientQueryMsg()
{
    Clock::stop(this); // deregister time-out
}


bool CANClientQueryMsg::tick()
{
    sendRegistrationQuery();
    return false;
}

//
// The private transmitter routine
//
bool CANClientQueryMsg::sendRegistrationQuery()
{
    Message* msg = _nmeaRef.newMessage();

    msg->buffer = "$PJPREG, ";
    msg->buffer += Q_TOKEN;
    msg->buffer += ", XCVCLIENT\r\n";
    return DEV::Send(msg);
}


//
// Receiver routines
//
dl_action_t CANClientQueryMsg::registration(NmeaPlugin *plg)
{
    // grab token from e.g. message "$PJMACC, 123, client_id, master_id"
    ProtocolState *sm = plg->getNMEA().getSM();
    CANClientQueryMsg *me = reinterpret_cast<CANClientQueryMsg*>(plg);
    ESP_LOGI(FNAME, "Client received ACC");
    if ( sm->_frame.size() < 12 ) {
        return NOACTION;
    }
    int pos = 8;
    std::string tail = NMEA::extractWord(sm->_frame, pos);
    ESP_LOGI(FNAME, "MS token >%s<", tail.c_str());

    // check on correct token
    if ( tail.compare(0, 3, me->Q_TOKEN) != 0 ) {
        return NOACTION;
    }

    // read the client id
    int c_id = std::stoi(NMEA::extractWord(sm->_frame, pos));

    // read the master id
    int m_id = std::stoi(NMEA::extractWord(sm->_frame, pos));

    // simple check
    if ( c_id > 0 && c_id < 0x7ff 
        && m_id > 0 && m_id < 0x7ff ) {
        // success
        ESP_LOGI(FNAME, "Client registered (ID=%d)", c_id);
        DEVMAN->addDevice(XCVARIO_DEV, XCVSYNC_P, c_id, m_id, CAN_BUS);

        Clock::stop(me);
    }
    return NOACTION;
}


dl_action_t CANClientQueryMsg::restart_query(NmeaPlugin *plg)
{
    // restart registration loop
    CANClientQueryMsg *me = reinterpret_cast<CANClientQueryMsg*>(plg);
    Clock::start(me);
    me->tick(); // first trial
    return NOACTION;
}

dl_action_t CANClientQueryMsg::rejected(NmeaPlugin *plg)
{
    // lol
    ESP_LOGI(FNAME, "Received not accepted");
    return NOACTION;
}

const ParserEntry CANClientQueryMsg::_pt[] = {
    { Key("MACC"), CANClientQueryMsg::registration },
    { Key("MLOR"), CANClientQueryMsg::restart_query },
    { Key("MNAC"), CANClientQueryMsg::rejected },
    {}
};
