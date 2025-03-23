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

#include "logdefnone.h"

#include <cmath>


// The XCVario protocol parser.
//

datalink_action_t XCVarioMsg::parseExcl_xsX(NmeaPrtcl *nmea)
{
    ProtocolState *sm = nmea->getSM();
    const std::vector<int> *word = &sm->_word_start;

    ESP_LOGI(FNAME,"parseXS %s", sm->_frame.c_str() );
    char role = sm->_frame[3]; // M | C
    int pos = word->at(0);
    std::string key = NMEA::extractWord(sm->_frame, pos);
    char type = sm->_frame[word->at(1)];
    float val = atof(sm->_frame.c_str() + word->at(2));
    ESP_LOGI(FNAME,"parsed NMEA: role=%c type=%c key=%s val=%f vali=%d", role, type , key.c_str(), val, (int)val );
    SetupCommon *item = SetupCommon::getMember( key.c_str() );
    if ( item ) {
        if( type == 'F' ) {
            SetupNG<float> *mi = static_cast<SetupNG<float> *>(item);
            if( role == 'A' )
                mi->ack( val );
            else
                mi->set( val, false );
        }
        else if( type == 'I' ) {
            SetupNG<int> *mi = static_cast<SetupNG<int> *>(item);
            if( role == 'A' && val == mi->get() ) {
                mi->ack( val );
            } else {
                mi->set( (int)val, false );
            }
        }
    }
    else {
        ESP_LOGW(FNAME,"Setup item with key %s not found", key.c_str() );
    }

    return NOACTION;
}




ConstParserMap XCVarioMsg::_pm = {
    {Key("xsA"), XCVarioMsg::parseExcl_xsX},
    {Key("xsC"), XCVarioMsg::parseExcl_xsX},
    {Key("xsM"), XCVarioMsg::parseExcl_xsX},
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
    ESP_LOGD(FNAME, "xcv %s", msg->buffer.c_str());
    DEV::Send(msg);
}


