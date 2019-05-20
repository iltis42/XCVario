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
#include "rom/uart.h"
#include <iostream>
#include <map>
#include <math.h>
#include <rom/miniz.h>

#include "esp_task_wdt.h"

char Setup::_ID[14];

void Setup::factorySetting()
{
		printf("Setup::factorySetting()\n");
		memset( _setup._bt_name, 0, sizeof( _setup._bt_name) );
		printf("BT-Name: %s\n", _ID );
		memcpy( _setup._bt_name, _ID, strlen( _ID )  );
		commit();
		printf("end Setup::factorySetting()\n");
}

void Setup::begin() {
	printf( "Setup CMD begin()\n");
	esp_err_t success;
	void * obj = NVS.getObject("SetupXC", &success);

	if ( success != ESP_OK ){
		printf( "Setup data not yet in NVS: Init\n");
	    factorySetting();
	}
	else{
		memcpy( &_setup, obj, sizeof( _setup) );
		if( checkSum() == false )
			factorySetting();
		else {
			printf( "Setup: NVS data ok\n");
		}
		if( _setup._factory_reset == 1 ){
			printf( "Setup: Factory Setting requested\n");
			factorySetting();
		}
	}
}

bool Setup::checkSum( bool set ) {
	uint32_t cs=0;
	uint8_t * p=(uint8_t *)&_setup;
	for( int i=0; i< sizeof(_setup)-4; i++ )
		cs += *p++;
	if( set == true ) {
	   _setup._checksum = cs;
		return( true );
	}
	else {
		if (_setup._checksum == cs ) {
		   printf( "Setup: NVS checksum ok %d\n", cs );
		   return( true );
		}
		else {
			printf( "Setup: NVS checksum NOT ok !\n");
			return( false );
		}
	}
}

void Setup::commit( ) {

	bool success;
	checkSum( true );
	success = NVS.setObject("SetupXC", &_setup, sizeof(_setup) );
	if ( success != true ){
		printf( "Error storing data NVS, maybe HW defect\n");
	}
	else
		printf( "Store data in NVS success\n");
}

char * Setup::getID() {
	if( strlen( _ID ) == 0 ) {
	uint8_t mac[6];
	unsigned long  crc = 0;
	if ( esp_efuse_mac_get_default(mac) == ESP_OK ){
		crc = mz_crc32(0L, mac, 6);
		}
	int id = int(crc % 1000);
	sprintf( _ID, "Flarm-%d", id );
	}
	return _ID;
}




