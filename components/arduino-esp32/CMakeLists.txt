set( src_dirs
  "variants/esp32"
  "cores/esp32"
  "libraries/EEPROM/src"
  "libraries/ESP32/src"
  "libraries/FFat/src"
  "libraries/FS/src"
  "libraries/Preferences/src"
  "libraries/SPIFFS/src"
  "libraries/SPI/src"
  "libraries/Ticker/src"
  "libraries/Update/src"
  "libraries/Wire/src"
  "libraries/BluetoothSerial/src"
  "libraries/BLE/src"
  )

set( inc_dirs
  "variants/esp32"
  "cores/esp32"
  "libraries/EEPROM/src"
  "libraries/ESP32/src"
  "libraries/FFat/src"
  "libraries/FS/src"
  "libraries/Preferences/src"
  "libraries/SPIFFS/src"
  "libraries/SPI/src"
  "libraries/Ticker/src"
  "libraries/Update/src"
  "libraries/Wire/src"
  "libraries/BluetoothSerial/src"
  "libraries/BLE/src"
  )



idf_component_register( SRC_DIRS "${src_dirs}"
         	        INCLUDE_DIRS "${inc_dirs}"
         		REQUIRES spi_flash mbedtls mdns soc app_update bt fatfs spiffs bt )

