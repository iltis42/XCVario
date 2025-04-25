/*
 * Setup.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#include "SetupCommon.h"

#include "SetupNG.h"
#include "sdkconfig.h"
#include "ESP32NVS.h"
#include "sensor.h"
#include "comm/DeviceMgr.h"
#include "comm/CanBus.h"
#include "logdef.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_http_server.h>
#include <miniz.h>
#include <esp_mac.h>

#include <cstdio>
#include <string>
#include <sstream>

char SetupCommon::_ID[16] = { 0 };
char SetupCommon::default_id[6] = { 0 };
std::vector<SetupCommon *> SetupCommon::instances;
XCVSyncMsg *SetupCommon::syncProto = nullptr;


SetupCommon::SetupCommon(const char *k) :
	_key(k)
{
	instances.push_back( this );  // add into vector of setup vars
}

SetupCommon::~SetupCommon() {
	for (auto it = instances.begin(); it != instances.end(); ++it) {
		if ( *it == this ) {
			instances.erase(it);
			break;
		}
	}
}

bool SetupCommon::erase() {
	if( flags._volatile != PERSISTENT ){
		return true;
	}
	bool ret = NVS.erase(_key.data());
	if( !ret ){
		return false;
	}
	else {
		// ESP_LOGI(FNAME,"NVS erased key  %s", _key.data());
		setDefault();
		return true;
	}
}

bool SetupCommon::exists() const {
	if( flags._volatile != PERSISTENT ) {
		return true;
	}
	size_t size;
	bool ret = NVS.getBlob(_key.data(), nullptr, &size);
	return ret;
}

bool SetupCommon::commit() {
	// ESP_LOGI(FNAME,"NVS commit(): %s ", _key.data());
	if( flags._volatile != PERSISTENT ){
			return true;
	}
	write();
	bool ret = NVS.commit();
	if( !ret ) {
		return false;
	}
	flags._dirty = false;
	return true;
}

SetupCommon *SetupCommon::getMember( const char * key ){
	for(int i = 0; i < instances.size(); i++ ) {
		if( std::string(key) == instances[i]->key() ){
			// ESP_LOGI(FNAME,"found key %s", instances[i]->key() );
			return instances[i];
		}
	}
	return nullptr;
}

// at time of connection establishment
bool SetupCommon::syncEntry( int entry ){
    if( entry < instances.size() ) {
        return instances[entry]->sync();
    }
    return false;
}


void SetupCommon::giveConfigChanges( httpd_req *req, bool log_only ){
	ESP_LOGI(FNAME,"giveConfigChanges");
	for(int i = 0; i < instances.size(); i++ ) {
		if( instances[i]->isDefault() == false ) {
			std::string val = instances[i]->valueAsStr();
			if( ! val.empty() ){
				std::string cfg(instances[i]->key());
				cfg += ',' + val + '\n';
				ESP_LOGI(FNAME,"%s,%s", instances[i]->key(), val.c_str() );
				if( !log_only )
					httpd_resp_send_chunk( req, cfg.c_str(), cfg.size() );
			}
		}
	}
	if( !log_only ) {
		std::string cfg;
		httpd_resp_send_chunk( req, cfg.c_str(), 0 ); // send a zero length message
	}
}


int SetupCommon::restoreConfigChanges( int len, char *data ){
	ESP_LOGI(FNAME,"restoreConfigChanges len: %d \n %s", len, data );
	std::istringstream fs;
	fs.str( data );
	std::string line;
	int i=0;
	int valid=0;
	while( std::getline(fs, line, '\n') ) {
		if( line.find( "xcvario-config" ) != std::string::npos ){
			valid++;
			ESP_LOGI(FNAME,"found xcvario-config, valid=%d", valid );
		}
		else if( line.find( "text/comma-separated-values" ) != std::string::npos ){
			valid++;
			ESP_LOGI(FNAME,"found text/comma-separated-values, valid=%d", valid );
		}
		else if( line.find( "text/csv" ) != std::string::npos ){
			valid++;
			ESP_LOGI(FNAME,"found text/csv, valid=%d", valid );
		}
		else if( (line.length() > 1) && (valid >= 2) && line.find( "," ) != std::string::npos ){
			ESP_LOGI(FNAME, "%d, len:%d, %s\n", i, line.length(), line.c_str() );
			std::string key = line.substr(0, line.find(','));
			std::string value = line.substr(line.find(',')+1, line.length());
			ESP_LOGI(FNAME, "%d %s ", i, key.c_str()  );
			SetupCommon * item = getMember( key.c_str() );
			ESP_LOGI(FNAME, ", typename: %c \n", item->typeName()  );
			item->setValueFromStr( value.c_str() );
			item->commit();  // lets do that lazy later
			i++;
		}
	}
	ESP_LOGI(FNAME,"return %d", i);
	return i;
}

void SetupCommon::commitDirty(){
	for(int i = 0; i < instances.size(); i++ ) {
		if( instances[i]->dirty() )
			instances[i]->commit();
	}
}

void SetupCommon::prepareFactoryReset(){
	factory_reset.set(1);
	factory_reset.commit();
}

bool SetupCommon::factoryReset(){
	ESP_LOGI(FNAME,"\n\n******  FACTORY RESET ******");
	bool retsum = true;
	for(int i = 0; i < instances.size(); i++ ) {
		ESP_LOGI(FNAME,"i=%d %s erase", i, instances[i]->key() );
		if( instances[i]->mustReset() ){
			bool ret = instances[i]->erase();
			if( ret != true ) {
				ESP_LOGE(FNAME,"Error erasing %s", instances[i]->key() );
				retsum = false;
			}
			ret = instances[i]->init();
			if( ret != true ) {
				ESP_LOGE(FNAME,"Error init with default %s", instances[i]->key() );
				retsum = false;
			}
			else {
				ESP_LOGI(FNAME,"%s successfully initialized with default", instances[i]->key() );
			}
		}
	}
	if( retsum ) {
		ESP_LOGI(FNAME,"Factory reset SUCCESS");
	} else {
		ESP_LOGI(FNAME,"Factory reset FAILED!");
	}
	return retsum;
}

bool SetupCommon::initSetup( bool& present ) {
	bool ret=true;
	ESP_LOGI(FNAME,"SetupCommon::initSetup()");
	NVS.begin();
	if( ahrs_licence_dig1.exists() ) {
		present = true;
	} else {
		present = false;
	}

	for(int i = 0; i < instances.size(); i++ ) {
		if( ! instances[i]->init() ) {
			ret = false;
			ESP_LOGE(FNAME,"Error init with default NVS: %s", instances[i]->key() );
		}
	}

	if( factory_reset.get() ) {
		ret = factoryReset();
		commitDirty();
	}
	last_volume = (int)default_volume.get();
	giveConfigChanges( 0, true );
	return ret;
};


char * SetupCommon::getID()
{
	sprintf( _ID, "%s%s", getFixedID(), custom_wireless_id.get().id );
	return _ID;
}

// String from crc of MAC address to get a unique ID
char * SetupCommon::getDefaultID() {
	uint8_t mac[6];
	unsigned long  crc = 0;
	if ( esp_efuse_mac_get_default(mac) == ESP_OK ){
		crc = mz_crc32(0L, mac, 6);
	}
	if( hardwareRevision.get() >= XCVARIO_21 ){
		sprintf( default_id, "%04d", int(crc % 10000) );
	}
	else{
		sprintf( default_id, "%03d", int(crc % 1000));
	}
	return default_id;
}

const char * SetupCommon::getFixedID() {
	if( hardwareRevision.get() >= XCVARIO_21 ){
		return "XCVario-";
	}
	else{
		return "iVario-";
	}
}


bool SetupCommon::isMaster()
{
	return syncProto && syncProto->isMaster();
}

bool SetupCommon::haveWLAN(){
	return( (wireless == WL_WLAN_MASTER) ||  (wireless == WL_WLAN_STANDALONE) ||  (wireless == WL_WLAN_CLIENT ) );
}

bool SetupCommon::isClient()
{
	return xcv_role.get() == SECOND_ROLE;
}

bool SetupCommon::isWired()
{
	// is the CAN bus in use
	if ( CAN && CAN->getNrDLinks() > 0 ) {
		return true;
	}
	return false;
}


int SetupCommon::numEntries() {
	return instances.size();
}

