/*
 * OTA.cpp
 *
 *  Created on: Mar 17, 2020
 *      Author: iltis
 *
 *
 *
 */

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"

#include "esp_ota_ops.h"
#include "freertos/event_groups.h"
extern "C" {
#include "OTAServer.h"
#include "MyWiFi.h"
}
#include "IpsDisplay.h"
#include "Setup.h"
#include "OTA.h"


OTA::OTA(){
	pressed = false;
	_rotary->attach(this);
}

void OTA::begin(ESPRotary * aRotary){
	_rotary = aRotary;
}

void OTA::press() {
	printf("OTA::press()\n");
	pressed = true;
}

void OTA::release() {
	printf("OTA::release()\n");
	// pressed = false;
}

// OTA
void OTA::doSoftwareUpdate(IpsDisplay * p ){
	printf("Now start Wifi OTA\n");
	// xTaskCreate(&systemRebootTask, "rebootTask", 2048, NULL, 5, NULL);
	p->clear();
	int line=1;
	p->writeText(line++,"SOFTWARE DOWNLOAD");
	p->writeText(line++,"Use   Wifi: ESP32 OTA");
	p->writeText(line++,"Open: http://192.168.0.1");
	p->writeText(line++,"Then follow the dialogue");
	init_wifi_softap(&OTA_server);
	for( int i=900; i>0; i-- ) {
		char txt[40];
		sprintf(txt,"Timeout in %d sec  ", i);
		p->writeText(line+2,txt);
		std::string pro( "Progress: ");
		pro += std::to_string( getProgress() ) + " %";
		p->writeText(line+3,pro.c_str());
		sleep(1);
		if( getFlashStatus() == 1 ){
			p->writeText(line+5,"Download SUCCESS !");
			sleep(3);
			break;
		}
		if( pressed ) {
			p->writeText(line+5,"Abort, Now Restart");
			sleep(3);
			break;
		}
	}
	software_update.set( 0 );
	esp_restart();
}
