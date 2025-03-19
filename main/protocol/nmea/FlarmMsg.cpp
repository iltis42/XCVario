/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "FlarmMsg.h"
#include "protocol/FlarmBin.h"
#include "Flarm.h"

#include "comm/DeviceMgr.h"

#include "logdef.h"


// The FLARM protocol parser.
//
// Supported messages:
// PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>[,<NoTrack>[,<Source>,<RSSI>]]
// PFLAE,<QueryType>,<Severity>,<ErrorCode>[,<Message>]
// PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>[,<ID>]
// PFLAX,A*2E


// PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>
// e.g.
// $PFLAA,0,-1234,1234,220,2,DD8F12,180,,30,-1.4,1*
datalink_action_t FlarmMsg::parsePFLAA(NmeaPrtcl *nmea)
{
    ESP_LOGD(FNAME, "parsePFLAA");
    return NOACTION;
}


// PFLAE,<QueryType>,<Severity>,<ErrorCode>[,<Message>]
//
// <QueryType> R = request FLARM to send status and error codes; other parameters should then be omitted
//         A = FLARM sends status (requested and spontaneous)
// <Severity> Decimal integer value. Range: from 0 to 3.
//         0 = no error, i.e. normal operation. Disregard other parameters.
//         1 = information only, i.e. normal operation
//         2 = functionality may be reduced
//         3 = fatal problem, device will not work
// <ErrorCode> Hexadecimal value. Range: from 0 to FFF. Error codes:
//         11 = Firmware expired (requires valid GPS information, i.e. will not be available in the first minute or so after power-on)
//         12 = Firmware update error
//         21 = Power (e.g. voltage < 8V)
//         22 = UI error
//         23 = Audio error
//         24 = ADC error
//         25 = SD card error
//         26 = USB error
//         27 = LED error
//         28 = EEPROM error
//         29 = General hardware error
//         2A = Transponder receiver Mode-C/S/ADS-B unserviceable
//         2B = EEPROM error
//         2C = GPIO error
//         31 = GPS communication
//         32 = Configuration of GPS module
//         33 = GPS antenna
//         41 = RF communication
//         42 = Another FLARM device with the same radio ID is being received. Alarms are suppressed for the relevant device.
//         43 = Wrong ICAO 24-bit address or radio ID
//         51 = Communication
//         61 = Flash memory
//         71 = Pressure sensor
//         81 = Obstacle database (e.g. incorrect file type)
//         82 = Obstacle database expired.
//         91 = Flight recorder
//         93 = Engine-noise recording not possible
//         94 = Range analyzer
//         A1 = Configuration error, e.g. while reading
//         flarmcfg.txt from SD/USB.
//         B1 = Invalid obstacle database license (e.g. wrong
//         serial number)
//         B2 = Invalid IGC feature license
//         B3 = Invalid AUD feature license
//         B4 = Invalid ENL feature license
//         B5 = Invalid RFB feature license
//         B6 = Invalid TIS feature license
//         100 = Generic error
//         101 = Flash File System error
//         110 = Failure updating firmware of external display
//         120 = Device is operated outside the designated region. The device does not work.
//         F1 = Other
// <Message> Field is omitted if data port version <7 or if DEVTYPE = Flarm04.
//         String. Maximum 40 ASCII characters. Textual description of the error in English. The field may be empty.

datalink_action_t FlarmMsg::parsePFLAE(NmeaPrtcl *nmea)
{
    ESP_LOGD(FNAME, "parsePFLAE");
    ProtocolState *sm = nmea->getSM();
    const std::vector<int> *word = &sm->_word_start;
    Flarm::timeout = 10;

    if ( word->size() == 3
        && sm->_frame.at(word->at(0)) == 'A' 
        && sm->_frame.at(word->at(1)) == '0' 
        && sm->_frame.at(word->at(2)) == '0' )
    {
        ESP_LOGI(FNAME, "got PFLAE");
    }
    return DO_ROUTING;
}


// PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>,<ID>
//  $PFLAU,3,1,2,1,2,-30,2,-32,755*FLARM is working properly and currently receives 3 other aircraft.
//  The most dangerous of these aircraft is at 11 o’clock, position 32m below and 755m away. It is a level 2 alarm
//
// <AcftType>
// 0 = unknown
// 1 = glider / motor glider
// 2 = tow / tug plane
// 3 = helicopter / rotorcraft
// 4 = skydiver
// 5 = drop plane for skydivers
// 6 = hang glider (hard)
// 7 = paraglider (soft)
// 8 = aircraft with reciprocating engine(s)
// 9 = aircraft with jet/turboprop engine(s)
// A = unknown
// B = balloon
// C = airship
// D = unmanned aerial vehicle (UAV)
// E = unknown
// F = static object
//
datalink_action_t FlarmMsg::parsePFLAU(NmeaPrtcl *nmea)
{
    ESP_LOGD(FNAME,"parsePFLAU");
    ProtocolState *sm = nmea->getSM();
    const std::vector<int> *word = &sm->_word_start;
    if ( word->size() < 9 ) {
        return NOACTION;
    }

    const char *s = sm->_frame.c_str();
    Flarm::RX          = atoi(s + word->at(0));
    Flarm::TX          = atoi(s + word->at(1));
    Flarm::GPS         = atoi(s + word->at(2));
    Flarm::Power       = atoi(s + word->at(3));
    Flarm::AlarmLevel  = atoi(s + word->at(4));
    Flarm::RelativeBearing  = atoi(s + word->at(5));
    Flarm::RelativeVertical = atoi(s + word->at(6));
    Flarm::RelativeDistance = atoi(s + word->at(7));
    sprintf( Flarm::ID, "%06x", atoi(s + word->at(8)));
    ESP_LOGI(FNAME,"RB: %d ALT:%d  DIST %d", Flarm::RelativeBearing, Flarm::RelativeVertical, Flarm::RelativeDistance);
    Flarm::_tick=0;
    Flarm::timeout = 10;
    return DO_ROUTING;
}


// $PFLAX,A*2E
//
// Note, if the Flarm switch to binary mode was accepted, Flarm will answer
// with $PFLAX,A*2E. In error case you will get as answer $PFLAX,A,<error>*
// and the Flarm stays in text mode.
datalink_action_t FlarmMsg::parsePFLAX(NmeaPrtcl *nmea)
{
    ESP_LOGI(FNAME,"parsePFLAX A ----------------> switch to binary");
    ProtocolState *sm = nmea->getSM();
    const std::vector<int> *word = &sm->_word_start;

    if ( word->size() == 2 && *(sm->_frame.c_str() + word->at(0)) == 'A' ) {
        // this is the confirmation from flarm to go binary
        DataLink *host = DEVMAN->getFlarmHost();
        if ( host && host->getProtocol(FLARMBIN_P) && nmea->getDL()->getProtocol(FLARMBIN_P)) {
            // Host side
            FlarmBinary *hostfb = static_cast<FlarmBinary*>(host->goBIN());
            // Device side
            FlarmBinary *devfb = static_cast<FlarmBinary*>(nmea->getDL()->goBIN());
            // Cross link them
            devfb->setPeer(hostfb);
            hostfb->setPeer(devfb);
        }
        Flarm::timeout = 10;
        return DO_ROUTING;
    }
    return NOACTION;
}


ConstParserMap FlarmMsg::_pm = {
    { Key("FLAA"), FlarmMsg::parsePFLAA },
    { Key("FLAE"), FlarmMsg::parsePFLAE },
    { Key("FLAU"), FlarmMsg::parsePFLAU },
    { Key("FLAX"), FlarmMsg::parsePFLAX }
};


