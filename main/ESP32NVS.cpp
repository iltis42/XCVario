#include "ESP32NVS.h"
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <freertos/portmacro.h>
#include <freertos/semphr.h>

xSemaphoreHandle nvMutex=NULL;

ESP32NVS * ESP32NVS::Instance = 0;

ESP32NVS::ESP32NVS(){
	_nvs_handle = 0;
	_err = 0;
}


bool ESP32NVS::begin(){
  _nvs_handle = 0;
  nvMutex=xSemaphoreCreateMutex();
  _err = nvs_flash_init();
  if (_err == ESP_ERR_NVS_NO_FREE_PAGES) {
    const esp_partition_t* nvs_partition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_NVS, NULL);
    _err = (esp_partition_erase_range(nvs_partition, 0, nvs_partition->size));
  }
  open();
  return true;
}

void ESP32NVS::close(){
  printf("ESP32NVS::close() %04X\n", _nvs_handle );
  if( _nvs_handle ) {
     nvs_close(_nvs_handle);
     _nvs_handle = 0;
  }
  else
	  printf("ESP32NVS handle already closed\n");
}

bool ESP32NVS::open(){
	printf("ESP32NVS::open()");
	if( _nvs_handle != 0 ) {
		printf("ESP32NVS handle already open\n");
		return true;
	}
	esp_err_t _err = nvs_open("Setup", NVS_READWRITE, &_nvs_handle);
	if(_err != ESP_OK){
		printf("ESP32NVS open storage error\n");
		return false;
	}
	printf(" handle: %04X  OK\n", _nvs_handle );
	return true;
}

bool ESP32NVS::eraseAll(){
  _err = nvs_erase_all(_nvs_handle);
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::erase(std::string key){
  _err =  nvs_erase_key(_nvs_handle, key.c_str());
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::commit(){
  printf("ESP32NVS::commit() %04X\n", _nvs_handle );
  _err = nvs_commit(_nvs_handle);
  if(_err != ESP_OK)  {
	  printf("ESP32NVS::commit() error\n");
	  return false;
  }
  printf("ESP32NVS::commit() OK\n");
  return true;
}




bool ESP32NVS::setInt(std::string key, uint8_t value){
  _err = nvs_set_u8(_nvs_handle, (char*)key.c_str(), value);
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::setInt(std::string key, int16_t value){
  _err = nvs_set_i16(_nvs_handle, (char*)key.c_str(), value);
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::setInt(std::string key, uint16_t value){
  _err = nvs_set_u16(_nvs_handle, (char*)key.c_str(), value);
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::setInt(std::string key, int32_t value){
  _err = nvs_set_i32(_nvs_handle, (char*)key.c_str(), value);
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::setInt(std::string key, uint32_t value){
  _err = nvs_set_u32(_nvs_handle, (char*)key.c_str(), value);
  if(_err != ESP_OK) return false;
  return commit();
}
bool ESP32NVS::setInt(std::string key, int64_t value){
  _err = nvs_set_i64(_nvs_handle, (char*)key.c_str(), value);
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::setInt(std::string key, uint64_t value){
  _err = nvs_set_u64(_nvs_handle, (char*)key.c_str(), value);
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::setCharArray(std::string key, const char* value){
  _err = nvs_set_str(_nvs_handle, (char*)key.c_str(), value);
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::setString(std::string key, std::string value){
  _err = nvs_set_str(_nvs_handle, (char*)key.c_str(), (char*)value.c_str());
  if(_err != ESP_OK) return false;
  return commit();
}

bool ESP32NVS::setObject(std::string key, void* value, size_t length){
  printf("ESP32NVS::setObject(%s , %08x, %d, handle: %04x)\n", key.c_str(), (uint32_t)value, length, _nvs_handle);
  xSemaphoreTake(nvMutex,portMAX_DELAY );
  esp_err_t _err = nvs_set_blob(_nvs_handle, (char*)key.c_str(), value, length);
  if(_err != ESP_OK) {
	  printf("set blob error %d\n", _err );
	  xSemaphoreGive(nvMutex);
	  return false;
  }
  printf("set blob OK\n");
  bool cret = commit();
  if( !cret )
	  printf("commit error\n");
  xSemaphoreGive(nvMutex);
  return cret;
}


int64_t ESP32NVS::getInt(std::string key){
  uint8_t   v_u8;
  int16_t   v_i16;
  uint16_t  v_u16;
  int32_t   v_i32;
  uint32_t  v_u32;
  int64_t   v_i64;
  uint64_t  v_u64;

  _err = nvs_get_u8(_nvs_handle, (char*)key.c_str(), &v_u8);
  if(_err == ESP_OK) return (int64_t) v_u8;

  _err = nvs_get_i16(_nvs_handle, (char*)key.c_str(), &v_i16);
  if(_err == ESP_OK) return (int64_t) v_i16;

  _err = nvs_get_u16(_nvs_handle, (char*)key.c_str(), &v_u16);
  if(_err == ESP_OK) return (int64_t) v_u16;

  _err = nvs_get_i32(_nvs_handle, (char*)key.c_str(), &v_i32);
  if(_err == ESP_OK) return (int64_t) v_i32;

  _err = nvs_get_u32(_nvs_handle, (char*)key.c_str(), &v_u32);
  if(_err == ESP_OK) return (int64_t) v_u32;

  _err = nvs_get_i64(_nvs_handle, (char*)key.c_str(), &v_i64);
  if(_err == ESP_OK) return (int64_t) v_i64;

  _err = nvs_get_u64(_nvs_handle, (char*)key.c_str(), &v_u64);
  if(_err == ESP_OK) return (int64_t) v_u64;

  return 0;
}

std::string ESP32NVS::getString(std::string key){
  printf("ESP32NVS::getString(%s)\n", key.c_str());
  size_t required_size;
  nvs_get_str(_nvs_handle, key.c_str(), NULL, &required_size);
  char* value = (char*) malloc(required_size);
  nvs_get_str(_nvs_handle, key.c_str(), value, &required_size);
  return std::string(value);
}

char* ESP32NVS::getCharArray(std::string key) {
  printf("ESP32NVS::getCharArray(%s)\n", key.c_str());
  size_t required_size;
  nvs_get_str(_nvs_handle, key.c_str(), NULL, &required_size);
  char* value = (char*) malloc(required_size);
  nvs_get_str(_nvs_handle, key.c_str(), value, &required_size);
  return value;
}

char blob[300];

void* ESP32NVS::getObject(std::string key, esp_err_t *err){
  printf("ESP32NVS::getObject(%s)\n", key.c_str());
  size_t required_size;
  *err = nvs_get_blob(_nvs_handle, key.c_str(), NULL, &required_size);
  printf("Object size: %d\n", required_size );
  // void* blob = (void*) malloc(required_size);
  // printf("blob %d", blob );
  if( required_size > sizeof(blob) ){
	  printf("ESP32NVS::getObject() ERROR, size overflow\n");
  }
  *err = nvs_get_blob(_nvs_handle, key.c_str(), blob, &required_size);
  printf("*blob: %d\n", *err );
  return blob;
}

bool ESP32NVS::setFloat(std::string key, float value){
  return setObject( key, &value, sizeof(float));
}

float ESP32NVS::getFloat(std::string key, esp_err_t *err){
  printf("ESP32NVS::getFloat(%s)\n", key.c_str());
  float *pFloat = (float*) getObject(key, err);
  return *pFloat;
}

// bool ESP32NVS::setDouble(std::string key, double value){
//   return setObject( key, &value, sizeof(double));
// }

// double ESP32NVS::getDouble(std::string key){
//   double *pDouble = (double*) getObject(key);
//   return *pDouble;
// }

nvs_handle_t ESP32NVS::get_nvs_handle(){
  return _nvs_handle;
}


