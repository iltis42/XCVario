/***********************************************************
 ***   THIS DOCUMENT CONTAINS PROPRIETARY INFORMATION.   ***
 ***    IT IS THE EXCLUSIVE CONFIDENTIAL PROPERTY OF     ***
 ***     Rohs Engineering Design AND ITS AFFILIATES.     ***
 ***                                                     ***
 ***       Copyright (C) Rohs Engineering Design         ***
 ***********************************************************/

#pragma once

#include "ProtocolItf.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>
#include <map>
#include <algorithm>


// Assuming that all seen NMEA message id's last 4 chars are different,
// this key builds up a hash table to retrieve the proper message parser.
union Key {
    char str[4];
    uint32_t value;

    static inline bool isNotAsciiLetter(char c) {
        return (unsigned)(c - 'A') > ('Z' - 'A') && (unsigned)(c - 'a') > ('z' - 'a');
    }

    Key() = default;
    constexpr Key(const uint32_t v) : value(v) {}
    // Initialize and truncate to the right most four chars
    constexpr Key(const char* s) : value(0) {
        for (int i=0; i<4; i++) {
            if ( isNotAsciiLetter(s[i]) ) break;
            value = (value<<8) | s[i];
        }
    }

    inline bool shiftIn(char c) {
        if ( isNotAsciiLetter(c) ) { return true; } // finished the header
        value = (value<<8) | c;
        return false;
    }

    std::string toString() const {
        std::string rev;
        for (int last = 3; last>=0; last--) {
            if (str[last] != '\0') {
                rev.push_back(str[last]);
            }
        }
        return rev;
    }

    // comparator for the std::map
    bool operator<(const Key& other) const {
        return value < other.value;
    }
};

// message table
class NmeaPrtcl;
class NmeaPlugin;
using NmeaMessageParser = std::function<dl_action_t(NmeaPlugin*)>;
typedef std::pair<Key, NmeaMessageParser> ParserEntry; // use const to reside in flash memory
typedef std::pair<NmeaMessageParser, NmeaPlugin*> MapValue;
typedef std::map<Key, MapValue> ParserMap;

// nmea message extension
class NmeaPlugin
{
public:
    explicit NmeaPlugin(NmeaPrtcl &nr, ProtocolType ptyp, bool as=true);
    virtual ~NmeaPlugin() = default;
    ProtocolType getPtyp() const { return _pid; }
    DeviceId getRouteId() const { return _routeId; };
    void setRouteId(DeviceId did) { _routeId = did; }
    NmeaPrtcl &getNMEA() const { return _nmeaRef; }
    void setExplicit() { _auto = false; }
    bool getAuto() const { return _auto; }
    bool operator==(const NmeaPlugin& other) const {
        return _pid == other._pid;
    }
    // API
    virtual const ParserEntry* getPT() const = 0;

protected:
    // access to state machine and buffers for the parse routines
    NmeaPrtcl &_nmeaRef;
    ProtocolType _pid;
private:
    bool _auto; // most plugins get installed together with the principal NMEA parser
    DeviceId _routeId = NO_DEVICE; // the device id for the plugin's routing
};



// generic mnea message parser
class NmeaPrtcl final : public ProtocolItf
{
public:
    explicit NmeaPrtcl(DeviceId did, int mp, ProtocolType pt, ProtocolState &sm, DataLink &dl) : ProtocolItf(did, mp, sm, dl), _ptyp(pt) {};
    virtual ~NmeaPrtcl();

public:
    // general API
    ProtocolType getProtocolId() const override { return _ptyp; }
    bool hasProtocol(ProtocolType p) const override;
    void addAliveMonitor(AliveMonitor *am) override { _alive = am; }
    dl_control_t nextBytes(const char* c, int len) override;
    // house keeping
    void addPlugin(NmeaPlugin *pm);
    void removeProtocol(ProtocolType p);
    int getNrPlugs() const { return _plugs.size(); }
    const std::vector<NmeaPlugin*>& getAllPlugs() const { return _plugs; }
    ProtocolItf* asPrtclItfPtr() { return static_cast<ProtocolItf*>(this); }


    // XCVario transmitter routines
    void sendStdXCVario(float baro, float dp, float te, float temp, float ias, float tas,
        float mc, int bugs, float aballast, bool cruise, float alt, bool validTemp, 
        float acc_x, float acc_y, float acc_z, float gx, float gy, float gz);
    void sendXcvRPYL(float roll, float pitch, float yaw, float acc_z);
    void sendXcvAPENV1(float ias, float alt, float te);
    void sendXcvGeneric(float te, float alt, float tas);
    void sendOpenVario(float baro, float dp, float te, float temp, bool validTemp);
    void sendBorgelt(float te, float temp, float ias, float tas, float mc, int bugs, float aballast, bool cruise, bool validTemp);
    void sendCambridge(float te, float tas, float mc, int bugs, float alt);
    void sendXCVCrewWeight(float w);
    void sendXCVEmptyWeight(float w);
    void sendXCVWaterWeight(float v);
    void sendXCVVersion(int v);
    void sendXCVNmeaHDM(float heading);
    void sendXCVNmeaHDT(float heading);
    void sendXCV(const char *str) const;

    // MagSens transmitter
    bool sendHello();
    bool sendCalibration(); // Todo add calib param
    bool startStream(int choice);
    bool killStream();
    bool prepareUpdate(int len, int pack);
    bool firmwarePacket(const char *buf, int len);
    int waitConfirmation();
    
    // JumboCmd transmitter
    bool sendJPConnect();
    bool sendJPGetConfig(const int index);
    bool sendJPGetInfo();
    bool sendJPSelectConfig(const int wingconfig); // 0=full, 1=reduced, 2=config
    bool sendJPStartWipe(const int side); // 0=Right, 1=Left
    bool sendJPAbortWipe(const int side);
    bool sendJPShortPress(const int side);
    bool sendJPHoldPressed(const int side);
    bool sendJPReleasePressed(const int side);
    
private:
    const ProtocolType _ptyp;   // a protocol id different per instance
    ParserMap _parsmap;
    std::vector<NmeaPlugin*> _plugs;
    Key      _mkey;     // identified message key
    MapValue _parser;   // found parser, incl. parameter for the parser
    inline void nmeaIncrCRC(int &crc, const char c) {crc ^= c;}
    AliveMonitor *_alive = nullptr; // alive monitor for the protocol
};

extern NmeaPrtcl *ToyNmeaPrtcl;

