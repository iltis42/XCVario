#pragma once

#include <nvs.h>


class ESP32NVS
{
private:
	ESP32NVS();

public:
	static ESP32NVS& CreateInstance();
	
	bool begin();
	nvs_handle_t open();
	void close(nvs_handle_t h);
	bool commit();
	bool setBlob(const char *key, void* object, size_t length);
	bool eraseAll();
	bool erase(const char *key);
	bool getBlob(const char *key, void* object, size_t *length);

public:
	static ESP32NVS *Instance;
};

#define NVS (*ESP32NVS::Instance)

