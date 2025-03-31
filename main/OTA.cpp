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
#include <esp_netif.h>

#include "esp_ota_ops.h"
#include "freertos/event_groups.h"
#include "MyWiFi.h"
#include "IpsDisplay.h"
#include "Setup.h"
#include "OTA.h"
#include <logdef.h>
#include "ESPAudio.h"
#include "Webserver.h"
#include "qrcodegen.h"

OTA::OTA(){
	pressed = false;
	tick = 0;
}

void OTA::begin(){
	attach();
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

const char* ssid = "ESP32 OTA";
const char* pwd = "xcvario-21";

// OTA
void OTA::doSoftwareUpdate(IpsDisplay * p ){
	ESP_LOGI(FNAME,"Now start Wifi OTA");
	init_wifi_softap(nullptr);

	p->clear();
	int line=1;
	p->writeText(line++,"SOFTWARE DOWNLOAD");
	p->writeText(line++,"Use Wifi: ESP32 OTA");
	p->writeText(line++,"Password: xcvario-21");
	p->writeText(line++,"Open: http://192.168.4.1");
	// p->writeText(line++,"Then follow the dialogue");

	enum qrcodegen_Ecc errCorLvl = qrcodegen_Ecc_LOW; // Error correction level
	// ESP_LOGI(FNAME, "Generate QRCODE");

	// Make and print the QR Code symbol
	const size_t textBufferSize = 128;
	char *textBuffer = (char *)malloc(textBufferSize);
	uint8_t *tempBuffer = (uint8_t *)malloc(qrcodegen_BUFFER_LEN_FOR_VERSION(4));
	uint8_t *qrcodeBuffer = (uint8_t *)malloc(qrcodegen_BUFFER_LEN_FOR_VERSION(4));

	snprintf(textBuffer, textBufferSize, "WIFI:S:%s;T:WPA;P:%s;;", ssid, pwd);
	bool qrSuccess = qrcodegen_encodeText(textBuffer, tempBuffer, qrcodeBuffer, errCorLvl, 4, 4, qrcodegen_Mask_AUTO, true);

	size_t qrCodeMaxWidth = 114;
	size_t yOffset = 130;
	size_t xOffset = 0;

	const char *wifiText = "WIFI:";
	size_t strWidth = p->ucg->getStrWidth(wifiText);
	p->ucg->setPrintPos((120 - strWidth) / 2, 130);
	p->ucg->print(wifiText);

	if( qrSuccess ) {
		// Calculate module size for best fit
		int size = qrcodegen_getSize(qrcodeBuffer);
		int dotSize = qrCodeMaxWidth / size;

		// Center QRCode
		xOffset += (120 - (size * dotSize)) / 2;

		for( int y = 0; y < size; y++ ) {
			for( int x = 0; x < size; x++ ) {
				if( qrcodegen_getModule(qrcodeBuffer, x, y) ) {
				  p->ucg->drawBox(xOffset + (x * dotSize), yOffset + (y * dotSize), dotSize, dotSize);
				}
			}
		}
	} else {
		p->ucg->drawFrame(xOffset + ((120 - qrCodeMaxWidth) / 2), yOffset, qrCodeMaxWidth, qrCodeMaxWidth);
	}

    // Generate URL QR Code using the AP IP-address
    // TODO: Show after Wifi has been connected?
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);    // Return ESPs IP for everybody else

	snprintf(textBuffer, textBufferSize, "http://" IPSTR, IP2STR(&ip_info.ip));
	qrSuccess = qrcodegen_encodeText(textBuffer, tempBuffer, qrcodeBuffer, errCorLvl, 4, 4, qrcodegen_Mask_AUTO, true);

	const char *urlText = "URL:";
	strWidth = p->ucg->getStrWidth(urlText);
	p->ucg->setPrintPos(120 + (120 - strWidth) / 2, 130);
	p->ucg->print(urlText);

	xOffset = 120;
	if( qrSuccess ) {
		// Calculate module size for best fit
		int size = qrcodegen_getSize(qrcodeBuffer);
		int dotSize = qrCodeMaxWidth / size;

		// Center QRCode
		xOffset += (120 - (size * dotSize)) / 2;

		for( int y = 0; y < size; y++ ) {
			for( int x = 0; x < size; x++ ) {
				if( qrcodegen_getModule(qrcodeBuffer, x, y) ) {
				  p->ucg->drawBox(xOffset + (x * dotSize), yOffset + (y * dotSize), dotSize, dotSize);
				}
			}
		}
	} else {
		// In case of error draw empty rectangle
		p->ucg->drawFrame(xOffset + ((120 - qrCodeMaxWidth) / 2), yOffset, qrCodeMaxWidth, qrCodeMaxWidth);
	}

	free(textBuffer);
	free(qrcodeBuffer);
	free(tempBuffer);

    Webserver.start();

    line = 9;
	for( tick=0; tick<900; tick++ ) {
		char txt[40];
		sprintf(txt,"Timeout in %d sec  ", 900-tick );
		p->writeText(line+2,txt);
		std::string pro( "Progress: ");
		pro += std::to_string( Webserver.getOtaProgress() ) + " %";
		p->writeText(line+3,pro.c_str());
		vTaskDelay(1000/portTICK_PERIOD_MS);
		if( Webserver.getOtaStatus() == otaStatus::DONE ){
			ESP_LOGI(FNAME,"Flash status, Now restart");
			p->writeText(line+5,"Download SUCCESS !");
			vTaskDelay(3000/portTICK_PERIOD_MS);
			break;
		}
		if( pressed ) {
			ESP_LOGI(FNAME,"pressed");
			p->writeText(line+5,"Abort, Now Restart");
			vTaskDelay(3000/portTICK_PERIOD_MS);
			break;
		}
	}
    Webserver.stop();
	ESP_LOGI(FNAME,"Now restart");
	software_update.commit();
	Audio::shutdown();
	esp_restart();
}
