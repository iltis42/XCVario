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
#include <iostream>
#include <map>
#include <math.h>
#include "ESPAudio.h"
#include "BMPVario.h"
#include <esp32/rom/miniz.h>
#include "Polars.h"
#include "SetupVolt.h"

#include "esp_task_wdt.h"

char Setup::_ID[14];

void Setup::factorySetting()
{
		printf("Setup::factorySetting()\n");
		_setup._speedcal = 0.0;
		_setup._deadband = 0.3;
		_setup._deadband_neg = -0.3;
		_setup._center_freq = 500.0;
		_setup._tone_var = 2.0;
		_setup._dual_tone = 1;
		_setup._high_tone_var = 12.0;
		_setup._vario_delay = 3.0; // seconds
		_setup._analog_adj = 0.0;
		_setup._QNH = 1013.25;
		_setup._blue_enable = 1;
		_setup._range = 5.0;
		_setup._ballast = 0.0;
		_setup._MC = 0.5;
		_setup._voltmeter_adj = 0;
		_setup._factory_reset = 0;
		_setup._audio_range = 0;
		_setup._alt_select = 1;
		_setup._offset = 0;
		_setup._contrast_adj = 85.0;
		_setup._s2f_speed = 100.0;
		_setup._audio_mode = 3;
		_setup._glider_type = 0;
		_setup._ps_display = 1;
		_setup._bat_low_volt = 11.5;
		_setup._bat_red_volt = 11.75;
		_setup._bat_yellow_volt = 12.0;
		_setup._bat_full_volt = 12.8;
		_setup._display_type = UNIVERSAL;
		_setup._core_climb_min = 0.5;
		_setup._core_climb_history = 45;
		_setup._elevation = -1;
		_setup._display_orientation = 0; // NORMAL
		_setup._flap_enable = 0;  // disable
		_setup._flap_minus_2 = 165;
		_setup._flap_minus_1 = 105;
		_setup._flap_0 = 88;
		_setup._flap_plus_1 = 78;
		_setup._default_volume = 40.0;
		_setup._alt_unit = 0;  // m
		_setup._ias_unit = 0;  // km/h
		_setup._vario_unit = 0; // m/s
		_setup._rot_default = 0; // Volume
		_setup._serial2_speed = 0;   // 0: OFF 1:4800 2:9600 3:19200 4: 38400..
		_setup._serial2_rxloop = 0;  // 0: dont loop, 1: loop
		_setup._serial2_tx = 0;      // 0: dont TX, 1: TX
		_setup._serial2_tx_inverted = 1;      // 0: normal, 1: inverted signal
		_setup._serial2_rx_inverted = 1;      // 0: normal, 1: inverted signal
		_setup._software_update = 0;
		_setup._factory_volt_adjust = 0.00815;
		printf("sefault _polar %d\n", Polars::numPolars() );
		_setup._polar = Polars::getPolar(0); // default user polar
		memset( _setup._bt_name, 0, sizeof( _setup._bt_name) );
		printf("BT-Name: %s\n", _ID );
		memcpy( _setup._bt_name, _ID, strlen( _ID )  );
		commit();
		printf("end Setup::factorySetting()\n");
}

void Setup::begin() {
	printf( "Setup CMD begin()\n");
	esp_err_t success;
	void * obj = NVS.getObject("SetupXC2", &success);

	if ( success != ESP_OK ){
		printf( "Setup data not yet in NVS: Init ret=%d\n", success );
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
	for( int i=0; i< sizeof(_setup)-4; i+=4 )
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
    printf("New NVS checksum %04X\n", _setup._checksum );
	success = NVS.setObject("SetupXC2", &_setup, sizeof(_setup) );
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
	sprintf( _ID, "iVario-%d", id );
	}
	return _ID;
}




