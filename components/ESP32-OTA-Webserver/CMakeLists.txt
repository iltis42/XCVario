idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "."
        		       REQUIRES esp_https_ota esp_http_server app_update driver ESP32-coredump nvs_flash
		       )
