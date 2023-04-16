/*
 * BTAdvertisedDevice.h
 *
 *  Created on: Feb 5, 2021
 *      Author: Thomas M. (ArcticSnowSky)
 */

#ifndef __BTADVERTISEDDEVICE_H__
#define __BTADVERTISEDDEVICE_H__

#include "BTAddress.h"


class BTAdvertisedDevice {
public:
    // virtual ~BTAdvertisedDevice() {};

    virtual BTAddress   getAddress() = 0;
    virtual uint32_t    getCOD() = 0;
    virtual std::string getName() = 0;
    virtual int8_t      getRSSI() = 0;


    virtual bool        haveCOD() = 0;
    virtual bool        haveName() = 0;
    virtual bool        haveRSSI() = 0;

    virtual std::string toString() = 0;
};

class BTAdvertisedDeviceSet : public BTAdvertisedDevice {
public:
    BTAdvertisedDeviceSet();
    //~BTAdvertisedDeviceSet() = default;
    

    BTAddress   getAddress();
    uint32_t    getCOD();
    std::string getName();
    int8_t      getRSSI();


    bool        haveCOD();
    bool        haveName();
    bool        haveRSSI();

    std::string toString();

    void setAddress(BTAddress address);
    void setCOD(uint32_t cod);
    void setName(std::string name);
    void setRSSI(int8_t rssi);

    bool m_haveCOD;
    bool m_haveName;
    bool m_haveRSSI;


    BTAddress   m_address = BTAddress((uint8_t*)"\0\0\0\0\0\0");
    uint32_t    m_cod;
    std::string m_name;
    int8_t      m_rssi;
};

#endif
