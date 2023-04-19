idf_component_register( SRC_DIRS "."
                        INCLUDE_DIRS "."
                        EMBED_TXTFILES ${project_dir}/html/index.html ${project_dir}/html/milligram.min.css
                        EMBED_FILES ${project_dir}/html/index.html.gz ${project_dir}/html/milligram.min.css.gz
                        REQUIRES arduino-esp32 esp_adc_cal soc driver esp_https_ota ESP32-OTA-Webserver I2Cbus MPUdriver ESP32-coredump eglib)
