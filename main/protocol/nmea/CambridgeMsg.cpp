/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "CambridgeMsg.h"
#include "protocol/nmea_util.h"
#include "comm/Messages.h"
#include "SetupNG.h"
#include "Units.h"

#include "logdefnone.h"


// The Cambridge protocol parser.
//
// Supported messages:


ConstParserMap CambridgeMsg::_pm = {
};

/*
    Cambridge 302 Format:
    !W,<1>,<2>,<3>,<4>,<5>,<6>,<7>,<8>,<9>,<10>,<11>,<12>,<13>*hh<CR><LF>
    <1>    Vector wind direction in degrees
    <2>    Vector wind speed in 10ths of meters per second
    <3>    Vector wind age in seconds
    <4>    Component wind in 10ths of Meters per second + 500 (500 = 0, 495 = 0.5 m/s tailwind)
    <5>    True altitude in Meters + 1000
    <6>    Instrument QNH setting
    <7>    True airspeed in 100ths of Meters per second
    <8>    Variometer reading in 10ths of knots + 200
    <9>    Averager reading in 10ths of knots + 200
    <10>   Relative variometer reading in 10ths of knots + 200
    <11>   Instrument MacCready setting in 10ths of knots
    <12>   Instrument Ballast setting in percent of capacity
    <13>   Instrument Bug setting
    *hh   Checksum, XOR of all bytes of the sentence after the ‘!’ and before the ‘*’
*/
void NmeaPrtcl::sendCambridge(float te, float tas, float mc, int bugs, float alt)
{
    Message* msg = newMessage();

    msg->buffer = "!w,0,0,0,0,";
    char buffer[50];
    std::sprintf(buffer, "%d", int(alt+1000.5));
    msg->buffer += buffer;
    std::sprintf(buffer, ",%4.2f", QNH.get());
    msg->buffer += buffer;
    std::sprintf(buffer, ",%d", int(Units::kmh2ms(tas)*100));
    msg->buffer += buffer;
    std::sprintf(buffer, ",%d", int((Units::ms2knots(te)*10)+200));
    msg->buffer += buffer;
    std::sprintf(buffer, ",0,0,%d", int(Units::mcval2knots(mc)*10));
    msg->buffer += buffer;
    std::sprintf(buffer, ",%d", int((100*ballast_kg.get() / polar_max_ballast.get())));
    msg->buffer += buffer;
    std::sprintf(buffer, ",%d", 100-bugs);

    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    ESP_LOGD(FNAME, "Cambridge %s", msg->buffer.c_str());
    DEV::Send(msg);
}


