idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "."
        		       REQUIRES esp_https_ota esp_http_server app_update driver arduino-esp32 ESP32-coredump
		       )
