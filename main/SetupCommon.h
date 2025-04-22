/*
 * SetupCommon.h
 *
 *  Created on: August 23, 2020
 *      Author: iltis
 */

#pragma once

#include "protocol/nmea/XCVSyncMsg.h"
#include <vector>

struct httpd_req;

class SetupCommon {
public:
	SetupCommon();
	virtual ~SetupCommon();

	// virtual methods to be implemented in derived class
	virtual bool init() = 0;
	virtual bool erase() = 0;
	virtual bool write() = 0;
	virtual bool commit() = 0;
	virtual void setValueStr( const char * val ) = 0;
	virtual bool mustReset() = 0;
	virtual bool isDefault() = 0;
	virtual bool value_str(char *str) = 0;
	virtual const char* key() = 0;
	virtual char typeName() = 0;
	virtual bool sync() = 0;
	virtual bool dirty() = 0;
	virtual uint8_t getSync() = 0;
	// virtual char* showSetting( bool nondefault=true ) = 0;

	static bool initSetup( bool &present );  // returns false if FLASH was completely blank
	static char *getID();
	static char *getDefaultID();
	static const char *getFixedID();
	static SetupCommon * getMember( const char * key );
	static bool syncEntry( int entry );
	static int numEntries();
	static bool factoryReset();
	static void giveConfigChanges( httpd_req *req, bool log_only=false );
	static int restoreConfigChanges( int len, char *data );

	// housekeeping supporters
	static void setSyncProto(XCVSyncMsg *sp) { syncProto = sp; }
	static bool isMaster();
	static bool isClient();
    static bool isWired();
    static void commitDirty();

	static bool haveWLAN();

    // variables
    static std::vector<SetupCommon *> *instances;
	static XCVSyncMsg *syncProto;

protected:

private:
	static char _ID[16];
	static char default_id[6];
};

