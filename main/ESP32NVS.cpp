#include "ESP32NVS.h"
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/portmacro.h>
#include <freertos/semphr.h>
#include <logdef.h>


SemaphoreHandle_t nvMutex=NULL;
ESP32NVS * ESP32NVS::Instance = 0;

ESP32NVS::ESP32NVS(){
}

bool ESP32NVS::begin(){
	ESP_LOGI(FNAME,"ESP32NVS::begin()");
	nvMutex=xSemaphoreCreateMutex();
	esp_err_t _err = nvs_flash_init();
	if (_err == ESP_ERR_NVS_NO_FREE_PAGES) {
		const esp_partition_t* nvs_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
		_err = (esp_partition_erase_range(nvs_partition, 0, nvs_partition->size));
	}
	return true;
}

nvs_handle_t ESP32NVS::open(){
	// ESP_LOGI(FNAME,"ESP32NVS::open()");
	nvs_handle_t h;
	if(nvs_open("SetupNG", NVS_READWRITE, &h) != ESP_OK){
		ESP_LOGE(FNAME,"ESP32NVS open storage error");
		return 0;
	}
	return h;
}

void ESP32NVS::close( nvs_handle_t h ){
  // ESP_LOGI(FNAME,"ESP32NVS::close() %d", h );
  nvs_close(h);
}


bool ESP32NVS::commit(){
	// ESP_LOGI(FNAME,"ESP32NVS::commit()");
	bool ret=true;
	xSemaphoreTake(nvMutex,portMAX_DELAY );
	nvs_handle_t h = open();
	esp_err_t _err = nvs_commit(h);
	if(_err != ESP_OK)  {
		ESP_LOGE(FNAME,"ESP32NVS::commit() error");
		ret=false;
	}
	close(h);
	xSemaphoreGive(nvMutex);
	return ret;
}

bool ESP32NVS::setBlob(const char * key, void* value, size_t length){
	// ESP_LOGI(FNAME,"ESP32NVS::setBlob(key:%s, addr:%p, len:%d)", key, value, length );
	bool ret=true;
	xSemaphoreTake(nvMutex,portMAX_DELAY );
	nvs_handle_t h = open();
	if( !h )
		return false;
	esp_err_t _err = nvs_set_blob(h, (char*)key, value, length);
	if(_err != ESP_OK) {
		ESP_LOGE(FNAME,"set blob error %d", _err );
		ret=false;
	}
	// ESP_LOGI(FNAME,"set blob OK");
	close(h);
	xSemaphoreGive(nvMutex);
	return ret;
}

bool ESP32NVS::eraseAll(){
	bool ret=true;
	xSemaphoreTake(nvMutex,portMAX_DELAY );
	nvs_handle_t h = open();
	if( !h )
		return false;
	esp_err_t _err = nvs_erase_all(h);
	if(_err != ESP_OK){
		ret = false;
	}
	close(h);
	xSemaphoreGive(nvMutex);
	return ret;
}

bool ESP32NVS::erase(const char * key){
	xSemaphoreTake(nvMutex,portMAX_DELAY );
	bool ret=true;
	nvs_handle_t h = open();
	if( !h )
		return false;
	esp_err_t _err =  nvs_erase_key(h, key);
	if(_err != ESP_OK){
		ret = false;
	}
	close(h);
	xSemaphoreGive(nvMutex);
	return ret;
}

bool ESP32NVS::getBlob(const char * key, void *blob, size_t *length){
	xSemaphoreTake(nvMutex,portMAX_DELAY );
	bool ret=true;
	nvs_handle_t h = open();
	if( !h )
		return false;
	esp_err_t err = nvs_get_blob(h, key, blob, length);
	if( err != ESP_OK ){
		ESP_LOGE(FNAME,"Error getting blob!");
		ret = false;
	}
	close(h);
	xSemaphoreGive(nvMutex);
	return ret;
}
