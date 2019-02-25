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
#include "ESPAudio.h"
#include "BMPVario.h"
#include <rom/miniz.h>


char cb[128];

#include "esp_task_wdt.h"

void Setup::factorySetting()
{
		_setup._speedcal = 0.0;
		_setup._deadband = 0.3;
		_setup._deadband_neg = -0.3;
		_setup._center_freq = 500.0;
		_setup._tone_var = 2.0;
		_setup._vario_delay = 3.0; // seconds
		_setup._analog_adj = 0.0;
		_setup._QNH = 1013.25;
		_setup._blue_enable = 1;
		_setup._range = 5.0;
		_setup._v1 = 100.0;
		_setup._w1 = -0.7;
		_setup._v2 = 150.0;
		_setup._w2 = -1.2;
		_setup._v3 = 200.0;
		_setup._w3 = -2.2;
		_setup._ballast = 0.0;
		_setup._MC = 0.0;
		_setup._voltmeter_adj = 0;
		_setup._factory_reset = 0;
		_setup._audio_range = 0;
		_setup._alt_select = 1;
		_setup._offset = 0;
		_setup._contrast_adj = 85.0;
		_setup._s2f_speed = 100.0;
		_setup._audio_mode = 3;

		memset( _setup._bt_name, 0, sizeof( _setup._bt_name) );
		uint8_t mac[6];
		char bt_name[12] = "iVario";
		unsigned long  crc = 0;
		if ( esp_efuse_mac_get_default(mac) == ESP_OK ){
			crc = mz_crc32(0L, mac, 6);
			printf("CRC %ld\n", crc );
			sprintf( bt_name, "iVario-%d", int(crc % 1000));
			printf("BT-Name: %s\n", bt_name );
		}
		memcpy( _setup._bt_name, bt_name, strlen( bt_name)  );
		commit();
}

void Setup::begin( BTSender *btsender ) {
	printf( "Setup CMD begin()\n");
	_btsender = btsender;
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
	_test_ms=0.0;
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
		sprintf( cb, "Error storing data NVS, maybe HW defect");
	}
	else
		sprintf( cb,"Store data in NVS success\n");
}





