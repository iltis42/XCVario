idf_component_register(SRC_DIRS "." "comm" "protocol" "wmm"
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem
                       REQUIRES soc driver esp_adc esp_wifi esp_system esp_https_ota ESP32-OTA-Webserver bt I2Cbus MPUdriver ESP32-coredump eglib qrcodegen simplex)
