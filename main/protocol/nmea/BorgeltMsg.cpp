/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "BorgeltMsg.h"
#include "protocol/nmea_util.h"
#include "comm/Messages.h"
#include "Units.h"

#include "logdefnone.h"

#include <cmath>

// The Borgelt protocol parser.
//
// Supported messages:


ConstParserMap BorgeltMsg::_pm = {
};

/*
    Sentence has following format:
    $PBB50,AAA,BBB.B,C.C,DDDDD,EE,F.FF,G,HH*CHK crlf
    AAA = TAS 0 to 150 knots
    BBB.B = Vario, -10 to +15 knots, negative sign for sink
    C.C = MacCready 0 to 8.0 knots
    DDDDD = IAS squared 0 to 22500
    EE = bugs degradation, 0 = clean to 30 %
    F.FF = Ballast 1.00 to 1.60
    G = 0 in climb, 1 in cruise
    HH = Outside airtemp in degrees celcius ( may have leading negative sign )
    CHK = standard NMEA checksum
*/
void NmeaPrtcl::sendBorgelt(float te, float temp, float ias, float tas, float mc, int bugs, float aballast, bool cruise, bool validTemp)
{
    Message* msg = newMessage();

    if( !validTemp ) {
        temp = 0;
    }

    float iaskn = Units::kmh2knots( ias );

    msg->buffer = "$PBB50,";
    char buffer[50];
    std::sprintf(buffer, "%03d", (int)std::round(Units::kmh2knots(tas)));
    msg->buffer += buffer;
    std::sprintf(buffer, ",%3.1f", Units::ms2knots(te));
    msg->buffer += buffer;
    std::sprintf(buffer, ",%1.1f", Units::mcval2knots(mc));
    msg->buffer += buffer;
    std::sprintf(buffer, ",%d", (int)std::round(iaskn*iaskn));
    msg->buffer += buffer;
    std::sprintf(buffer, ",%d", bugs);
    msg->buffer += buffer;
    std::sprintf(buffer, ",%1.2f", (aballast+100.f)/100.f);
    msg->buffer += buffer;
    std::sprintf(buffer, ",%1d", !cruise);
    msg->buffer += buffer;
    std::sprintf(buffer, ",%2d", (int)std::round(temp));

    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    ESP_LOGD(FNAME, "Borgelt %s", msg->buffer.c_str());
    DEV::Send(msg);
}


