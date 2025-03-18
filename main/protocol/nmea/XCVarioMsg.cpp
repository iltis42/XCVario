/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "XCVarioMsg.h"
#include "protocol/nmea_util.h"
#include "comm/Messages.h"
#include "SetupNG.h"
#include "KalmanMPU6050.h"
#include "sensor.h"

#include "logdef.h"

#include <cmath>


// The XCVario protocol parser.
//
// Supported messages:


datalink_action_t XCVarioMsg::parsePXCV(NmeaPrtcl *nmea)
{

    return DO_ROUTING;
}



ParserMap XCVarioMsg::_pm = {
    // { Key("V"), XCVarioMsg::parsePXCV }
};


/*
    Sentence has following format:
    $PXCV,
    BBB.B = Vario, -30 to +30 m/s, negative sign for sink,
    C.C = MacCready 0 to 10 m/s
    EE = bugs degradation, 0 = clean to 30 %,
    F.FF = Ballast 1.00 to 1.60,
    G = 0 in climb, 1 in cruise,
    HH.H = Outside airtemp in degrees celcius ( may have leading negative sign ),
    QQQQ.Q = QNH e.g. 1013.2,
    PPPP.P: static pressure in hPa,
    QQQQ.Q: dynamic pressure in Pa,
    RRR.R: roll angle,
    III.I: pitch angle,
    X.XX:   acceleration in X-Axis,
    Y.YY:   acceleration in Y-Axis,
    Z.ZZ:   acceleration in Z-Axis,
*/
void NmeaPrtcl::sendStdXCVario(float baro, float dp, float te, float temp, float ias, float tas,
                            float mc, int bugs, float aballast, bool cruise, float alt, bool validTemp, 
                            float acc_x, float acc_y, float acc_z, 
                            float gx, float gy, float gz)
{
    Message* msg = newMessage();

    if( !validTemp ) {
        temp=0;
    }

    msg->buffer = "$PXCV,";
    // msg->buffer += std::format("{:3.1f}", te); // commented because of an 150KB extra of lib code
    char buffer[50];
    std::sprintf(buffer, "%3.1f", te);
    msg->buffer += buffer;
    // msg->buffer += ',' + std::format("{:1.2f}", mc);
    std::sprintf(buffer, ",%1.2f", mc);
    msg->buffer += buffer;
    msg->buffer += ',' + std::to_string(bugs);
    msg->buffer += ',';
    if( _protocol_version <= 1 ) {
        // msg->buffer += std::format("{:1.3f}",  (aballast+100.f)/100.f);
        std::sprintf(buffer, "%1.3f", (aballast+100.f)/100.f);
        msg->buffer += buffer;
    }
    msg->buffer += ',' + std::to_string(!cruise);
    // msg->buffer += ',' + std::format("{:2.1f}", std::roundf(temp*10.f)/10.f);
    std::sprintf(buffer, ",%2.1f", std::roundf(temp*10.f)/10.f);
    msg->buffer += buffer;
    // msg->buffer += ',' + std::format("{:4.1f}", QNH.get());
    std::sprintf(buffer, ",%4.1f", QNH.get());
    msg->buffer += buffer;
    // msg->buffer += ',' + std::format("{:4.1f}", baro);
    std::sprintf(buffer, ",%4.1f", baro);
    msg->buffer += buffer;
    // msg->buffer += ',' + std::format("{:.1f}", dp);
    std::sprintf(buffer, ",%.1f", dp);
    msg->buffer += buffer;
    
    // optional IMU additions
    if( gflags.haveMPU && attitude_indicator.get() ) {
        // msg->buffer += ',' + std::format("{:3.1f}", IMU::getRoll());
        std::sprintf(buffer, ",%3.1f", IMU::getRoll());
        msg->buffer += buffer;
        // msg->buffer += ',' + std::format("{:3.1f}", IMU::getXCSPitch());
        std::sprintf(buffer, ",%3.1f", IMU::getXCSPitch());
        msg->buffer += buffer;
        // msg->buffer += ',' + std::format("{:1.2f}", acc_x);
        std::sprintf(buffer, ",%1.2f", acc_x);
        msg->buffer += buffer;
        // msg->buffer += ',' + std::format("{:1.2f}", acc_y);
        std::sprintf(buffer, ",%1.2f", acc_y);
        msg->buffer += buffer;
        // msg->buffer += ',' + std::format("{:1.2f}", acc_z);
        std::sprintf(buffer, ",%1.2f", acc_z);
        msg->buffer += buffer;
    } else {
        msg->buffer += ",,,,,";
    }
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    ESP_LOGI(FNAME, "xcv %s", msg->buffer.c_str());
    DEV::Send(msg);
}


