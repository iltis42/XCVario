#ifndef __ESP32NVS_H__
#define __ESP32NVS_H__

extern "C" {
#include "esp_partition.h"
#include "esp_err.h"
#include "nvs_flash.h"
#include "nvs.h"
}
#include <string>


#ifdef TRIDENTTD_DEBUG_MODE
    #define DEBUG_PRINTER Serial
    #define DEBUG_PRINT(...) { DEBUG_PRINTER.print(__VA_ARGS__); }
    #define DEBUG_PRINTLN(...) { DEBUG_PRINTER.println(__VA_ARGS__); }
    #define DEBUG_PRINTF(fmt, args...) { DEBUG_PRINTER.printf(fmt,## args); }
#else
    #define DEBUG_PRINT(...) { }
    #define DEBUG_PRINTLN(...) { }
    #define DEBUG_PRINTF(fmt, args...) { }
#endif

class ESP32NVS {
public:
  ESP32NVS();
  static ESP32NVS& instance(){
	    if( Instance == 0 )
	    	Instance = new ESP32NVS();
        return *Instance;
  }
  bool    begin();
  void    close();

  bool    eraseAll();
  bool    erase(std::string key);

  bool    setInt(std::string key, uint8_t value);
  bool    setInt(std::string key, int16_t value);
  bool    setInt(std::string key, uint16_t value);
  bool    setInt(std::string key, int32_t value);
  bool    setInt(std::string key, uint32_t value);
  bool    setInt(std::string key, int64_t value);
  bool    setInt(std::string key, uint64_t value);
  bool    setFloat(std::string key, float value);
  bool    setString(std::string key, std::string value);
  bool    setCharArray(std::string key, const char* value);
  bool    setObject(std::string key, void* object, size_t length);

  int64_t getInt(std::string key);
  float   getFloat(std::string key, esp_err_t *err);
  std::string  getString(std::string key);
  char*   getCharArray(std::string key);
  void*   getObject(std::string key,esp_err_t *err);

//  bool    setDouble(std::string key, double value);
//  double  getDouble(std::string key);

private:
  nvs_handle_t  _nvs_handle;
  esp_err_t   _err;
  static ESP32NVS * Instance;

  bool        commit();
  nvs_handle_t  get_nvs_handle();
};



#define NVS ESP32NVS::instance()

#endif
