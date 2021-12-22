/*
 * SetupCommon.h
 *
 *  Created on: August 23, 2020
 *      Author: iltis
 */

#pragma once

#include "BTSender.h"
#include "Polars.h"
#include "MPU.hpp" // change from .h to .hpp for Windows toolchain compatibility

#include <esp_partition.h>
#include <esp_err.h>
#include <nvs_flash.h>
#include <nvs.h>

#include <freertos/FreeRTOS.h>
#include <esp_timer.h>
#include <freertos/queue.h>
#include <esp_system.h>
#include <esp_log.h>

#include <string>
#include <stdio.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <vector>
#include "Compass.h"
#include "SetupNG.h"



class SetupCommon {
public:
	SetupCommon() { memset( _ID, 0, sizeof( _ID )); }
	virtual ~SetupCommon() {};
	virtual bool init() = 0;
	virtual bool erase() = 0;
	virtual bool mustReset() = 0;
	virtual const char* key() = 0;
	virtual char typeName() = 0;
	virtual bool sync() = 0;
	virtual uint8_t getSync() = 0;
	static std::vector<SetupCommon *> entries;
	static bool initSetup( bool &present );  // returns false if FLASH was completely blank
	static char *getID();
	static void sendSetup( uint8_t sync, const char * key, char type, void *value, int len, bool ack=false );
	static SetupCommon * getMember( const char * key );
	static bool syncEntry( int entry );
	static int numEntries() { return entries.size(); };
	static bool factoryReset();
	static bool isMaster();
	static bool isClient();
    static bool isWired();
    static bool mustSync( uint8_t sync);
	static bool haveWLAN();
    static bool lazyCommit;
    static bool commitNow();
    static QueueHandle_t commitSema;

protected:
    static bool open(nvs_handle_t &h) {
        esp_err_t err = nvs_open("SetupNG", NVS_READWRITE, &h);
        if(err != ESP_OK){
            ESP_LOGE(FNAME,"ESP32NVS open storage error %d", err );
            h = 0;
            return( false );
        }
        else {
            // ESP_LOGI(FNAME,"ESP32NVS handle: %04X  OK", _nvs_handle );
            return( true );
        }
	};
	static void close(nvs_handle_t &h) {
		if( h != 0) {
			nvs_close(h);
			h = 0;
		}
	};
private:
    static esp_timer_handle_t _timer;
    static bool _dirty;
	static char _ID[14];
};

