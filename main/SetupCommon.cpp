/*
 * Setup.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#include "Setup.h"
#include <string>
#include <stdio.h>
#include "esp_system.h"
#include <esp_log.h>
#include "sdkconfig.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "ESP32NVS.h"
#include "esp32/rom/uart.h"
#include <math.h>
#include "ESPAudio.h"
#include "BMPVario.h"
#include <esp32/rom/miniz.h>
#include "Polars.h"
#include "esp_task_wdt.h"
#include <logdef.h>
#include "sensor.h"
#include "Router.h"
#include "Switch.h"
#include "CircleWind.h"
#include "Protocols.h"
#include "ESPAudio.h"
#include "SetupCommon.h"

bool SetupCommon::lazyCommit = true;
QueueHandle_t SetupCommon::commitSema = nullptr;
esp_timer_handle_t SetupCommon::_timer = nullptr;
bool SetupCommon::_dirty = false;
char SetupCommon::_ID[14];
std::vector<SetupCommon *> *SetupCommon::instances = 0;


SetupCommon::SetupCommon() {
	memset( _ID, 0, sizeof( _ID ));
	if( !instances )  // instantiate first
		instances = new std::vector<SetupCommon *>;
}

SetupCommon::~SetupCommon() {
	if( !instances->size() )
		delete instances;
}

// TimeOut callback
void SetupCommon::timeout(QueueHandle_t arg)
{
    uint32_t stat = 0;
    xQueueSendFromISR(arg, &stat, NULL);
}

void SetupCommon::sendSetup( uint8_t sync, const char *key, char type, void *value, int len, bool ack ){
	// ESP_LOGI(FNAME,"sendSetup(): key=%s, type=%c, len=%d, ack=%d", key, type, len, ack );
	OV.sendItem( key, type, value, len, ack );
}

SetupCommon * SetupCommon::getMember( const char * key ){
	for(int i = 0; i < instances->size(); i++ ) {
		if( std::string( key ) == std::string( (*instances)[i]->key() )){
			// ESP_LOGI(FNAME,"found key %s", (*instances)[i]->key() );
			return (*instances)[i];
		}
	}
	return 0;
}

// at time of connection establishment
bool SetupCommon::syncEntry( int entry ){
    if( entry < instances->size() ) {
        return (*instances)[entry]->sync();
    }
    return false;
}

bool SetupCommon::factoryReset(){
	ESP_LOGI(FNAME,"\n\n******  FACTORY RESET ******");
	bool retsum = true;
	for(int i = 0; i < instances->size(); i++ ) {
		ESP_LOGI(FNAME,"i=%d %s erase", i, (*instances)[i]->key() );
		if( (*instances)[i]->mustReset() ){
			bool ret = (*instances)[i]->erase();
			if( ret != true ) {
				ESP_LOGE(FNAME,"Error erasing %s", (*instances)[i]->key() );
				retsum = false;
			}
			ret = (*instances)[i]->init();
			if( ret != true ) {
				ESP_LOGE(FNAME,"Error init with default %s", (*instances)[i]->key() );
				retsum = false;
			}
			else
				ESP_LOGI(FNAME,"%s successfully initialized with default", (*instances)[i]->key() );
		}
	}
	if( retsum )
		ESP_LOGI(FNAME,"Factory reset SUCCESS");
	else
		ESP_LOGI(FNAME,"Factory reset FAILED!");
	return retsum;
}

bool SetupCommon::initSetup( bool& present ) {
	bool ret=true;
	ESP_LOGI(FNAME,"SetupCommon::initSetup()");
	esp_err_t _err = nvs_flash_init();
	if (_err == ESP_ERR_NVS_NO_FREE_PAGES) {
		ESP_LOGE(FNAME,"Error no more space in NVS: erase partition");
		const esp_partition_t* nvs_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
		_err = (esp_partition_erase_range(nvs_partition, 0, nvs_partition->size));
		if ( _err != ESP_OK ){
			ESP_LOGE(FNAME, "partition erase returned error ret=%d", _err );
			ret = false;
		}
	}
	if( ahrs_licence_dig1.exists() )
		present = true;
	else
		present = false;

	for(int i = 0; i < instances->size(); i++ ) {
			bool ret = (*instances)[i]->init();
			if( ret != true )
				ESP_LOGE(FNAME,"Error init with default NVS: %s", (*instances)[i]->key() );
	}

	if( factory_reset.get() ) {
		ESP_LOGI(FNAME,"\n\n******  FACTORY RESET ******");
		for(int i = 0; i < instances->size(); i++ ) {
			ESP_LOGI(FNAME,"i=%d %s erase", i, (*instances)[i]->key() );
			if( (*instances)[i]->mustReset() ){
				bool ret = (*instances)[i]->erase();
				if( ret != true ) {
					ESP_LOGE(FNAME,"Error erasing %s", (*instances)[i]->key() );
					ret = false;
				}
				ret = (*instances)[i]->init();
				if( ret != true ) {
					ESP_LOGE(FNAME,"Error init with default %s", (*instances)[i]->key() );
					ret = false;
				}
				else
					ESP_LOGI(FNAME,"%s successfully initialized with default", (*instances)[i]->key() );
			}
		}
	}
	last_volume = (int)default_volume.get();

    if ( _timer == nullptr ) {
        // create event queue to connect to the timer callback
        commitSema = xQueueCreate(2, sizeof(uint32_t));

        esp_timer_create_args_t timer_args = {
            .callback = (esp_timer_cb_t)timeout,
            .arg = commitSema,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "lazy_nvs",
            .skip_unhandled_events = true,
        };
        ESP_ERROR_CHECK(esp_timer_create(&timer_args, &_timer));
        ESP_ERROR_CHECK(esp_timer_start_periodic(_timer, 5ULL * 1000000)); // 5 sec in usec
    }

	return ret;
};

char * SetupCommon::getID() {
	if( strlen( _ID ) == 0 ) {
		uint8_t mac[6];
		unsigned long  crc = 0;
		if ( esp_efuse_mac_get_default(mac) == ESP_OK ){
			crc = mz_crc32(0L, mac, 6);
		}
		if( hardwareRevision.get() >= 3 ){
			sprintf( _ID, "XCVario-%04d", int(crc % 10000) );
		}
		else{
			sprintf( _ID, "iVario-%03d", int(crc % 1000));
		}
	}
	return _ID;
}

bool SetupCommon::isMaster(){
	bool ret = (wireless == WL_WLAN_MASTER) || ((can_speed.get() != CAN_SPEED_OFF) && (can_mode.get() == CAN_MODE_MASTER));
	// ESP_LOGI(FNAME,"isMaster ret:%d", ret );
	return( ret );
}

bool SetupCommon::mustSync( uint8_t sync ){
	if( !isClient() && !isMaster() )
		return false;
	else
		return( ( (isClient() && sync == SYNC_FROM_CLIENT) || (isMaster() && sync == SYNC_FROM_MASTER) || sync == SYNC_BIDIR ) );
}

bool SetupCommon::haveWLAN(){
	return( (wireless == WL_WLAN_MASTER) ||  (wireless == WL_WLAN_STANDALONE) ||  (wireless == WL_WLAN_CLIENT ) );
}

bool SetupCommon::isClient(){
	// ESP_LOGI(FNAME,"wireless:%d can_speed: %d can_mode: %d", wireless, can_speed.get(), can_mode.get() );
	return((wireless == WL_WLAN_CLIENT) || ((can_speed.get() != CAN_SPEED_OFF) && (can_mode.get() == CAN_MODE_CLIENT) ));
}

bool SetupCommon::isCanClient(){
	// ESP_LOGI(FNAME,"wireless:%d can_speed: %d can_mode: %d", wireless, can_speed.get(), can_mode.get() );
	return( (can_speed.get() != CAN_SPEED_OFF) && (can_mode.get() == CAN_MODE_CLIENT) );
}

bool SetupCommon::isCanMaster(){
	// ESP_LOGI(FNAME,"wireless:%d can_speed: %d can_mode: %d", wireless, can_speed.get(), can_mode.get() );
	return( (can_speed.get() != CAN_SPEED_OFF) && (can_mode.get() == CAN_MODE_MASTER) );
}

bool SetupCommon::isWired(){
	return(can_speed.get() && (can_mode.get() == CAN_MODE_CLIENT || can_mode.get() == CAN_MODE_MASTER));
}

bool SetupCommon::commitNow()
{
    nvs_handle_t h;
    if( _dirty && open(h) ) {
        esp_err_t ret = nvs_commit(h);
		close(h);
        _dirty = false;
        ESP_LOGI(FNAME,"nvs commiting now!" );
        return ret == ESP_OK;
    }
    return false;
}

int SetupCommon::numEntries() {
	return instances->size();
};

bool SetupCommon::open(nvs_handle_t &h) {
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
}

void SetupCommon::close(nvs_handle_t &h) {
	if( h != 0) {
		nvs_close(h);
		h = 0;
	}
}
