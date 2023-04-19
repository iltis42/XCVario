idf_component_register(SRC_DIRS "."
                       INCLUDE_DIRS "."
		      # EMBED_TXTFILES WebsiteFiles/index.html WebsiteFiles/favicon.ico WebsiteFiles/jquery-3.4.1.min.js
		       REQUIRES esp_https_ota esp_http_server app_update driver arduino-esp32 ESP32-coredump
		       )
