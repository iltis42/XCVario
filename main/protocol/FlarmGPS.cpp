/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "FlarmGPS.h"
#include "Flarm.h"

#include "nmea_util.h"
#include "comm/Messages.h"
#include "SetupNG.h"
#include "sensor.h"

#include <logdef.h>

#include <cstring>
#include <time.h>
#include <sys/time.h>


static void ageBincom()
{
    if (Flarm::bincom > 0)
    {
        Flarm::bincom--;
        ESP_LOGI(FNAME, "Flarm::bincom %d", Flarm::bincom);
    }
}

// The FLARM protocol parser.
//
// Supported messages:
// GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
// GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh
// PGRMZ,<Value>,F,2
// PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>[,<NoTrack>[,<Source>,<RSSI>]]
// PFLAE,<QueryType>,<Severity>,<ErrorCode>[,<Message>]
// PFLAU,<RX>,<TX>,<GPS>,<Power>,<AlarmLevel>,<RelativeBearing>,<AlarmType>,<RelativeVertical>,<RelativeDistance>[,<ID>]
// PFLAX,A*2E

gen_state_t FlarmGPS::nextByte(const char c)
{
    int pos = _sm._frame.size() - 1; // c already in the buffer
    ESP_LOGD(FNAME, "state %d, pos %d next char %c", _sm._state, pos, c);
    switch(_sm._state) {
    case START_TOKEN:
        if ( c == '$' ) {
            _sm._state = HEADER;
            _crc_buf[0] = '\0';
            _word_start.clear();
            ESP_LOGD(FNAME, "Msg START_TOKEN");
        }
        break;
    case HEADER:
        NMEA::incrCRC(_sm._crc,c);
        if ( pos < 4 ) { break; }
        _header_len = 3;
        if ( _sm._frame.substr(1,2) != "GP" ) { // GPS NMEA sender id
            _header_len = 4;
            if ( _sm._frame.substr(1,3) != "PFL" // Flarm sender id
                && _sm._frame.substr(1,3) != "PGR" ) { // Garmin sender id
                break;
            }
        }
        _sm._state = PAYLOAD;
        ESP_LOGD(FNAME, "Msg HEADER");
        break;
    case PAYLOAD:
        if ( c == ',' ) {
            _word_start.push_back(pos+1); // another word start
        }
        if ( c == '*' ) {
            _word_start.push_back(pos+1);
            _sm._state = CHECK_CRC; // Expecting a CRC to check
            break;
        }
        if ( c != '\r' && c != '\n' ) {
            ESP_LOGD(FNAME, "Msg PAYLOAD");
            NMEA::incrCRC(_sm._crc,c);
            break;
        }
        else {
            _sm._state = STOP_TOKEN;
        }
        // Fall through 
    case CHECK_CRC:
        if( _sm._state == CHECK_CRC ) { // did we not fall through
            if ( _crc_buf[0] == '\0' ) {
                // this is the first crc character
                _crc_buf[0] = c;
                break;
            }
            else {
                _crc_buf[1] = c;
                _crc_buf[2] = '\0';
                char read_crc = (char)strtol(_crc_buf, NULL, 16);
                ESP_LOGD(FNAME, "Msg CRC %s/%x - %x", _crc_buf, read_crc, _sm._crc);
                if ( read_crc != _sm._crc ) {
                    _sm._state = START_TOKEN;
                    break;
                }
                _sm._state = CHECK_OK; // fall through
            }
        }
        // Fall through 
    case STOP_TOKEN:
    case CHECK_OK:
    case COMPLETE:
        {
            _sm._frame.push_back('\0'); // terminate the string buffer
            gen_state_t next_state = START_TOKEN; // restart parsing
            ESP_LOGI(FNAME, "Msg complete %c%c%c", _sm._frame.at(_header_len), _sm._frame.at(_header_len+1), _sm._frame.at(_header_len+2));
            int32_t mid = (_sm._frame.at(_header_len)<<8) + _sm._frame.at(_header_len+1);
            if ( _sm._frame.at(_header_len+2) != ',' ) {
                mid <<= 8;
                mid |= _sm._frame.at(_header_len+2);
            }
            switch (mid) {
                case (('R' << 16) | ('M' << 8) | 'C'):
                    parseGPRMC();
                    ageBincom();
                    break;
                case (('G' << 16) | ('G' << 8) | 'A'):
                    parseGPGGA();
                    ageBincom();
                    break;
                case (('M' << 8) | 'Z'):
                    parsePGRMZ();
                    ageBincom();
                    break;
                case (('A' << 8) | 'E'):
                    parsePFLAE();
                    ageBincom();
                    break;
                case (('A' << 8) | 'U'):
                     parsePFLAU();
                     ageBincom();
                    break;
                case (('A' << 8) | 'X'):
                    if ( parsePFLAX() ) {
                        next_state = GO_BINARY;
                    }
                    break;
                default:
                    break;
            }
            _sm._state = next_state;
        }
        break;
    default:
        break;
    }
    return _sm._state;
}


// $GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
// $GPRMC,225446,A,4916.45,N,12311.12,W,000.5,054.7,191194,020.3,E*68
// $GPRMC,201914.00,A,4857.58740,N,00856.94735,E,0.172,122.95,310321,,,A*6D
//
//            225446.00    Time of fix 22:54:46 UTC
//            A            Navigation receiver warning A = OK, V = warning
//            4916.45,N    Latitude 49 deg. 16.45 min North
//            12311.12,W   Longitude 123 deg. 11.12 min West
//            000.5        Speed over ground, Knots
//            054.7        Course Made Good, True
//            191194       Date of fix  19 November 1994
//            020.3,E      Magnetic variation 20.3 deg East
//  *68          mandatory checksum
void FlarmGPS::parseGPRMC()
{
    if ( _word_start.size() < 9 ) {
        return;
    }
    ESP_LOGI(FNAME, "parseGPRMC");
    struct tm t;
    const char *s = _sm._frame.c_str();
    int valid_time_scan = sscanf( s + _word_start[0], "%02d%02d%02d", &t.tm_hour, &t.tm_min, &t.tm_sec );
    char warn = *(s + _word_start[1]);
    sscanf( s + _word_start[6], "%f", &Flarm::gndSpeedKnots );
    sscanf( s + _word_start[7], "%f", &Flarm::gndCourse );
    int valid_date_scan = sscanf( s + _word_start[8],"%02d%02d%02d", &t.tm_mday, &t.tm_mon, &t.tm_year );
    t.tm_year +=100;
    ESP_LOGI(FNAME,"SC: %d/%d/%d %02d:%02d:%02d ", t.tm_year, t.tm_mon, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec );

    // ESP_LOGI(FNAME,"G: %s",_gprmc );
    // ESP_LOGI(FNAME,"DT: %d/%02d/%02d %02d:%02d:%02d ", now.tm_year-100, now.tm_mon, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec );
    // ESP_LOGI(FNAME,"parseGPRMC() GPS: %d, Speed: %3.1f knots, Track: %3.1f° warn:%c date:%s ", myGPS_OK, gndSpeedKnots, gndCourse, warn, date  );
    // ESP_LOGI(FNAME,"GP%s, GPS_OK:%d warn:%c T:%s D:%s", gprmc+3, myGPS_OK, warn, time, date  );

    if (warn == 'A')
    {
        if (Flarm::myGPS_OK == false)
        {
            Flarm::myGPS_OK = true;
            if (wind_enable.get() & WA_STRAIGHT || wind_enable.get() & WA_CIRCLING)
            {
                CircleWind::gpsStatusChange(true);
            }
            ESP_LOGI(FNAME, "GPRMC, GPS status changed to good, gps:%d", Flarm::myGPS_OK);
        }
        theWind.calculateWind();
        // ESP_LOGI(FNAME,"Track: %3.2f, GPRMC: %s", gndCourse, gprmc );
        CircleWind::newSample(Vector(Flarm::gndCourse, Units::knots2kmh(Flarm::gndSpeedKnots)));
        if (!Flarm::time_sync && (valid_time_scan && valid_date_scan))
        {
            ESP_LOGI(FNAME, "Start TimeSync");
            long int epoch_time = mktime(&t);
            timeval epoch = {epoch_time, 0};
            const timeval *tv = &epoch;
            timezone utc = {0, 0};
            const timezone *tz = &utc;
            settimeofday(tv, tz);
            Flarm::time_sync = true;
            ESP_LOGI(FNAME, "Finish Time Sync");
        }
    }
    else
    {
        if (Flarm::myGPS_OK == true)
        {
            Flarm::myGPS_OK = false;
            ESP_LOGI(FNAME, "GPRMC, GPS status changed to bad, gps:%d", Flarm::myGPS_OK);
            if (wind_enable.get() & WA_STRAIGHT || wind_enable.get() & WA_CIRCLING)
            {
                CircleWind::gpsStatusChange(false);
                ESP_LOGW(FNAME, "GPRMC, GPS not OK.");
            }
        }
    }
    Flarm::timeout = 10;
    // ESP_LOGI(FNAME,"parseGPRMC() GPS: %d, Speed: %3.1f knots, Track: %3.1f° ", myGPS_OK, gndSpeedKnots, gndCourse );
}


// eg. $GPGGA,121318.00,4857.58750,N,00856.95715,E,1,05,3.87,247.7,M,48.0,M,,*52
//
// hhmmss.ss = UTC of position
// llll.ll = latitude of position
// a = N or S
// yyyyy.yy = Longitude of position
// a = E or W
// x = GPS Quality indicator (0=no fix, 1=GPS fix, 2=Dif. GPS fix)
// xx = number of satellites in use
// x.x = horizontal dilution of precision
// x.x = Antenna altitude above mean-sea-level
// M = units of antenna altitude, meters
// x.x = Geoidal separation
// M = units of geoidal separation, meters
// x.x = Age of Differential GPS data (seconds)
// xxxx = Differential reference station ID
//
extern unsigned long _gps_millis;

void FlarmGPS::parseGPGGA()
{
    ESP_LOGI(FNAME, "parseGPGGA");
    _gps_millis = millis();

    if (_word_start.size() > 13)
    {
        int numSat = atoi(_sm._frame.c_str() + _word_start[6]);
        ESP_LOGI(FNAME, "numSat=%d", numSat);
        if ((numSat != Flarm::_numSat) && (wind_enable.get() != WA_OFF))
        {
            Flarm::_numSat = numSat;
            CircleWind::newConstellation(numSat);
        }
        Flarm::timeout = 10;
    }
}

// $PGRMZ,880,F,2*3A
//
void FlarmGPS::parsePGRMZ()
{
    ESP_LOGI(FNAME, "parsePGRMZ");
    if (alt_select.get() != AS_EXTERNAL) {
        return;
    }
    if ( _word_start.size() == 4 && _sm._frame.at(_word_start[1]) == 'F' ) {
        int alt1013_ft = atoi(_sm._frame.c_str()+_word_start[0]);
        alt_external = Units::feet2meters((float)(alt1013_ft + 0.5));
        ESP_LOGI(FNAME, "PGRMZ %d: ALT(1013):%5.0f m", alt1013_ft, alt_external);
        Flarm::timeout = 10;
        Flarm::ext_alt_timer = 10; // Fall back to internal Barometer after 10 seconds
    }
}


// PFLAA,<AlarmLevel>,<RelativeNorth>,<RelativeEast>,<RelativeVertical>,<IDType>,<ID>,<Track>,<TurnRate>,<GroundSpeed>,<ClimbRate>,<AcftType>
// e.g.
// $PFLAA,0,-1234,1234,220,2,DD8F12,180,,30,-1.4,1*
void FlarmGPS::parsePFLAA()
{
    ESP_LOGI(FNAME, "parsePFLAA");
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

void FlarmGPS::parsePFLAE()
{
    ESP_LOGI(FNAME, "parsePFLAE");
    Flarm::timeout = 10;

    if ( !SetupCommon::isClient() 
        && _word_start.size() == 3
        && _sm._frame.at(_word_start[0]) == 'A' 
        && _sm._frame.at(_word_start[1]) == '0' 
        && _sm._frame.at(_word_start[2]) == '0' )
    {
        ESP_LOGI(FNAME, "got PFLAE");
    }
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
void FlarmGPS::parsePFLAU()
{
    ESP_LOGI(FNAME,"parsePFLAU");
    if ( _word_start.size() < 9 ) {
        return;
    }

    const char *s = _sm._frame.c_str();
    Flarm::RX          = atoi(s + _word_start[0]);
    Flarm::TX          = atoi(s + _word_start[1]);
    Flarm::GPS         = atoi(s + _word_start[2]);
    Flarm::Power       = atoi(s + _word_start[3]);
    Flarm::AlarmLevel  = atoi(s + _word_start[4]);
    Flarm::RelativeBearing  = atoi(s + _word_start[5]);
    Flarm::RelativeVertical = atoi(s + _word_start[6]);
    Flarm::RelativeDistance = atoi(s + _word_start[7]);
    sprintf( Flarm::ID, "%06x", atoi(s + _word_start[8]));
    ESP_LOGI(FNAME,"RB: %d ALT:%d  DIST %d", Flarm::RelativeBearing, Flarm::RelativeVertical, Flarm::RelativeDistance);
    Flarm::_tick=0;
    Flarm::timeout = 10;
}

// $PFLAX,A*2E
//
// Note, if the Flarm switch to binary mode was accepted, Flarm will answer
// with $PFLAX,A*2E. In error case you will get as answer $PFLAX,A,<error>*
// and the Flarm stays in text mode.
bool FlarmGPS::parsePFLAX()
{
    ESP_LOGI(FNAME,"parsePFLAX");

    if( _word_start.size() == 2 && *(_sm._frame.c_str()+_word_start[0]) == 'A' 
        && !SetupCommon::isClient() ) {
        return true;
        Flarm::timeout = 10;
    }
    return false;
}

