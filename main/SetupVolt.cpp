/*
 * SetupVolt.cpp
 *
 *  Created on: Dec 23, 2017
 *      Author: iltis
 */

#include "SetupVolt.h"
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

#include "esp_task_wdt.h"

setup_volt_t SetupVolt::_setup;

void SetupVolt::factorySetting()
{
		printf("SetupVolt::factorySetting()\n");
		_setup._factory_volt_adjust = 0.00815;
		commit();
		printf("end SetupVolt::factorySetting()\n");
}

void SetupVolt::begin() {
	printf( "SetupVolt CMD begin()\n");
	esp_err_t success;
	void * obj = NVS.getObject("SetupVoltXC", &success);

	if ( success != ESP_OK ){
		printf( "SetupVolt data not yet in NVS: Init\n");
	    factorySetting();
	}
	else{
		memcpy( &_setup, obj, sizeof( _setup) );
		if( checkSum() == false )
			factorySetting();
		else {
			printf( "SetupVolt: NVS data ok\n");
		}
	}
}

bool SetupVolt::checkSum( bool set ) {
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
		   printf( "SetupVolt: NVS checksum ok %d\n", cs );
		   return( true );
		}
		else {
			printf( "SetupVolt: NVS checksum NOT ok !\n");
			return( false );
		}
	}
}

void SetupVolt::commit( ) {

	bool success;
	checkSum( true );
	success = NVS.setObject("SetupVoltXC", &_setup, sizeof(_setup) );
	if ( success != true ){
		printf( "Error storing data NVS, maybe HW defect\n");
	}
	else
		printf( "Store data in NVS success\n" );
}





