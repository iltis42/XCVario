#ifndef __ESP32NVS_H__
#define __ESP32NVS_H__

extern "C" {
#include "esp_partition.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"
}
#include <string>


class ESP32NVS {

public:
	ESP32NVS();
	static ESP32NVS& instance(){
		if( Instance == 0 )
			Instance = new ESP32NVS();
		return *Instance;
	}
	bool    begin();
	nvs_handle_t   open();
	void    close(nvs_handle_t h);
	bool    commit();
	bool    setBlob(const char *key, void* object, size_t length);
	bool    eraseAll();
	bool    erase(const char *key);
	bool    getBlob(const char *key, void* object, size_t *length);

private:
	nvs_handle_t  _nvs_handle;
	static ESP32NVS * Instance;
};

#define NVS ESP32NVS::instance()

#endif
