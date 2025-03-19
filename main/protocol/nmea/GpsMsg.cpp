/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#include "GpsMsg.h"
#include "Flarm.h"
#include "sensor.h"

#include "logdefnone.h"

#include <cstring>
#include <time.h>
#include <sys/time.h>


// The GPS protocol parser.
//
// Supported messages:
// GPRMC,081836,A,3751.65,S,14507.36,E,000.0,360.0,130998,011.3,E*62
// GPGGA,hhmmss.ss,llll.ll,a,yyyyy.yy,a,x,xx,x.x,x.x,M,x.x,M,x.x,xxxx*hh


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
datalink_action_t GpsMsg::parseGPRMC(NmeaPrtcl *nmea)
{
    ProtocolState *sm = nmea->getSM();
    const std::vector<int> *word = &sm->_word_start;
    if ( word->size() < 9 ) {
        return NOACTION;
    }
    ESP_LOGD(FNAME, "parseGPRMC");

    struct tm t;
    const char *s = sm->_frame.c_str();
    int valid_time_scan = sscanf( s + word->at(0), "%02d%02d%02d", &t.tm_hour, &t.tm_min, &t.tm_sec );
    char warn = *(s + word->at(1));
    sscanf( s + word->at(6), "%f", &Flarm::gndSpeedKnots );
    sscanf( s + word->at(7), "%f", &Flarm::gndCourse );
    int valid_date_scan = sscanf( s + word->at(8),"%02d%02d%02d", &t.tm_mday, &t.tm_mon, &t.tm_year );
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
            ESP_LOGD(FNAME, "Start TimeSync");
            long int epoch_time = mktime(&t);
            timeval epoch = {epoch_time, 0};
            const timeval *tv = &epoch;
            timezone utc = {0, 0};
            const timezone *tz = &utc;
            settimeofday(tv, tz);
            Flarm::time_sync = true;
            ESP_LOGD(FNAME, "Finish Time Sync");
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
    return DO_ROUTING;
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

datalink_action_t GpsMsg::parseGPGGA(NmeaPrtcl *nmea)
{
    ProtocolState *sm = nmea->getSM();
    const std::vector<int> *word = &sm->_word_start;
    ESP_LOGD(FNAME, "parseGPGGA");

    _gps_millis = millis();

    if (word->size() > 13)
    {
        int numSat = atoi(sm->_frame.c_str() + word->at(6));
        ESP_LOGI(FNAME, "numSat=%d", numSat);
        if ((numSat != Flarm::_numSat) && (wind_enable.get() != WA_OFF))
        {
            Flarm::_numSat = numSat;
            CircleWind::newConstellation(numSat);
        }
        Flarm::timeout = 10;
    }
    return DO_ROUTING;
}

ConstParserMap GpsMsg::_pm = {
    { Key("PRMC"), GpsMsg::parseGPRMC },
    { Key("PGGA"), GpsMsg::parseGPGGA }
};


