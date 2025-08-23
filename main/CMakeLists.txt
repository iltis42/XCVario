idf_component_register(SRC_DIRS "." "comm" "math" "protocol" "protocol/nmea" "screen" "screen/element" "setup" "wind" "wmm"
                       INCLUDE_DIRS "."
                       EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem
                       REQUIRES soc driver nvs_flash esp_adc esp_driver_dac esp_wifi esp_system esp_http_server esp_https_ota bt I2Cbus MPUdriver ESP32-coredump eglib qrcodegen simplex glider)


# uncommet for release builds
#add_compile_definitions(CONFIG_LOG_DISABLED=1)
