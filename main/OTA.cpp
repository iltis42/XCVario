/*
 * OTA.cpp
 *
 *  Created on: Mar 17, 2020
 *      Author: iltis
 *
 *
 *
 */

#include "ESPRotary.h"

#include "comm/WifiApSta.h"
#include "IpsDisplay.h"
#include "setup/SetupNG.h"
#include "OTA.h"
#include "logdef.h"
#include "Webserver.h"
#include "qrcodegen.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_netif.h>
#include <esp_ota_ops.h>


const char* ssid = "ESP32 OTA";
const char* pwd = "xcvario-21";

// OTA
void OTA::doSoftwareUpdate(IpsDisplay * disp ){
	ESP_LOGI(FNAME,"Now start Wifi OTA");
	WifiApSta *wifi = WifiApSta::createWifiApSta();
	wifi->ConfigureIntf(80);

	// Break the cycle of booting into OTA mode early
	software_update.commit();

	disp->clear();
	int line=1;
	disp->writeText(line++,"SOFTWARE DOWNLOAD");
	disp->writeText(line++,"Use Wifi: ESP32 OTA");
	disp->writeText(line++,"Password: xcvario-21");
	disp->writeText(line++,"Open: http://192.168.4.1");
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
	size_t strWidth = disp->ucg->getStrWidth(wifiText);
	disp->ucg->setPrintPos((120 - strWidth) / 2, 130);
	disp->ucg->print(wifiText);

	if( qrSuccess ) {
		// Calculate module size for best fit
		int size = qrcodegen_getSize(qrcodeBuffer);
		int dotSize = qrCodeMaxWidth / size;

		// Center QRCode
		xOffset += (120 - (size * dotSize)) / 2;

		for( int y = 0; y < size; y++ ) {
			for( int x = 0; x < size; x++ ) {
				if( qrcodegen_getModule(qrcodeBuffer, x, y) ) {
				  disp->ucg->drawBox(xOffset + (x * dotSize), yOffset + (y * dotSize), dotSize, dotSize);
				}
			}
		}
	} else {
		disp->ucg->drawFrame(xOffset + ((120 - qrCodeMaxWidth) / 2), yOffset, qrCodeMaxWidth, qrCodeMaxWidth);
	}

    // Generate URL QR Code using the AP IP-address
    // TODO: Show after Wifi has been connected?
    esp_netif_ip_info_t ip_info;
    esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);    // Return ESPs IP for everybody else

	snprintf(textBuffer, textBufferSize, "http://" IPSTR, IP2STR(&ip_info.ip));
	qrSuccess = qrcodegen_encodeText(textBuffer, tempBuffer, qrcodeBuffer, errCorLvl, 4, 4, qrcodegen_Mask_AUTO, true);

	const char *urlText = "URL:";
	strWidth = disp->ucg->getStrWidth(urlText);
	disp->ucg->setPrintPos(120 + (120 - strWidth) / 2, 130);
	disp->ucg->print(urlText);

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
				  disp->ucg->drawBox(xOffset + (x * dotSize), yOffset + (y * dotSize), dotSize, dotSize);
				}
			}
		}
	} else {
		// In case of error draw empty rectangle
		disp->ucg->drawFrame(xOffset + ((120 - qrCodeMaxWidth) / 2), yOffset, qrCodeMaxWidth, qrCodeMaxWidth);
	}

	free(textBuffer);
	free(qrcodeBuffer);
	free(tempBuffer);

    Webserver.start();

	Rotary->readSwitch(); //empty the button queue

    line = 9;
	for( tick=0; tick<900; tick++ ) {
		char txt[40];
		sprintf(txt,"Timeout in %d sec  ", 900-tick );
		disp->writeText(line+2,txt);
		std::string pro( "Progress: ");
		pro += std::to_string( Webserver.getOtaProgress() ) + " %";
		disp->writeText(line+3, pro.c_str());
		vTaskDelay(1000/portTICK_PERIOD_MS);
		if( Webserver.getOtaStatus() == otaStatus::DONE ){
			ESP_LOGI(FNAME,"Flash status, Now restart");
			disp->writeText(line+3,"Download SUCCESS !");
			vTaskDelay(3000/portTICK_PERIOD_MS);
			break;
		}
		if( Rotary->readSwitch() ) {
			ESP_LOGI(FNAME,"pressed");
			disp->writeText(line+3,"Abort OTA, restarting ...");
			vTaskDelay(3000/portTICK_PERIOD_MS);
			break;
		}
	}
    Webserver.stop();
	ESP_LOGI(FNAME,"Now restart");
	esp_restart();
}
