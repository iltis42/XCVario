/*
 * OTA.cpp
 *
 *  Created on: Mar 17, 2020
 *      Author: iltis
 *
 *
 *
 */

 #include "OTA.h"

#include "IpsDisplay.h"
#include "comm/WifiApSta.h"
#include "setup/SetupNG.h"
#include "Webserver.h"
#include "qrcodegen.h"
#include "sensor.h"
#include "logdef.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_netif.h>
#include <esp_ota_ops.h>


const char* ssid = "ESP32 OTA";
const char* pwd = "xcvario-21";

// OTA
void OTA::display(int some){
	ESP_LOGI(FNAME,"Now start Wifi OTA");
	WifiApSta *wifi = WifiApSta::createWifiApSta();
	wifi->ConfigureIntf(80);

	// Break the cycle of booting into OTA mode early
	software_update.commit();

	Display->clear();
	int line=1;
	Display->writeText(line++,"SOFTWARE DOWNLOAD");
	Display->writeText(line++,"Use Wifi: ESP32 OTA");
	Display->writeText(line++,"Password: xcvario-21");
	Display->writeText(line++,"Open: http://192.168.4.1");

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
	size_t strWidth = Display->ucg->getStrWidth(wifiText);
	Display->ucg->setPrintPos((120 - strWidth) / 2, 130);
	Display->ucg->print(wifiText);

	if( qrSuccess ) {
		// Calculate module size for best fit
		int size = qrcodegen_getSize(qrcodeBuffer);
		int dotSize = qrCodeMaxWidth / size;

		// Center QRCode
		xOffset += (120 - (size * dotSize)) / 2;

		for( int y = 0; y < size; y++ ) {
			for( int x = 0; x < size; x++ ) {
				if( qrcodegen_getModule(qrcodeBuffer, x, y) ) {
				  Display->ucg->drawBox(xOffset + (x * dotSize), yOffset + (y * dotSize), dotSize, dotSize);
				}
			}
		}
	} else {
		Display->ucg->drawFrame(xOffset + ((120 - qrCodeMaxWidth) / 2), yOffset, qrCodeMaxWidth, qrCodeMaxWidth);
	}

	// Generate URL QR Code using the AP IP-address
	// TODO: Show after Wifi has been connected?
	esp_netif_ip_info_t ip_info;
	esp_netif_get_ip_info(esp_netif_get_handle_from_ifkey("WIFI_AP_DEF"), &ip_info);    // Return ESPs IP for everybody else

	snprintf(textBuffer, textBufferSize, "http://" IPSTR, IP2STR(&ip_info.ip));
	qrSuccess = qrcodegen_encodeText(textBuffer, tempBuffer, qrcodeBuffer, errCorLvl, 4, 4, qrcodegen_Mask_AUTO, true);

	const char *urlText = "URL:";
	strWidth = Display->ucg->getStrWidth(urlText);
	Display->ucg->setPrintPos(120 + (120 - strWidth) / 2, 130);
	Display->ucg->print(urlText);

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
				  Display->ucg->drawBox(xOffset + (x * dotSize), yOffset + (y * dotSize), dotSize, dotSize);
				}
			}
		}
	} else {
		// In case of error draw empty rectangle
		Display->ucg->drawFrame(xOffset + ((120 - qrCodeMaxWidth) / 2), yOffset, qrCodeMaxWidth, qrCodeMaxWidth);
	}

	free(textBuffer);
	free(qrcodeBuffer);
	free(tempBuffer);

	Webserver.start();

	line = 9;
	for( tick=0; tick<900; tick++ ) {
		char txt[40];
		sprintf(txt,"Timeout in %d sec  ", 900-tick );
		Display->writeText(line+2,txt);
		std::string pro( "Progress: ");
		pro += std::to_string( Webserver.getOtaProgress() ) + " %";
		Display->writeText(line+3, pro.c_str());
		vTaskDelay(1000/portTICK_PERIOD_MS);
		if( Webserver.getOtaStatus() == otaStatus::DONE ){
			ESP_LOGI(FNAME,"Flash status, Now restart");
			Display->writeText(line+3,"Download SUCCESS !");
			vTaskDelay(3000/portTICK_PERIOD_MS);
			break;
		}
		if( _abort ) {
			ESP_LOGI(FNAME,"pressed");
			Display->writeText(line+3,"Abort OTA, restarting ...");
			vTaskDelay(1500/portTICK_PERIOD_MS);
			break;
		}
	}
	Webserver.stop();
	ESP_LOGI(FNAME,"Now restart");
	esp_restart();
}
