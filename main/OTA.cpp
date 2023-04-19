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
#include "OTAServer.h"
#include "MyWiFi.h"
#include "IpsDisplay.h"
#include "Setup.h"
#include "OTA.h"
#include <logdef.h>
#include "ESPAudio.h"

OTA::OTA(){
	pressed = false;
	tick = 0;
}

void OTA::begin(){
	attach(this);
}

void OTA::press() {
	ESP_LOGI(FNAME,"OTA::press() %d", tick );
	if( tick > 2 ){
		pressed = true;
	}
}

void OTA::longPress() {
	ESP_LOGI(FNAME,"OTA::longPress() %d", tick );
	if( tick > 2 ){
		pressed = true;
	}
}

void OTA::release() {
	ESP_LOGI(FNAME,"OTA::release()");
}

// OTA
void OTA::doSoftwareUpdate(IpsDisplay * p ){
	ESP_LOGI(FNAME,"Now start Wifi OTA");
	p->clear();
	int line=1;
	p->writeText(line++,"SOFTWARE DOWNLOAD");
	p->writeText(line++,"Use Wifi: ESP32 OTA");
	p->writeText(line++,"Password: xcvario-21");
	p->writeText(line++,"Open: http://192.168.4.1");
	p->writeText(line++,"Then follow the dialogue");
	init_wifi_softap(&OTA_server);
	for( tick=0; tick<900; tick++ ) {
		char txt[40];
		sprintf(txt,"Timeout in %d sec  ", 900-tick );
		p->writeText(line+2,txt);
		std::string pro( "Progress: ");
		pro += std::to_string( getProgress() ) + " %";
		p->writeText(line+3,pro.c_str());
		sleep(1);
		if( getFlashStatus() == 1 ){
			ESP_LOGI(FNAME,"Flash status, Now restart");
			p->writeText(line+5,"Download SUCCESS !");
			sleep(3);
			break;
		}
		if( pressed ) {
			ESP_LOGI(FNAME,"pressed");
			p->writeText(line+5,"Abort, Now Restart");
			sleep(3);
			break;
		}
	}
	ESP_LOGI(FNAME,"Now restart");
	software_update.set( 0 );
	software_update.commit();
	Audio::shutdown();
	esp_restart();
}
