/*
 * SetupCommon.h
 *
 *  Created on: August 23, 2020
 *      Author: iltis
 */

#pragma once

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
#include "SetupNG.h"

class SetupCommon {
public:
	SetupCommon();
	virtual ~SetupCommon();

	// virtual methods to be implemented in derived class
	virtual bool init() = 0;
	virtual bool erase() = 0;
	virtual bool mustReset() = 0;
	virtual const char* key() = 0;
	virtual char typeName() = 0;
	virtual bool sync() = 0;
	virtual uint8_t getSync() = 0;

	static bool initSetup( bool &present );  // returns false if FLASH was completely blank
	static char *getID();
	static void sendSetup( uint8_t sync, const char * key, char type, void *value, int len, bool ack=false );
	static SetupCommon * getMember( const char * key );
	static bool syncEntry( int entry );
	static int numEntries();
	static bool factoryReset();

	// housekeeping supporters
	static bool isMaster();
	static bool isClient();
	static bool isCanClient();
	static bool isCanMaster();
    static bool isWired();

    static bool mustSync( uint8_t sync);
	static bool haveWLAN();
    static bool lazyCommit;
    static bool commitNow();

    // variables
    static std::vector<SetupCommon *> *instances;
    static QueueHandle_t commitSema;

protected:
    static bool open(nvs_handle_t &h);
	static void close(nvs_handle_t &h);

private:
	static void timeout(QueueHandle_t arg);
    static esp_timer_handle_t _timer;
    static bool _dirty;
	static char _ID[14];
};

