/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "XCVarioMsg.h"
#include "protocol/nmea_util.h"
#include "comm/DataLink.h"
#include "comm/Messages.h"
#include "setup/SetupNG.h"
#include "KalmanMPU6050.h"
#include "sensor.h"

#include "logdef.h"

#include <string>
#include <cmath>


uint8_t XCVarioMsg::_protocol_version = 1;

// The XCVario protocol parser.
//

dl_action_t XCVarioMsg::parseExcl_xcs(NmeaPlugin *plg)
{
    ProtocolState *sm = plg->getNMEA().getSM();
    const std::vector<int> *word = &sm->_word_start;

    ESP_LOGI(FNAME, "xcs teleg: %s", sm->_frame.c_str());

    if (word->size() < 2)
    {
        // Telegramm incomplete
        return NOACTION;
    }
    const char *s = sm->_frame.c_str();
    int value = atoi(s + word->at(1));
    s += word->at(0);
    if (strncmp(s, "crew-weight", 11) == 0)
    {
        ESP_LOGI(FNAME, "New crew-weight: %d", value);
        crew_weight.set((float)value);
    }
    else if (strncmp(s, "empty-weight", 12) == 0)
    {
        ESP_LOGI(FNAME, "New empty_weight: %d", value);
        empty_weight.set((float)value);
    }
    else if (strncmp(s, "bal-water", 9) == 0)
    {
        ESP_LOGI(FNAME, "New ballast: %d", value);
        ballast_kg.set((float)value);
    }
    else if (strncmp(s, "version", 7) == 0)
    {
        ESP_LOGI(FNAME, "Got xcv protocol version: %d", value);
        _protocol_version = value;
        plg->getNMEA().sendXCVVersion(2);
    }

    return NOACTION;
}

const ParserEntry XCVarioMsg::_pt[] = {
    {Key("xcs"), XCVarioMsg::parseExcl_xcs},
    {}
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
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    Message *msg = newMessage();

    if (!validTemp)
    {
        temp = 0;
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
    if (XCVarioMsg::getXcvProtocolVersion() <= 1)
    {
        std::sprintf(buffer, "%1.3f", (aballast + 100.f) / 100.f);
        msg->buffer += buffer;
    }
    msg->buffer += ',' + std::to_string(!cruise);
    std::sprintf(buffer, ",%2.1f", std::roundf(temp * 10.f) / 10.f);
    msg->buffer += buffer;
    std::sprintf(buffer, ",%4.1f", QNH.get());
    msg->buffer += buffer;
    std::sprintf(buffer, ",%4.1f", baro);
    msg->buffer += buffer;
    std::sprintf(buffer, ",%.1f", dp);
    msg->buffer += buffer;

    // optional IMU additions
    if (gflags.haveMPU && attitude_indicator.get())
    {
        std::sprintf(buffer, ",%3.1f", IMU::getRoll());
        msg->buffer += buffer;
        std::sprintf(buffer, ",%3.1f", IMU::getXCSPitch());
        msg->buffer += buffer;
        std::sprintf(buffer, ",%1.2f", acc_x);
        msg->buffer += buffer;
        std::sprintf(buffer, ",%1.2f", acc_y);
        msg->buffer += buffer;
        std::sprintf(buffer, ",%1.2f", acc_z);
        msg->buffer += buffer;
    }
    else
    {
        msg->buffer += ",,,,,";
    }
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    ESP_LOGD(FNAME, "xcv %s", msg->buffer.c_str());
    DEV::Send(msg);
}

void NmeaPrtcl::sendXcvRPYL(float roll, float pitch, float yaw, float acc_z)
{
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    Message *msg = newMessage();

    // LEVIL_AHRS  $RPYL,Roll,Pitch,MagnHeading,SideSlip,YawRate,G,errorcode,
    msg->buffer = "$RPYL,";
    char str[50];
    sprintf(str, "%d,%d,%d,0,0,%d,0",
            (int)std::roundf(roll*10.f),         // Bank == roll     (deg)
            (int)std::roundf(pitch*10.f),        // Pitch            (deg)
            (int)std::roundf(yaw*10.f),        // Magnetic Heading (deg) ?? fixme what ??
            (int)std::roundf(acc_z*1000.f));
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    DEV::Send(msg);
}

void NmeaPrtcl::sendXcvAPENV1(float ias, float alt, float te)
{
    Message *msg = newMessage();

    msg->buffer = "$APENV1,";
    char str[50];
    std::sprintf(str, "%d,%d,0,0,0,%d", (int)std::roundf(Units::kmh2knots(ias)),(int)std::roundf(Units::meters2feet(alt)),(int)std::roundf(Units::ms2fpm(te)));
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    DEV::Send(msg);
}

/*
 * $PTAS1,xxx,yyy,zzzzz,aaa*CS<CR><LF>
 * xxx:   CV or current vario. =vario*10+200 range 0-400(display +/-20.0 knots)
 * yyy:   AV or average vario. =vario*10+200 range 0-400(display +/-20.0 knots)
 * zzzzz: Barometric altitude in feet +2000
 * aaa:   TAS knots 0-200
 */
void NmeaPrtcl::sendXcvGeneric(float te, float alt, float tas)
{
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    Message *msg = newMessage();

    msg->buffer = "$PTAS1,";
    char str[50];
    sprintf(str, "%d,0,%d,%d", (int)std::roundf(Units::ms2knots(te)*10.f+200.), (int)std::roundf(Units::meters2feet(alt)+2000.f), (int)std::roundf(Units::kmh2knots(tas)) );

    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    DEV::Send(msg);
}

void NmeaPrtcl::sendXCVCrewWeight(float w)
{
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    if (XCVarioMsg::getXcvProtocolVersion() > 1) {
        Message *msg = newMessage();

        msg->buffer = "!xcv,crew-weight," + std::to_string((int)(w+0.5));
        msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
        ESP_LOGI(FNAME,"send XCV crew weight: %f", w);
        DEV::Send(msg);
    }
}


void NmeaPrtcl::sendXCVEmptyWeight(float w)
{
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    if (XCVarioMsg::getXcvProtocolVersion() > 1) {
        Message *msg = newMessage();

        msg->buffer = "!xcv,empty-weight," + std::to_string((int)(w+0.5));
        msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
        ESP_LOGI(FNAME,"send XCV empty weight: %f", w);
        DEV::Send(msg);
    }
}

void NmeaPrtcl::sendXCVWaterWeight(float w)
{
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    if (XCVarioMsg::getXcvProtocolVersion() > 1) {
        Message *msg = newMessage();

        msg->buffer = "!xcv,bal-water," + std::to_string((int)(w+0.5));
        msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
        ESP_LOGI(FNAME,"send XCV water weight: %f", w);
        DEV::Send(msg);
    }
}


void NmeaPrtcl::sendXCVVersion(int v)
{
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    Message *msg = newMessage();

    msg->buffer = "!xcv,version," + std::to_string(v);
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    ESP_LOGI(FNAME,"send XCV version: %d", v);
    DEV::Send(msg);
}

/*
    HDM - Heading - Magnetic

            1   2 3
            |   | |
    $--HDM,x.x,M*hh<CR><LF>

    Field Number:
    1) Heading Degrees, magnetic
    2) M = magnetic
    3) Checksum
*/
void NmeaPrtcl::sendXCVNmeaHDM(float heading)
{
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    Message *msg = newMessage();

    msg->buffer = "$HCHDM,";
    char str[12];
    sprintf( str,"%3.1f,M", heading );
    ESP_LOGI(FNAME,"Magnetic Heading: %3.1f", heading );
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    DEV::Send(msg);
}

/*
    HDT - Heading - True

            1   2 3
            |   | |
    $--HDT,x.x,T*hh<CR><LF>

    Field Number:
    1) Heading Degrees, true
    2) T = True
    3) Checksum
*/
void NmeaPrtcl::sendXCVNmeaHDT( float heading )
{
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    Message *msg = newMessage();

    msg->buffer = "$HCHDT,";
    char str[12];
    sprintf( str,"%3.1f,T", heading );
    ESP_LOGI(FNAME,"True Heading: %3.1f", heading );
    msg->buffer += "*" + NMEA::CheckSum(msg->buffer.c_str()) + "\r\n";
    DEV::Send(msg);
}

// send a prepared nmea telegram
void NmeaPrtcl::sendXCV(const char *str) const
{
    if ( _dl.isBinActive() ) {
        return; // no NMEA output in binary mode
    }
    Message *msg = newMessage();

    msg->buffer.assign(str);
    ESP_LOGI(FNAME,"Preped %s", str);
    DEV::Send(msg);
}


//
// seemingly unused code remains from Protocols.cpp
//
// void Protocols::sendNMEA( proto_t proto, char* str, float baro, float dp, float te, float temp, float ias, float tas,
//     float mc, int bugs, float aballast, bool cruise, float alt, bool validTemp, float acc_x, float acc_y, float acc_z, float gx, float gy, float gz ){
// if( !validTemp )
//     temp=0;
// if( proto == P_EYE_PEYA ){
//     // Static pressure from aircraft pneumatic system [hPa] (i.e. 1015.5)
//     // Total pressure from aircraft pneumatic system [hPA] (i.e. 1015.5)
//     // Pressure altitude [m] (i.e. 10260)
//     // Calculated local QNH [mbar] (i.e. 1013.2)
//     // Direction from were the wind blows [°] (0 - 359)
//     // Wind speed [km/h]
//     // True air speed [km/h] (i.e. 183)
//     // Vertical speed from anemometry (m/s) (i.e. +05.4)
//     // Outside Air Temperature (?C) (i.e. +15.2)
//     // Relative humidity [%] (i.e. 095)
//     float pa = alt - 8.92*( QNH.get() - 1013.25);


//     if( validTemp )
//         sprintf(str, "$PEYA,%.2f,%.2f,%.2f,%.2f,,,%.2f,%.2f,%.2f,,", baro, baro+(dp/100),pa, QNH.get(),tas,te,temp);
//     else
//         sprintf(str, "$PEYA,%.2f,%.2f,%.2f,%.2f,,,%.2f,%.2f,0,,", baro, baro+(dp/100),alt, QNH.get(),tas,te);
// }
// else if( proto == P_EYE_PEYI ){
//     float roll = IMU::getRoll();
//     float pitch = IMU::getPitch();
//     // ESP_LOGI(FNAME,"roll %.2f pitch %.2f yaw %.2f", roll, pitch, yaw  );
//     /*
//             $PEYI,%.2f,%.2f,,,,%.2f,%.2f,%.2f,,%.2f,
//             lbank,         // Bank == roll    (deg)           SRC
//             lpitch,         // pItch           (deg)
//             x,
//             y,
//             z,
//             );
//      */
//     sprintf(str, "$PEYI,%.2f,%.2f,,,,%.2f,%.2f,%.2f,,", roll, pitch,acc_x,acc_y,acc_z );
// }

// else {
//     ESP_LOGW(FNAME,"Not supported protocol %d", proto );
// }
// int cs = calcNMEACheckSum(&str[1]);
// int i = strlen(str);
// sprintf( &str[i], "*%02X\r\n", cs );
// // Router::sendXCV(str); fixme
// }
