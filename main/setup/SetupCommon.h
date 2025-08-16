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
	bool _volatile :1; // none nvs, run-time (black board) item only
	uint8_t _sync  :2; // sync mode with client device
	uint8_t _quant :3; // quantity
	bool _dirty    :1; // has changed
};

class SetupCommon {
public:
	SetupCommon(const char *k);
	virtual ~SetupCommon();

	// virtual methods to be implemented in derived class
	virtual char typeName() const = 0;
	virtual std::string getValueAsStr() const = 0;
	virtual void setValueFromStr( const char * val ) = 0;
	virtual void* getPtr() = 0;
	virtual int getSize() = 0;
	virtual bool isDefault() = 0;
	virtual void setDefault() = 0;
	virtual bool isValid() const = 0;

	bool init();
	bool erase();
	bool write();
	bool exists() const ;
	bool commit();
	bool mustReset() const { return flags._reset; }
	const char* key() const { return _key.data(); }
	bool sync();
	bool getDirty() const { return flags._dirty; }
	void setDirty() { flags._dirty = true; }
	uint8_t getSync() const { return flags._sync; }

	static bool initSetup();  // returns false if at least one entry was blank
	static char *getID();
	static char *getDefaultID(bool enforce_four_diggits = false);
	static const char *getFixedID();
	static SetupCommon * getMember( const char * key );
	static bool syncEntry( int entry );
	static int numEntries();
	static void giveConfigChanges( httpd_req *req, bool log_only=false );
	static int restoreConfigChanges( int len, char *data );

	// housekeeping supporters
	static void setSyncProto(XCVSyncMsg *sp) { syncProto = sp; }
	static bool isMaster();
	static bool isClient();
    static bool isWired();
	static bool haveWLAN();
    static void commitDirty();
    static void prepareFactoryReset();

    // variables
protected:
	const std::string_view _key; // unique identification TAG
	t_setup_flags flags = {false, false, 0, 0, false};
	void (*_action)(); // action on a value change

private:
	static XCVSyncMsg *syncProto;
	static bool factoryReset();
	static std::vector<SetupCommon *> instances;
	static char _ID[18];
	static char default_id[6];
};
