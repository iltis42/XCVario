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

struct t_setup_flags {
	bool _reset    :1; // reset data on factory reset
	bool _volatile :1;
	uint8_t _sync  :2; // sync mode with client device
	uint8_t _unit  :3;
	bool _dirty    :1;
};

class SetupCommon {
public:
	SetupCommon();
	virtual ~SetupCommon();

	// virtual methods to be implemented in derived class
	virtual bool init() = 0;
	bool erase();
	virtual bool write() = 0;
	bool exists() const ;
	bool commit();
	virtual void setDefault() = 0;
	virtual void setValueFromStr( const char * val ) = 0;
	bool mustReset() const { return flags._reset; }
	virtual bool isDefault() = 0;
	virtual std::string valueAsStr() const = 0;
	const char* key() const { return _key.data(); }
	virtual char typeName() const = 0;
	virtual bool sync() = 0;
	bool dirty() const { return flags._dirty; }
	void setDirty() { flags._dirty = true; }
	uint8_t getSync() const { return flags._sync; }

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
	static bool haveWLAN();
    static void commitDirty();

    // variables
	static XCVSyncMsg *syncProto;

protected:
	std::string_view _key; // unique identification TAG
	t_setup_flags flags = {false, false, 0, 0, false};
	void (*_action)(); // action on a value change

private:
	static std::vector<SetupCommon *> instances;
	static char _ID[16];
	static char default_id[6];
};

